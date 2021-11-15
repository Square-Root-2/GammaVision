#pragma once

class Evaluation {
	int moves;
	double pawns;
public:
	Evaluation();
	Evaluation(double pawns, int moves);
	void operator=(Evaluation evaluation);
	bool operator>(Evaluation evaluation);
	bool operator==(Evaluation evaluation);
	bool operator>=(Evaluation evaluation);
	int getMoves();
	double getPawns();
	void incrementMoves();
	void negatePawns();
};
