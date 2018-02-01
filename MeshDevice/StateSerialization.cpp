#include "StateSerialization.h"
#include <EEPROM.h>
#include "Arduino.h"

StateSerialization::StateSerialization()
{
    EEPROM.begin(4096);
}

void StateSerialization::SerializeState(const State& state)
{
    uint16_t offset = 0;
    uint16_t device_type_location = GetVariableSize("mDeviceType", state.mDeviceType);
    offset += device_type_location;
    uint16_t device_type_location = GetVariableSize("mLedCount", state.mLedCount) + offset;
    offset += device_type_location;
    uint16_t device_type_location = GetVariableSize("mBrightness", state.mBrightness) + offset;
    offset += device_type_location;
    uint16_t device_type_location = GetVariableSize("mGroup", state.mGroup) + offset;
    offset += device_type_location;

    // device_type_t mDeviceType{device_type_t::LED};
	// uint16_t mLedCount{20};
	// uint8_t mBrightness{255};
	// uint8_t mGroup{0};
	// char mDeviceName[MAX_NAME_LENGTH+1] = "untitled";
}

bool StateSerialization::DeserializeState(State& state)
{
    return true;
}

uint16_t StateSerialization::WriteVariable(const String& name, 
                                        uint16_t location, 
                                        const uint8_t* data, 
                                        uint16_t count)
{
    //returns the size of the data written.
    uint16_t total_length = 0;

    //write name
    uint16_t string_length = name.length() + 1;
    WriteBytes(location, (uint8_t*)name.c_str(), string_length);
    total_length += string_length;
    location += string_length;

    //write data.
    WriteBytes(location, data, count);
    total_length += count;
    location += count;

    //terminate with a null for fun
    EEPROM.write(location, 0);
    total_length += 1;
    location += 1;

    EEPROM.commit();
    return total_length;
}

bool StateSerialization::ReadVariable(const String& name, 
                                        uint16_t location, 
                                        uint8_t* read_buffer, 
                                        uint16_t count)
{
    //read name length and verify.
    uint16_t string_length = name.length() + 1;
    //to avoid dynamic allocation, lets verify the string matches one character at a time.
    for(uint16_t i = 0;i < string_length; ++i)
    {
        if( EEPROM.read( location + i ) != name.c_str()[i] )
            return false;
    }
    location += string_length;

    //read contents:
    ReadBytes(location, read_buffer, count);
    location += count;

    //verify null termination, for fun.
    if(EEPROM.read(location)!= 0x00)
        return false;

    return true;
}

void StateSerialization::WriteBytes(uint16_t location, uint8_t* data, uint16_t length)
{
    for(uint16_t i = 0; i < length; ++i)
        EEPROM.write(location + i, data[i]);
}

void StateSerialization::ReadBytes(uint16_t location, uint8_t* buffer, uint16_t count)
{
    for(uint16_t i = 0; i < length; ++i)
        buffer[i] = EEPROM.read(location + i);
}

uint16_t StateSerialization::GetVariableSizeBytes(const String& name, const uint8_t* data, uint16_t count)
{
    uint16_t total_length = 0;

    //write name
    uint16_t string_length = name.length() + 1;
    total_length += string_length;

    //write data.
    total_length += count;


    //terminate with a null for fun
    total_length += 1;

    return total_length;
}