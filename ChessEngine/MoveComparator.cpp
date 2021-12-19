#include "Evaluator.h"
#include "MoveComparator.h"

bool MoveComparator::compareCaptures(Move& move1, Move& move2) {
	if (Evaluator::getCentipawnEquivalent(move1.getVictim()) != Evaluator::getCentipawnEquivalent(move2.getVictim()))
		return Evaluator::getCentipawnEquivalent(move1.getVictim()) > Evaluator::getCentipawnEquivalent(move2.getVictim());
	if (Evaluator::getCentipawnEquivalent(move1.getAggressor()) != Evaluator::getCentipawnEquivalent(move2.getAggressor()))
		return Evaluator::getCentipawnEquivalent(move1.getAggressor()) < Evaluator::getCentipawnEquivalent(move2.getAggressor());
	return false;
}
bool MoveComparator::comparePromotions(Move& move1, Move& move2) {
	return move1.getMoveType() < move2.getMoveType();
}
bool MoveComparator::compareQuietMoves(Move& move1, Move& move2) {
	unordered_set<Move>::iterator it1 = killerMoves.find(move1);
	unordered_set<Move>::iterator it2 = killerMoves.find(move2);
	if (it1 == killerMoves.end())
		return false;
	if (it2 == killerMoves.end())
		return true;
	return false;
}
MoveComparator::MoveComparator(unordered_set<Move>& killerMoves, Move& hashMove) {
	this->killerMoves = killerMoves;
	this->hashMove = hashMove;
}
bool MoveComparator::operator()(Move& move1, Move& move2) {
	if (move1 == hashMove)
		return true;
	if (move2 == hashMove)
		return false;
	if (move1.isCapture() && !move2.isCapture())
		return true;
	if (move2.isCapture() && !move1.isCapture())
		return false;
	if (move1.isCapture())
		return compareCaptures(move1, move2);
	if (move1.isPromotion() && !move2.isPromotion())
		return true;
	if (move2.isPromotion() && !move1.isPromotion())
		return false;
	if (move1.isPromotion())
		return comparePromotions(move1, move2);
	return compareQuietMoves(move1, move2);
}
