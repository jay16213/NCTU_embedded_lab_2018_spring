#!/usr/bin/python
from gyro import getGyro
from acceler import getAcceler
from pressure import getPressure
import time
# from compass

def _main():
    output = open("output.csv", 'w')
    output.write("ACCx, ACCy, ACCz, GYRx, GYRy, GYRz, Temp(*C), Pressure(Pa), Altitude(m), Sealevel Pressure(Pa)\n")
    for i in range(500):
        a = getAcceler()
        print("acceler: {0}, {1}, {2}".format(a[0], a[1], a[2]))
        output.write("{0}, {1}, {2},".format(a[0], a[1], a[2]))


        g = getGyro()
        print("gyro: {0}, {1}, {2}".format(g[0], g[1], g[2]))
        output.write("{0}, {1}, {2},".format(g[0], g[1], g[2]))

        p = getPressure()
        print("temp: {}".format(p[0]))
        print("pressure: {}".format(p[1]))
        print("altitude: {}".format(p[2]))
        print("sealevel_pressure: {}".format(p[3]))
        output.write("{0}, {1}, {2}, {3}\n".format(p[0], p[1], p[2], p[3]))

        time.sleep(1)

    output.close()



if __name__=='__main__':
    _main()
