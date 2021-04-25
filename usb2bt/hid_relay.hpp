#ifndef HID_RELAY_HPP
#define HID_RELAY_HPP

#include <Arduino.h>

class HIDRelay 
{
  public:
    void HandleKeyboard(uint8_t len, uint8_t *buf);
    void HandleMouse(uint8_t len, uint8_t *buf);
    void HandleMedia(uint8_t len, uint8_t *buf);
    void HandleCommand(uint8_t len, uint8_t *buf);
    void HandleRawHIDData(uint8_t ep, bool is_rpt_id, uint8_t len, uint8_t *buf);
    void HandleHIDReport(uint8_t len, uint8_t *buf);

  private:
  
  
};

extern HIDRelay Relay;

#endif
