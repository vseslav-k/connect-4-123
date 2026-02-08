#pragma once
#include "Game.h"
#include "string"
class Connect4 final: public Game{

public:

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

    bool isLowest(const BitHolder &holder) const;

    std::pair<int, int> getHolderCords(const BitHolder &holder) const;

    Bit* PieceForPlayer(int player);

    int aiPlayer;

    static const int RED = 0;
    static const int YELLOW = 1;



    // Board representation
    Grid*       _grid;

};