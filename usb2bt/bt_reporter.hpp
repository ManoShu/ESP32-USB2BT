#ifndef BT_REPORTER_HPP
#define BT_REPORTER_HPP

#include <Arduino.h>

class HIDReporter
{
  public:
    const static uint8_t REPORT_HEADER_CODE = 0xFE;
    const static size_t BUFFER_SIZE = 50;
    
    void Update(int data,void (*messageReceived)(uint8_t, uint8_t*));
    bool HasMessage();
    uint8_t GetMessageLength();
    uint8_t* GetMessage();
    
  private:
    uint8_t buffer[BUFFER_SIZE];
    size_t buffer_index = 0;
    size_t messageSize = -1;

    void clearBuffer();

};

extern HIDReporter BTReporter;

#endif
