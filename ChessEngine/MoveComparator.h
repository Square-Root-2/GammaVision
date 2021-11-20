#pragma once

#include "Move.h"
#include <unordered_set>

using namespace std;

class MoveComparator {
	int currentDepth;
	unordered_set<Move> killerMoves;
public:
	MoveComparator(unordered_set<Move>& killerMoves);
	bool operator()(Move move1, Move move2);
};
