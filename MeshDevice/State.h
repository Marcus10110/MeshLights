#pragma once
#include <stdint.h>

#define MAX_GROUPS	8
#define MAX_NAME_LENGTH	16	//not including null termination. 17 bytes minimum size.

struct State;

using StateChangedCallback = void(*)(const State* state);
enum class device_type_t { LED, DIMMER };
enum class device_mode_t {NETWORK, INDIVIDUAL, OPENPIXEL};

const uint32_t AUTOMATIC_STATE_DURRATION_MS = 1000; //how long all automatic animations last.

//the first N states are for regular animations.
//then we have special states after that.

const uint8_t NUM_LED_STATES = 5; //increment this when adding new led animations.
const uint8_t NUM_BULB_STATES = 4; //increment this when adding new dimmer animations.
const uint8_t MAX_REGULAR_STATES = NUM_LED_STATES > NUM_BULB_STATES ? NUM_LED_STATES : NUM_BULB_STATES;
const uint8_t SHOW_CURRENT_ANIMATION_STATE_INDEX = MAX_REGULAR_STATES;
const uint8_t SHOW_CURRENT_MODE_INDEX = SHOW_CURRENT_ANIMATION_STATE_INDEX + 1;
const uint8_t ENABLING_ANIMATION_INDEX = SHOW_CURRENT_MODE_INDEX + 1;
const uint8_t DISABLING_ANIMATION_INDEX = ENABLING_ANIMATION_INDEX + 1;

const uint8_t STATE_COUNT = DISABLING_ANIMATION_INDEX + 1;

struct State
{
	//persisted
	device_type_t mDeviceType{device_type_t::LED};
	uint16_t mLedCount{20};
	uint8_t mBrightness{255};
	uint8_t mGroup{0};
	char mDeviceName[MAX_NAME_LENGTH+1] = "untitled";
	
	//dynamic:
	uint8_t mLocalState{0};
	uint8_t mNetworkState[MAX_GROUPS]{0,0,0,0,0,0,0,0};
	bool mAutoAdvance{false};
	uint32_t mAutoAdvanceTime{0};
    uint8_t mNextState{0};
    bool mAutoDisableOnNextState{false};
	device_mode_t mMode{device_mode_t::NETWORK};
    bool mEnabled{true};
    bool mNetworkBroadcastRequired{false};
};


class StateManager
{
	public:
		StateManager( StateChangedCallback callback );
		const State* GetState();

		//actions:
		//buttons:
		void AdvanceAnimation(uint32_t time);
		void AdvanceMode(uint32_t time);
		void ToggleEnabled(uint32_t time);
		
		//persisted actions:
		void SetDeviceType(device_type_t device_type);
		void SetLedCount(uint16_t count);
		void SetBrightness(uint8_t brightness);
		void SetGroup(uint8_t group);
		void SetDeviceName(const char* name);
		
		//mesh actions:
        void NetworkStateChanged( uint8_t group, uint8_t state );
        void FinishedTransmittingState();
		
		//loop actions:
		void TimeUpdate(uint32_t time);

	private:
        State mState;
        StateChangedCallback mOnChangeCallback;
};