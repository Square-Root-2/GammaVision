#include "MoveComparator.h"

MoveComparator::MoveComparator(unordered_set<Move>& killerMoves) {
	this->currentDepth = currentDepth;
	this->killerMoves = killerMoves;
}
bool MoveComparator::operator()(Move move1, Move move2) {
	if (!move2.isQuiet())
		return false;
	if (!move1.isQuiet())
		return true;
	unordered_set<Move>::iterator it1 = killerMoves.find(move1);
	unordered_set<Move>::iterator it2 = killerMoves.find(move2);
	if (it1 == killerMoves.end())
		return false;
	if (it2 == killerMoves.end())
		return true;
	return false;
}
