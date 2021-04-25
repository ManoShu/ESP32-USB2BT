#include "bt_reporter.hpp"

HIDReporter BTReporter;

void HIDReporter::clearBuffer()
{
  messageSize = -1;
  buffer_index = 0;

  for (size_t i = 0; i < BUFFER_SIZE; i++)
  {
    buffer[i] = 0;
  }
}

void HIDReporter::Update(int data, void (*messageReceived)(uint8_t, uint8_t*))
{
  if (data == REPORT_HEADER_CODE)
  {
    if (buffer_index > 0)
    {
      Serial.print("[!!!!!] Discarded: ");
      for (size_t i = 0; i < buffer_index; i++)
      {
        Serial.print(buffer[i], HEX);
        if (i < buffer_index - 1)
        {
          Serial.print(" ");
        }
      }
      Serial.println();
    }
    clearBuffer();
  }
  else if (messageSize == -1)
  {
    messageSize = data;
  }
  else
  {
    buffer[buffer_index++] = data;
  }

  if (messageSize == buffer_index)
  {
    messageReceived(messageSize, (uint8_t*)&buffer);

    clearBuffer();
  }
}

bool HIDReporter::HasMessage()
{
  return messageSize == buffer_index;
}

uint8_t HIDReporter::GetMessageLength()
{
  return messageSize + 2;
}

uint8_t* HIDReporter::GetMessage()
{
  return (uint8_t*)&buffer;
}
