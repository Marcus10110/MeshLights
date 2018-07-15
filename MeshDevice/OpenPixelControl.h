#pragma once
#include <functional>
#include "State.h"

struct OpcPacket
{
    uint8_t mChannel{0};
    uint8_t mCommand{0};
    uint16_t mLength{0};
    uint8_t* mPayload{nullptr};

    void Print();

    static OpcPacket ParsePacket( uint8_t* buffer, int count);
};

class OpenPixelControl
{
    public:
        OpenPixelControl(StateManager* state_manager, std::function<void(uint8_t*, uint16_t)> render_callback);
        void Init();
        void Update();

    private:
        bool mEnabled {false};
        StateManager* mStateManager;
        std::function<void(uint8_t*, uint16_t)> mRenderCallback;
};

uint8_t RgbToLuminance(uint8_t r, uint8_t g, uint8_t b);

uint16_t GetLuminanceFromPacket(const OpcPacket* packet, uint8_t* output, uint16_t buffer_size );