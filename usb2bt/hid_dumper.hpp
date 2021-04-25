#ifndef HID_DUMPER_HPP
#define HID_DUMPER_HPP

#include "Arduino.h"

template <class CLASS_TYPE, class LEN_TYPE, class OFFSET_TYPE>
class HIDDumper : public CLASS_TYPE
{
    uint8_t byteCount;
    OFFSET_TYPE byteTotal;

public:
   HIDDumper() : byteCount(0), byteTotal(0) {
    };

    void Initialize() {
      byteCount = 0;
      byteTotal = 0;
    };

    void Parse(const LEN_TYPE len, const uint8_t *pbuf, const OFFSET_TYPE &offset);

    uint8_t *data = NULL;
    size_t data_length = 0;
};

template <class CLASS_TYPE, class LEN_TYPE, class OFFSET_TYPE>
void HIDDumper<CLASS_TYPE, LEN_TYPE, OFFSET_TYPE>::Parse(const LEN_TYPE len, const uint8_t *pbuf, const OFFSET_TYPE &offset __attribute__((unused)))
{
    this->data_length = len;
    this->data = new uint8_t[len];

    for (LEN_TYPE j = 0; j < len; j++)
    {
        this->data[j] = pbuf[j];
    }
}

#endif