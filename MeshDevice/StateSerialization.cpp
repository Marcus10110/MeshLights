#include "StateSerialization.h"
#include <EEPROM.h>
#include "ArduinoJson.h"
#include "Arduino.h"

static const uint16_t gStringMarker = 0x0F5A;
static const uint16_t gStateAddress = 0;

StateSerialization::StateSerialization()
{
    EEPROM.begin(1024);
}

void StateSerialization::SerializeState(const State* state)
{
    // device_type_t mDeviceType{device_type_t::LED};
	// uint16_t mLedCount{20};
	// uint8_t mBrightness{255};
	// uint8_t mGroup{0};
	// char mDeviceName[MAX_NAME_LENGTH+1] = "untitled";
    //estimated json size: 150.
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["mDeviceType"] = (int)state->mDeviceType;
    root["mLedCount"] = state->mLedCount;
    root["mBrightness"] = state->mBrightness;
    root["mGroup"] = state->mGroup;
    root["mDeviceName"] = state->mDeviceName;

    String write_buffer;
    root.printTo(write_buffer);



    Serial.println("serializing state:");
    root.printTo(Serial);
    Serial.println("\ndone");
}

bool StateSerialization::DeserializeState(State* state)
{
    String readback_string;
    if(!ReadString(gStateAddress, readback_string))
    {
        Serial.println("DeserializeState failed - no valid string saved");
    }
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(readback_string);
    int device_type_int = root["mDeviceType"];
    state->mDeviceType = (device_type_t)device_type_int;
    state->mLedCount = root["mLedCount"];
    state->mBrightness = root["mBrightness"];
    state->mGroup = root["mGroup"];
    const char* device_name = root["mDeviceName"];
    if( strlen(device_name) > MAX_NAME_LENGTH)
    {
        return false;
    }
    strcpy(state->mDeviceName, device_name);
    return root.success();
}

void StateSerialization::TestEEPROM()
{
    String test_string = "mark was here";
    WriteString(0, test_string);
    String readback_string;
    bool success = ReadString(0, readback_string);
    if( success )
        Serial.println("ReadString returned true");
    else
        Serial.println("ReadString returned false");

    if( test_string == readback_string)
    {
        Serial.println("EEPROM test passed!");
    }
    else
    {
        Serial.println("EEROM test failed!");
        Serial.println("test string:");
        Serial.println(test_string);
        Serial.println("readback string:");
        Serial.println(readback_string);
        Serial.print("src length: ");
        Serial.println(test_string.length());
        Serial.print("readback length: ");
        Serial.println(readback_string.length());
    }
}

uint16_t StateSerialization::WriteString(uint16_t location, const String& contents)
{
    Serial.print("write marker location: ");
    Serial.println(location);


    //put a marker at the front of the string, indicating a valid string.
    EEPROM.put(location, gStringMarker);
    location += sizeof(gStringMarker);

    Serial.print("write length location: ");
    Serial.println(location);

    //write the string length
    uint16_t string_length = contents.length()+1; //including null terminator
    EEPROM.put(location, string_length);
    location += sizeof(string_length);

    Serial.print("write string location: ");
    Serial.println(location);

    //write the string itself.
    const char* str = contents.c_str();

    for(uint16_t i = 0; i < string_length; ++i )
    {
        EEPROM.write(location, str[i]);
        location += sizeof(char);
    }
}

bool StateSerialization::ReadString(uint16_t location, String& buffer)
{

    Serial.print("read marker location: ");
    Serial.println(location);

    //verify marker.
    uint16_t marker_readback;
    EEPROM.get(location, marker_readback);
    if( marker_readback != gStringMarker )
        return false;
    location += sizeof(marker_readback);

    Serial.print("read length location: ");
    Serial.println(location);
    
    //read the string length.
    uint16_t string_length;
    EEPROM.get(location, string_length);
    location += sizeof(string_length);

    Serial.print("read string location: ");
    Serial.println(location);

    //read the string.
    buffer.reserve(string_length);
    for(uint16_t i = 0; i < string_length; ++i )
    {
        char read_char = EEPROM.read(location);
        if(i == string_length-1)
        {
            if( read_char != '\0' )
                return false;
            return true; //don't append null.
        }
        buffer.concat(read_char);
        location += sizeof(char);
    }

    return true;
}