#define MAX_DIMMER_COUNT 8
#include "CRC32.h"
#include <Dimmer.h>


#define RAMP_TIME 0.1
#define CHANNEL_COUNT 8

Dimmer dimmers[ MAX_DIMMER_COUNT ] = { Dimmer( 3, DIMMER_RAMP, RAMP_TIME ), Dimmer( 4, DIMMER_RAMP, RAMP_TIME ),
                                       Dimmer( 5, DIMMER_RAMP, RAMP_TIME ), Dimmer( 6, DIMMER_RAMP, RAMP_TIME ),
                                       Dimmer( 7, DIMMER_RAMP, RAMP_TIME ), Dimmer( 8, DIMMER_RAMP, RAMP_TIME ),
                                       Dimmer( 9, DIMMER_RAMP, RAMP_TIME ), Dimmer( 10, DIMMER_RAMP, RAMP_TIME ) };

void setup()
{
    // put your setup code here, to run once:
    Serial.begin( 115200 );
    Serial.println( "start" );

    pinMode( 2, INPUT );

    for( int i = 0; i < MAX_DIMMER_COUNT; i++ )
    {
        dimmers[ i ].begin();
    }
    Serial.println( "setup done" );
}

void loop()
{
    // put your main code here, to run repeatedly:
    Serial.println( "top of loop" );
    uint8_t buffer[ MAX_DIMMER_COUNT ];
    for( int16_t i = 0; i < MAX_DIMMER_COUNT; ++i )
        buffer[ i ] = 0;
    SerialReadDimmerValues( buffer, MAX_DIMMER_COUNT );

    Serial.write( buffer, MAX_DIMMER_COUNT );

    for( int i = 0; i < MAX_DIMMER_COUNT; i++ )
    {
        dimmers[ i ].set( buffer[ i ] );
    }
}

void SerialReadDimmerValues( uint8_t* buffer, uint16_t length )
{
    // header: 0x55, 0xF0, num_channels, <data>, checksum, checksum, checksum. checksum.
    CRC32 crc;
    while( true )
    {
        if( Serial.available() < ( 3 + 4 + 8 ) ) // 3 header + 4 CRC + 8 data.
            continue;

        crc.reset();

        int16_t rx; //
        rx = Serial.read();
        if( rx == -1 || rx != 0x55 )
            continue;
        crc.update( ( uint8_t )rx );
        // Serial.println("found 0x55");

        rx = Serial.read();
        if( rx == -1 || rx != 0xF0 )
            continue;
        crc.update( ( uint8_t )rx );
        // Serial.println("found 0xF0");

        uint16_t length = Serial.read();
        if( length < 0 || length > MAX_DIMMER_COUNT )
            continue;
        crc.update( ( uint8_t )length );
        // Serial.println("found read length correctly");

        for( int16_t i = 0; i < length; ++i )
        {
            uint16_t rx_value = Serial.read();
            if( rx_value < 0 )
                continue;
            buffer[ i ] = rx_value;
            crc.update( ( uint8_t )rx_value );
            // Serial.println("found value");
        }

        uint32_t rx_crc = 0;
        if( Serial.readBytes( ( uint8_t* )&rx_crc, sizeof( rx_crc ) ) != sizeof( rx_crc ) )
            continue;
        uint32_t computed_crc = crc.finalize();
        if( rx_crc != computed_crc )
            continue;
        // Serial.println("validated CRC");
        break;
    }
}