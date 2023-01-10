import math
import time
from simple_pid import PID
pid = PID(10,0,0, setpoint=1.5)
rotation = 300
windDir = 220


def main():
    while True:
        voltage = fakeVoltage()
        control = pid(voltage)
        control = round(control)
        rotateTurbine(control)
        print("Degrees Change: %5.2i Rotation: %5.2f Voltage: %5.2f" %
              (control, rotation, voltage))
        time.sleep(0.1)


def fakeVoltage():
    return 1.5*math.exp(-0.0001*math.pow(rotation-windDir, 2))


def rotateTurbine(change):
    global rotation
    rotation += change
    rotation = rotation % 360


if __name__ == "__main__":
    main()
