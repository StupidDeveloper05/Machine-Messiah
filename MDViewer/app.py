from flask import Flask, render_template
from flask_socketio import SocketIO, send
from flask import request
from engineio.async_drivers import gevent
import socket

import json
import time
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("--key", type=str);
parser.add_argument("--port", type=int);
parser.add_argument("--file", type=str);
args = parser.parse_args()

ACCESS_KEY = args.key;
print(f"ACCESS KEY is {ACCESS_KEY}")

app = Flask(__name__)   
socket_io = SocketIO(app, cors_allowed_origins="*")

Cache = []
Data = {}

def load_data(path):
    global Data
    with open(path, "r") as f:
        json_object = json.load(f)
        for chat in json_object["chatting"]:
            Data[chat] = []
            for i in json_object["chatting"][chat]["data"]:
                Data[chat].append((i["role"], i["content"]))

if args.file != "":
    load_data(args.file)

@app.route('/')
def home():
    return '<h1>웹 마크다운 뷰어</h1>'
    
@app.route('/chat/<uuid>')
def chat_view(uuid):
    accessKey = request.args.get("key", "")
    if accessKey == ACCESS_KEY:
        return render_template("index.html", 
                               Data=[] if Data.get(uuid) == None else Data[uuid], 
                               UUID=uuid)
    else:
        return "<h1>액세스 키가 유효하지 않습니다.</h1>"
    
@app.route('/chat2/<uuid>')
def chat_view2(uuid):
    accessKey = request.args.get("key", "")
    if accessKey == ACCESS_KEY:
        return render_template("develope.html", 
                               Data=[] if Data.get(uuid) == None else Data[uuid], 
                               UUID=uuid)
    else:
        return "<h1>액세스 키가 유효하지 않습니다.</h1>"
    
@app.route('/shutdown', methods=['GET'])
def shutdown():
    accessKey = request.args.get("key", "")
    if accessKey == ACCESS_KEY:
        socket_io.stop()
        return "Server closed"
    else:
        return "액세스키가 유효하지 않습니다."

@socket_io.on("message")
def request_io(msg):
    if msg["key"] == ACCESS_KEY:
        if msg['uuid'] not in Data:
            Data[msg['uuid']] = []
        if msg["status"] == "running":
            Data[msg['uuid']][-1] = (msg["type"], msg["msg"])
        else:
            Data[msg['uuid']].append((msg["type"], msg["msg"]))
        if msg["clear"]:
            Data[msg['uuid']] = []
        else:
            send(msg, broadcast=True)
    print(f"[{time.strftime('%Y.%m.%d %p %I:%M:%S')}] {msg['type'].upper()} : {msg['status']}")

if __name__ == "__main__":
    socket_io.run(app, port=args.port, use_reloader=False)