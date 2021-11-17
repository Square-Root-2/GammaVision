#include "Engine.h"
#include "Move.h"
#include <iostream>

using namespace std;

int main() {
    Engine e(100);
    while (true) {
        string FEN;
        getline(cin, FEN);
        tuple<Move, double, int> optimalMove = e.getOptimalMove(FEN, 15);
        cout << "Move: " << char(get<0>(optimalMove).getBeginColumn() + 'a') << " " << 8 - get<0>(optimalMove).getBeginRow() << " " << char(get<0>(optimalMove).getEndColumn() + 'a') << " " << 8 - get<0>(optimalMove).getEndRow() << "\n";
        cout << "Evaluation: " << get<1>(optimalMove) << "\n";
        cout << "Depth: " << get<2>(optimalMove) << "\n\n";
    }
}
