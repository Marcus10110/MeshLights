#pragma once
#include "Arduino.h"
#include "State.h"

class StateSerialization
{
    public:
        StateSerialization();
        bool SerializeState(const State& state);
        void DeserializeState(State& state);
    private:
        uint16_t WriteVariable(const String& name, uint16_t location, const uint8_t* data, uint16_t count);
        bool ReadVariable(const String& name, uint16_t location, uint8_t* read_buffer, uint16_t count);
        void WriteBytes(uint16_t location, uint8_t* data, uint16_t length);
        void ReadBytes(uint16_t location, uint8_t* buffer, uint16_t count);
        uint16_t GetVariableSizeBytes(const String& name, const uint8_t* data, uint16_t count);
        template <class T>
        uint16_t WriteVariable (const String& name, uint16_t location, const T& data) {
            uint8_t* data_ptr = (uint8_t*)(&data);
            uint16_t count = sizeof(data);
            return WriteVariable(name, location, data_ptr, count );
        }
        template <class T>
        bool ReadVariable (const String& name, uint16_t location, T& data) {
            uint8_t* data_ptr = (uint8_t*)(&data);
            uint16_t count = sizeof(data);
            return ReadVariable(name, location, data_ptr, count );
        }
        template <class T>
        uint16_t GetVariableSize (const String& name, T& data) {
            uint8_t* data_ptr = (uint8_t*)(&data);
            uint16_t count = sizeof(data);
            return GetVariableSizeBytes(name, data_ptr, count );
        }

}


/*

template <class myType>
myType GetMax (myType a, myType b) {
 return (a>b?a:b);
}

*/
