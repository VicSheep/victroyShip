from fastapi import FastAPI, HTTPException
from fastapi.responses import Response
import uvicorn
import ollama
from pydantic import BaseModel as fastapiBaseModel#이건 fastapi에서 사용하는 basemodel
import json
class NPC_schedule(fastapiBaseModel):
    npc_name : str
    time : str
    special_plan : str

def lowercase_strings(string_list):#리스트 내용물 전부 소문자로
    return [s.lower() for s in string_list]

places = ["Home", 'Cafe', 'Fishing Spot', 'Park','Farm']#장소 리스트
places = lowercase_strings(places)
#각 장소에서 할 수 있는 행동들. 인게임에서 구현된 상호작용들과 같아야 한다
house_activities = [
    "Read a book",
    "have a meal",
    "Watch TV",
    "Exercise",
    "Sleep",
    "Clean the house",
    "Call a friend",
    "Take a shower",
    "Use a desktop",
    "paint on a tablet",
    "write a book"
]
house_activities=lowercase_strings(house_activities)
cafe_activities = [
    "Order coffee",
    "Chat with barista",
    "Read a magazine",
    "Work on laptop",
    "Meet a friend",
    "Listen to music",
    "People watch",
    "Sketch in a notebook",
]
cafe_activities=lowercase_strings(cafe_activities)
park_activities = [
    "Jog",
    "Walk the dog",
    "Have a picnic",
    "Read a book on a bench",
    "Feed the ducks",
    "Take photos",
    "Ride a bike",
    "Meditate",
]
park_activities=lowercase_strings(park_activities)
fishingSpot_activities = [
    "Fishing",
    "Photography"
]
fishingSpot_activities = lowercase_strings(fishingSpot_activities)
farm_activities = [
    'Farming',
    'Relaxation'
]
farm_activities = lowercase_strings(farm_activities)
#인물 페르소나
persona_ryan = '''
 name : Ryan 
 age : 29
 Innate tendency : imaginative, patient, kind
 Learned tendency : Ryan is a software engineer who loves to solve problems. He is constantly looking for ways to improve existing systems.
 Currently : Ryan is working on a project to create a new mobile app. He is also reading up on the latest technologies to stay ahead of the curve.
 Lifestyle : Ryan goes to bed around 1am, awakes up around 9am, eats dinner around 5pm.
'''
sysprom = "You are an imaginative game story writer. Answer the request."
oldScedule = "'7:00, home, sleep'"#직전에 인물이 한 행동

def setUserProm(time,persona,special_plan):
    userprom = f"""Understand the persona of the given people, {persona}.
    Around the people, there are places such as {places}.
    What you can do at home is {house_activities}. 
    What you can do at a cafe is {cafe_activities}. 
    What you can do at a fishing spot is {fishingSpot_activities}. 
    What you can do at a park is {park_activities}. 
    What you can do at a farm is {farm_activities}.
    This is what a person has done before. {oldScedule}  
    {special_plan}
    Make one prediction about what a person will do at {time}.The time is given in a 24-hour format.  Make a prediction based on the given persona and what you can do in the surrounding places and places. Answer in one concise sentence, no more than 500 characters.
    """
    return userprom

#챗봇에게 다음 스케줄 만들어달라고 하는 함수
def make_schedule(time,persona,special_plan):
    #time = convert_time(time)
    stream = ollama.chat(
    model='llama3',
    messages=[
        {"role": "system", "content": sysprom},
        {"role": "user", "content": setUserProm(time,persona,special_plan)}
    ],
    options={
             "repeat_penalty": 1.3,
             "num_ctx": 1024,
             },
    stream=False,
    )
    return stream['message']['content']

def find_places_in_string(enter_str):
    enter_str = enter_str.lower()

    for place in places:
        if place in enter_str:
            return place
    print('place error')
    return places[0]

def find_activity_in_string(activity_list,enter_str):
    enter_str = enter_str.lower()
    for activity in activity_list:
        if activity in enter_str:
            return activity
    print('activity error')
    return activity_list[0]#대답이 이상할 경우 리스트의 첫번째 행동 출력

def search_place(chatbot_response):
    stream = ollama.chat(
    model='llama3',
    messages=[
        {"role": "system", "content": f'Understand the given sentence and answer it succinctly by choosing one of the most appropriate places from the list. {places}'},
        {"role": "user", "content": chatbot_response}
    ],
    options={"temperature":0.5
            },
    stream=False,
    )
    stream['message']['content'] = find_places_in_string(stream['message']['content'])
    return stream['message']['content']

def serach_activity(place,userprom):
    place_activities = {"home":house_activities, 'cafe':cafe_activities, 'park':park_activities, 'farm':farm_activities, 'fishing spot':fishingSpot_activities}.get(place,house_activities)
    stream = ollama.chat(
    model='llama3',
    messages=[
        {"role": "system", "content": f'Understand the given sentence and answer it succinctly by choosing one action from the list that is most relevant to it.{place_activities}'},
        {"role": "user", "content": userprom}
    ],
        options={"temperature":0.5
                },
    stream=False,
    )
    stream['message']['content'] = find_activity_in_string(place_activities,stream['message']['content'])
    return stream['message']['content']

def escape_quotes(s):
    return s.replace('"', '\"').replace("'", "\'")

def schedule_json_maker(time,persona,special_plan):
    schedule = make_schedule(time,persona,special_plan)
    escape_quotes(schedule)
    scheduled_place = search_place(schedule)
    escape_quotes(scheduled_place)
    scheduled_activity = serach_activity(scheduled_place,schedule)
    escape_quotes(scheduled_activity)
    global oldScedule
    oldScedule = f"'{time},{scheduled_activity}'"
    json_str = f"""{'{'}
  "name": "Ryan", 
  "place": "{scheduled_place}", 
  "activity": "{scheduled_activity}", 
  "description": "{schedule}"
{'}'}"""
    return json_str

app = FastAPI()
@app.get("/")
def index():
    return "make schedule test"

@app.post("/get-schedule/")
async def generate_response(data:NPC_schedule):
    persona = {'ryan':persona_ryan}.get(data.npc_name,persona_ryan)
    json_str = schedule_json_maker(data.time,persona,data.special_plan)
    return Response(content=json_str, media_type="application/json")

# Uvicorn 서버 실행
if __name__ == "__main__":
    import uvicorn
    uvicorn.run(app, host="127.0.0.1", port=8001)
