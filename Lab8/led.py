#!/usr/bin/env python
import RPi.GPIO as GPIO
import argparse
import time

LED_PIN = 12

parser = argparse.ArgumentParser()
parser.add_argument("switch")


def _main(args):
    GPIO.setmode(GPIO.BOARD)
    GPIO.setup(LED_PIN, GPIO.OUT)

    if args.switch == 'on':
        print("on")
        GPIO.output(LED_PIN, GPIO.HIGH)
    elif args.switch == 'off':
        print("off")
        GPIO.output(LED_PIN, GPIO.LOW)
    else:
        print("error")

    GPIO.cleanup()

if __name__=='__main__':
    _main(parser.parse_args())

