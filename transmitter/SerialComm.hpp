#pragma once

#include <SimpleSerialProtocol.h>

// inintialize hardware constants
const long BAUDRATE = 9600; // speed of serial connection
const long CHARACTER_TIMEOUT = 500; // wait max 500 ms between single chars to be received

// initialize command constants
const byte COMMAND_ID_RECEIVE_DESCRIPTOR = 'd';
const byte COMMAND_ID_RECEIVE_REPORT = 'r';
const byte COMMAND_ID_SEND_LEDS = 'l';


typedef void (*bufLenFuncPtr)(uint8_t* , size_t);

//Wish I could put these inside the class
static bufLenFuncPtr  _CallBackReportPointer;
static bufLenFuncPtr  _CallBackDescriptorPointer;

class SerialComm
{
  private:
    static void DescriptorReceived();
    static void ReportReceived();

    

  public:
    static void Init();
    static void Loop();
    static void SendLeds(uint8_t leds);
    static void onError(uint8_t errorNum);

    static void SetDescriptorCallback(bufLenFuncPtr CallBackFuncitonPointer);
    static void SetReportCallback(bufLenFuncPtr CallBackFuncitonPointer);

};

//extern SerialComm serialComm;
