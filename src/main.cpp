#include <cstdio>
#include "board.hpp"
#include "sysclock/sysclock.hpp"

void main_app() {
    auto board = board::Board{};
    board.Init();

    sysclock::SysClock::Reset();

    printf("Hello headlight corrector!\n");
}
