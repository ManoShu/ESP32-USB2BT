#pragma once

#include "Arduino.h"

typedef void (*ledFuncPtr)(uint8_t);
static ledFuncPtr  _ledCallBackFuncitonPointer;

static uint8_t* reportDescriptor;
static size_t reportSize;

class BTComm
{
  private:
    
    static void bluetoothTask(void*);

  public:
    static void Init(uint8_t* desc_data, size_t desc_data_length);
    static void SendReport(uint8_t* buf, size_t len);
    static void SetLedCallback(ledFuncPtr CallBackFuncitonPointer);
};
