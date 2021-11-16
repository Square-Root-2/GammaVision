#include "MoveComparer.h"

MoveComparer::MoveComparer(map<pair<tuple<string, bool, int, int>, int>, Evaluation>& evaluations, State state, int depths) {
	this->evaluations = evaluations;
	this->state = state;
	this->depths = depths;
}
bool MoveComparer::operator()(Move move1, Move move2) {
	tuple<string, bool, int, int> hashCode = state.getHashCode();
	state.makeMove(move1);
	map<pair<tuple<string, bool, int, int>, int>, Evaluation>::iterator it1 = evaluations.find(pair<tuple<string, bool, int, int>, int>(state.getHashCode(), depths - 1));
	state.setHashCode(hashCode);
	state.makeMove(move2);
	map<pair<tuple<string, bool, int, int>, int>, Evaluation>::iterator it2 = evaluations.find(pair<tuple<string, bool, int, int>, int>(state.getHashCode(), depths - 1));
	state.setHashCode(hashCode);
	if (it1 == evaluations.end())
		return false;
	if (it2 == evaluations.end())
		return true; 
	return Evaluation(-it1->second.getPawns(), abs(it1->second.getPawns()) == INFINITY ? it1->second.getMoves() + 1 : INT32_MAX) > Evaluation(-it2->second.getPawns(), abs(it2->second.getPawns()) == INFINITY ? it2->second.getMoves() + 1 : INT32_MAX);
}
