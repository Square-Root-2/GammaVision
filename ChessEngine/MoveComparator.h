#pragma once

#include "Move.h"
#include <unordered_set>

using namespace std;

class MoveComparator {
	Move hashMove;
	unordered_set<Move> killerMoves;
	bool compareCaptures(Move& move1, Move& move2);
	bool comparePromotions(Move& move1, Move& move2);
	bool compareQuietMoves(Move& move1, Move& move2);
public:
	MoveComparator(unordered_set<Move>& killerMoves);
	MoveComparator(unordered_set<Move>& killerMoves, Move& hashMove);
	bool operator()(Move& move1, Move& move2);
};
