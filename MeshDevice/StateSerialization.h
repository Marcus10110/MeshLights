#pragma once
#include "Arduino.h"
#include "State.h"

class StateSerialization
{
    public:
        StateSerialization();
        void SerializeState(const State* state);
        bool DeserializeState(State* state);
        void TestEEPROM();
    private:
        uint16_t WriteString(uint16_t location, const String& contents);
        bool ReadString(uint16_t location, String& buffer);
};
