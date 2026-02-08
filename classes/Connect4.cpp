#include "Connect4.h"
#include "C:\Libraries\imgui\logger\logger.h"

Connect4::Connect4(int aiPlayer){
    this->aiPlayer = aiPlayer;
    _grid = new Grid(7,6);
    
}
Connect4::~Connect4(){
    delete _grid;
}

void Connect4::setUpBoard() {
    setNumberOfPlayers(2);
    if(aiPlayer != -1)
        setAIPlayer(aiPlayer);
    
    _gameOptions.rowX = 7;
    _gameOptions.rowY = 6;
    _grid->initializeSquares(80, "boardsquare.png");

    
    startGame();
}

Player* Connect4::checkForWinner(){
    return nullptr;
}
bool Connect4::checkForDraw(){
    return false;
}
std::string Connect4::initialStateString(){
    return "000000000000000000000000000000000000000000";
}
std::string Connect4::stateString(){
    return "000000000000000000000000000000000000000000";
}
void Connect4::setStateString(const std::string &s){
    
}

Bit* Connect4::PieceForPlayer(int playerNumber){
    Bit *bit = new Bit();
    // should possibly be cached from player class?
    bit->LoadTextureFromFile(playerNumber == 1 ? "yellow.png" : "red.png");
    bit->setOwner(getPlayerAt(playerNumber == 1 ? 1 : 0));
    return bit;
}

std::pair<int, int> Connect4::getHolderCords(const BitHolder &holder) const{

    for(int i = 0; i < 7; ++i){
        for(int j = 0; j < 6; ++j){
            if(_grid->getSquare(i, j) == &holder){
                log(Debug, "holder cords" + numToStr(i)+", "+numToStr(j));
                return {i, j};
            }
        }
    }
    log(Error, "getHolderCords holder not found");
    return {-1, -1};
            
        
}

bool Connect4::isLowest(const BitHolder &holder) const{
    std::pair<int,int> cords = getHolderCords(holder);

    if(cords.second == 5){
        log(Debug, "holder is the bottom row");
        return true;
    }

    if(_grid->getSquare(cords.first, cords.second+1)->bit() != nullptr){
        log(Debug, "holder has stone below it");
        return true;
    }
    log(Debug, "holder is not lowest");
    return false;

}

bool Connect4::actionForEmptyHolder(BitHolder &holder){
    if (holder.bit())
        return false;

    if(!isLowest(holder)){
        std::pair<int, int> cords = getHolderCords(holder);
        
        return actionForEmptyHolder(*_grid->getSquare(cords.first, cords.second+1));
    }

    
    
    Bit *bit = PieceForPlayer(getCurrentPlayer()->playerNumber()%2);
    if (bit) {
        bit->setPosition(holder.getPosition());
        holder.setBit(bit);
        endTurn();
        int currPlayerDebug = getCurrentPlayer()->playerNumber();
        return true;
    }
    return false;
}
bool Connect4::canBitMoveFrom(Bit &bit, BitHolder &src){
    return false;
}
bool Connect4::canBitMoveFromTo(Bit &bit, BitHolder &src, BitHolder &dst){
    return false;
}
void Connect4::stopGame(){
    _grid->forEachSquare([](ChessSquare* square, int x, int y) {
        square->destroyBit();
    });
}

void Connect4::updateAI(){
    if(aiPlayer != getCurrentPlayer()->playerNumber()) return;

    for(int i = 0; i < 7; ++i)
        for(int j = 0; j < 6; ++j)
            if(actionForEmptyHolder(*_grid->getSquare(i, j))) return;
        
    
}
