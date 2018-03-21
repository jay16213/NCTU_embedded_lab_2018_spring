#!/usr/bin/env python
import RPi.GPIO as GPIO
import time

ONE_UNIT = 1
LED_PIN = 12
v = 343         # (331 + 0.6*20)
TRIGGER_PIN = 16
ECHO_PIN = 18
GPIO.setmode(GPIO.BOARD)
GPIO.setup(LED_PIN, GPIO.OUT)
GPIO.setup(TRIGGER_PIN,GPIO.OUT)
GPIO.setup(ECHO_PIN,GPIO.IN)

def bling():
    GPIO.output(LED_PIN, GPIO.HIGH)
    time.sleep(1)
    GPIO.output(LED_PIN, GPIO.LOW)
    time.sleep(1)

def bling_s():
    GPIO.output(LED_PIN, GPIO.HIGH)
    time.sleep(0.2)
    GPIO.output(LED_PIN, GPIO.LOW)
    time.sleep(0.2)

def measure() :
    GPIO.output(TRIGGER_PIN, GPIO.HIGH)
    time.sleep(0.00001)     #10us
    GPIO.output(TRIGGER_PIN, GPIO.LOW)
    pulse_start = time.time()
    while GPIO.input(ECHO_PIN) == GPIO.LOW:
        pulse_start = time.time()
    while GPIO.input(ECHO_PIN) == GPIO.HIGH:
        pulse_end = time.time()
    t = pulse_end - pulse_start
    d = t * v
    d = d/2
    return d*100

while True:
    distance = measure()
    print("distance: {}".format(distance))
    if distance < 15:
        bling_s()
    elif distance < 30:
        bling()



GPIO.cleanup()
