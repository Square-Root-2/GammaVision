#include "MoveComparer.h"

MoveComparer::MoveComparer(map<pair<tuple<string, bool, int, int>, int>, pair<Move, Evaluation>>& principalVariation, State state, int depths) {
	this->principalVariation = principalVariation;
	this->state = state;
	this->depths = depths;
}
bool MoveComparer::operator()(Move move1, Move move2) {
	return false;
	tuple<string, bool, int, int> hashCode = state.getHashCode();
	state.makeMove(move1);
	map<pair<tuple<string, bool, int, int>, int>, pair<Move, Evaluation>>::iterator it1 = principalVariation.find(pair<tuple<string, bool, int, int>, int>(state.getHashCode(), depths - 1));
	state.setHashCode(hashCode);
	state.makeMove(move2);
	map<pair<tuple<string, bool, int, int>, int>, pair<Move, Evaluation>>::iterator it2 = principalVariation.find(pair<tuple<string, bool, int, int>, int>(state.getHashCode(), depths - 1));
	state.setHashCode(hashCode);
	if (it1 == principalVariation.end())
		return false;
	if (it2 == principalVariation.end())
		return true; 
	return Evaluation(-it1->second.second.getPawns(), abs(it1->second.second.getPawns()) == INFINITY ? it1->second.second.getMoves() + 1 : INT32_MAX) > Evaluation(-it2->second.second.getPawns(), abs(it2->second.second.getPawns()) == INFINITY ? it2->second.second.getMoves() + 1 : INT32_MAX);
}
