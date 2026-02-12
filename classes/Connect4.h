#pragma once
#include "Game.h"
#include "string"
#include <array>
#include <bit>
#include <bitset>
#include "C:\Libraries\imgui\logger\logger.h"


inline constexpr std::array<uint64_t, 69> calcWinningPatterns(){ 
        std::array<uint64_t, 69> winningPatterns{};
        //horizontal _
        winningPatterns[0] = (0b1111000000000000000000000000000000000000000000000000000000000000);

        for(int i = 1; i < 4*6;++i)
            winningPatterns[i] = winningPatterns[i-1] >> (i%4 == 0? 4 :1);
        //vertical |
        winningPatterns[24] = (0b100000010000001000000100000000000000000000<<22);
        
        for(int i = 25; i < 24+3*7; ++i)
            winningPatterns[i] = winningPatterns[i-1] >> 1;
        
        //diagonal /
        winningPatterns[45] = (0b100000001000000010000000100000000000000000<<22);

        for(int i = 46; i < 57; ++i)
            winningPatterns[i] = winningPatterns[i-1] >> ((i == 49 || i == 53 )? 4 : 1);

        winningPatterns[57] = (0b0001000001000001000001000000000000000000000000000000000000000000);

        //diagonal \ 
        for(int i = 58; i < 69; ++i)
            winningPatterns[i] = winningPatterns[i-1] >> ((i == 61 || i == 65)?4 : 1);
        

        return winningPatterns;

        //horizontal = 0-23
        //vertical = 24-44
        //diagonal / = 45-56
        //diagonal \ = 57-68
        
}

inline uint64_t setBit(uint64_t x, unsigned pos, bool value) {
    if (pos >= 64) {
        log(Error, "Bit position out of range");
        return 0;
    }

    // 0 = MSB, 63 = LSB
    const unsigned shift = 63 - pos;
    const uint64_t mask = 1ULL << shift;

    if (value) {
        x |= mask;   // set to 1
    } else {
        x &= ~mask;  // set to 0
    }
    return x;
}

inline void setBitInPlace(uint64_t& x, unsigned pos, bool value) {
    if (pos >= 64) {
        log(Error, "Bit position out of range");
        return;
    }

    // 0 = MSB, 63 = LSB
    const unsigned shift = 63 - pos;
    const uint64_t mask = 1ULL << shift;

    x = (x & ~mask) | (static_cast<uint64_t>(value) << shift);
}
inline int getBit(uint64_t x, unsigned pos) {
    if (pos >= 64) {
        log(Error, "Bit position out of range");
        return 0;
    }
    return static_cast<int>((x >> (63 - pos)) & 1ULL);
}





class Connect4 final: public Game{

public:

    enum Color{
        RED = 0,
        YELLOW = 1
    };

    struct Board{
        std::array<uint64_t, 2> pieces;
    };



    Connect4(int aiPlayer = 1);
    ~Connect4();

    void        setUpBoard() override;
    Player*     checkForWinner() override;
    bool        checkForDraw() override;
    std::string initialStateString() override;
    std::string stateString() override ;
    void        setStateString(const std::string &s) override;
    bool        actionForEmptyHolder(BitHolder &holder) override;
    bool        canBitMoveFrom(Bit &bit, BitHolder &src) override;
    bool        canBitMoveFromTo(Bit &bit, BitHolder &src, BitHolder &dst) override;
    void        stopGame() override;

     // AI methods
    void        updateAI() override;
    bool        gameHasAI() override  { return aiPlayer != -1; } // Set to true when AI is implemented
    Grid* getGrid() override final { return _grid; }
private:

    Board _board;
    static constexpr std::array<uint64_t, 69> WINNING_PATTERNS = calcWinningPatterns();
    static constexpr uint64_t FULL_BOARD = 0b1111111111111111111111111111111111111111110000000000000000000000;
    static constexpr uint64_t MID_COLUMN = 0b0001000000100000010000001000000100000010000000000000000000000000;
    static constexpr int32_t MATE = 9999;

    bool isLowest(const BitHolder &holder) const;

    std::pair<int, int> getHolderCords(const BitHolder &holder) const;

    Bit* PieceForPlayer(int player);

    int assessWinPattern(const Board& board, Color color, int patternIdx);
    int evalBoardState(const Board& board, Color color);

    std::pair<int, int> cordsBoardToGrid(int boardIdx);
    int cordsGridToBoard(std::pair<int, int> gridCords);

    int negamax(Color player, int a = -99999999999, int b = 99999999999, int d = 8);
    int legalMoveInCol(int col);
    bool moveIsLegal(uint64_t board, int i);
    bool boardIsFull();


    int aiPlayer;

    // Board representation
    Grid*       _grid;

};

