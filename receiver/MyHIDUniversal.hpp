#pragma once

#include <usbhid.h>
#include <hiduniversal.h>
#include <hidescriptorparser.h>
#include <usbhub.h>

#include "Debugger.hpp"
#include "DescriptorDumper.hpp"

class MyHIDUniversal : public HIDUniversal
{
public:
    MyHIDUniversal(USB *usb) : HIDUniversal(usb){};

    DescriptorDumper<USBReadParser, uint16_t, uint16_t> Dumper;

protected:
    uint8_t OnInitSuccessful();
};

uint8_t MyHIDUniversal::OnInitSuccessful()
{
    uint8_t rcode;

    if ((rcode = GetReportDescr(0, &Dumper)))
        goto FailGetReportDescr1;

    return 0;

FailGetReportDescr1:
    USBTRACE("GetReportDescr1:");
    goto Fail;

Fail:
    DBG_PRINTLNFMT(rcode, HEX);
    Release();
    return rcode;
}