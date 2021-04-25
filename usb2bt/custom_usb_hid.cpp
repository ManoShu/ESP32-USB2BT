#include "custom_usb_hid.hpp"

#include <usbhub.h>

#include "HIDSelector.hpp"
//#include "MouseReporter.hpp"

#include "bt_reporter.hpp"

#define KBD_ADDR 1
#define KBD_IF 0

CustomUSBHID USB_HID;

USB Usb;
//USBHub          Hub(&Usb);
HIDSelector hidSelector(&Usb);

//HIDBoot < USB_HID_PROTOCOL_KEYBOARD | USB_HID_PROTOCOL_MOUSE > HidComposite(&Usb);
//HIDBoot<USB_HID_PROTOCOL_MOUSE>    HidMouse(&Usb);

//MouseRptParser                               MousePrs;

int CustomUSBHID::Init()
{
  auto result = Usb.Init();
  
  //HidMouse.SetReportParser(0, &MousePrs);
  //HidComposite.SetReportParser(0, &MousePrs);
  //HidMouse.SetReportParser(0, &MousePrs);

  return result;
}

void CustomUSBHID::Update()
{
  Usb.Task();
}

void CustomUSBHID::SetLEDs(uint8_t leds)
{
  auto res = hidSelector.SetReport(0, 0 /*hid->GetIface()*/, 2, 0, 1, &leds);
  Serial.print("LEDs: 0x");
  Serial.println(leds, BIN);
  if (res != 0)
  {
    Serial.print("[!!!!!!!] SetReport returned ");
    Serial.print(res, HEX);
    Serial.print(" for lock key state ");
    Serial.println(leds, HEX);
  }
}

void CustomUSBHID::SendReport(uint8_t len, uint8_t *buf)
{
  //LED status
  if (len == 2 && buf[0] == 0x1)
  {
    uint8_t lockLeds = (uint8_t)buf[1]; // added

    this->SetLEDs(lockLeds);
  }
  //else
  {
    //Serial.print("[!!!!!!]Unknown message: ");
    Serial.print("Report message: ");
    Serial.print(HIDReporter::REPORT_HEADER_CODE, HEX);
    Serial.print(" ");
    Serial.print(len, HEX);
    Serial.print(" ");
    for (size_t i = 0; i < len; i++)
    {
      Serial.print(buf[i], HEX);
      if (i < len - 1)
      {
        Serial.print(" ");
      }
    }
    Serial.println();
  }
}
