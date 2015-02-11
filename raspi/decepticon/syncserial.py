import serial
import time
import sys

## NOTE: This is required for the serial.c file! ##

if len(sys.argv) != 3:
  print "Not enough arguments: {0} [port] [baud]\n"
  sys.exit()

s = serial.Serial(sys.argv[1], int(sys.argv[2]))
if s.isOpen():
  for i in range(10):
    s.readline()
  s.close()
else:
  print "Cannot sync to {0}".format(sys.argv[1])
