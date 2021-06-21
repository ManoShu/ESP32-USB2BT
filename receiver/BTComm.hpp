#pragma once

#include <SimpleSerialProtocol.h>
//#include <NeoSWSerial.h>
//#include <SoftwareSerial.h>

#include "Debugger.hpp"

// inintialize hardware constants
const long BAUDRATE = 9600;          // speed of serial connection
const long CHARACTER_TIMEOUT = 500;  // wait max 500 ms between single chars to be received

// initialize command constants
const byte COMMAND_ID_RECEIVE_DESCRIPTOR = 'd';
const byte COMMAND_ID_RECEIVE_REPORT = 'r';
const byte COMMAND_ID_SEND_LEDS = 'l';

#define rxPin 4
#define txPin 5
#define COMM_BAUDRATE 9600

typedef void (*ledFuncPtr)(uint8_t);
static ledFuncPtr _ledCallBackFuncitonPointer;

//NeoSWSerial btSerial(rxPin, txPin);
//SoftwareSerial btSerial(rxPin, txPin);

void LEDsReceived();

void onError(uint8_t errorNum) {
  DBG_PRINT("SSP error ");
  DBG_PRINTLN(errorNum);
}

SimpleSerialProtocol ssp(Serial, BAUDRATE, CHARACTER_TIMEOUT, onError, 'a', 'z');  // ASCII: 'a' - 'z' (26 byes of RAM is reserved)

void LEDsReceived() {
  uint8_t leds = ssp.readUnsignedInt8();
  if (_ledCallBackFuncitonPointer != NULL) {
    _ledCallBackFuncitonPointer(leds);
  }
  ssp.readEot();  // read and expect the end-of-transmission byte. important, don't forget!
}


bool sspStarted = false;

class BTComm {
private:
  static void PrintBuffer(const char *name, uint8_t *buf, size_t len) {
    DBG_PRINT("[BT] ");
    DBG_PRINT(name);
    DBG_PRINT(" (");
    DBG_PRINT(len);
    DBG_PRINT("): ");
    for (size_t i = 0; i < len; i++) {
      DBG_PRINTFMT(buf[i], HEX);
      DBG_PRINT(" ");
    }
    DBG_PRINTLN();
  }

public:
  static void Init(uint8_t *description, size_t len) {
    ssp.init();
    ssp.registerCommand(COMMAND_ID_SEND_LEDS, LEDsReceived);

    //PrintBuffer("Descriptor", description, len);

    ssp.writeCommand(COMMAND_ID_RECEIVE_DESCRIPTOR);  // start command with command id
    ssp.writeUnsignedInt8(len);
    for (size_t i = 0; i < len; i++) {
      ssp.writeUnsignedInt8(description[i]);
    }
    ssp.writeEot();

    sspStarted = true;
  }

  static void SendReport(uint8_t *data, size_t len) {
    //PrintBuffer("Report", data, len);
    ssp.writeCommand(COMMAND_ID_RECEIVE_REPORT);  // start command with command id
    ssp.writeUnsignedInt8(len);
    for (size_t i = 0; i < len; i++) {
      ssp.writeUnsignedInt8(data[i]);
    }
    ssp.writeEot();
  }

  static void SetLedCallback(ledFuncPtr func) {
    _ledCallBackFuncitonPointer = func;
  }

  //    static void LEDsReceived()
  //    {
  //        uint8_t leds = ssp.readUnsignedInt8();
  //        if (_ledCallBackFuncitonPointer != NULL)
  //        {
  //            _ledCallBackFuncitonPointer(leds);
  //        }
  //        ssp.readEot(); // read and expect the end-of-transmission byte. important, don't forget!
  //    }

  static void Loop() {
    if (sspStarted) {
      ssp.loop();
    }
  }

private:
};
