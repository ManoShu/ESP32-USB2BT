#pragma once

#include "Debugger.hpp"

#define PARSE_TIMEOUT 100 //miliseconds
#define BUFFER_SIZE 255
#define DESCRIPTOR_NOT_READY -1

template <class BASE_CLASS, class LEN_TYPE, class OFFSET_TYPE>
class DescriptorDumper : public HexDumper<BASE_CLASS, LEN_TYPE, OFFSET_TYPE>
{
private:
    OFFSET_TYPE receivedByteTotal = 0;
    bool descriptorStarted = false;
    unsigned long timeSinceLastParse = 0;

public:
    uint8_t buffer[BUFFER_SIZE];
    void Parse(const LEN_TYPE len, const uint8_t *pbuf, const OFFSET_TYPE &offset);
    LEN_TYPE DescriptorReady(const unsigned long delta);
};

template <class BASE_CLASS, class LEN_TYPE, class OFFSET_TYPE>
void DescriptorDumper<BASE_CLASS, LEN_TYPE, OFFSET_TYPE>::Parse(const LEN_TYPE len, const uint8_t *pbuf, const OFFSET_TYPE &offset __attribute__((unused)))
{
    if (UsbDEBUGlvl >= 0x80)
    { // Fully bypass this block of code if we do not debug.
        if (!descriptorStarted)
        {
            descriptorStarted = true;
        }
        for (LEN_TYPE j = 0; j < len; j++, receivedByteTotal++)
        {
            buffer[receivedByteTotal] = pbuf[j];
        }
        timeSinceLastParse = 0;
    }
}
template <class BASE_CLASS, class LEN_TYPE, class OFFSET_TYPE>
LEN_TYPE DescriptorDumper<BASE_CLASS, LEN_TYPE, OFFSET_TYPE>::DescriptorReady(const unsigned long delta)
{
    if (descriptorStarted)
    {
        timeSinceLastParse += delta;
        return (timeSinceLastParse > PARSE_TIMEOUT) ? receivedByteTotal : DESCRIPTOR_NOT_READY;
    }
    return DESCRIPTOR_NOT_READY;
}