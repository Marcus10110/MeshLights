#pragma once

// extern C is required to avoid this error: conflicts with new declaration with 'C' linkage
extern "C"
{
#include "user_interface.h"
}

#include "State.h"

using GetTimeMsCallback = uint32_t ( * )();

class SerialDimmer
{
  public:
    SerialDimmer( uint16_t channel_count, StateManager* state_manager, GetTimeMsCallback time_getter );
    void Init();
    void UpdateFromOpc( uint8_t* values, uint16_t count );

  private:
    void OnTimerEvent();
    void RenderState( uint32_t time_ms, uint8_t state );
    void RenderSpin( uint32_t time_ms );
    void RenderChandelier( uint32_t time_ms );
    void RenderWave( uint32_t time_ms );
    void RenderStateIndicator( uint32_t time_ms );

    // for writing to the device:
    void SetDimmerState( uint8_t* values, uint16_t count );

    os_timer_t mTimer;
    StateManager* mStateManager;
    const uint16_t mChannelCount;
    GetTimeMsCallback mGetMeshTimeMs;

    static void StaticOnTimerEvent( void* arg );
};
