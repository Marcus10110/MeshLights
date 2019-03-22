#include "Button.h"


Button::Button( StateManager* state_manager, uint8_t button_pin ) : mStateManager( state_manager ), mButtonPin( button_pin )
{
}

void Button::Init()
{
    pinMode( mButtonPin, INPUT );
}

void Button::Update()
{
    int last_button_state = mButtonState;
    mButtonState = digitalRead( mButtonPin );

    if( last_button_state == BUTTON_IDLE && mButtonState == BUTTON_ACTIVE )
    {
        // active edge.
        mLastButtonPressTime = millis();
    }
    else if( last_button_state == BUTTON_ACTIVE && mButtonState == BUTTON_IDLE )
    {
        // inactive edge.
        uint32_t durration = millis() - mLastButtonPressTime;
        HandlePress( durration );
    }
}

void Button::HandlePress( uint32_t durration )
{
    uint32_t time = millis();
    if( durration >= SHORT_PRESS_MIN && durration < LONG_PRESS_MIN )
    {
        mStateManager->AdvanceAnimation( time );
    }
    else if( durration >= LONG_PRESS_MIN && durration < EXTRA_LONG_RESS_MIN )
    {
        mStateManager->AdvanceMode( time );
    }
    else if( durration >= EXTRA_LONG_RESS_MIN )
    {
        mStateManager->ToggleEnabled( time );
    }
}