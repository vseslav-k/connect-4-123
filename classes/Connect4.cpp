#include "Connect4.h"
#include "C:\Libraries\imgui\Timer\Timer.h"
Timer timer = Timer();
int Connect4::assessWinPattern(const Board& board, Color color, int patternIdx){
    if((winningPatterns[patternIdx] & board.pieces[!color]) != 0)
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

template<class typ>
inline std::string formatBoard(typ board, int x = 0){
    std::string res;
    res.reserve(75);
    std::string bin = numToStrBin(board);

    for(int i = 0; i < 64; ++i){
        if(i > 0 && i % 7 == 0 && i < 42) res.push_back('\n');
        if(i == 42) res.push_back('\n');
        res.push_back(bin[i]);
    }
    res += "\n\n";

    return res;

}



Connect4::Connect4(int aiPlayer){
    this->aiPlayer = aiPlayer;
    _grid = new Grid(7,6);
    _board.pieces[RED] = 0;
    _board.pieces[YELLOW] = 0;

    std::string boards = ptrToStr(winningPatterns.data(), winningPatterns.size(), formatBoard, "\n");

    //log(Debug, boards);
    
}
Connect4::~Connect4(){
    delete _grid;
}




Player* Connect4::checkForWinner(){
    for(int i = 0; i < 69; ++i)
        if(std::popcount(winningPatterns[i] & _board.pieces[RED]) >= 4){
            return getPlayerAt(RED);
        }

    for(int i = 0; i < 69; ++i)
        if(std::popcount(winningPatterns[i] & _board.pieces[YELLOW]) >= 4){
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
    return "000000000000000000000000000000000000000000\n000000000000000000000000000000000000000000";
}
std::string Connect4::stateString(){
    return formatBoard(_board.pieces[RED]) + formatBoard(_board.pieces[YELLOW]);
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
        //log(Error, "RED: "+numToStrBin(_board.pieces[RED]));
       //log(Warn, "YEL: "+numToStrBin(_board.pieces[YELLOW]));
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

    int bestMoveIdx = -1;

    int bestScore = -999999999;
    int res = -999999;

    int d = 7;

    d += std::popcount(_board.pieces[RED] | _board.pieces[YELLOW])/(7);

    log(Debug, "d= "+numToStr(d));
    timer.setPt("AI Thinking Start");
    for(int i = 0; i < 42; ++i){
        if(!moveIsLegal((_board.pieces[RED] | _board.pieces[YELLOW]), i)) continue;

        setBitInPlace(_board.pieces[aiPlayer], i, true);
        res = -negamax(static_cast<Color>(!aiPlayer), -99999999999, 99999999999, d);
        setBitInPlace(_board.pieces[aiPlayer], i, false);

        if(res > bestScore){
            bestScore = res;
            bestMoveIdx = i;
        }

    }
    timer.setPt("AI Thinking End");
    log(Info, "AI thought for: "+fltToStr(timer.milliPassed("AI Thinking Start", "AI Thinking End")));

    if(bestMoveIdx == -1) {
        log(Error, "No legal moves available");
        return;
    }
    std::pair<int, int> bestMoveCords = cordsBoardToGrid(bestMoveIdx);
    
    actionForEmptyHolder(*_grid->getSquare(bestMoveCords.first, bestMoveCords.second));
        
    //log(Debug, "Action made at " + numToStr(bestMoveIdx) + "("+numToStr(bestMoveCords.first)+","+numToStr(bestMoveCords.second)+")");
}


bool Connect4::moveIsLegal(uint64_t board, int i){  
    if(getBit(board, i) == 1) return false;

    if( i >= 35) return true;

    if(getBit(board, i+7) == 1) return true;

    return false;
}


int Connect4::legalMoveInCol(int col){
    int moveIdx = 41 - (7-col);
    
    while(getBit(_board.pieces[RED] | _board.pieces[YELLOW], moveIdx) == 1 && moveIdx > 0)
        moveIdx -=7;
    
    return moveIdx;

    /*
    
    00 01 02 03 04 05 06 
    07 08 09 10 11 12 13 
    14 15 16 17 18 19 20 
    21 22 23 24 25 26 27 
    28 29 30 31 32 33 34 
    35 36 37 38 39 40 41
    */
}

int Connect4::negamax(Color player, int a, int b, int d)
{   int score = evalBoardState(_board, player);

    Player* winner = checkForWinner();

    if(winner){
        if(winner->playerNumber() == player){ 
            return 9999999/(d+1);
        }
        if(winner->playerNumber() == !player){ 
            return -99999*(d+1);
        }
    }



    if(checkForDraw()){
        return 0;
    }
    if(d <= 0){
        return score;
    }

    int bestScore = -9999999;
    int res = -9999;

    for(int i = 0; i < 42; ++i){
        if(!moveIsLegal(_board.pieces[RED] | _board.pieces[YELLOW], i)) continue;

        setBitInPlace(_board.pieces[player], i, true);
        res = -negamax(static_cast<Color>(!player), -b, -a, d-1);
        setBitInPlace(_board.pieces[player], i, false);

        bestScore = std::max(bestScore, res);
        a = std::max(a, res);

        if(a >= b) return bestScore;

    }

    return bestScore;


}