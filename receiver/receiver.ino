#include "Debugger.hpp"
#include "USBComm.hpp"
#include "BTComm.hpp"

unsigned long lastPass;
bool ledPass = false;

void setup() 
{
  DBG_INIT(9600);

  DBG_PRINTLN("Receiver BEGIN");

  delay(1000); //to wait for the transmitter to be ready first

  pinMode(3, OUTPUT);
  
  USBComm::SetDescriptorCallback(BTComm::Init);
  USBComm::SetReportCallback(BTComm::SendReport);
  BTComm::SetLedCallback(USBComm::ReceiveLEDs);
  USBComm::Init();
  
  DBG_PRINTLN("Receiver Setup done");

  lastPass = millis();
}

void loop() 
{
  USBComm::Loop();
  BTComm::Loop();

  if(!ledPass && millis() - lastPass > 10000)
  {
    ledPass = true;
    digitalWrite(3, HIGH);
  }

}
