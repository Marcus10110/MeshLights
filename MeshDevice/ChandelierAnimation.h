#include <stdint.h>

namespace Animations
{
    class Chandelier
    {
        public:
            uint8_t Animate(uint32_t time_ms);
            void Reset(uint32_t time_ms);
        private:
            void UpdateTargets(uint32_t time_ms);
            uint8_t ComputeBrightness(uint32_t time_ms);
            uint32_t mLastFrameTimeMs{0};
            uint32_t mTimeOfNextPeakMs{0};
            uint32_t mTimeOfLastPeakMs{0};
            uint8_t mNextPeakAmp{0};
            uint8_t mPreviousPeakAmp{0};

            // animation parameters:
            uint8_t mMaxPeakAmp{70};
            uint8_t mMinPeakAmp{1};
            uint32_t mMinPeriodMs{500};
            uint32_t mMaxPeriodMs{10000};
    };
}