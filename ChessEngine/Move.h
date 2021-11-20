#pragma once

#include <functional>
#include "MoveType.h"

class Move {
	char aggressor, victim;
	int beginColumn, beginRow, endColumn, endRow;
	MoveType type;
public:
	Move();
	Move(int beginRow, int beginColumn, int endRow, int endColumn, MoveType type, char aggressor, char victim);
	bool operator==(Move move) const;
	char getAggressor();
	int getBeginColumn();
	int getBeginRow();
	int getEndColumn();
	int getEndRow();
	MoveType getType();
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
