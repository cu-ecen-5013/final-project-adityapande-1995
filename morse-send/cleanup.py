#!python3
import RPi.GPIO as GPIO

GPIO.setmode(GPIO.BOARD)
GPIO.setup(8, GPIO.OUT)
GPIO.setup(10, GPIO.OUT)

GPIO.output(8,0)
GPIO.output(10,0)
