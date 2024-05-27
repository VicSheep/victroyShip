from fastapi import FastAPI, HTTPException
import uvicorn
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
# from langchain.chat_models import ChatOllama
from langchain_core.prompts import ChatPromptTemplate, MessagesPlaceholder
import json
import torch
from openvoice import se_extractor
from openvoice.api import ToneColorConverter
from melo.api import TTS

import time

app = FastAPI() #  uvicorn farmlife_server:app --reload

load_dotenv()

#model = ChatOpenAI(model="gpt-4o", temperature=0.8, max_tokens=200, stream=False)
client = OpenAI()

wav_path = "../WavFiles/Speech.wav" #tts 음성파일 저장될 경로 설정
output_path = "../WavFiles/Output.wav"

class NPC_Name(fastapiBaseModel):
    npc_name : str

class NPC_Input(fastapiBaseModel):
    npcName : str #대화할 npc 이름 입력, 해당 이름이 곧 세션이름, persona_ㅇㅇㅇ.txt 파일 필요
    question : str
    likeability : int

# pydantic 모델 설정 - 출력양식
class NPC_Output(LangChainBaseModel):
    answer: str = Field(description="요청에 대한 답변")
    emotion: str = Field(description="Respond to emotional changes caused by talking to humans with either joy, surprise, sadness, or anger.")
    #human과의 대화로 인한 감정의 변화를 기쁨, 놀람, 슬픔, 화남 중 하나로 응답할 것
    likeability: int = Field(description="Respond to the change in favorability from talking to a human as an integer between -10 and 10.")
    #human과의 대화로 인한 호감도의 변화량을 -10 에서 10 사이의 정수로 응답할 것
    file_path: str = Field(description="답변 음성 파일 경로")

# NPC 인사 설정
class NPC_Greeting_Input(fastapiBaseModel):
    npc_name : str
    text : str
    likeability : int

class NPC_Greeting_Output(fastapiBaseModel):
    npc_name : str
    answer : str
    file_path : str
    likeability : int

# 언어 모델 설정
llm = ChatOpenAI(
    temperature=0.5,  # 창의성 (0.0 ~ 2.0)
    max_tokens=150,  # 최대 토큰수
    model_name="gpt-4o",  # 모델명
)
# llm = ChatOllama(model="Llama-3-Open-Ko-8B-Q6_K:latest", temperature=0.5, num_predict=200,)
# llm = ChatOllama(model="llama-3-Korean-Bllossom-8B-Q4_K_M", temperature=0.8, num_predict=200,)
# llm = ChatOllama(model="EEVE-Korean-10.8B-Q4_K_M", temperature=0.5, num_predict=200,)


#당신은 감정을 가진 인격체입니다. 당신의 페르소나인 {persona}를 이해하고, 해당 인물이 되어서 주어진 요청에 이전 대화 기록과 대사 예시를 바탕으로 해당 인물이 할 듯한 응답을 200자 이내의 한국어 대화체로 응답하세요.
template = """
You are a person with feelings. Understand your persona, {persona}, and respond to a given request in a conversational style of no more than 100 characters in Korean, based on previous dialogue history and example dialogue. {dialogue_example}

request:
{request_content}

FORMAT:
{format}
"""
prompt = PromptTemplate.from_template(template=template)
parser = PydanticOutputParser(pydantic_object=NPC_Output)# parser 설정
prompt = prompt.partial(format=parser.get_format_instructions())# 프롬프트 설정

chain = prompt | llm | parser

name = ""
Gpersona = ""

def load_persona(session_id):#페르소나 로드
    global name
    global Gpersona
    if name==session_id:
        return Gpersona#같은 사람이랑 이야기할 때는 계속 페르소나를 읽지 않고 저장된 변수에서 가져오도록
    name = session_id
    persona_file = f"./Personas/persona_{session_id}.txt"
    if os.path.isfile(persona_file):
        with open(persona_file, 'r', encoding='utf-8') as f:
            persona = f.read()
            Gpersona = persona
    else:
        print(f"파일 '{persona_file}'이 존재하지 않습니다.")
        persona = "" 
    return persona

data = ""
def get_sentences_as_string(npcName, preference_level:int):
    preference_level = set_preference(preference_level)
    # JSON 파일을 읽어서 파싱합니다.
    global name
    global data
    if name != npcName:#같은 사람과 이야기할 때는 읽어오기 않고 저장된 변수에서 가져오도록
        with open(f'./Personas/sentence_{npcName}.json', 'r', encoding='utf-8') as file:
            data = json.load(file)
            name = npcName

    # 입력된 호감도에 따라 문장을 가져옵니다.
    if preference_level in data:
        sentences = data[preference_level]
        # 각 문장을 " "로 감싸고 ,로 구분하여 하나의 문자열로 만듭니다.
        result = ', '.join(f'"{sentence}"' for sentence in sentences)
        return result
    else:
        return f"'{preference_level}'에 해당하는 데이터가 없습니다."
    
def set_preference(intPref:int):
    if intPref > 70:
        return "호감도 높은 경우 (친한 친구 또는 가까운 사람)"
    elif intPref > 30:
        return "호감도 중간 (친해지고 있는 이웃 또는 지인)"
    else:
        return "호감도 낮은 경우 (낯선 사람 또는 처음 만난 사람)"
    

session_store = {} # 메시지 기록(세션)을 저장할 딕셔너리

# 목소리 딕셔너리
voice_dict = {'미라':'ani', '이준호':'codingApple', '새로만듦':'codingApple', '이춘식':'rammus', '빈칸2':'hakers', '빈칸3':'hakers'}

def getChatLog(session_ids : str):#npc 개별로 채팅 기록 생성, 각각 기존의 채팅 내역을 기억하고 대화에 반영함.
    if session_ids not in session_store: # 세션 기록이 없을 경우 - 유저가 대화한 적이 없을 경우 -> 새 채팅창 생성
        session_store[session_ids] = ChatMessageHistory()# 새로운 객체 생성해서 세션 스토어에 저장하기
    return session_store[session_ids] # 대화한 적이 있을 경우 저장된 세션 ID 기록 반환하기


def talk2npc(npcName:str,dialog:str,preperence:int):#이름, 대화문, 현재 호감도
    getChatLog(npcName).add_user_message(dialog) #유저의 입력을 로그에 입력
    response = chain.invoke(
        {
            "persona": load_persona(npcName),
            "request_content": getChatLog(npcName).messages,
            "dialogue_example": get_sentences_as_string(npcName,preperence),
        }
    )

    getChatLog(npcName).add_ai_message(response.answer)#결과 중 대답 부분만 로그에 저장
    return response


def summarizeChat(npcName):#지금까지의 대화를 요약 후 저장함, 기존 대화내역은 삭제됨, 대화 끝날때 실행됨
    chatLog = getChatLog(npcName).messages
    if len(chatLog)==0:
        return False
    summarizationPrompt = ChatPromptTemplate.from_messages(
        [
            MessagesPlaceholder(variable_name="chat_history"),
            (
                "user",
                "Distill the above chat messages into a single summary message. Include as many specific details as you can.",
            ),
        ]
    )
    summarizationChain = summarizationPrompt | llm
    SC = summarizationChain.invoke({"chat_history":chatLog})
    print(SC)
    getChatLog(npcName).clear()
    getChatLog(npcName).add_message(SC.content)
    return SC.content


def tts(response:NPC_Output, npc_name):
    begin_time = time.time()
    model.tts_to_file(response.answer, 0, wav_path, speed=1.2)#melo tts 한국어 모델
    voiceChange(wav_path, voice_dict[npc_name])#음성변조
    response.file_path = wav_path
    end_time = time.time()
    print(f'tts: {end_time - begin_time: .5f} sec')
    return response.file_path


### 목소리 변조
ckpt_converter = 'OpenVoice/checkpoints_v2/converter'
device = "cuda:0" if torch.cuda.is_available() else "cpu"
output_dir = 'outputs_v2'
tone_color_converter = ToneColorConverter(f'{ckpt_converter}/config.json', device=device)
tone_color_converter.load_ckpt(f'{ckpt_converter}/checkpoint.pth')
os.makedirs(output_dir, exist_ok=True)

### 목소리 샘플 목록
voiceRefs = {}
voice_ref_list = ['codingApple', 'hakers', 'jax', 'rammus', 'ani', 'teemo']
for voice in voice_ref_list:
    reference_speaker = f'OpenVoice/resources/{voice}.mp3'
    target_se, audio_name = se_extractor.get_se(reference_speaker, tone_color_converter, vad=False)
    voiceRefs[voice] = (target_se,audio_name)

source_se = torch.load(f'OpenVoice/checkpoints_v2/base_speakers/ses/kr.pth', map_location=device)#tts 모델 경로

# src_path='outputs_v2/tmp.wav'
def voiceChange(src_path:str,voiceRef:str):
    # source_se = torch.load(f'OpenVoice/checkpoints_v2/base_speakers/ses/kr.pth', map_location=device)#tts 모델 경로
    # Run the tone color converter
    target_se = voiceRefs[voiceRef][0]
    encode_message = "@MyShell"
    tone_color_converter.convert(
        audio_src_path=src_path, #변조할 원본 음성
        src_se=source_se, #??
        tgt_se=target_se, #레퍼런스 음성
        output_path=wav_path,#저장경로
        message=encode_message)
    
###한국어 특화 tts모델
device = "cuda:0" if torch.cuda.is_available() else "cpu"
model = TTS(language="KR", device=device)


output_parser = StrOutputParser()

template = """
당신은 농장에서 자라는 식물입니다. 대화가 당신에게 긍정적인지 부정적인지 판단하고, 문자 없이 오로지 정수 형태로만 답변해주세요.
가장 긍정적인 대화를 10으로, 가장 부정적인 대화를 -10으로 합니다.
긍정적인지 부정적인지 판단하기 어렵다면, 0점으로 답변하세요.

대화:
{question}
"""

prompt_plant = PromptTemplate.from_template(template=template)
chain_plant = prompt_plant | llm | output_parser

def talk2plant(speech):
    result = chain_plant.invoke({"question":f"{speech}"})
    return result

## 요청 양식 설정
## NPC 대화
class STTData(fastapiBaseModel):
    file_name : str
    file_path : str

class TextData(fastapiBaseModel):
    npc_name : str
    chat_text : str
    likeability : int

class NPCData(fastapiBaseModel):
    npc_name : str
    likeability : int

## 식물 성장
class STT2Plant(fastapiBaseModel):
    file_name : str
    file_path : str

class Text2Plant(fastapiBaseModel):
    chat_text : str

latest_speech = ""
latest_comment = ""
talk2plant_score = ""
latest_npc_name = ""

# 음성 파일 경로 -> 텍스트 추출
def stt(name:str, path:str):
    begin_time = time.time()
    terminal_command = f"whisper {path} --language Korean --model tiny --output_format txt" 
    os.system(terminal_command)

    save_path = f'{name}.txt'
    with open(save_path, 'r', encoding='UTF-8') as file:
        text = file.read()
        end_time = time.time()
        print(f'stt: {end_time - begin_time: .5f} sec')
        return text


latest_dict = {}

# NPC 대화
@app.post("/post-speech")
async def post_speech(data:STTData):
    try:
        global latest_speech
        latest_speech = stt(data.file_name, data.file_path)
        return latest_speech
    except Exception as e:
        raise HTTPException(status_code=400, detail=f"{str(e)}")

@app.get("/get-speech")
async def get_speech():
    global latest_speech
    return latest_speech[:len(latest_speech) - 1]


@app.post("/post-conv")
async def post_conv(data:NPCData):
    try:
        global latest_dict, latest_npc_name
        latest_npc_name = data.npc_name
        latest_dict = talk2npc(data.npc_name, latest_speech, data.likeability)
        return latest_dict
    except Exception as e:
        raise HTTPException(status_code=400, detail=f"{str(e)}")

@app.get("/get-conv")
async def get_conv():
    global latest_dict
    return latest_dict


## NPC 대화(텍스트)
@app.post("/post-text")
async def post_text(data:TextData):
    try:
        global latest_speech, latest_dict, latest_npc_name
        latest_speech = data.chat_text
        latest_npc_name = data.npc_name
        latest_dict = talk2npc(data.npc_name, latest_speech, data.likeability)
        return latest_dict
    except Exception as e:
        raise HTTPException(status_code=400, detail=f"{str(e)}")

@app.get("/get-text")
async def get_text():
    global latest_dict
    return latest_dict


### NPC TTS 가져오기
@app.get("/get-tts")
async def get_tts():
    return tts(latest_dict, latest_npc_name)


#### NPC 인사 초기화
greetings = {}
requested_npc_name = ""

@app.post("/init-greeting")
async def init_greeting(data:NPC_Greeting_Input):
    try:
        greeting = talk2npc(data.npc_name, latest_speech, data.likeability)
        greeting.file_path = tts(greeting, data.npc_name)
        greetings[data.npc_name] = greeting
        return greetings[data.npc_name]
    except Exception as e:
        raise HTTPException(status_code=400, detail=f"{str(e)}")
    
@app.post("/post-greeting")
async def post_greeting(npc_name:str):
    global requested_npc_name
    requested_npc_name = npc_name

@app.get("/get-greeting")
async def get_greeting():
    return greetings[requested_npc_name]


##### NPC 대화 기록 지우기
@app.post("/end-chat")
def end_chat(data:NPC_Name):
    return summarizeChat(data.npc_name)

########################################################################################

# 식물 칭찬/비난
@app.post("/post-talk2plant")
async def post_talk2plant(data:STT2Plant):
    try:
        global latest_comment, talk2plant_score
        latest_comment = stt(data.file_name, data.file_path)
        talk2plant_score = talk2plant(latest_comment)
        return talk2plant_score
    except Exception as e:
        raise HTTPException(status_code=400, detail=f"{str(e)}")

@app.get("/get-talk2plant")
async def get_talk2plant():
    global talk2plant_score
    return talk2plant_score
    

## 식물 칭찬 / 비난(텍스트)
@app.post("/post-text2plant")
async def post_talk2plant(data:Text2Plant):
    try:
        global latest_comment, talk2plant_score
        latest_comment = data.chat_text
        talk2plant_score = talk2plant(latest_comment)
        return talk2plant_score
    except Exception as e:
        raise HTTPException(status_code=400, detail=f"{str(e)}")

@app.get("/get-text2plant")
async def get_talk2plant():
    global talk2plant_score
    return talk2plant_score


# Uvicorn 서버 실행
if __name__ == "__main__":
    import uvicorn
    uvicorn.run(app, host="127.0.0.1", port=8000)