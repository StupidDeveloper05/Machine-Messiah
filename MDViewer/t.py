import json

result = {}
with open("C:\\Users\\yuksu\\AppData\\Local\\Packages\\36b4c69c-84cf-4be9-a7fb-56eda70b1b2a_sze2rqgp4k4ww\\LocalState\\userData.json", "r") as f:
    json_object = json.load(f)
    for chat in json_object["chatting"]:
        result[chat] = []
        for i in json_object["chatting"][chat]["data"]:
            result[chat].append((i["role"], i["content"]))

print(result)