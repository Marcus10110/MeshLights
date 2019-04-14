#include "ChandelierAnimation.h"

#include <Arduino.h>

namespace Animations
{
    namespace
    {
        uint32_t RandomInRange( uint32_t min, uint32_t max )
        {
            return random( min, max );
        }

        uint8_t RandomInRange( uint8_t min, uint8_t max )
        {
            return random( min, max );
        }

        float ComputeProgressInRange( uint32_t start, uint32_t end, uint32_t position )
        {
            uint32_t range = end - start;
            uint32_t relative_position = position - start;
            return static_cast<float>( relative_position ) / static_cast<float>( range );
        }

        uint8_t ClampToU8( float value )
        {
            if( value >= 255 )
                value = 255;
            if( value <= 0 )
                value = 0;
            return static_cast<uint8_t>( value );
        }
    }

    uint8_t Chandelier::Animate( uint32_t time_ms )
    {
        // auto-reset if necessary:
        if( mLastFrameTimeMs == 0 || time_ms < mLastFrameTimeMs )
        {
            Reset( time_ms );
        }

        if( time_ms >= mTimeOfNextPeakMs )
        {
            UpdateTargets( time_ms );
        }

        auto brightness = ComputeBrightness( time_ms );

        mLastFrameTimeMs = time_ms;

        return brightness;
    }
    void Chandelier::Reset( uint32_t time_ms )
    {
        mLastFrameTimeMs = time_ms;
        mTimeOfLastPeakMs = 0;
        mTimeOfNextPeakMs = 0;
        UpdateTargets( time_ms );
    }

    void Chandelier::UpdateTargets( uint32_t time_ms )
    {
        mTimeOfLastPeakMs = mTimeOfNextPeakMs;
        mPreviousPeakAmp = mNextPeakAmp;
        mTimeOfNextPeakMs = time_ms + RandomInRange( mMinPeriodMs, mMaxPeriodMs );
        mNextPeakAmp = RandomInRange( mMinPeakAmp, mMaxPeakAmp );
    }

    uint8_t Chandelier::ComputeBrightness( uint32_t time_ms )
    {
        // value, 0 to 1.
        auto const time_progress = ComputeProgressInRange( mTimeOfLastPeakMs, mTimeOfNextPeakMs, time_ms );
        float brightness = ( static_cast<float>( mNextPeakAmp ) - static_cast<float>( mPreviousPeakAmp ) ) * time_progress +
                           static_cast<float>( mPreviousPeakAmp );
        return ClampToU8( brightness );
    }
}