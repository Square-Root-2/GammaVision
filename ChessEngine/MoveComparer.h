#pragma once

#include "Evaluation.h"
#include <map>
#include "State.h"

class MoveComparer {
	int depths;
	map<pair<tuple<string, bool, int, int>, int>, Evaluation> evaluations;
	State state;
public:
	MoveComparer(map<pair<tuple<string, bool, int, int>, int>, Evaluation>& principalVariation, State state, int depths);
	bool operator()(Move move1, Move move2);
};
