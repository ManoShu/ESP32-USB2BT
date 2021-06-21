A version of [juancgarcia's HID-Relay](https://github.com/juancgarcia/HID-Relay) using a ESP32 instead of a modified HC-06 Bluetooth transciever.

Advantage would be no need for hacking components, trading off a larger footprint.

# Parts required

The "receiver" is for your usual Pro Mini + Mini UHS combo

The "transmitter" is for a ESP32 board (in my case board ID "mhetesp32devkit")

**As the Pro Mini only have one HW serial port, it's being used to communicate with the ESP32, so no Serial.print debugging**

# UNSTABLE! 
As it is, it's losing connection and not recovering. Also, not connecting when turned on after it was paired before.

It's more of a proof of concept at this point.

# TODO
* Wiring diagram (it's simple but it's good to have reference)
* Stabilize connection
* Set BT to auto reconnect
