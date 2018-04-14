#!/usr/bin/python
import csv

f = open("output.csv", "r")
csvCursor = csv.DictReader(f)

for data in csvCursor:
    pressure = float(data['Pressure(Pa)'])
    altitude = 44330.0 * (1.0 - pow(pressure / 101100.0, (1.0/5.255)))
    print(altitude)
