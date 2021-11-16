#include "Move.h"

Move::Move() {
	beginRow = 0;
	beginColumn = 0;
	endRow = 0;
	endColumn = 0;
	type = MoveType::DEFAULT;
}
Move::Move(int beginRow, int beginColumn, int endRow, int endColumn, MoveType type) {
	this->beginRow = beginRow;
	this->beginColumn = beginColumn;
	this->endRow = endRow;
	this->endColumn = endColumn;
	this->type = type;
}
int Move::getBeginColumn() {
	return beginColumn;
}
int Move::getBeginRow() {
	return beginRow;
}
int Move::getEndColumn() {
	return endColumn;
}
int Move::getEndRow() {
	return endRow;
}
MoveType Move::getType() {
	return type;
}
