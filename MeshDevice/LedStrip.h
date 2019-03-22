#pragma once

#include <NeoPixelAnimator.h>
#include <NeoPixelBrightnessBus.h>

#include "State.h"

using GetTimeMsCallback = uint32_t(*)();

class LedStrip
{
public:
    LedStrip( uint16_t max_led_count, uint8_t led_pin, StateManager* state_manager, GetTimeMsCallback time_getter );
    void Init();
private:
    void OnTimerEvent();
    void RenderState(uint32_t time_ms, uint8_t state, uint16_t led_count);
    void RenderRainbow(uint32_t time_ms, uint16_t led_count);
    void RenderStateIndicator(uint32_t time_ms, uint16_t led_count);
    os_timer_t mTimer;
    NeoPixelBrightnessBus<NeoGrbFeature, NeoEsp8266Uart0800KbpsMethod> mStrip;
    StateManager* mStateManager;
    const uint16_t mMaxLedCount;
    GetTimeMsCallback mGetMeshTimeMs;

    static void StaticOnTimerEvent(void* arg);
};
