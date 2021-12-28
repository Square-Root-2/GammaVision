#ifndef _MOVE_COMPARATOR_H_
#define _MOVE_COMPARATOR_H_

#pragma once

#include "Evaluator.h"
#include "Move.h"
#include <unordered_set>

using namespace std;

class MoveComparator 
{
	Move hashMove;
	unordered_set<Move> killerMoves;
	bool compareCaptures(const Move& move1, const Move& move2) const;
	bool comparePromotions(const Move& move1, const Move& move2) const;
	bool compareQuietMoves(const Move& move1, const Move& move2) const;
public:
	MoveComparator(const unordered_set<Move>& killerMoves);
	MoveComparator(const unordered_set<Move>& killerMoves, const Move& hashMove);
	bool operator()(const Move& move1, const Move& move2) const;
};

inline bool MoveComparator::compareCaptures(const Move& move1, const Move& move2) const 
{
	if (Evaluator::getCentipawnEquivalent(move1.getVictim()) != Evaluator::getCentipawnEquivalent(move2.getVictim()))
		return Evaluator::getCentipawnEquivalent(move1.getVictim()) > Evaluator::getCentipawnEquivalent(move2.getVictim());
	if (Evaluator::getCentipawnEquivalent(move1.getAggressor()) != Evaluator::getCentipawnEquivalent(move2.getAggressor()))
		return Evaluator::getCentipawnEquivalent(move1.getAggressor()) < Evaluator::getCentipawnEquivalent(move2.getAggressor());
	return false;
}
inline bool MoveComparator::comparePromotions(const Move& move1, const Move& move2) const 
{
	return move1.getMoveType() < move2.getMoveType();
}
inline bool MoveComparator::compareQuietMoves(const Move& move1, const Move& move2) const 
{
	unordered_set<Move>::iterator it1 = killerMoves.find(move1);
	unordered_set<Move>::iterator it2 = killerMoves.find(move2);
	if (it1 == killerMoves.end())
		return false;
	if (it2 == killerMoves.end())
		return true;
	return false;
}
inline MoveComparator::MoveComparator(const unordered_set<Move>& killerMoves) 
{
	this->killerMoves = killerMoves;
}
inline MoveComparator::MoveComparator(const unordered_set<Move>& killerMoves, const Move& hashMove) 
{
	this->killerMoves = killerMoves;
	this->hashMove = hashMove;
}
inline bool MoveComparator::operator()(const Move& move1, const Move& move2) const 
{
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

#endif //_MOVE_COMPARATOR_H_
