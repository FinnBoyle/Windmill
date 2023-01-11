import math
from random import randrange
import time
from simple_pid import PID
pid = PID(10,0,0, setpoint=1.5)
rotation = 300
windDir = 180

#Time of previous change in wind direction
prev_time = time.time()


def main():
    global windDir, prev_time
    while True:
        current_time = time.time()
        if ((current_time - prev_time) > 5):
            windDir = randrange(360) 
            prev_time = current_time
        
        voltage = fakeVoltage()
        control = pid(voltage)
        control = control
        rotateTurbine(control)
        print("WindDir: %5i Degrees Change: %5.2f Rotation: %5.2f Voltage: %5.2f" %
              (windDir, control, rotation, voltage))
        time.sleep(0.1)


def fakeVoltage():
    return 1.5*math.exp(-0.0001*math.pow(rotation-windDir, 2))


def rotateTurbine(change):
    global rotation
    rotation += change
    rotation = rotation % 360


if __name__ == "__main__":
    main()
