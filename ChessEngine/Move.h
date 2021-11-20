#pragma once

#include <functional>
#include "MoveType.h"

class Move {
	int beginColumn, beginRow, endColumn, endRow;
	bool capture;
	MoveType type;
	bool isCapture();
	bool isPromotion();
public:
	Move();
	Move(int beginRow, int beginColumn, int endRow, int endColumn, MoveType type, bool capture);
	bool operator==(Move move) const;
	int getBeginColumn();
	int getBeginRow();
	int getEndColumn();
	int getEndRow();
	MoveType getType();
	bool isQuiet();
};

namespace std {
	template<> struct hash<Move> {
		int operator()(Move move) const;
	};
}
