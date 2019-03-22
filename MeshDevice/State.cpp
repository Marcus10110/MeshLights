#include "State.h"
#include <string.h>

StateManager::StateManager( StateChangedCallback callback ) : mOnChangeCallback( callback )
{
}

void StateManager::Init()
{
    mStateSerializer.Init();
    Serial.println( "loading state..." );
    if( !mStateSerializer.DeserializeState( &mState ) )
    {
        Serial.println( "failed to read state. attempting write..." );
        // serialize the default state:
        State reset_default_state;
        mState = reset_default_state; // default copy assignment.
        mStateSerializer.SerializeState( &mState );
        Serial.println( "finished writing default state" );
        return;
    }
    Serial.println( "successfully read state" );
}

const State* StateManager::GetState()
{
    return &mState;
}

// actions:
// buttons:
void StateManager::AdvanceAnimation( uint32_t time )
{
    // advance the state, using the animation interframe.
    uint8_t current_state = mState.mLocalState;
    // if we're already advancing, be sure to pick up the next real state.
    if( mState.mAutoAdvance )
    {
        current_state = mState.mNextState;
    }
    uint8_t next_state = current_state + 1;
    if( mState.mDeviceType == device_type_t::LED && next_state >= NUM_LED_STATES )
    {
        next_state = 0;
    }
    else if( mState.mDeviceType == device_type_t::DIMMER && next_state >= NUM_BULB_STATES )
    {
        next_state = 0;
    }

    // modify the state.
    mState.mAutoAdvance = true;
    mState.mNextState = next_state;
    mState.mAutoAdvanceTime = time + AUTOMATIC_STATE_DURRATION_MS;
    // show the interstate animation now.
    mState.mLocalState = SHOW_CURRENT_ANIMATION_STATE_INDEX;
    if( mState.mMode == device_mode_t::NETWORK )
    {
        // broadcast the new state to the network.
        mState.mNetworkState[ mState.mGroup ] = mState.mLocalState;
        mState.mNetworkBroadcastRequired = true;
    }

    mOnChangeCallback( &mState );
}
void StateManager::AdvanceMode( uint32_t time )
{
    // advance mode, play mode animation.
    // NETWORK, INDIVIDUAL, OPENPIXEL, in order.

    // current state is now showing the current mode.
    uint8_t current_state_backup = mState.mLocalState;
    if( mState.mAutoAdvance )
    {
        current_state_backup = mState.mNextState;
    }

    mState.mLocalState = SHOW_CURRENT_MODE_INDEX; // show the mode changing animation.

    mState.mAutoAdvance = true;
    mState.mAutoAdvanceTime = time + AUTOMATIC_STATE_DURRATION_MS;

    switch( mState.mMode )
    {
    case device_mode_t::NETWORK:
        // switch to individual mode
        mState.mMode = device_mode_t::INDIVIDUAL;
        mState.mNextState = current_state_backup;
        break;
    case device_mode_t::INDIVIDUAL:
        // switch to openpixel mode. state no longer matters.
        mState.mMode = device_mode_t::OPENPIXEL;
        // moving internally back to state 0, although any value is fine.
        mState.mNextState = 0;
        break;
    case device_mode_t::OPENPIXEL:
        // switch to network. be sure to copy the state from the network.
        mState.mMode = device_mode_t::NETWORK;
        mState.mNextState = mState.mNetworkState[ mState.mGroup ];
        break;
    }

    mOnChangeCallback( &mState );
}
void StateManager::ToggleEnabled( uint32_t time )
{
    // toggle enable / disable. play animation.
    uint8_t current_state_backup = mState.mLocalState;
    if( mState.mAutoAdvance )
    {
        current_state_backup = mState.mNextState;
    }


    mState.mNextState = current_state_backup;

    mState.mAutoAdvance = true;
    mState.mAutoAdvanceTime = time + AUTOMATIC_STATE_DURRATION_MS;

    if( mState.mEnabled )
    {
        mState.mLocalState = DISABLING_ANIMATION_INDEX;
        mState.mAutoDisableOnNextState = true;
    }
    else
    {
        mState.mLocalState = ENABLING_ANIMATION_INDEX;
        mState.mAutoDisableOnNextState = false;
        // enable now to show the enabling animation.
        mState.mEnabled = true;
    }

    mOnChangeCallback( &mState );
}

// persisted actions:
void StateManager::SetDeviceType( device_type_t device_type )
{
    // note: we could be an an invalid index. TODO: decide what we want to do about that.
    mState.mDeviceType = device_type;
    mStateSerializer.SerializeState( &mState );
    mOnChangeCallback( &mState );
}
void StateManager::SetLedCount( uint16_t count )
{
    mState.mLedCount = count;
    mStateSerializer.SerializeState( &mState );
    mOnChangeCallback( &mState );
}
void StateManager::SetBrightness( uint8_t brightness )
{
    mState.mBrightness = brightness;
    mStateSerializer.SerializeState( &mState );
    mOnChangeCallback( &mState );
}
void StateManager::SetGroup( uint8_t group )
{
    // TODO: switch over to new local state.
    mState.mGroup = group;
    mStateSerializer.SerializeState( &mState );
    mOnChangeCallback( &mState );
}
void StateManager::SetDeviceName( const char* name )
{
    strncpy( mState.mDeviceName, name, MAX_NAME_LENGTH );
    mStateSerializer.SerializeState( &mState );
    mOnChangeCallback( &mState );
}

void StateManager::SetConnectionMode( connection_mode_t connection_mode )
{
    mState.mConnectionMode = connection_mode;
    mStateSerializer.SerializeState( &mState );
    mOnChangeCallback( &mState );
}

// mesh actions:
void StateManager::NetworkStateChanged( uint8_t group, uint8_t state )
{
    mState.mNetworkState[ group ] = state;

    // if we're in network mode, and our group's state changes, lets advance the state!
    if( mState.mMode == device_mode_t::NETWORK && mState.mGroup == group )
    {
        // stop any in-progress animations
        mState.mAutoAdvance = false;
        // change to the new state.
        mState.mLocalState = state;
    }

    mOnChangeCallback( &mState );
}

void StateManager::FinishedTransmittingState()
{
    mState.mNetworkBroadcastRequired = false;
    mOnChangeCallback( &mState );
}

// loop actions:
void StateManager::TimeUpdate( uint32_t time )
{
    // check for automatic state advancement.
    // lots going on in this function, we need to keep an eye on that.

    // detect automatic state advancement!
    if( mState.mAutoAdvance && mState.mAutoAdvanceTime <= time )
    {
        // switch state!
        mState.mLocalState = mState.mNextState;
        mState.mAutoAdvance = false;
        // special case: handle disabling the device.
        if( mState.mAutoDisableOnNextState )
        {
            mState.mAutoDisableOnNextState = false;
            mState.mEnabled = false;
        }

        // handle the case where we need to update the mesh!
        // do not broadcast if we are turning off, or if we're at the start of the the turning on animation.
        if( mState.mMode == device_mode_t::NETWORK && mState.mEnabled == true )
        {
            // broadcast the new state to the network.
            mState.mNetworkState[ mState.mGroup ] = mState.mLocalState;
            mState.mNetworkBroadcastRequired = true;
        }

        mOnChangeCallback( &mState );
    }
}
