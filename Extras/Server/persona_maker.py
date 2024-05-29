from openai import OpenAI
from dotenv import load_dotenv
from pydantic import BaseModel

load_dotenv()
client = OpenAI()

class input(BaseModel):
    work : str
    model : str
    persona : str
    question : str

print('페르소나 생성 시작')

# 당신은 게임 스토리 작가입니다. 기획할 게임은 농촌 배경의 npc와의 관계가 메인인 게임입니다.해당 게임에서 플레이어의 이웃이 될 요청받은 npc 캐릭터의 롤플레잉을 위한 독특한 개성과 특징을 가진 페르소나를 자세히 구상하고 서술하세요. 캐릭터 페르소나에는 아래의 항목들이 포함되어야 합니다.
persona_prompt = '''
You are a game story writer. The game to be planned is a game that mainly revolves around relationships with NPCs in a rural setting. If input has a name, set the npc's name to that. Detail a persona with unique personalities and characteristics for role-playing of the NPC character who is asked to be the player's neighbor in the game and write it in English. Your character persona should include the following:
name
age
gender
vocation
background
Looks
feature
character
specialty
hobby
objective
'''

# 아래 명령어 터미널에 복사+붙여넣기
# python persona_maker.py

####################### 만들고자 하는 npc 컨셉 적기 #######################

npc_name = "김옥자"
persona = f"나이가 많고 타인에게 친절하며, 요리를 좋아하는 이름이 {npc_name}인 할머니 npc의 페르소나를 만드시오."

#########################################################################

system_content = persona_prompt
completion = client.chat.completions.create(
    model="gpt-4-turbo",
    messages=[
        {"role": "system", "content": system_content},
        {"role": "user", "content": persona}
    ],
    temperature=0.5,
    max_tokens=1000
)
msg = completion.choices[0].message

####################### persona 파일 만들기 #######################

p_begin = str(completion.choices[0].message).find('=') + 2
p_end = str(completion.choices[0].message).find('role=') -3
p_result = str(completion.choices[0].message)[p_begin:p_end]

persona_path = f'./Personas/persona_{npc_name}.txt'

with open(persona_path, 'w', encoding='UTF-8-sig') as file:
    file.write(p_result)
    print(f'persona_{npc_name}.txt 생성 완료')

###################################################################

npcPersona = msg.content

sysprom = """주어지는 페르소나를 기반으로 해당 인물이 할 법한 한국어 대사 예시를 호감도별로 각각 5개씩 작성하시오. 대사 예시의 말투, 대화 길이는 대화 상대와의 호감도에 따라 달라집니다.
호감도 낮은 경우 (낯선 사람 또는 처음 만난 사람)
호감도 중간 (친해지고 있는 이웃 또는 지인)
호감도 높은 경우 (친한 친구 또는 가까운 사람)
"""
completion = client.chat.completions.create(
    model="gpt-4-turbo",
    messages=[
        {"role": "system", "content": sysprom},
        {"role": "user", "content": npcPersona}
    ],
    temperature=0.5,
    max_tokens=1000
)
result_text = completion.choices[0].message.content

### 대화 예시 파일 만들기
low = []
middle = []
high = []

idx = 0
while idx < len(result_text):
    if result_text[idx] != '\"':
        idx += 1
        continue
    # 대화 긁어오기
    begin, end = idx, idx + 1
    while result_text[end] != '\"':
        end += 1
    text = result_text[begin:end + 1]
    if len(low) < 5: 
        low.append(text)
    elif len(middle) < 5: 
        middle.append(text)
    else:
        high.append(text)
        if len(high) == 5:
            break
    
    idx = end + 1

template = f"""{{
    "호감도 낮은 경우 (낯선 사람 또는 처음 만난 사람)": [
    {low[0]},
    {low[1]},
    {low[2]},
    {low[3]},
    {low[4]}
    ],
    "호감도 중간 (친해지고 있는 이웃 또는 지인)": [
    {middle[0]},
    {middle[1]},
    {middle[2]},
    {middle[3]},
    {middle[4]}
    ],
    "호감도 높은 경우 (친한 친구 또는 가까운 사람)": [
    {high[0]},
    {high[1]},
    {high[2]},
    {high[3]},
    {high[4]}
    ],
}}"""

sentence_path = f'./Personas/sentence_{npc_name}.json'

with open(sentence_path, 'w', encoding='UTF-8-sig') as file:
    file.write(template)
    print(f'sentence_{npc_name}.json 생성 완료')

print('페르소나 생성 완료')