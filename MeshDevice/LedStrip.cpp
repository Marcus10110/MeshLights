#include "LedStrip.h"
#include "Global.h"

static LedStrip* gThis;

LedStrip::LedStrip( uint16_t max_led_count, uint8_t led_pin, StateManager* state_manager, GetTimeMsCallback time_getter ) : mStrip(max_led_count, led_pin), mStateManager( state_manager ), mMaxLedCount( max_led_count ), mGetMeshTimeMs( time_getter )
{

}
void LedStrip::Init()
{
    //init the strip
    mStrip.Begin();
    mStrip.SetBrightness( mStateManager->GetState()->mBrightness );
    mStrip.ClearTo(RgbColor( 0, 0, 0 ) );
    mStrip.Show();

    //init the timer.
    gThis = this;
    os_timer_setfn(&mTimer, StaticOnTimerEvent, NULL);
    os_timer_arm(&mTimer, 1000 / FRAME_RATE, true); //1000ms, repeat = true;
}

void LedStrip::OnTimerEvent()
{
    //none of this is thread safe at the moment.
    const State* state = mStateManager->GetState();



    if( state->mMode == device_mode_t::OPENPIXEL || state->mDeviceType != device_type_t::LED)
    {
        //ignore the case where OpenPixelControl is in play.
        return;
    }

    //if we're disabled, blank it out.
    if( state->mEnabled == false)
    {
        mStrip.ClearTo( RgbColor( 0, 0, 0 ) );
        mStrip.Show();
        return;
    }

    uint32_t node_time = mGetMeshTimeMs();

    uint16_t led_count = state->mLedCount;
    uint8_t animation_state = state->mLocalState;

    //ok, we just need to draw the current state!
    RenderState( node_time, animation_state, led_count );
}

void LedStrip::RenderState(uint32_t time_ms, uint8_t state, uint16_t led_count)
{
    if( state < NUM_LED_STATES )
    {
        RenderRainbow(time_ms, led_count);
    }

    if( state == SHOW_CURRENT_ANIMATION_STATE_INDEX)
    {
        RenderStateIndicator(time_ms, led_count);
    }

    mStrip.Show();
}

void LedStrip::RenderRainbow(uint32_t time_ms, uint16_t led_count)
{
    const uint32_t period_ms = 1000;
    const uint32_t local_time_ms = time_ms % period_ms;
    float time_fraction = (float)local_time_ms / period_ms;
  
    for ( uint16_t i = 0; i < led_count; i++ ) {
      float hue = (float)i / led_count + time_fraction;
      hue = hue - (int)hue; //roll off to integer range.
      
      HsbColor color = HsbColor( hue, 1.0, 0.5 );
      mStrip.SetPixelColor( i, color );
    }
}

void LedStrip::RenderStateIndicator(uint32_t time_ms, uint16_t led_count)
{
    const uint8_t current_state = mStateManager->GetState()->mNextState;


    const uint32_t period_ms = 500;
    const uint32_t local_time_ms = time_ms % period_ms;
    const byte brightness = local_time_ms <= (period_ms/2) ? local_time_ms * 255 / (period_ms/2) : (period_ms - local_time_ms) * 255 / (period_ms/2);
    
    for ( uint16_t i = 0; i < led_count; i++ ) {
      RgbColor color = RgbColor( 0, 0, 0 );
      if ( i == current_state )
      {
        color = RgbColor( brightness, brightness, brightness );
      }
      mStrip.SetPixelColor(i, color );
    }
}

void LedStrip::StaticOnTimerEvent(void* arg)
{
    gThis->OnTimerEvent();
}