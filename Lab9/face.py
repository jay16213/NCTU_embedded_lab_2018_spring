from picamera.array import PiRGBArray
from picamera import PiCamera
import time
import cv2
import numpy as np

#Load a cascade file for detecting faces
face_cascade = cv2.CascadeClassifier('haarcascade_frontalface_default.xml')

# initialize the camera and grab a reference to the raw camera capture
camera = PiCamera()
camera.resolution = (320, 240)
camera.framerate = 32
rawCapture = PiRGBArray(camera, size=(320, 240))

# allow the camera to warmup
time.sleep(0.1)

# capture frames from the camera
for frame in camera.capture_continuous(rawCapture, format="bgr", use_video_port=True):
    # grab the raw NumPy array representing the image, then initialize the timestamp
    # and occupied/unoccupied text
    image = frame.array

    #Look for faces in the image using the loaded cascade file
    faces = face_cascade.detectMultiScale(image, 1.1, 5)
    #Draw a rectangle around every found face
    for (x,y,w,h) in faces:
        face_region = image[y:y+h, x:x+w]
        face_region = cv2.cvtColor(face_region, cv2.COLOR_BGR2GRAY)

        image.setflags(write=1)
        for _x in range(x, x+w):
            for _y in range(y, y+h):
                image[_y][_x] = face_region[_y - y][_x - x]

        cv2.rectangle(image,(x,y),(x+w,y+h), (255,255,0), 2)



    # show the frame
    cv2.imshow("Frame", image)
    key = cv2.waitKey(1) & 0xFF
    # clear the stream in preparation for the next frame
    rawCapture.truncate(0)
    # if the `q` key was pressed, break from the loop
    if key == ord("q"):
        break
