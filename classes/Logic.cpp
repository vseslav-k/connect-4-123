#include <array>
#include <bit>
using uint64_t = unsigned long long;


enum Color{
    RED = 0,
    YELLOW = 1
};

struct Board{
    std::array<uint64_t, 2> pieces;
};





constexpr std::array<uint64_t, 69> calcWinningPatterns(){ 
    std::array<uint64_t, 69> winningPatterns{};

    winningPatterns[0] = (0b1111 << 60);

    for(int i = 1; i < 4*6;++i)
        winningPatterns[i] = winningPatterns[i-1] >> (i%4? 4 :1);

    winningPatterns[24] = (0b100000010000001000000100000010000001000000<<22);
    
    for(int i = 25; i < 24+3*7; ++i)
        winningPatterns[i] = winningPatterns[i-1] >> 1;
    
    winningPatterns[45] = (0b100000001000000010000000100000000000000000<<22);

    for(int i = 46; i < 45+6*2; ++i)
        winningPatterns[i] = winningPatterns[i-1] >> (i%4? 4 :1);

    winningPatterns[45] = (0b000000000000000000001000001000001000001000);

    for(int i = 46; i < 69; ++i)
        winningPatterns[i] = winningPatterns[i-1] << (i%4? 4 :1);
    

    return winningPatterns;

    //horizontal = 0-23
    //vertical = 24-44
    //diagonal / = 45-56
    //diagonal \ = 57-68
}

constexpr std::array<uint64_t, 69> winningPatterns = calcWinningPatterns();

int assessWinPattern(const Board& board, Color color, int patternIdx){
    if(winningPatterns[patternIdx] & board.pieces[!color] != 0)
        return 0;

    
    int piecesMatched = __builtin_popcount(board.pieces[color] & winningPatterns[patternIdx]);

    return piecesMatched == 4? 9999 : 1 << piecesMatched;
}


int evalBoardState(const Board& board, Color color){
    int score = 0;
    int oppScore = 0;

    for(int i = 0; i < 69; ++i){
        score += assessWinPattern(board, color, i);
        oppScore += assessWinPattern(board, static_cast<Color>(!color), i);
    }

    return score - (oppScore/1.2)

}
