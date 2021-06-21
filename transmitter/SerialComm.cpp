#include "SerialComm.hpp"

#include "Debugger.hpp"

SimpleSerialProtocol ssp(Serial2, BAUDRATE, CHARACTER_TIMEOUT, SerialComm::onError, 'a', 'z'); // ASCII: 'a' - 'z' (26 byes of RAM is reserved)

void SerialComm::Init()
{

  ssp.init();

  ssp.registerCommand(COMMAND_ID_RECEIVE_DESCRIPTOR, SerialComm::DescriptorReceived);
  ssp.registerCommand(COMMAND_ID_RECEIVE_REPORT, SerialComm::ReportReceived);
}

void SerialComm::Loop()
{
  ssp.loop();
}

void SerialComm::SendLeds(uint8_t leds)
{
  ssp.writeCommand(COMMAND_ID_SEND_LEDS); // start command with command id
  ssp.writeUnsignedInt8(leds);
  ssp.writeEot();
}

void SerialComm::DescriptorReceived()
{
  uint8_t descriptorSize = ssp.readUnsignedInt8();

  uint8_t *descriptor = new uint8_t[descriptorSize];

  for (int i = 0; i < descriptorSize; i++)
  {
    descriptor[i] = ssp.readUnsignedInt8();
  }

  ssp.readEot(); // read and expect the end-of-transmission byte. important, don't forget!

  if (_CallBackDescriptorPointer)
  {
    _CallBackDescriptorPointer(descriptor, descriptorSize);
  }
  delete descriptor;
}
void SerialComm::ReportReceived()
{
  uint8_t reportSize = ssp.readUnsignedInt8();

  uint8_t *report = new uint8_t[reportSize];

  for (int i = 0; i < reportSize; i++)
  {
    report[i] = ssp.readUnsignedInt8();
  }
  ssp.readEot(); // read and expect the end-of-transmission byte. important, don't forget!

  if (_CallBackReportPointer)
  {
    _CallBackReportPointer(report, reportSize);
  }
  delete report;
}
void SerialComm::onError(uint8_t errorNum)
{
  DBG_PRINT("Serial error ");
  DBG_PRINTLN(errorNum);
}

void SerialComm::SetDescriptorCallback(bufLenFuncPtr CallBackFuncitonPointer)
{
  _CallBackDescriptorPointer = CallBackFuncitonPointer;
}

void SerialComm::SetReportCallback(bufLenFuncPtr CallBackFuncitonPointer)
{
  _CallBackReportPointer = CallBackFuncitonPointer;
}
