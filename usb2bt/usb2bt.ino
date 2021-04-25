#include "bt_comm.hpp"
#include "custom_usb_hid.hpp"

#define BAUD_RATE 57600

void setup()
{
  Serial.begin( BAUD_RATE );
#if !defined(__MIPSEL__)
  while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif

 

  Serial.print("Starting USB-HID...");

  if (USB_HID.Init() == -1)
  {
    Serial.println("USB-HID didn't started...");
  }
  else
  {
    Serial.println("OK!");
  }

   Serial.print("Starting HID-Bluetooth...");

  Bluetooth.Init(BAUD_RATE);

  Serial.println("OK!");


  delay( 200 );
}

void loop()
{
  USB_HID.Update();

  Bluetooth.CheckReports();
}
