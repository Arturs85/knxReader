from api import Detector
import os
import numpy as np
import cv2 
from PIL import Image
from datetime import datetime
from subprocess import call

st = "nvarguscamerasrc sensor-id=0 ! video/x-raw(memory:NVMM),width=1640, height=1232,framerate=21/1, format=NV12 ! nvvidconv flip-method=2 ! video/x-raw,format=BGRx, width=1640, height=1232, pixel-aspect-ratio=1/1 ! videoconvert ! video/x-raw,format=BGR ! appsink drop=true  max-buffers=1"
cap = cv2.VideoCapture(st,cv2.CAP_GSTREAMER)
if not cap.isOpened():
    print("Cannot open camera")
    exit()

# Initialize detector
detector = Detector(model_name='rapid',
                    weights_path='./weights/pL1_MWHB1024_Mar11_4000.ckpt',
                    use_cuda=True)

def update_db(detections):
    # make command for db update
    com1 = "curl -X PATCH -H"
    com15 = '{ \"name\": \"status\", \"fields\": {\"isStarted\": {\"booleanValue\": false},\"persons\": {\"arrayValue\": {\"values\": [';

    com2 = "]}},\"lastImageTime\": {\"timestampValue\": \"";

    start = "{\"mapValue\": {\"fields\": {\"xCord\": {\"integerValue\": \"";
    mid = "\"},\"yCord\": {\"integerValue\": \"";

    end = "\"}}}}";

    detStr='' 
    for detection in detections: 
      detStr+=start+str(int(detection[0]))+mid+str(int(detection[1]))+end+','

    detStr = detStr[:-1]

    time = datetime.now().strftime('%Y-%m-%dT%H:%M:%SZ')

    com3 = "\" }}}"
    com4 =  'https://content-firestore.googleapis.com/v1beta1/projects/ceilingcounterui/databases/(default)/documents/containera/status?currentDocument.exists=true&alt=json';

    r = com15+detStr+com2+time+com3
    # send update command with curl
    print(r)
    #os.system(r)
    call(['curl', '-X', 'PATCH', '-H', 'Content-Type: application/json', '-d' ,r,com4])

while True:
    # Capture frame-by-frame
    ret, img = cap.read()
    # if frame is read correctly ret is True
    if not ret:
        print("Can't receive frame (stream end?). Exiting ...")
        break
  
    img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
    pilimg = Image.fromarray(img)

    detections = detector.detect_one(pil_img=pilimg,
                    input_size=1024, conf_thres=0.1,
                    visualize=True)
  
    update_db(detections)


#        detections: rows of (x,y,w,h,angle,conf,...)
    print(f'detections.size', detections.size())

# When everything done, release the capture
cap.release()
cv2.destroyAllWindows()

