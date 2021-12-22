#include "Engine.h"
#include "Evaluator.h"
#include <iostream>
#include "Move.h"
#include "MoveType.h"

using namespace std;

int main() {
    State::initialize();
    Engine engine;
    State state("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    while (true) {
        cout << ">> ";
        string s;
        getline(cin, s);
        int k = s.find_first_of(' ');
        string command = s.substr(0, k);
        string option = s.substr(k + 1, s.size() - (k + 1));
        if (command == "fen")
            state = State(option);
        else if (command == "perft")
            engine.perft(state, stoi(option));
        else if (command == "movetime")
            engine.getOptimalMoveMoveTimeVersion(state, stoi(option));
        else if (command == "depth")
            engine.getOptimalMoveDepthVersion(state, stoi(option));
    }
}
