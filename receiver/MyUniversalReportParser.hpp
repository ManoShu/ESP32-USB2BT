#pragma once

#include <usbhid.h>
#include <hiduniversal.h>
#include <hidescriptorparser.h>
#include <usbhub.h>

#include <ArduinoQueue.h>

#include "Debugger.hpp"

#define REPORT_BUFFER_SIZE 32
#define REPORT_QUEUE_SIZE 4

typedef struct ReportData
{
    uint8_t buffer[REPORT_BUFFER_SIZE];
    uint8_t len;
};

ArduinoQueue<ReportData> ReportQueue(REPORT_QUEUE_SIZE);

class MyUniversalReportParser : public UniversalReportParser
{
public:

    void Parse(USBHID *hid,
               bool is_rpt_id,
               uint8_t len,
               uint8_t *buf);
};

void MyUniversalReportParser::Parse(USBHID *hid,
                                    bool is_rpt_id,
                                    uint8_t len,
                                    uint8_t *buf)
{
    ReportData data;
    data.len = len;
    for(size_t i = 0; i < len; i++)
    {
        data.buffer[i] = buf[i];
    }
    ReportQueue.enqueue(data);
}
