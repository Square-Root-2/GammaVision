#ifndef _MOVE_H_
#define _MOVE_H_

#pragma once

#include <functional>
#include "MoveType.h"

class Move 
{
	char aggressor, victim;
	int beginColumn, beginRow, endColumn, endRow;
	MoveType moveType;
public:
	Move();
	Move(int beginRow, int beginColumn, int endRow, int endColumn, MoveType moveType, char aggressor, char victim);
	Move(MoveType moveType);
	bool operator==(const Move& move) const;
	char getAggressor() const;
	int getBeginColumn() const;
	int getBeginRow() const;
	int getEndColumn() const;
	int getEndRow() const;
	MoveType getMoveType() const;
	char getVictim() const;
	bool isCapture() const;
	bool isPromotion() const;
	bool isQuiet() const;
};

inline Move::Move() : Move(MoveType::NO_MOVE) 
{

}
inline Move::Move(int beginRow, int beginColumn, int endRow, int endColumn, MoveType moveType, char aggressor, char victim) 
{
	this->beginRow = beginRow;
	this->beginColumn = beginColumn;
	this->endRow = endRow;
	this->endColumn = endColumn;
	this->moveType = moveType;
	this->aggressor = aggressor;
	this->victim = victim;
}
inline Move::Move(MoveType moveType)
{
	this->beginRow = 0;
	this->beginColumn = 0;
	this->endRow = 0;
	this->endColumn = 0;
	this->moveType = moveType;
	this->aggressor = '.';
	this->victim = '.';
}
inline bool Move::operator==(const Move& move) const
{
	return beginRow == move.getBeginRow() && beginColumn == move.getBeginColumn() && endRow == move.getEndRow() && endColumn == move.getEndColumn() && moveType == move.getMoveType();
}
inline char Move::getAggressor() const 
{
	return aggressor;
}
inline int Move::getBeginColumn() const 
{
	return beginColumn;
}
inline int Move::getBeginRow() const 
{
	return beginRow;
}
inline int Move::getEndColumn() const 
{
	return endColumn;
}
inline int Move::getEndRow() const 
{
	return endRow;
}
inline MoveType Move::getMoveType() const 
{
	return moveType;
}
inline char Move::getVictim() const 
{
	return victim;
}
inline bool Move::isCapture() const 
{
	return victim != '.';
}
inline bool Move::isPromotion() const 
{
	return getMoveType() == MoveType::PROMOTION_TO_BISHOP || getMoveType() == MoveType::PROMOTION_TO_KNIGHT || getMoveType() == MoveType::PROMOTION_TO_QUEEN || getMoveType() == MoveType::PROMOTION_TO_ROOK;
}
inline bool Move::isQuiet() const 
{
	return !isCapture() && !isPromotion();
}

namespace std 
{
	template<> struct hash<Move> 
	{
		int operator()(Move move) const;
	};
}

inline int std::hash<Move>::operator()(Move move) const 
{
	return 512 * move.getBeginRow() + 64 * move.getBeginColumn() + 8 * move.getEndRow() + move.getEndColumn();
}

#endif //_MOVE_H_
