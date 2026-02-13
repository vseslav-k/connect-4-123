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


inline constexpr std::array<uint64_t, 19> makeUtilPatterns(){
    std::array<uint64_t, 19> utilPatterns{};

    utilPatterns[0] = 0ULL;
    utilPatterns[1] = 0b1111111111111111111111111111111111111111110000000000000000000000;
    utilPatterns[2] = 0b1000000100000010000001000000100000010000000000000000000000000000;

    for(int i = 3; i < 9; ++i)
        utilPatterns[i] = utilPatterns[i-1] >> 1;

    
    utilPatterns[9] = 0b1111111000000000000000000000000000000000000000000000000000000000;

    for(int i = 10; i < 15; ++i)
        utilPatterns[i] = utilPatterns[i-1] >> 7;


    utilPatterns[15] = utilPatterns[2] | utilPatterns[3] | utilPatterns[4] | utilPatterns[5];
    utilPatterns[16] = utilPatterns[9] | utilPatterns[10] | utilPatterns[11];
    utilPatterns[17] = utilPatterns[15] & utilPatterns[16];
    utilPatterns[18] = (utilPatterns[5] | utilPatterns[6] | utilPatterns[7] | utilPatterns[8]) & utilPatterns[16];

    return utilPatterns;
 }

inline uint64_t setBit(uint64_t x, const unsigned int pos, const bool value) {
    if (pos >= 64) {
        log(Error, "Bit position out of range");
        return x;
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

inline void setBitInPlace(uint64_t& x, const unsigned int pos, const bool value) {
    if (pos >= 64) {
        log(Error, "Bit position out of range");
        return;
    }

    // 0 = MSB, 63 = LSB
    const unsigned shift = 63 - pos;
    const uint64_t mask = 1ULL << shift;

    x = (x & ~mask) | (static_cast<uint64_t>(value) << shift);
}

inline int getBit(uint64_t x, const unsigned int pos) {
    if (pos >= 64) {
        log(Error, "Bit position out of range");
        return 0;
    }
    return static_cast<int>((x >> (63 - pos)) & 1ULL);
}





class Connect4 final: public Game{

public:

    enum Color: bool{
        RED = 0,
        YELLOW = 1
    };

    enum UtilPatternIdx{
        EMPT = 0,
        FULL = 1,

        COL1 = 2,COL2 = 3,COL3 = 4,COL4 = 5,COL5 = 6,COL6 = 7,COL7 = 8,

        ROW1 = 9, ROW2 = 10, ROW3 = 11, ROW4 = 12, ROW5 = 13, ROW6 = 14,
        HORIZONTAL_START = 15, VERTICAL_START = 16, DIAGONAL_LR_START = 17, DIAGONAL_RL_START = 18 

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
    void        setStateString(const std::string &s) override{}

    bool        actionForEmptyHolder(BitHolder &holder) override;
    bool        canBitMoveFrom(Bit &bit, BitHolder &src) override{return false;}
    bool        canBitMoveFromTo(Bit &bit, BitHolder &src, BitHolder &dst) override{return false;}
    void        stopGame() override;

     // AI methods
    void        updateAI() override;
    bool        gameHasAI() override  { return aiPlayer != -1; } // Set to true when AI is implemented
    Grid*       getGrid() override final { return _grid; }
private:

    static constexpr std::array<uint64_t, 69> WINNING_PATTERNS = calcWinningPatterns();
    static constexpr std::array<uint64_t, 19> UTIL_PATTERNS = makeUtilPatterns();
    static constexpr std::array<uint8_t, 42> SORTED_CELL_VALUES = {24, 17, 23, 25, 16, 18, 31, 10, 22, 26, 30, 32, 9, 11, 15, 19, 38, 3, 29, 33, 8, 12, 21, 27, 37, 39, 2, 4, 14, 20, 28, 34, 36, 40, 1, 5, 7, 13, 35, 41, 0, 6};
    //                                         idx pairs:    1-2 3-4 5-6 7-8 9-10 11-12 13-14 15-16 17-18 19-20 21-22 23-24 25-26 27-28 29-30 31-32 33-34 35-36 37-38 39-40 41-42
    //                                     max calc left:    41  39  37  35   33   31    29    27    25    23    21    19    17    15    13    11     9     7     5     3     1   
    static constexpr std::array<uint8_t, 42> DEPTH_AT_TURN = {9,  9,  8,  9,  10,  11,   13,   14,   17,   30,   20,   20,   20,   20,   20,   10,   10,   10,   10,   10,  10};
    static constexpr int32_t MATE = 99999;

    Bit*                PieceForPlayer(int player);

    bool                isLowest(const BitHolder &holder) const;
    std::pair<int, int> getHolderCords(const BitHolder &holder) const;
    



    bool        comboWon(const uint64_t piecies) const;
    bool        boardIsFull() const;
    bool        moveIsLegal(const uint64_t board, const int i) const;
    bool        forcedToPlay(const Color& col, int idx) const;



    int         assessWinPattern(const Board& board, const Color color, const int patternIdx) const;
    int         evalBoardState(const Board& board, const Color color) const;


    int         negamax(const Color player, int a = -MATE, const int b = MATE, const int d = 8);

    bool        currPlayer(){return _turns.size() % 2 == 0;}
    
    static std::pair<int, int> cordsBoardToGrid(int boardIdx);
    static int                 cordsGridToBoard(std::pair<int, int> gridCords);
    static std::array<int, 42> makeHeatMap(const uint64_t *arr, const int len);

    void        updateAI2();
    int         negamax2(const Color player, int a = -MATE, const int b = MATE, const int d = 8);
    int         assessWinPattern2(const Board& board, const Color color, const int patternIdx) const;
    int         evalBoardState2(const Board& board, const Color color) const;




    int aiPlayer;
    bool ai2GoesFirst;
    Grid*       _grid;
    Board _board;

     /*
    00 01 02 03 04 05 06 
    07 08 09 10 11 12 13 
    14 15 16 17 18 19 20 
    21 22 23 24 25 26 27 
    28 29 30 31 32 33 34 
    35 36 37 38 39 40 41
    */

};

