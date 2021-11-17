#include "Move.h"

Move::Move(int beginRow, int beginColumn, int endRow, int endColumn, MoveType type, bool capture) {
	this->beginRow = beginRow;
	this->beginColumn = beginColumn;
	this->endRow = endRow;
	this->endColumn = endColumn;
	this->type = type;
	this->capture = capture;
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
bool Move::isCapture() {
	return capture;
}
