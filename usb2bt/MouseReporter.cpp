#include "MouseReporter.hpp"

#include "hid_relay.hpp"

void MouseRptParser::UpdateMouse(MOUSEINFO* mi)
{
  //Serial.println("MouseUpdate");

  int8_t buttons =
    mi->bmLeftButton << 0 |
    mi->bmRightButton << 1 |
    mi->bmMiddleButton << 2;

  int8_t x = mi->dX;
  int8_t y = mi->dY;

  uint8_t data[6];
  data[0] = 1; // dummy
  data[1] = buttons;
  data[2] = x;
  data[3] = y;
  data[4] = 0; // Z?
  data[5] = 0; //scroll

  Serial.print("INFO X ");
  Serial.print(x);
  Serial.print(" Y ");
  Serial.println(y);

  //Relay.HandleMouse(6, (uint8_t*)&data);
}

void MouseRptParser::OnMouseMove(MOUSEINFO *mi)
{
  UpdateMouse(mi);
};

void MouseRptParser::OnLeftButtonUp  (MOUSEINFO *mi)
{
  UpdateMouse(mi);
};

void MouseRptParser::OnLeftButtonDown (MOUSEINFO *mi)
{
  UpdateMouse(mi);
};
void MouseRptParser::OnRightButtonUp  (MOUSEINFO *mi)
{
  UpdateMouse(mi);
};
void MouseRptParser::OnRightButtonDown  (MOUSEINFO *mi)
{
  UpdateMouse(mi);
};
void MouseRptParser::OnMiddleButtonUp (MOUSEINFO *mi)
{
  UpdateMouse(mi);
};
void MouseRptParser::OnMiddleButtonDown (MOUSEINFO *mi)
{
  UpdateMouse(mi);
};
