#include "Connect4.h"
#include "C:\Libraries\imgui\Timer\Timer.h"
#include <random>
Timer timer = Timer();

inline bool random01() {
    static thread_local std::mt19937 rng{std::random_device{}()};
    static thread_local std::uniform_int_distribution<int> dist(0, 1);
    return dist(rng);
}


//depends on logger
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

    ai2GoesFirst = random01();
    
}
Connect4::~Connect4(){
    delete _grid;
}


void Connect4::setUpBoard() {
    setNumberOfPlayers(2);
    if(aiPlayer == 1 || aiPlayer == 2)
        setAIPlayer(aiPlayer-1);
    if(aiPlayer == 3){
        setAIPlayer(0);
        setAIPlayer(1);
        _gameOptions.AIvsAI = true;
    }
    
    _gameOptions.rowX = 7;
    _gameOptions.rowY = 6;
    _grid->initializeSquares(80, "boardsquare.png");

    
    startGame();
}

Player* Connect4::checkForWinner(){
    for(int i = 0; i < 69; ++i)
        if((WINNING_PATTERNS[i] & _board.pieces[RED]) == WINNING_PATTERNS[i]){
            return getPlayerAt(RED);
        }

    for(int i = 0; i < 69; ++i)
        if((WINNING_PATTERNS[i] & _board.pieces[YELLOW]) == WINNING_PATTERNS[i]){
            return getPlayerAt(YELLOW);
        }

    return nullptr;
    
}

bool Connect4::checkForDraw(){
    return (!checkForWinner() && boardIsFull());

}

std::string Connect4::initialStateString(){
    return "000000000000000000000000000000000000000000\n000000000000000000000000000000000000000000";
}
std::string Connect4::stateString(){
    return formatBoard(_board.pieces[RED]) + formatBoard(_board.pieces[YELLOW]);
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
        endTurn();
        return true;
    }
    return false;
}

void Connect4::stopGame(){
    _grid->forEachSquare([](ChessSquare* square, int x, int y) {
        square->destroyBit();
    });
    _board.pieces[RED] = 0;
    _board.pieces[YELLOW] = 0;
    ai2GoesFirst = random01();
}


void Connect4::updateAI(){

    if(checkForWinner() || checkForDraw()) return;

    
    bool me = currPlayer();

    log(Debug, "aiPlayer "+numToStr(aiPlayer));
    log(Debug, "ai2GoesFirst "+numToStr(static_cast<int>(ai2GoesFirst)));
    log(Debug, "me "+numToStr(static_cast<int>(me)));

    if(aiPlayer == 3 && ai2GoesFirst && me == 0){
        updateAI2();
        return;
    }

    //log(Debug, "AI Player's turn? " + numToStr(static_cast<int>(me == aiPlayer)));

    if(me != getCurrentPlayer()->playerNumber()) return;

    int bestMoveIdx = -1;

    int bestScore = -MATE*100;
    int res = -MATE/10;

    int d = DEPTH_AT_TURN[(this->_turns.size()-1)/2];



    log(Debug, "Turn: " + numToStr(this->_turns.size()));
    log(Debug, "Depth: " + numToStr(d));
    timer.setPt("AI Thinking Start");
    for(int i = 0; i < 42; ++i){
        if(!moveIsLegal((_board.pieces[RED] | _board.pieces[YELLOW]), i)) continue;

        setBitInPlace(_board.pieces[me], i, true);
        res = -negamax(static_cast<Color>(!me), -MATE, MATE, d);
        setBitInPlace(_board.pieces[me], i, false);

        if(res > bestScore){
            bestScore = res;
            bestMoveIdx = i;
        }

    }
    timer.setPt("AI Thinking End");
    log(Info, "Thinking Time: "+fltToStr(timer.milliPassed("AI Thinking Start", "AI Thinking End")));

    if(bestMoveIdx == -1 && !boardIsFull()) {
        log(Error, "No legal moves available");
        return;
    }
    std::pair<int, int> bestMoveCords = cordsBoardToGrid(bestMoveIdx);
    
    actionForEmptyHolder(*_grid->getSquare(bestMoveCords.first, bestMoveCords.second));
        
    //log(Debug, "Action made at " + numToStr(bestMoveIdx) + "("+numToStr(bestMoveCords.first)+","+numToStr(bestMoveCords.second)+")");
}

int Connect4::negamax(const Color player, int a, const int b, const int d){   

    switch(comboWon(_board.pieces[player]) *1 + comboWon(_board.pieces[!player]) *2 + boardIsFull()*3){
        case 1: return MATE/(d+1);
        case 2: return -MATE*(d+1);
        case 3: return 0;
        case 4: return MATE/(d+1);
        case 5: return -MATE*(d+1);
    }

    /* equivalent to
    if(comboWon(_board.pieces[player])){ 
        return MATE/(d+1);
    }
    if(comboWon(_board.pieces[!player])){ 
        return -MATE*(d+1);
    }
    if(boardIsFull()){
        return 0;
    }
    */
    if(d <= 0){
        return evalBoardState(_board, player);
    }

    int bestScore = -MATE;
    int res = -MATE/10;

    for(int i = 0; i < 42; ++i){
        int bestMove = SORTED_CELL_VALUES[i];

        if(!moveIsLegal(_board.pieces[RED] | _board.pieces[YELLOW], bestMove)) continue;
        setBitInPlace(_board.pieces[player], bestMove, true);
        res = -negamax(static_cast<Color>(!player), -b, -a, d-1);
        setBitInPlace(_board.pieces[player], bestMove, false);

        bestScore = std::max(bestScore, res);
        a = std::max(a, res);

        if(a >= b) return bestScore;

    }

    return bestScore;


}


int Connect4::assessWinPattern(const Board& board, const Color color, const int patternIdx) const{
    if((WINNING_PATTERNS[patternIdx] & board.pieces[!color]) != 0)
        return 0;

    
    int piecesMatched = std::popcount(board.pieces[color] & WINNING_PATTERNS[patternIdx]);

    return piecesMatched == 4? MATE : 1 << piecesMatched;
}


int Connect4::evalBoardState(const Board& board, const Color color) const{
    int score = 0;
    int oppScore = 0;

    for(int i = 0; i < 69; ++i){
        score += assessWinPattern(board, color, i);
        oppScore += assessWinPattern(board, static_cast<Color>(!color), i);
    }

    return score - (oppScore/1.2);

}


#include "Connect4Bot2.h"


bool Connect4::boardIsFull() const{
    return ((_board.pieces[RED] | _board.pieces[YELLOW]) == UTIL_PATTERNS[FULL]);
}


bool Connect4::comboWon(const uint64_t piecies) const{


    return 
    
    // Horizontal (stride 1)
    (piecies & (piecies << 1) & (piecies << 2) & (piecies << 3) & UTIL_PATTERNS[HORIZONTAL_START]) ||

    // Vertical (stride 7)
    (piecies & (piecies << 7) & (piecies << 14) & (piecies << 21) & UTIL_PATTERNS[VERTICAL_START]) ||


    // Diagonal LR '\' (stride 8)
    (piecies & (piecies << 8) & (piecies << 16) & (piecies << 24) & UTIL_PATTERNS[DIAGONAL_LR_START]) ||
    
    // Diagonal RL '/' (stride 6)
    (piecies & (piecies << 6) & (piecies << 12) & (piecies << 18) & UTIL_PATTERNS[DIAGONAL_RL_START]);
    
}


bool Connect4::moveIsLegal(const uint64_t board, const int i) const{

    return (getBit(board, i) == 0 && ( (i>=35) || ((getBit(board, i+7) == 1))));

}

bool Connect4::forcedToPlay(const Color& col, const int idx) const{
    return (42 - this->_turns.size())/2;
}




Bit* Connect4::PieceForPlayer(int playerNumber){
    Bit *bit = new Bit();
    // should possibly be cached from player class?
    bit->LoadTextureFromFile(playerNumber == 1 ? "yellow.png" : "red.png");
    bit->setOwner(getPlayerAt(playerNumber == 1 ? 1 : 0));
    return bit;
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



std::pair<int, int> Connect4::cordsBoardToGrid(int boardIdx) {
    return {boardIdx%7, boardIdx/7};
}

int Connect4::cordsGridToBoard(std::pair<int, int> gridCords){
    return gridCords.first + gridCords.second*7 ;
}

std::array<int, 42> Connect4::makeHeatMap(const uint64_t *arr, const int len){
    std::array<int, 42> res{};

    for(int i = 0; i < len; ++i){
        for(int j = 0; j < 42; ++j)
            res[j] += getBit(arr[i], j);
        
    }

    return res;
}
