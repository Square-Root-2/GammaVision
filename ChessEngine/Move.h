#pragma once

#include <functional>
#include "MoveType.h"

class Move {
	char aggressor, victim;
	int beginColumn, beginRow, endColumn, endRow;
	MoveType moveType;
public:
	Move();
	Move(int beginRow, int beginColumn, int endRow, int endColumn, MoveType moveType, char aggressor, char victim);
	bool operator==(Move move) const;
	char getAggressor();
	int getBeginColumn();
	int getBeginRow();
	int getEndColumn();
	int getEndRow();
	MoveType getMoveType();
	char getVictim();
	bool isCapture();
	bool isPromotion();
	bool isQuiet();
};

namespace std {
	template<> struct hash<Move> {
		int operator()(Move move) const;
	};
}
