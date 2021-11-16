#include "Engine.h"
#include "Evaluation.h"
#include "Move.h"
#include <iostream>

using namespace std;

int main() {
    Engine e;
    while (true) {
        string FEN;
        getline(cin, FEN);
        tuple<Move, Evaluation, int> optimalMove = e.getOptimalMove(FEN, 60);
        cout << "Move: " << char(get<0>(optimalMove).getBeginColumn() + 'a') << " " << 8 - get<0>(optimalMove).getBeginRow() << " " << char(get<0>(optimalMove).getEndColumn() + 'a') << " " << 8 - get<0>(optimalMove).getEndRow() << "\n";
        cout << "Pawn Advantage: " << get<1>(optimalMove).getPawns() << "\n";
        cout << "Moves Until Checkmate/Stalemate: " << get<1>(optimalMove).getMoves() << "\n";
        cout << "Depth: " << get<2>(optimalMove) << "\n\n";
        vector<Move> principalVariation = e.getPrincipalVariation(FEN, get<2>(optimalMove));
        cout << "Principal Variation: " << "\n";
        for (Move move: principalVariation)
            cout << "Move: " << char(move.getBeginColumn() + 'a') << " " << 8 - move.getBeginRow() << " " << char(move.getEndColumn() + 'a') << " " << 8 - move.getEndRow() << "\n";
        cout << "\n";
    }
}
