#include "Engine.h"
#include "Evaluator.h"
#include "Move.h"
#include "MoveType.h"
#include <iostream>

using namespace std;

int main() {
    Engine e;
    while (true) {
        string FEN;
        getline(cin, FEN);
        tuple<Move, double, int> optimalMove = e.getOptimalMove(FEN, 30);
        cout << "Move: " << char(get<0>(optimalMove).getBeginColumn() + 'a') << " " << 8 - get<0>(optimalMove).getBeginRow() << " " << char(get<0>(optimalMove).getEndColumn() + 'a') << " " << 8 - get<0>(optimalMove).getEndRow() << " ";
        if (get<0>(optimalMove).getType() == MoveType::PROMOTION_TO_BISHOP)
            cout << "Promotion to Bishop";
        else if (get<0>(optimalMove).getType() == MoveType::PROMOTION_TO_KNIGHT)
            cout << "Promotion to Knight";
        else if (get<0>(optimalMove).getType() == MoveType::PROMOTION_TO_QUEEN)
            cout << "Promotion to Queen";
        else if (get<0>(optimalMove).getType() == MoveType::PROMOTION_TO_ROOK)
            cout << "Promotion to Rook";
        cout << "\n";
        cout << "Evaluation: ";
        if (abs(get<1>(optimalMove)) > Evaluator::getMaximumEvaluation())
            cout << "Mate in " << (Evaluator::getMaximumEvaluation() + Engine::getMaximumNegamaxDepth() + Engine::getMaximumQuiescenceDepth() + 1) - abs(get<1>(optimalMove)) << "\n";
        else
            cout << get<1>(optimalMove) << "\n";
        cout << "Depth: " << get<2>(optimalMove) << "\n\n";
    }
}
