#include "Engine.h"
#include <iostream>

using namespace std;

int main() {
    Engine e;
    while (true) {
        string FEN;
        getline(cin, FEN);
        tuple<int, int, int, int, int, double, int, int> optimalMove = e.getOptimalMove(FEN, 20);
        cout << "Move: " << char(get<1>(optimalMove) + 'a') << " " << 8 - get<0>(optimalMove) << " " << char(get<3>(optimalMove) + 'a') << " " << 8 - get<2>(optimalMove) << " " << ((abs(get<4>(optimalMove) - 1.5) <= 1.5) ? to_string(get<4>(optimalMove)) : "") << "\n";
        cout << "Evaluation: " << get<5>(optimalMove) << "\n";
        cout << "Moves Until Checkmate/Stalemate: " << get<6>(optimalMove) << "\n";
        cout << "Depth: " << get<7>(optimalMove) << "\n\n";
    }
}
