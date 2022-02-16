import board
import digitalio
import time

pin_name = board.LED

led = digitalio.DigitalInOut(pin_name)
led.direction = digitalio.Direction.OUTPUT
while True:
    led.value = True
    time.sleep(0.1)
    led.value = False
    time.sleep(0.1)