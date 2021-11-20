#pragma once

#include "Move.h"
#include <unordered_set>

using namespace std;

class MoveComparator {
	unordered_set<Move> killerMoves;
	bool compareCaptures(Move move1, Move move2);
	bool comparePromotions(Move move1, Move move2);
	bool compareQuietMoves(Move move1, Move move2);
public:
	MoveComparator(unordered_set<Move>& killerMoves);
	bool operator()(Move move1, Move move2);
};
