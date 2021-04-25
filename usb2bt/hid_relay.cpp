#include "hid_relay.hpp"

#include "bt_comm.hpp"
#include "custom_usb_hid.hpp"

HIDRelay Relay;

struct MOUSEINFO {

  struct {
    uint8_t bmLeftButton : 1;
    uint8_t bmRightButton : 1;
    uint8_t bmMiddleButton : 1;
    uint8_t bmDummy : 5;
  };
  uint8_t id;
  int8_t dX;
  int8_t dY;
};

void HIDRelay::HandleKeyboard(uint8_t len, uint8_t *buf)
{
  Bluetooth.SendKeyboard(len, buf);
}

void printBin(uint8_t var) {
  const size_t bits = 8;
  for (uint8_t test = (1 << (bits - 1)); test; test >>= 1) {
    Serial.write(var  & test ? '1' : '0');
  }
  //Serial.println();
}

void HIDRelay::HandleMouse(uint8_t len, uint8_t *buf)
{
  //  uint8_t byte2 = (uint8_t)buf[2];
  //  uint8_t byte3 = (uint8_t)buf[3];
  //  uint8_t byte4 = (uint8_t)buf[4];
  uint8_t byte2 = (uint8_t)buf[len - 5];
  uint8_t byte3 = (uint8_t)buf[len - 4];
  uint8_t byte4 = (uint8_t)buf[len - 3];

  int leftByte3 = (byte3 & 0b11110000) >> 4;
  int rightByte3 = byte3 & 0b1111;
  int8_t trueX = byte2 | (rightByte3 << 8);
  int8_t trueY = (byte4 << 4) | leftByte3;

  int8_t btn = (int8_t)buf[1];
  //int8_t scr = (int8_t)buf[5];
  int8_t scr = (int8_t)buf[len - 2];

  Bluetooth.SendMouse(btn, trueX, trueY, scr);
}

int mediaKey = 0;

void HIDRelay::HandleMedia(uint8_t len, uint8_t *buf)
{
  auto p1 = (uint8_t)buf[1];
  auto p2 = (uint8_t)buf[2];

  auto knownKey = true;

  uint8_t highByte = 0;
  uint8_t lowByte = 0;

  auto calculator = false;

  //Serial.print("Multimedia: ");

  //play/pause
  if (p1 == 205 && p2 == 0) {
    highByte = 0x00;
    lowByte = 0x80;
    Serial.println("Play/Pause");
  }
  //stop
  else if (p1 == 183 && p2 == 0) {
    highByte = 0x04;
    lowByte = 0x00;
    Serial.println("Stop");
  }
  //previous
  else if (p1 == 182 && p2 == 0) {
    highByte = 0x02;
    lowByte = 0x00;
    Serial.println("Previous");
  }
  //next
  else if (p1 == 181 && p2 == 0) {
    highByte = 0x01;
    lowByte = 0x00;
    Serial.println("Next");
  }
  //mute
  else if (p1 == 226 && p2 == 0) {
    highByte = 0x00;
    lowByte = 0x40;
    Serial.println("Mute");
  }
  //vol up
  else if (p1 == 233 && p2 == 0) {
    highByte = 0x00;
    lowByte = 0x10;
    Serial.println("Volume Up");
  }
  //vol down
  else if (p1 == 234 && p2 == 0) {
    highByte = 0x00;
    lowByte = 0x20;
    Serial.println("Volume Down");
  }
  //media player
  else if (p1 == 131 && p2 == 1) {
    //highByte = 0x08;
    //lowByte = 0x00;
    Serial.println("Media Player");
  }
  //email
  else if (p1 == 138 && p2 == 1) {
    highByte = 0x00;
    lowByte = 0x02;
    Serial.println("Email");
  }
  //calculator
  else if (p1 == 146 && p2 == 1) {
    calculator = true;
    Serial.println("Calculator");
  }
  //my computer
  else if (p1 == 148 && p2 == 1) {
    Serial.println("My Computer");
  }
  //www home page
  else if (p1 == 35 && p2 == 2) {
    highByte = 0x00;
    lowByte = 0x01;
    Serial.println("WWW Home Page");
  }
  //www search
  else if (p1 == 33 && p2 == 2) {
    //highByte = 0x00;
    //lowByte = 0x04;
    Serial.println("WWW Search");
  }
  ///www back
  else if (p1 == 36 && p2 == 2) {
    Serial.println("WWW Back");
  }
  //www forward
  else if (p1 == 37 && p2 == 2) {
    Serial.println("WWW Forward");
  }
  //www stop
  else if (p1 == 38 && p2 == 2) {
    Serial.println("WWW Stop");
  }
  //www refresh
  else if (p1 == 39 && p2 == 2) {
    Serial.println("WWW Refresh");
  }
  //www favorites
  else if (p1 == 42 && p2 == 2) {
    Serial.println("WWW Favorites");
  }
  //key release
  else if (p1 == 0 && p2 == 0) {
    //Serial.println("[Release]");
  }
  else
  {
    knownKey = false;
    Serial.print("[UNKNOWN] ");
    Serial.print(p1);
    Serial.print(", ");
    Serial.println(p2);
  }

  if (knownKey)
  {
    Bluetooth.SendMedia(lowByte, highByte);
  }

  if(calculator)
  {
    mediaKey++;

//    Serial.print("Sending media key ");
//    Serial.print(mediaKey, HEX);
//    Serial.println("...");

    lowByte = 0xc0;
    highByte = 0;

//    if(mediaKey > 255)
//    {
//      highByte = (uint8_t)(mediaKey >> 8);
//      lowByte = (uint8_t)(mediaKey & 0b11111111);
//    }
//    else
//    {
//      lowByte = (uint8_t)mediaKey;
//    }
    
    Bluetooth.SendByte(0xfd);
    Bluetooth.SendByte(0x03);
    Bluetooth.SendByte(0x03);
    Bluetooth.SendByte(lowByte);    
    Bluetooth.SendByte(highByte);
  }
}

void HIDRelay::HandleCommand(uint8_t len, uint8_t *buf)
{
  auto p1 = (uint8_t)buf[1];

  if (p1 ==  1)
  {
    Serial.println("Power Off");
  }
  else if (p1 ==  2)
  {
    Serial.println("Sleep");
  }
  else if (p1 == 0)
  {
    Serial.println("[Release]");
  }
  else
  {
    Serial.print("[UNKNOWN] ");
    Serial.println(p1);
  }
}

void HIDRelay::HandleRawHIDData(uint8_t ep, bool is_rpt_id, uint8_t len, uint8_t *buf)
{
  Serial.print("RAW EP ");
  Serial.print(ep);
  Serial.print(" IS ID ");
  Serial.print(is_rpt_id);
  Serial.print(" Buffer [");

  for (size_t i = 0; i < len; i++) {
    auto current = (uint8_t)buf[i];
    //Serial.print(current, HEX);
    printBin(current);
    if (i < len - 1) Serial.print(" ");
  }
  Serial.println("]");
}

void HIDRelay::HandleHIDReport(uint8_t len, uint8_t *buf)
{
  USB_HID.SendReport(len, buf);
}
