#include "Connect4.h"


int Connect4::assessWinPattern(const Board& board, Color color, int patternIdx){
    if(winningPatterns[patternIdx] & board.pieces[!color] != 0)
        return 0;

    
    int piecesMatched = std::popcount(board.pieces[color] & winningPatterns[patternIdx]);

    return piecesMatched == 4? 9999 : 1 << piecesMatched;
}


int Connect4::evalBoardState(const Board& board, Color color){
    int score = 0;
    int oppScore = 0;

    for(int i = 0; i < 69; ++i){
        score += assessWinPattern(board, color, i);
        oppScore += assessWinPattern(board, static_cast<Color>(!color), i);
    }

    return score - (oppScore/1.2);

}

std::pair<int, int> Connect4::cordsBoardToGrid(int boardIdx){
    return {boardIdx%7, boardIdx/7};
}

int Connect4::cordsGridToBoard(std::pair<int, int> gridCords){
    return gridCords.first + gridCords.second*7 ;
}



Connect4::Connect4(int aiPlayer){
    this->aiPlayer = aiPlayer;
    _grid = new Grid(7,6);
    _board.pieces[RED] = 0;
    _board.pieces[YELLOW] = 0;
    
}
Connect4::~Connect4(){
    delete _grid;
}

Player* Connect4::checkForWinner(){
    for(int i = 0; i < 69; ++i)
        if(std::popcount(winningPatterns[i] & _board.pieces[RED]) >= 4){
            log(Debug, "Win pattern " + numToStrBin(winningPatterns[i]));
            return getPlayerAt(RED);
        }

    for(int i = 0; i < 69; ++i)
        if(std::popcount(winningPatterns[i] & _board.pieces[YELLOW]) >= 4){
            log(Debug, "Win pattern " + numToStrBin(winningPatterns[i]));
            return getPlayerAt(YELLOW);
        }

    return nullptr;
    
}

bool Connect4::checkForDraw(){
    if(!checkForWinner() && std::popcount(_board.pieces[RED] & _board.pieces[YELLOW]) == 42) return true;

    return false;
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
              //  log(Debug, "holder cords" + numToStr(i)+", "+numToStr(j));
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
        //log(Debug, "holder is the bottom row");
        return true;
    }

    if(_grid->getSquare(cords.first, cords.second+1)->bit() != nullptr){
       // log(Debug, "holder has stone below it");
        return true;
    }
    //log(Debug, "holder is not lowest");
    return false;

}

bool Connect4::actionForEmptyHolder(BitHolder &holder){
    if (holder.bit())
        return false;

    if(!isLowest(holder)){
        std::pair<int, int> cords = getHolderCords(holder);
        
        return actionForEmptyHolder(*_grid->getSquare(cords.first, cords.second+1));
    }

    
    
    Bit *bit = PieceForPlayer(getCurrentPlayer()->playerNumber());
    if (bit) {
        bit->setPosition(holder.getPosition());
        holder.setBit(bit);
        setBitInPlace(_board.pieces[getCurrentPlayer()->playerNumber()], cordsGridToBoard(getHolderCords(holder)), true);
        //log(Debug, numToStr(cordsGridToBoard(getHolderCords(holder))));
        log(Error, "RED: "+numToStrBin(_board.pieces[RED]));
        log(Warn, "YEL: "+numToStrBin(_board.pieces[YELLOW]));
        endTurn();
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
    _board.pieces[RED] = 0;
    _board.pieces[YELLOW] = 0;
}

void Connect4::updateAI(){
    if(aiPlayer != getCurrentPlayer()->playerNumber()) return;

    for(int i = 0; i < 7; ++i)
        for(int j = 0; j < 6; ++j)
            if(actionForEmptyHolder(*_grid->getSquare(i, j))) return;
        
    
}


int Connect4::negamax(Board board, Color player, int a, int b, int d)
{   int score = evalBoardState(board, player);
    if(score > 999) {
        return score;
    }
    if(d == 0){
        return score;
    }
}