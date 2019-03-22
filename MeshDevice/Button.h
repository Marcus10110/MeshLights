#pragma once

#include "State.h"

#include <Arduino.h>

// button is active low. (idle high)
#define BUTTON_PIN 0 // GPIO 0, D3, flash button
#define BUTTON_ACTIVE LOW
#define BUTTON_IDLE HIGH
#define SHORT_PRESS_MIN 100
#define LONG_PRESS_MIN 3000
#define EXTRA_LONG_RESS_MIN 6000

class Button
{
  public:
    Button( StateManager* state_manager, uint8_t button_pin );
    void Init();
    void Update();

  private:
    void HandlePress( uint32_t durration );
    StateManager* mStateManager; // not owned.
    uint32_t mLastButtonPressTime{ 0 };
    uint8_t mButtonState{ BUTTON_IDLE };
    uint8_t mButtonPin;
};