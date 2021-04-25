#ifndef BT_COMM_HPP
#define BT_COMM_HPP

#include <Arduino.h>

class BTComm 
{
  public:
    void Init(long speed);
    void SendKeyboard(uint8_t len, uint8_t *buf);
    void SendMouse(int8_t btn, int8_t x, int8_t y, int8_t scroll);
    void SendMedia(uint8_t lowByte, uint8_t highByte);
    void CheckReports();

  //private:
    void SendByte(uint8_t byte);
    void SendSByte(int8_t byte);

    static void ReportMessageReceived(uint8_t len, uint8_t* buf);
  
  private:
    static void bluetoothTask(void*);
};

extern BTComm Bluetooth;

#endif
