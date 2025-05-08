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
    void MotorSteps(int steps);
    void ADCStart();
    uint32_t ADCFinished();
    void dmaADCInit(uint16_t* buf, std::size_t sz);
    void ADCInit();

    static std::array<uint16_t, 200> adcData;

private:
    void serialInit();
    void pwmInit();
    void opInit();
    void motorDirPinInit();
    void motorEnPinInit();
    void dmaPWMInit(uint16_t* data, uint16_t pulses);
    void ADCCalibrate();

    uint16_t dmaPwmLoadValue;
    uint32_t adcCalibFactor;
};

};

#endif /* BOARD_BOARD_HPP */
