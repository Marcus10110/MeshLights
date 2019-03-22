#include "StateSerialization.h"
#include "Arduino.h"
#include "ArduinoJson.h"
#include "State.h"
#include <EEPROM.h>

static const uint16_t gMaxStringLength = 300;
static const uint16_t gStringMarker = 0x0F5B; // revved when adding mConnectionMode.
static const uint16_t gStateAddress = 0;

StateSerialization::StateSerialization()
{
}

void StateSerialization::Init()
{
    EEPROM.begin( 1024 );
}

void StateSerialization::SerializeState( const State* state )
{
    // estimated json size: 150.
    StaticJsonBuffer<250> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root[ "mDeviceType" ] = ( int )state->mDeviceType;
    root[ "mLedCount" ] = state->mLedCount;
    root[ "mBrightness" ] = state->mBrightness;
    root[ "mGroup" ] = state->mGroup;
    root[ "mDeviceName" ] = state->mDeviceName;
    root[ "mConnectionMode" ] = ( int )state->mConnectionMode;

    String write_buffer;
    root.printTo( write_buffer );
    WriteString( gStateAddress, write_buffer );
    EEPROM.commit();
}

bool StateSerialization::DeserializeState( State* state )
{
    String readback_string;
    if( !ReadString( gStateAddress, readback_string ) )
    {
        Serial.println( "Serialization read failure: !ReadString" );
        return false;
    }
    StaticJsonBuffer<250> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject( readback_string );
    if( !root.success() )
    {
        Serial.println( "json parse failed (initial)" );
        Serial.println( "loaded string:" );
        Serial.println( readback_string );
        return false;
    }
    int device_type_int = root[ "mDeviceType" ];
    state->mDeviceType = ( device_type_t )device_type_int;
    state->mLedCount = root[ "mLedCount" ];
    state->mBrightness = root[ "mBrightness" ];
    state->mGroup = root[ "mGroup" ];
    const char* device_name = root[ "mDeviceName" ];
    if( ( device_name == nullptr ) || ( strlen( device_name ) > MAX_NAME_LENGTH ) )
    {
        Serial.println( "device name read failure" );
        return false;
    }
    strcpy( state->mDeviceName, device_name );
    int connection_mode_int = root[ "mConnectionMode" ];
    state->mConnectionMode = ( connection_mode_t )connection_mode_int;

    bool json_success = root.success();
    if( !json_success )
    {
        Serial.println( "json read failure (final)" );
    }
    return json_success;
}

void StateSerialization::TestEEPROM()
{
    String test_string = "mark was here";
    WriteString( 0, test_string );
    String readback_string;
    bool success = ReadString( 0, readback_string );
    if( success )
        Serial.println( "ReadString returned true" );
    else
        Serial.println( "ReadString returned false" );

    if( test_string == readback_string )
    {
        Serial.println( "EEPROM test passed!" );
    }
    else
    {
        Serial.println( "EEROM test failed!" );
        Serial.println( "test string:" );
        Serial.println( test_string );
        Serial.println( "readback string:" );
        Serial.println( readback_string );
        Serial.print( "src length: " );
        Serial.println( test_string.length() );
        Serial.print( "readback length: " );
        Serial.println( readback_string.length() );
    }
}

void StateSerialization::TestSerialization()
{
    State ref_state;
    ref_state.mDeviceType = device_type_t::DIMMER;
    ref_state.mLedCount = 5;
    ref_state.mBrightness = 57;
    ref_state.mGroup = 9;
    const char* ref_name = "markwashere";
    strcpy( ref_state.mDeviceName, ref_name );


    State target_state;

    SerializeState( &ref_state );
    bool success = DeserializeState( &target_state );
    Serial.println( "starting serialization test..." );
    Assert( success == true, "result was failure" );
    Assert( ref_state.mDeviceType == target_state.mDeviceType, "mDeviceType missmatch" );
    Assert( ref_state.mLedCount == target_state.mLedCount, "mLedCount missmatch" );
    Assert( ref_state.mBrightness == target_state.mBrightness, "mBrightness missmatch" );
    Assert( ref_state.mGroup == target_state.mGroup, "mGroup missmatch" );
    Assert( ref_state.mGroup == ( target_state.mGroup + 1 ), "testing group missmatch" );
    Assert( strcmp( ref_state.mDeviceName, target_state.mDeviceName ) == 0, "mDeviceName missmatch" );
    Assert( ref_state.mConnectionMode == target_state.mConnectionMode, "mConnectionMode missmatch" );
    Serial.println( "done with serialization test" );
}

uint16_t StateSerialization::WriteString( uint16_t location, const String& contents )
{
    // put a marker at the front of the string, indicating a valid string.
    EEPROM.put( location, gStringMarker );
    location += sizeof( gStringMarker );

    // write the string length
    uint16_t string_length = contents.length() + 1; // including null terminator
    EEPROM.put( location, string_length );
    location += sizeof( string_length );

    // write the string itself.
    const char* str = contents.c_str();

    for( uint16_t i = 0; i < string_length; ++i )
    {
        EEPROM.write( location, str[ i ] );
        location += sizeof( char );
    }
}

bool StateSerialization::ReadString( uint16_t location, String& buffer )
{
    // verify marker.
    uint16_t marker_readback;
    EEPROM.get( location, marker_readback );
    if( marker_readback != gStringMarker )
        return false;
    location += sizeof( marker_readback );

    // read the string length.
    uint16_t string_length;
    EEPROM.get( location, string_length );
    location += sizeof( string_length );
    if( string_length > gMaxStringLength )
    {
        Serial.println( "string length error" );
        Serial.println( string_length );
        return false;
    }

    // read the string.
    buffer.reserve( string_length );
    for( uint16_t i = 0; i < string_length; ++i )
    {
        char read_char = EEPROM.read( location );
        if( i == string_length - 1 )
        {
            if( read_char != '\0' )
            {
                Serial.println( "read_char != '\\0'" );
                return false;
            }
            return true; // don't append null.
        }
        buffer.concat( read_char );
        location += sizeof( char );
    }

    return true;
}

void Assert( bool assertion, const char* message )
{
    if( assertion )
        return;
    Serial.print( "ASSERT: " );
    Serial.println( message );
}
