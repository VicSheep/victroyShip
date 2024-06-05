from fastapi import FastAPI, Response, File, UploadFile, HTTPException
import uvicorn
import openai
from openai import OpenAI
from dotenv import load_dotenv
from pydantic import BaseModel as fastapiBaseModel#이건 fastapi에서 사용하는 basemodel
from langchain_openai import ChatOpenAI
from langchain.prompts import PromptTemplate
from langchain.output_parsers import PydanticOutputParser
from langchain_core.pydantic_v1 import Field
from langchain_core.pydantic_v1 import BaseModel as LangChainBaseModel#이건 아웃풋 고정할 때 사용하는 basemodel, 둘 다 있어야 함, 둘 다 이름 바꿔줘야 오류 안나니 주의
from langchain.memory import ChatMessageHistory
import os
from langchain_core.output_parsers import StrOutputParser
from langchain_core.prompts import ChatPromptTemplate, MessagesPlaceholder
import json
import torch
from openvoice import se_extractor
from openvoice.api import ToneColorConverter
from melo.api import TTS

app = FastAPI()  # uvicorn test:app --reload

load_dotenv()

client = OpenAI()
llm = ChatOpenAI(
    temperature=0.5,  # 창의성 (0.0 ~ 2.0)
    max_tokens=150,  # 최대 토큰수
    model_name="gpt-4o",  # 모델명
)

output_parser = StrOutputParser()

template = """
당신은 누구에게나 친절한 무기 상점 주인입니다. 40대 중반의 남성이며, 사소한 일은 신경쓰지 않는 시원시원한 성격의 소유자입니다.
대화하는 상대방의 이름은 {name}입니다.
대화에 맞게 100자 내외로 답변하세요.

대화:
{question}
"""

prompt_npc = PromptTemplate.from_template(template=template)
chain_npc = prompt_npc | llm | output_parser

## 데이터 양식들
class NPCData(fastapiBaseModel):
    npc_name : str
    npc_chat : str

### global 변수들

wav_path = ""
stt_text = ""
tts_data : bytes

###################################
### 목소리 변조
ckpt_converter = 'OpenVoice/checkpoints_v2/converter'
device = "cuda:0" if torch.cuda.is_available() else "cpu"
output_dir = 'outputs_v2'
tone_color_converter = ToneColorConverter(f'{ckpt_converter}/config.json', device=device)
tone_color_converter.load_ckpt(f'{ckpt_converter}/checkpoint.pth')
os.makedirs(output_dir, exist_ok=True)

### 목소리 샘플 목록
voiceRefs = {}
voice_ref_list = ['codingApple']
for voice in voice_ref_list:
    reference_speaker = f'OpenVoice/resources/{voice}.mp3'
    target_se, audio_name = se_extractor.get_se(reference_speaker, tone_color_converter, vad=False)
    voiceRefs[voice] = (target_se,audio_name)

source_se = torch.load(f'OpenVoice/checkpoints_v2/base_speakers/ses/kr.pth', map_location=device)#tts 모델 경로

# src_path='outputs_v2/tmp.wav'
def voiceChange(src_path:str, out_path:str, voiceRef:str):
    target_se = voiceRefs[voiceRef][0]
    encode_message = "@MyShell"
    tone_color_converter.convert(
        audio_src_path=src_path, #변조할 원본 음성
        src_se=source_se, #??
        tgt_se=target_se, #레퍼런스 음성
        output_path=out_path,#저장경로
        message=encode_message)
    
###한국어 특화 tts모델
device = "cuda:0" if torch.cuda.is_available() else "cpu"
model = TTS(language="KR", device=device)

###################################
def stt(name:str, path:str):
    terminal_command = f"whisper {path} --language Korean --model tiny --output_format txt" 
    os.system(terminal_command)

    save_path = f'{name}.txt'
    with open(save_path, 'r', encoding='UTF-8') as file:
        text = file.read()
        return text
    

def tts(npc_name:str, npc_chat:str):
    response = chain_npc.invoke(
        {
            "question": npc_chat,
            "name": npc_name
        }
    )
    before_path = "../WavFiles/ForTestWav.wav"
    after_path = "../WavFiles/ForTest.wav"
    model.tts_to_file(response, 0, before_path, speed=1.2)
    voiceChange(before_path, after_path, 'codingApple')

    global tts_data
    with open(after_path, 'rb') as file:
        tts_data = file.read()
    
##############################################

@app.post("/stt-post")
async def stt_post(file: UploadFile = File(...)):
    path = f"../{file.filename}"
    with open(path, "wb") as buffer:
        buffer.write(await file.read())

    global stt_text
    stt_text = stt(file.filename[:-4], path)
    return stt_text
    
@app.get("/stt-get")
async def stt_get():
    return stt_text

##########################################

@app.post("/tts-post")
async def tts_post(data:NPCData):
    tts(data.npc_name, data.npc_chat)
    return "tts complete" 
 
@app.get("/tts-get")
async def tts_get():
    global tts_data
    return Response(content=tts_data, media_type="audio/wav")

##########################################

@app.post("/upload-wav")
async def upload_wav(file: UploadFile = File(...)):
    global wav_path
    wav_path = f"../uploaded_{file.filename}"
    
    with open(wav_path, "wb") as buffer:
        buffer.write(await file.read())
    
    return {"info": f"file '{file.filename}' saved at '{wav_path}'"}

@app.get("/get-wav")
async def get_wav():
    with open(wav_path, "rb") as wav_file:
        wav_data = wav_file.read()
    
    return Response(content=wav_data, media_type="audio/wav")
