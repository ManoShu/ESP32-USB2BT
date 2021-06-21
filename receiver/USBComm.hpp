#pragma once

#include <usbhid.h>
#include <hiduniversal.h>
#include <hidescriptorparser.h>
#include <usbhub.h>

#include "Debugger.hpp"
#include "MyHIDUniversal.hpp"
#include "MyUniversalReportParser.hpp"

typedef void (*bufLenFuncPtr)(uint8_t *, size_t);

//Wish I could put these inside the class
static bufLenFuncPtr _CallBackDescriptorPointer;
static bufLenFuncPtr _CallBackReportPointer;
static unsigned long lastTimeDescriptorWasChecked;

USB Usb;
//USBHub Hub(&Usb);
MyHIDUniversal Hid(&Usb);
MyUniversalReportParser Uni;

bool descriptorObtained = false;

class USBComm
{
private:
public:
    static void SetDescriptorCallback(bufLenFuncPtr func)
    {
        _CallBackDescriptorPointer = func;
    }

    static void SetReportCallback(bufLenFuncPtr func)
    {
        _CallBackReportPointer = func;
    }

    static void Init()
    {

        if (Usb.Init() == -1)
            DBG_PRINTLN("OSC did not start.");

        delay(200);

        lastTimeDescriptorWasChecked = millis();

        if (!Hid.SetReportParser(0, &Uni))
            ErrorMessage<uint8_t>(PSTR("SetReportParser"), 1);
    }

    static void CheckForDescriptor()
    {
        if (!descriptorObtained)
        {
            auto timeNow = millis();

            auto delta = timeNow - lastTimeDescriptorWasChecked;
            uint16_t len;

            if ((len = Hid.Dumper.DescriptorReady(delta)) != DESCRIPTOR_NOT_READY)
            {
                descriptorObtained = true;

                if (_CallBackDescriptorPointer != NULL)
                {
                    uint8_t *pBuffer = &(Hid.Dumper.buffer[0]);
                    _CallBackDescriptorPointer(pBuffer, len);
                }
            }
            lastTimeDescriptorWasChecked = timeNow;
        }
    }

    static void CheckForReports()
    {
        while (!ReportQueue.isEmpty())
        {
            auto data = ReportQueue.dequeue();

            if (_CallBackReportPointer != NULL)
            {
                uint8_t *pBuffer = &(data.buffer[0]);
                _CallBackReportPointer(pBuffer, data.len);
            }
        }
    }

    static void Loop()
    {
        Usb.Task();

        CheckForDescriptor();

        CheckForReports();
    }

    static void ReceiveLEDs(uint8_t leds)
    {
        DBG_PRINTLNFMT(leds, HEX);
        auto res = Hid.SetReport(0, 0, 2, 0, 1, &leds);
        //error result
        DBG_PRINTLNFMT(res, HEX);
        if (res != 0)
        {
            DBG_PRINT("[!!!!!!!] SetReport returned ");
            DBG_PRINTFMT(res, HEX);
            DBG_PRINT(" for lock key state ");
            DBG_PRINTLNFMT(leds, HEX);
        }
    }

private:
};
