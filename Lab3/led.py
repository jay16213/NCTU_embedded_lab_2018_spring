#!/usr/bin/env python
import RPi.GPIO as GPIO
import time

GPIO.setmode(GPIO.BOARD)
LED_PIN = 12
ONE_UNIT = 1
GPIO.setup(LED_PIN, GPIO.OUT)

print("Yes")
# Y
GPIO.output(LED_PIN, GPIO.HIGH)
time.sleep(ONE_UNIT * 3)
GPIO.output(LED_PIN, GPIO.LOW)
time.sleep(ONE_UNIT)
    
GPIO.output(LED_PIN, GPIO.HIGH)
time.sleep(ONE_UNIT)
GPIO.output(LED_PIN, GPIO.LOW)
time.sleep(ONE_UNIT)

GPIO.output(LED_PIN, GPIO.HIGH)
time.sleep(ONE_UNIT * 3)
GPIO.output(LED_PIN, GPIO.LOW)
time.sleep(ONE_UNIT)

GPIO.output(LED_PIN, GPIO.HIGH)
time.sleep(ONE_UNIT * 3)

GPIO.output(LED_PIN, GPIO.LOW)
time.sleep(ONE_UNIT * 3)

# E
GPIO.output(LED_PIN, GPIO.HIGH)
time.sleep(ONE_UNIT)

GPIO.output(LED_PIN, GPIO.LOW)
time.sleep(ONE_UNIT * 3)

# S
GPIO.output(LED_PIN, GPIO.HIGH)
time.sleep(ONE_UNIT)
GPIO.output(LED_PIN, GPIO.LOW)
time.sleep(ONE_UNIT)

GPIO.output(LED_PIN, GPIO.HIGH)
time.sleep(ONE_UNIT)
GPIO.output(LED_PIN, GPIO.LOW)
time.sleep(ONE_UNIT)

GPIO.output(LED_PIN, GPIO.HIGH)
time.sleep(ONE_UNIT)
GPIO.output(LED_PIN, GPIO.LOW)
time.sleep(ONE_UNIT)


GPIO.cleanup()

