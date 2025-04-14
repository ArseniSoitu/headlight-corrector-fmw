#ifndef BOARD_BOARD_HPP
#define BOARD_BOARD_HPP

#include <cinttypes>
#include <array>

namespace board
{

class Board
{
public:
    explicit Board();
    Board(Board&) = delete;

    void Init();

    void MotorEnable();
    void MotorDisable();
    void MotorCW();
    void MotorCCW();
    void MotorSteps(std::size_t steps);

private:
    void serialInit();
    void pwmInit();
    void motorDirPinInit();
    void motorEnPinInit();
    void dmaInit();
};

};

#endif /* BOARD_BOARD_HPP */
