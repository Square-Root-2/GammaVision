#include "Evaluator.h"
#include "MoveComparator.h"
#include <unordered_map>

bool MoveComparator::compareCaptures(Move move1, Move move2) {
	if (Evaluator::getPawnEquivalent(move1.getVictim()) != Evaluator::getPawnEquivalent(move2.getVictim()))
		return Evaluator::getPawnEquivalent(move1.getVictim()) > Evaluator::getPawnEquivalent(move2.getVictim());
	if (Evaluator::getPawnEquivalent(move1.getAggressor()) != Evaluator::getPawnEquivalent(move2.getAggressor()))
		return Evaluator::getPawnEquivalent(move1.getAggressor()) < Evaluator::getPawnEquivalent(move2.getAggressor());
	return false;
}
bool MoveComparator::comparePromotions(Move move1, Move move2) {
	return move1.getType() < move2.getType();
}
bool MoveComparator::compareQuietMoves(Move move1, Move move2) {
	unordered_set<Move>::iterator it1 = killerMoves.find(move1);
	unordered_set<Move>::iterator it2 = killerMoves.find(move2);
	if (it1 == killerMoves.end())
		return false;
	if (it2 == killerMoves.end())
		return true;
	return false;
}
MoveComparator::MoveComparator(unordered_set<Move>& killerMoves) {
	this->killerMoves = killerMoves;
}
bool MoveComparator::operator()(Move move1, Move move2) {
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
