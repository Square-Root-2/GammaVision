#include "Move.h"

bool Move::isCapture() {
	return capture;
}
bool Move::isPromotion() {
	return getType() == MoveType::PROMOTION_TO_BISHOP || getType() == MoveType::PROMOTION_TO_KNIGHT || getType() == MoveType::PROMOTION_TO_QUEEN || getType() == MoveType::PROMOTION_TO_ROOK;
}
Move::Move() {

}
Move::Move(int beginRow, int beginColumn, int endRow, int endColumn, MoveType type, bool capture) {
	this->beginRow = beginRow;
	this->beginColumn = beginColumn;
	this->endRow = endRow;
	this->endColumn = endColumn;
	this->type = type;
	this->capture = capture;
}
bool Move::operator==(Move move) const {
	return beginRow == move.getBeginRow() && beginColumn == move.getBeginColumn() && endRow == move.getEndRow() && endColumn == move.getEndColumn();
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
bool Move::isQuiet() {
	return !isCapture() && !isPromotion();
}
int std::hash<Move>::operator()(Move move) const {
	return 512 * move.getBeginRow() + 64 * move.getBeginColumn() + 8 * move.getEndRow() + move.getEndColumn();
}
