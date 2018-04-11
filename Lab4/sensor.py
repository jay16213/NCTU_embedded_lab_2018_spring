import math
from time import sleep
#import smbus to access i2c port
import smbus
import string
import Adafruit_ADXL345
'''
Get the 3-axis tilt degree from sensor

params: previous angles from x-axis, y-axis, z-axis
return: update angles

'''

#converts 16 bit two's compliment reading to signed int
def getSignedNumber(number):
    if number & (1 << 15):
        return number | ~65535
    else:
        return number & 65535

#open /dev/i2c-1
i2c_bus=smbus.SMBus(1)
#i2c slave address of the L3G4200D
i2c_address=0x69

#initialise the L3G4200D

#normal mode and all axes on to control reg1
i2c_bus.write_byte_data(i2c_address,0x20,0x0F)
#full 2000dps to control reg4
i2c_bus.write_byte_data(i2c_address,0x23,0x20)


accel = Adafruit_ADXL345.ADXL345()

def getAngle():
    i2c_bus.write_byte(i2c_address,0x28)
    X_L = i2c_bus.read_byte(i2c_address)
    i2c_bus.write_byte(i2c_address,0x29)
    X_H = i2c_bus.read_byte(i2c_address)
    X = X_H << 8 | X_L

    i2c_bus.write_byte(i2c_address,0x2A)
    Y_L = i2c_bus.read_byte(i2c_address)
    i2c_bus.write_byte(i2c_address,0x2B)
    Y_H = i2c_bus.read_byte(i2c_address)
    Y = Y_H << 8 | Y_L

    i2c_bus.write_byte(i2c_address,0x2C)
    Z_L = i2c_bus.read_byte(i2c_address)
    i2c_bus.write_byte(i2c_address,0x2D)
    Z_H = i2c_bus.read_byte(i2c_address)
    Z = Z_H << 8 | Z_L

    X = getSignedNumber(X)
    Y = getSignedNumber(Y)
    Z = getSignedNumber(Z)

    #print("{0} {1} {2}".format(X, Y, Z))

    X_c = (X * 8.75) / 1000.0
    Y_c = (Y * 8.75) / 1000.0
    Z_c = (Z * 8.75) / 1000.0
    #print("{0} {1} {2}".format(X_c, Y_c, Z_c))
    # print(string.rjust(`X`, 10))
    # print(string.rjust(`Y`, 10))
    # print(string.rjust(`Z`, 10))
    return Z_c


# You have to read data and process them from sensor
# Hint: pitch, roll
# Be careful for the unit of measurement, the input/output format is "degree"

def getTiltDegree(prev_angleX, prev_angleY, prev_angleZ):
    angleX, angleY, angleZ = 0, 0, 0
    ge = getG()
    angleX = math.degrees(math.atan2(-ge[0], ge[2]))
    angleY = math.degrees(math.atan2(ge[1], math.sqrt(ge[0]**2 + ge[2]**2)))
    Z = getAngle()
    angleZ = prev_angleZ + Z * 5.0
    # read and process data
    return angleX, angleY, angleZ



def getG():
    # Read the X, Y, Z axis acceleration values and print them.
    x, y, z = accel.read()
    #print('X={0}, Y={1}, Z={2}'.format(x, y, z))

    x_2g = x / 256.0
    y_2g = y / 256.0
    z_2g = z / 256.0
    return x_2g, y_2g, z_2g
    #print('X={0}, Y={1}, Z={2}'.format(x_2g, y_2g, z_2g))
    # Wait half a second and repeat.







