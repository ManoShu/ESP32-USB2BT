#include "bt_comm.hpp"

#include <Arduino.h>
#include "BLEDevice.h"
#include "BLEHIDDevice.h"
#include "HIDTypes.h"
#include "HIDKeyboardTypes.h"

#include "custom_usb_hid.hpp"
#include "bt_reporter.hpp"
#include "hid_relay.hpp"

#define DEVICE_NAME "USB-2-BT-ESP32 (MNS-0)"
#define MANUFACTURER "Maker Community"

#define KEYBOARD_COUNTRY 0x00

#define KEYBOARD_PHYSICAL_LAYOUT 0x65 // US // 0x68 // ABNT Brazil

#define MODIFIER_ALT 0b100
#define MODIFIER_SHIFT 0b010
#define MODIFIER_CTRL 0b001

BTComm Bluetooth;

bool isBleConnected = false;

// Message (report) sent when a key is pressed or released
struct InputReport
{
  uint8_t modifiers;      // bitmask: CTRL = 1, SHIFT = 2, ALT = 4
  uint8_t reserved;       // must be 0
  uint8_t pressedKeys[6]; // up to six concurrenlty pressed keys
};

// Message (report) received when an LED's state changed
struct OutputReport
{
  uint8_t leds; // bitmask: num lock = 1, caps lock = 2, scroll lock = 4, compose = 8, kana = 16
};

// The report map describes the HID device (a keyboard in this case) and
// the messages (reports in HID terms) sent and received.
static const uint8_t REPORT_MAP[] = {
    USAGE_PAGE(1), 0x01,      // Generic Desktop Controls
    USAGE(1), 0x06,           // Keyboard
    COLLECTION(1), 0x01,      // Application
    REPORT_ID(1), 0x01,       //   Report ID (1)
    USAGE_PAGE(1), 0x07,      //   Keyboard/Keypad
    USAGE_MINIMUM(1), 0xE0,   //   Keyboard Left Control
    USAGE_MAXIMUM(1), 0xE7,   //   Keyboard Right Control
    LOGICAL_MINIMUM(1), 0x00, //   Each bit is either 0 or 1
    LOGICAL_MAXIMUM(1), 0x01,
    REPORT_COUNT(1), 0x08, //   8 bits for the modifier keys
    REPORT_SIZE(1), 0x01,
    HIDINPUT(1), 0x02,     //   Data, Var, Abs
    REPORT_COUNT(1), 0x01, //   1 byte (unused)
    REPORT_SIZE(1), 0x08,
    HIDINPUT(1), 0x01,     //   Const, Array, Abs
    REPORT_COUNT(1), 0x06, //   6 bytes (for up to 6 concurrently pressed keys)
    REPORT_SIZE(1), 0x08,
    LOGICAL_MINIMUM(1), 0x00,
    LOGICAL_MAXIMUM(1), KEYBOARD_PHYSICAL_LAYOUT, //   how many keys
    USAGE_MINIMUM(1), 0x00,
    USAGE_MAXIMUM(1), 0x65,
    HIDINPUT(1), 0x00,     //   Data, Array, Abs
    REPORT_COUNT(1), 0x05, //   5 bits (Num lock, Caps lock, Scroll lock, Compose, Kana)
    REPORT_SIZE(1), 0x01,
    USAGE_PAGE(1), 0x08,    //   LEDs
    USAGE_MINIMUM(1), 0x01, //   Num Lock
    USAGE_MAXIMUM(1), 0x05, //   Kana
    LOGICAL_MINIMUM(1), 0x00,
    LOGICAL_MAXIMUM(1), 0x01,
    HIDOUTPUT(1), 0x02,    //   Data, Var, Abs
    REPORT_COUNT(1), 0x01, //   3 bits (Padding)
    REPORT_SIZE(1), 0x03,
    HIDOUTPUT(1), 0x01, //   Const, Array, Abs
    END_COLLECTION(0)   // End application collection
};

BLEHIDDevice *hid;
BLECharacteristic *input;
BLECharacteristic *output;

const InputReport NO_KEY_PRESSED = {};

void BTComm::Init(long speed)
{
  //btSerial.begin(speed);
  // start Bluetooth task
  xTaskCreate(BTComm::bluetoothTask, "bluetooth", 20000, NULL, 5, NULL);
}

/*
 * Callbacks related to BLE connection
 */
class BleKeyboardCallbacks : public BLEServerCallbacks
{

  void onConnect(BLEServer *server)
  {
    isBleConnected = true;

    // Allow notifications for characteristics
    BLE2902 *cccDesc = (BLE2902 *)input->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
    cccDesc->setNotifications(true);

    Serial.println("Client has connected");
  }

  void onDisconnect(BLEServer *server)
  {
    isBleConnected = false;

    // Disallow notifications for characteristics
    BLE2902 *cccDesc = (BLE2902 *)input->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
    cccDesc->setNotifications(false);

    Serial.println("Client has disconnected");
  }
};

/*
 * Called when the client (computer, smart phone) wants to turn on or off
 * the LEDs in the keyboard.
 * 
 * bit 0 - NUM LOCK
 * bit 1 - CAPS LOCK
 * bit 2 - SCROLL LOCK
 */
class OutputCallbacks : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *characteristic)
  {
    OutputReport *report = (OutputReport *)characteristic->getData();
    USB_HID.SetLEDs((uint8_t)report->leds);
    // Serial.print("LED state: ");
    // Serial.print((int)report->leds);
    // Serial.println();
  }
};

void BTComm::bluetoothTask(void *)
{

  // initialize the device
  BLEDevice::init(DEVICE_NAME);
  BLEServer *server = BLEDevice::createServer();
  server->setCallbacks(new BleKeyboardCallbacks());

  // create an HID device
  hid = new BLEHIDDevice(server);
  input = hid->inputReport(1);   // report ID
  output = hid->outputReport(1); // report ID
  output->setCallbacks(new OutputCallbacks());

  // set manufacturer name
  hid->manufacturer()->setValue(MANUFACTURER);
  // set USB vendor and product ID
  hid->pnp(0x02, 0xe502, 0xa111, 0x0210);
  // information about HID device: device is not localized, device can be connected
  hid->hidInfo(KEYBOARD_COUNTRY, 0x02);

  // Security: device requires bonding
  BLESecurity *security = new BLESecurity();
  security->setAuthenticationMode(ESP_LE_AUTH_BOND);

  // set report map
  hid->reportMap((uint8_t *)REPORT_MAP, sizeof(REPORT_MAP));
  hid->startServices();

  // set battery level to 100%
  hid->setBatteryLevel(100);

  // advertise the services
  BLEAdvertising *advertising = server->getAdvertising();
  advertising->setAppearance(HID_KEYBOARD);
  advertising->addServiceUUID(hid->hidService()->getUUID());
  advertising->addServiceUUID(hid->deviceInfo()->getUUID());
  advertising->addServiceUUID(hid->batteryService()->getUUID());
  advertising->start();

  Serial.println("BLE ready");
  delay(portMAX_DELAY);
};

void BTComm::SendKeyboard(uint8_t len, uint8_t *buf)
{
  auto needToTriggerTheDamnKey = false;
  for (size_t i = 0; i < len; i++)
  {
    auto theByte = (uint8_t)buf[i];

    if (theByte == 135)
    {
      uint8_t mod = (uint8_t)buf[0];

      auto leftShiftPressed = mod == 2;
      auto rightShiftPressed = mod == 32;

      if (leftShiftPressed || rightShiftPressed)
      {
        needToTriggerTheDamnKey = true;
        buf[i] = 0;
      }
    }
  }

  if (needToTriggerTheDamnKey)
  {
    // create input report
    InputReport reportQuestion = {
        .modifiers = 5, //Ctrl + alt,
        .reserved = 0,
        .pressedKeys = {
            26, //W
            0, 0, 0, 0, 0}};

    // send the input report
    input->setValue((uint8_t *)&reportQuestion, sizeof(reportQuestion));
    input->notify();
  }

  // create input report
  InputReport report = {
      .modifiers = 0,
      .reserved = 0,
      .pressedKeys = {0, 0, 0, 0, 0, 0}};

  for (size_t i = 0; i < len; i++)
  {
    auto theByte = (uint8_t)buf[i];

    if (i == 0)
    {
      report.modifiers = theByte;
    }
    else if (i >= 2)
    {
      //HACK: VERY SPECIAL CASE
      if (theByte == 135 && (report.modifiers & MODIFIER_SHIFT) == 0)
      {
        //key pad slash
        theByte = 84;
      }

      report.pressedKeys[i - 2] = theByte;
    }
  }
  // send the input report
  input->setValue((uint8_t *)&report, sizeof(report));
  input->notify();
}

void BTComm::SendMouse(int8_t btn, int8_t x, int8_t y, int8_t scroll)
{
  SendByte(0xfd);
  SendByte(0x05);
  SendByte(0x02);
  SendSByte(btn);
  SendSByte(x);
  SendSByte(y);
  SendSByte(scroll);
}

void BTComm::SendMedia(uint8_t lowByte, uint8_t highByte)
{
  SendByte(0xfd);
  SendByte(0x03);
  SendByte(0x03);
  SendByte(lowByte);
  SendByte(highByte);
}

void BTComm::SendByte(uint8_t b)
{
  //btSerial.write(b);
}

void BTComm::SendSByte(int8_t b)
{
  //btSerial.write(b);
}

void BTComm::CheckReports()
{
  // while (btSerial.available())
  // {
  //   BTReporter.Update(btSerial.read(), BTComm::ReportMessageReceived);
  // }
}

void BTComm::ReportMessageReceived(uint8_t len, uint8_t *buf)
{
  Relay.HandleHIDReport(len, buf);
}
