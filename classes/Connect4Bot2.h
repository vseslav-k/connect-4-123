#pragma once

void Connect4::updateAI2(){
    if(checkForWinner() || checkForDraw()) return;
    
    bool me = currPlayer();

    //log(Debug, "AI Player's turn? " + numToStr(static_cast<int>(me == aiPlayer)));

    if(me != getCurrentPlayer()->playerNumber()) return;

    int bestMoveIdx = -1;

    int bestScore = -MATE*100;
    int res = -MATE/10;

    int d = DEPTH_AT_TURN[(this->_turns.size()-1)/2];



    log(Debug, "AI2 Turn: " + numToStr(this->_turns.size()));
    log(Debug, "AI2 Depth: " + numToStr(d));
    timer.setPt("AI2 Thinking Start");
    for(int i = 0; i < 42; ++i){
        if(!moveIsLegal((_board.pieces[RED] | _board.pieces[YELLOW]), i)) continue;

        setBitInPlace(_board.pieces[me], i, true);
        res = -negamax2(static_cast<Color>(!me), -MATE, MATE, d);
        setBitInPlace(_board.pieces[me], i, false);

        if(res > bestScore){
            bestScore = res;
            bestMoveIdx = i;
        }

    }
    timer.setPt("AI2 Thinking End");
    log(Info, "AI2 ThinkTime: "+fltToStr(timer.milliPassed("AI2 Thinking Start", "AI2 Thinking End")));

    if(bestMoveIdx == -1 && !boardIsFull()) {
        log(Error, "AI2 NoLegalMoves");
        return;
    }
    std::pair<int, int> bestMoveCords = cordsBoardToGrid(bestMoveIdx);
    
    actionForEmptyHolder(*_grid->getSquare(bestMoveCords.first, bestMoveCords.second));
        
    //log(Debug, "Action made at " + numToStr(bestMoveIdx) + "("+numToStr(bestMoveCords.first)+","+numToStr(bestMoveCords.second)+")");
}

int Connect4::negamax2(const Color player, int a, const int b, const int d){   

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
        return evalBoardState2(_board, player);
    }

    int bestScore = -MATE;
    int res = -MATE/10;

    for(int i = 0; i < 42; ++i){
        int bestMove = SORTED_CELL_VALUES[i];

        if(!moveIsLegal(_board.pieces[RED] | _board.pieces[YELLOW], bestMove)) continue;
        setBitInPlace(_board.pieces[player], bestMove, true);
        res = -negamax2(static_cast<Color>(!player), -b, -a, d-1);
        setBitInPlace(_board.pieces[player], bestMove, false);

        bestScore = std::max(bestScore, res);
        a = std::max(a, res);

        if(a >= b) return bestScore;

    }

    return bestScore;


}


int Connect4::assessWinPattern2(const Board& board, const Color color, const int patternIdx) const{
    if((WINNING_PATTERNS[patternIdx] & board.pieces[!color]) != 0)
        return 0;

    
    int piecesMatched = std::popcount(board.pieces[color] & WINNING_PATTERNS[patternIdx]);

    return piecesMatched == 4? MATE : 1 << piecesMatched;
}


int Connect4::evalBoardState2(const Board& board, const Color color) const{
    int score = 0;
    int oppScore = 0;

    for(int i = 0; i < 69; ++i){
        score += assessWinPattern2(board, color, i);
        oppScore += assessWinPattern2(board, static_cast<Color>(!color), i);
    }

    return score - (oppScore/1.2);

}