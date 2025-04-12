#include <chrono>

namespace sysclock
{

class SysClock
{
public:
    using rep        = std::int64_t;
    using period     = std::milli;
    using duration   = std::chrono::duration<rep, period>;
    using time_point = std::chrono::time_point<SysClock>;

    static constexpr bool is_steady = true;

    SysClock() = delete;
    SysClock(SysClock&) = delete;
    void operator=(const SysClock&) = delete;

    static void Reset() {
        ticks = 0;
    }

    static void SetValue(uint64_t v) {
        ticks = v;
    }

    static void IncValue() {
        ticks++;
    }

    static time_point now() noexcept
    {
        return time_point{duration{ticks}};
    }

    static uint64_t GetRawValue() {
        return ticks;
    }

private:
    static uint64_t ticks;
};

};
