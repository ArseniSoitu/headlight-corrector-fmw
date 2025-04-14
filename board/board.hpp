#ifndef BOARD_BOARD_HPP
#define BOARD_BOARD_HPP

namespace board
{

class Board
{
public:
    explicit Board();
    Board(Board&) = delete;

    void Init();

private:
    void serialInit();
    void pwmInit();
};

};

#endif /* BOARD_BOARD_HPP */
