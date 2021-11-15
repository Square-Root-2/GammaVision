#pragma once

#include "MoveType.h"

class Move {
	int beginColumn, beingRow, endColumn, endRow;
	MoveType type;
public:
	Move() {};
	Move(int beginRow, int beginColumn, int endRow, int endColumn, MoveType type);
	int getBeginColumn();
	int getBeginRow();
	int getEndColumn();
	int getEndRow();
	MoveType getType();
};
