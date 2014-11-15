## @package ArduinoSerial
import serial, sys

# TODO: design this to be like the controller class in c
# NOTE: This is meant to act as a base class for devices

# This is a connection class
class Connection():
  ## constructor for the Connection object
  #  @param self
  #     the object pointer
  def __init__(self):
    self.port = ""
    self.baudrate = 0
    self.connection = None

    self.id = ""
    self.inputs = {}
    self.outputs = {}
    self.readbuffer = ""
    self.writebuffer = ""
 
  ## connect to a device, get id, num input, num output
  #  @param self
  #     the object pointer
  #  @param port
  #     the port at which the device is located
  #  @param baudrate
  #     the baudrate of the serial connection
  #  @return
  #     None
  def connect(self, port, baudrate):
    if self.connection:
      self.disconnect()

    self.port = port
    self.baudrate = baudrate
    try:
      self.connection = serial.Serial(port, baudrate)

      # get id
      self.write("info\n")
      info = self.read()
      while not info:
        info = self.read()
      info = eval(info)
      self.id = info["id"]
      self.inputs = info["inputs"]
      self.outputs = info["outputs"]

    except serial.serialutil.SerialException:
      self.connection = None

  ## tries to identify if the device is connected
  #  @param self
  #     the object pointer
  #  @return
  #     boolean representing if connection successful
  def connected(self):
    if not os.path.exists(self.port):
      try:
        self.connection.close()
      except:
        pass
      finally:
        self.connection = None
    return self.connection != None

  ## disconnect the current device
  #  @param self
  #     the object pointer
  #  @return
  #     None
  def disconnect(self):
    if self.connected():
      self.connection.close()
      self.connection = None

  ## try to parse arduino specific data
  #  @param self
  #     the object pointer
  #  @param string
  #     the string of data to be evaluated
  #  @return
  #     positions of start and end on success, or None on error
  def parseData(self, string):
    end = string.find("\n")
    if end == -1:
      return None
    else:
      start = (len(string) - string[:end][::-1].find("\n") - 1) % len(string)
      return [start, end + 1]

  ## try to read in data
  #  @param self
  #     the object pointer
  #  @return
  #     an evaluated datastring
  def read(self):
    if not self.connected():
      return
    byteWait = self.connection.inWaiting()
    if byteWait:
      self.readbuffer += self.connection.read(byteWait)
      # shorten buffer
      if len(self.readbuffer) > 512:
        self.readbuffer = self.readbuffer[-512:]

    pos = self.parseData(self.readbuffer)
    if pos == None:
      return None
    else:
      rawString = self.readbuffer[pos[0]:pos[1]]
      # shorten buffer
      self.readbuffer = self.readbuffer[pos[1]:]
      try:
        string = eval(rawString)
        return string
      except:
        return ""

  ## tries to write to serial
  #  @param self
  #     the object pointer
  #  @param message
  #     the message to send over serial to device
  #  @return
  #     None
  def write(self):
    if not self.connected():
      return
    if message != self.writebuffer:
      self.writebuffer = message
      self.connection.write(self.writebuffer)
      print "Writing: " + str(self.writebuffer)
