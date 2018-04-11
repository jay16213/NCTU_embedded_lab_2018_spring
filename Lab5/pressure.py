import Adafruit_BMP.BMP085 as BMP085

sensor = BMP085.BMP085()
def getPressure():
    # print('Temp = {0:0.2f} *C'.format(sensor.read_temperature()))
    # print('Pressure = {0:0.2f} Pa'.format(sensor.read_pressure()))
    # print('Altitude = {0:0.2f} m'.format(sensor.read_altitude()))
    # print('Sealevel Pressure = {0:0.2f} Pa'.format(sensor.read_sealevel_pressure()))
    temp = sensor.read_temperature()
    pressure = sensor.read_pressure()
    altitude = sensor.read_altitude()
    sealevel_pressure = sensor.read_sealevel_pressure()
    return temp, pressure, altitude, sealevel_pressure
