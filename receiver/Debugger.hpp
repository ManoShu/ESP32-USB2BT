#pragma once

#define DEBUGGER_ACTIVE false

#define DBG_INIT(x) if(DEBUGGER_ACTIVE) Serial.begin(x)

#define DBG_PRINT(x) if(DEBUGGER_ACTIVE) Serial.print(x)
#define DBG_PRINTFMT(x, y) if(DEBUGGER_ACTIVE) Serial.print(x, y)

#define DBG_PRINTLN(x) if(DEBUGGER_ACTIVE) Serial.println(x)
#define DBG_PRINTLNFMT(x, y) if(DEBUGGER_ACTIVE) Serial.println(x, y)