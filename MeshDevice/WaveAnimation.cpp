#include "WaveAnimation.h"

#include <Arduino.h>

namespace
{
    // measured in bulbs.
    float const WaveLength = 8;

    // seconds per complete wave.
    float const SecPerWavelength = 1.0 / WaveLength;

    float FMap( float x, float in_min, float in_max, float out_min, float out_max )
    {
        return ( x - in_min ) * ( out_max - out_min ) / ( in_max - in_min ) + out_min;
    }

    float FClamp( float x, float min, float max)
    {
        if( x > max )
        {
            return max;
        }
        if( x < min)
        {
            return min;
        }
        return x;
    }

    uint8_t Normalize( float value )
    {
        // map from [-1, 1] t0 [0, 255]
        float result = FMap(value, -1, 1, 25, 125 );
        result = FClamp(result, 0, 255);
        return static_cast<uint8_t>( result );
    }

    uint8_t GetBulbBrightness( uint32_t time_ms, int bulb )
    {
        const float time_s = ( static_cast<float>( time_ms ) / 1000.0 );
        float angle = TWO_PI * time_s / WaveLength;
        float distance_wavelengths = SecPerWavelength * time_s;
        angle += distance_wavelengths * TWO_PI / WaveLength;
        float raw_wave_value = sin( angle );
        return Normalize( sin( angle ) );
    }
}
namespace Animations
{
    uint8_t Wave::Animate( uint32_t time_ms, int bulb )
    {
        return GetBulbBrightness( time_ms, bulb );
    }
}