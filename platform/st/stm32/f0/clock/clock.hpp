#ifndef F0_CLOCK_CLOCK_HPP
#define F0_CLOCK_CLOCK_HPP

class Clock
{
public:
    using callback = void (*)();

    Clock() = delete;
    Clock(Clock&) = delete;

    static void RegisterCallback(callback fn);
    static void Callback();

  private:
    static callback cb;
};

#endif /* F0_CLOCK_CLOCK_HPP */
