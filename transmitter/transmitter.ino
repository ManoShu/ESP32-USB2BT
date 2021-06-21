#include "Debugger.hpp"

#include "SerialComm.hpp"
#include "BTComm.hpp"

unsigned long lastTime;
int ledState = LOW;

void setup()
{
    DBG_INIT(115200);
    pinMode(LED_BUILTIN, OUTPUT);

    digitalWrite(LED_BUILTIN, ledState);

    SerialComm::SetDescriptorCallback(BTComm::Init);
    SerialComm::SetReportCallback(BTComm::SendReport);
    BTComm::SetLedCallback(SerialComm::SendLeds);

    SerialComm::Init();

    lastTime = millis();
    DBG_PRINTLN("Trasmitter OK");
}

void loop()
{
    SerialComm::Loop();

    // unsigned long timeNow = millis();
    // if (timeNow - lastTime >= 500)
    // {
    //     lastTime = timeNow;
    //     if (ledState == LOW)
    //     {
    //         ledState = HIGH;
    //     }
    //     else
    //     {
    //         ledState = LOW;
    //     }

    //     digitalWrite(LED_BUILTIN, ledState);
    // }
}