#include <cstdint>
#include "Evaluation.h"
#include <math.h>

Evaluation::Evaluation() {
	pawns = 0;
	moves = 0;
}
Evaluation::Evaluation(double pawns, int moves) {
	this->pawns = pawns;
	this->moves = moves;
}
void Evaluation::operator=(Evaluation evaluation) {
	this->pawns = evaluation.getPawns();
	this->moves = evaluation.getMoves();
}
bool Evaluation::operator>(Evaluation evaluation) {
	if (this->pawns != evaluation.getPawns())
		return this->pawns > evaluation.getPawns();
	if (this->pawns == -INFINITY)
		return evaluation.getMoves() > this->moves;
	if (this->pawns == INFINITY)
		return evaluation.getMoves() < this->moves;
	return false;
}
bool Evaluation::operator==(Evaluation evaluation) {
	return this->pawns == evaluation.getPawns() && this->moves == evaluation.getMoves();
}
bool Evaluation::operator>=(Evaluation evaluation) {
	if (this->pawns == evaluation.getPawns() && this->moves == evaluation.getMoves())
		return true;
	if (this->pawns != evaluation.getPawns())
		return this->pawns > evaluation.getPawns();
	if (this->pawns == -INFINITY)
		return evaluation.getMoves() > this->moves;
	if (this->pawns == INFINITY)
		return evaluation.getMoves() < this->moves;
	return false;
}
int Evaluation::getMoves() {
	return moves;
}
double Evaluation::getPawns() {
	return pawns;
}
void Evaluation::incrementMoves() {
	if (moves == INT32_MAX)
		return;
	moves++;
}
void Evaluation::negatePawns() {
	pawns = -pawns;
}
