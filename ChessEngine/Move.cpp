#include "Move.h"

Move::Move() {
	moveType = MoveType::DEFAULT;
}
Move::Move(int beginRow, int beginColumn, int endRow, int endColumn, MoveType moveType, char aggressor, char victim) {
	this->beginRow = beginRow;
	this->beginColumn = beginColumn;
	this->endRow = endRow;
	this->endColumn = endColumn;
	this->moveType = moveType;
	this->aggressor = aggressor;
	this->victim = victim;
}
bool Move::operator==(Move move) const {
	return beginRow == move.getBeginRow() && beginColumn == move.getBeginColumn() && endRow == move.getEndRow() && endColumn == move.getEndColumn() && moveType == move.getMoveType();
}
char Move::getAggressor() {
	return aggressor;
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
MoveType Move::getMoveType() {
	return moveType;
}
char Move::getVictim() {
	return victim;
}
bool Move::isCapture() {
	return victim != '.';
}
bool Move::isPromotion() {
	return getMoveType() == MoveType::PROMOTION_TO_BISHOP || getMoveType() == MoveType::PROMOTION_TO_KNIGHT || getMoveType() == MoveType::PROMOTION_TO_QUEEN || getMoveType() == MoveType::PROMOTION_TO_ROOK;
}
bool Move::isQuiet() {
	return !isCapture() && !isPromotion();
}
int std::hash<Move>::operator()(Move move) const {
	return 512 * move.getBeginRow() + 64 * move.getBeginColumn() + 8 * move.getEndRow() + move.getEndColumn();
}
