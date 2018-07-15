#include "SerialDimmer.h"
#include <Arduino.h>
#include "Global.h"
#include "CRC32.h"

static SerialDimmer* gThis;

SerialDimmer::SerialDimmer( uint16_t channel_count, StateManager* state_manager, GetTimeMsCallback time_getter )
:   mStateManager( state_manager ),
    mChannelCount( channel_count ),
    mGetMeshTimeMs( time_getter )
{

}

void SerialDimmer::Init()
{
    gThis = this;
    os_timer_setfn(&mTimer, StaticOnTimerEvent, NULL);
    os_timer_arm(&mTimer, 1000 / FRAME_RATE, true); //1000ms, repeat = true;
}

void SerialDimmer::UpdateFromOpc(uint8_t* values, uint16_t count)
{
    const State* state = mStateManager->GetState();
    if( state->mMode != device_mode_t::OPENPIXEL || state->mDeviceType != device_type_t::DIMMER)
    {
        //ignore the case when open pixel or dimmer is not selected.
        return;
    }

    if( count > MAX_DIMMER_COUNT)
        count = MAX_DIMMER_COUNT;

    Serial.printf("dimmers[%d] = { ", count);
    for( int i = 0; i < count; ++i)
    {
        if( i == count - 1)
            Serial.printf("%d", values[i]);
        else
            Serial.printf("%d,\t", values[i]);
    }
    Serial.println("}");

    SetDimmerState(values, count);
}

void SerialDimmer::OnTimerEvent()
{
    //none of this is thread safe at the moment.
    const State* state = mStateManager->GetState();

    if( state->mMode == device_mode_t::OPENPIXEL || state->mDeviceType != device_type_t::DIMMER)
    {
        //ignore the case where OpenPixelControl is in play.
        return;
    }

    //if we're disabled, blank it out.
    if( state->mEnabled == false)
    {
        uint8_t values[MAX_DIMMER_COUNT];
        for( uint16_t i = 0; i < mChannelCount; ++i)
            values[i] = 0;
        SetDimmerState(values, mChannelCount);
        return;
    }

    uint32_t node_time = mGetMeshTimeMs();

    uint8_t animation_state = state->mLocalState;

    //ok, we just need to draw the current state!
    RenderState( node_time, animation_state );
}

void SerialDimmer::RenderState(uint32_t time_ms, uint8_t state)
{
    if( state < NUM_BULB_STATES )
    {
        RenderSpin(time_ms);
    }

    if( state == SHOW_CURRENT_ANIMATION_STATE_INDEX)
    {
        RenderStateIndicator(time_ms);
    }
}

void SerialDimmer::RenderSpin(uint32_t time_ms)
{
    
    

    const uint32_t period_ms = 3000;
    const uint32_t local_time_ms = time_ms % period_ms;
    float time_fraction = (float)local_time_ms / period_ms;

    uint8_t values[MAX_DIMMER_COUNT];

    for( uint16_t i = 0; i < mChannelCount; ++i)
    {
        if( i == (uint16_t)round(time_fraction * mChannelCount))
            values[i] = 50;
        else
            values[i] = 0;
    }
    SetDimmerState(values, mChannelCount);
}

void SerialDimmer::RenderStateIndicator(uint32_t time_ms)
{  
    const uint8_t current_state = mStateManager->GetState()->mNextState;
    const uint32_t period_ms = 500;
    const uint32_t local_time_ms = time_ms % period_ms;
    const uint8_t brightness = local_time_ms <= (period_ms/2) ? local_time_ms * 255 / (period_ms/2) : (period_ms - local_time_ms) * 255 / (period_ms/2);

    uint8_t values[MAX_DIMMER_COUNT];
    
    for ( uint16_t i = 0; i < mChannelCount; i++ ) 
    {

      if ( i == current_state )
        values[i] = brightness;
      else
        values[i] = 0;
    }
    SetDimmerState(values, mChannelCount);
}

void SerialDimmer::SetDimmerState(uint8_t* values, uint16_t count)
{
    //serial write here.
    const uint8_t header_size = 3;
    uint8_t header[header_size];
    header[0] = 0x55;
    header[1] = 0xF0;
    header[2] = count;

    CRC32 crc;
    crc.update(header);
    crc.update<uint8_t>(values, count);
    uint32_t crc_value = crc.finalize();

    Serial.write(header, header_size);
    Serial.write(values, count);
    Serial.write((uint8_t*)&crc_value, sizeof(crc_value));

}

void SerialDimmer::StaticOnTimerEvent(void* arg)
{
    gThis->OnTimerEvent();
}
