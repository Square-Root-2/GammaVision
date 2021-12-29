#include "Engine.h"
#include "Evaluator.h"
#include "MoveComparator.h"
#include "MoveGenerator.h"
#include "MoveType.h"
#include <random>

const unordered_map<MoveType, string> Engine::promotionToString =
{
    {MoveType::PROMOTION_TO_KNIGHT, "n"},
    {MoveType::PROMOTION_TO_BISHOP, "b"},
    {MoveType::PROMOTION_TO_ROOK, "r"},
    {MoveType::PROMOTION_TO_QUEEN, "q"}
};
pair<int, stack<Move>*> Engine::negamax(State& state, int currentDepth, int depth, int alpha, int beta, bool isNullMoveOk, bool isActiveColorInCheck) 
{
    stack<Move>* variation = new stack<Move>;
    if (chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - start).count() >= seconds)
        return pair<int, stack<Move>*>(-TIMEOUT, variation);
    unordered_map<State, tuple<int, NodeType, int, Move>>::iterator it = transpositionTable.find(state);
    if (it != transpositionTable.end() && get<0>(it->second) >= min(depth, MAXIMUM_NEGAMAX_DEPTH) - currentDepth) 
    {
        if ((get<1>(it->second) == NodeType::PV || get<1>(it->second) == NodeType::ALL) && get<2>(it->second) <= alpha)
            return pair<int, stack<Move>*>(get<2>(it->second), variation);
        if ((get<1>(it->second) == NodeType::PV || get<1>(it->second) == NodeType::CUT) && get<2>(it->second) >= beta)
            return pair<int, stack<Move>*>(get<2>(it->second), variation);
    }
    int checkExtension = isActiveColorInCheck ? 1 : 0;
    if (currentDepth >= min(depth + checkExtension, MAXIMUM_NEGAMAX_DEPTH))
        return quiescenceSearch(state, currentDepth, alpha, beta);
    int nullMoveReduction = 0;
    if (!isActiveColorInCheck && isNullMoveOk) 
    {
        int possibleEnPassantTargetColumn = state.getPossibleEnPassantTargetColumn();
        makeNullMove(state);
        int R = min(depth, MAXIMUM_NEGAMAX_DEPTH) - currentDepth > 6 ? MAXIMUM_R : MINIMUM_R;
        pair<int, stack<Move>*> evaluation = negamax(state, currentDepth + 1, depth - R + checkExtension, -beta, -beta + 1, false, false);
        evaluation.first = -evaluation.first;
        evaluation.second->push(Move(MoveType::NULL_MOVE));
        if (evaluation.first == TIMEOUT) 
        {
            killerMoves[currentDepth + 1].clear();
            delete variation;
            return pair<int, stack<Move>*>(-TIMEOUT, evaluation.second);
        }
        delete evaluation.second;
        unmakeNullMove(state, possibleEnPassantTargetColumn);
        if (evaluation.first >= beta) 
        {
            nullMoveReduction = DR;
            if (currentDepth >= min(depth - nullMoveReduction + checkExtension, MAXIMUM_NEGAMAX_DEPTH)) 
            {
                delete variation;
                return quiescenceSearch(state, currentDepth, alpha, beta);
            }
        }
    }
    vector<Move> activeColorMoves;
    MoveGenerator::getMoves(activeColorMoves, state);
    mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    shuffle(activeColorMoves.begin(), activeColorMoves.end(), rng);
    if (it != transpositionTable.end())
        sort(activeColorMoves.begin(), activeColorMoves.end(), MoveComparator(killerMoves[currentDepth], get<3>(it->second)));
    else
        sort(activeColorMoves.begin(), activeColorMoves.end(), MoveComparator(killerMoves[currentDepth]));
    Move optimalMove;
    int optimalEvaluation = -INT32_MAX;
    int legalMoves = activeColorMoves.size();
    int searchedMoves = 0;
    int staticEvaluation = Evaluator::getEvaluation(state);
    for (int i = 0; i < activeColorMoves.size(); i++) 
    {
        bool couldActiveColorCastleKingside = state.canActiveColorCastleKingside();
        bool couldActiveColorCastleQueenside = state.canActiveColorCastleQueenside();
        int possibleEnPassantTargetColumn = state.getPossibleEnPassantTargetColumn();
        if (!makeMove(state, activeColorMoves[i])) 
        {
            legalMoves--;
            unmakeMove(state, activeColorMoves[i], couldActiveColorCastleKingside, couldActiveColorCastleQueenside, possibleEnPassantTargetColumn);
            continue;
        }
        bool isInactiveColorInCheck = MoveGenerator::isActiveColorInCheck(state);
        bool isFutilityPruningOk = min(depth, MAXIMUM_NEGAMAX_DEPTH) - currentDepth == 1 && !activeColorMoves[i].isCapture() && !isInactiveColorInCheck && !isActiveColorInCheck && abs(max(alpha, optimalEvaluation) - (MATE_IN_ZERO - min(depth, MAXIMUM_NEGAMAX_DEPTH))) > Evaluator::getCentipawnEquivalent('N') && abs(beta - (MATE_IN_ZERO - min(depth, MAXIMUM_NEGAMAX_DEPTH))) > Evaluator::getCentipawnEquivalent('N') && staticEvaluation + Evaluator::getCentipawnEquivalent('N') <= max(alpha, optimalEvaluation);
        if (isFutilityPruningOk) 
        {
            unmakeMove(state, activeColorMoves[i], couldActiveColorCastleKingside, couldActiveColorCastleQueenside, possibleEnPassantTargetColumn);
            continue;
        }
        int lateMoveReduction = searchedMoves >= 3 && activeColorMoves[i].isQuiet() && !isActiveColorInCheck && !isInactiveColorInCheck && min(depth, MAXIMUM_NEGAMAX_DEPTH) - currentDepth >= 3 ? 1 : 0;
        pair<int, stack<Move>*> evaluation = negamax(state, currentDepth + 1, depth - nullMoveReduction - lateMoveReduction + checkExtension, -beta, -max(alpha, optimalEvaluation), true, isInactiveColorInCheck);
        evaluation.first = -evaluation.first;
        evaluation.second->push(activeColorMoves[i]);
        if (evaluation.first == TIMEOUT) 
        {
            killerMoves[currentDepth + 1].clear();
            delete variation;
            return pair<int, stack<Move>*>(-TIMEOUT, evaluation.second);
        }
        if (lateMoveReduction == 1 && evaluation.first > max(alpha, optimalEvaluation)) 
        {
            delete evaluation.second;
            evaluation = negamax(state, currentDepth + 1, depth - nullMoveReduction + checkExtension, -beta, -max(alpha, optimalEvaluation), true, isInactiveColorInCheck);
            evaluation.first = -evaluation.first;
            evaluation.second->push(activeColorMoves[i]);
        }
        if (evaluation.first == TIMEOUT) 
        {
            killerMoves[currentDepth + 1].clear();
            delete variation;
            return pair<int, stack<Move>*>(-TIMEOUT, evaluation.second);
        }
        unmakeMove(state, activeColorMoves[i], couldActiveColorCastleKingside, couldActiveColorCastleQueenside, possibleEnPassantTargetColumn);
        if (evaluation.first >= beta) 
        {
            if (activeColorMoves[i].isQuiet())
                killerMoves[currentDepth].insert(activeColorMoves[i]);
            if (it == transpositionTable.end() || evaluation.first > get<2>(it->second))
                transpositionTable[state] = tuple<int, NodeType, int, Move>(min(depth, MAXIMUM_NEGAMAX_DEPTH) - currentDepth, NodeType::CUT, evaluation.first, activeColorMoves[i]);
            killerMoves[currentDepth + 1].clear();
            delete variation;
            return pair<int, stack<Move>*>(evaluation.first, evaluation.second);
        }
        if (evaluation.first > optimalEvaluation)
        {
            optimalMove = activeColorMoves[i];
            optimalEvaluation = evaluation.first;
            delete variation;
            variation = evaluation.second;
        }
        else
            delete evaluation.second;
        searchedMoves++;
    }
    killerMoves[currentDepth + 1].clear();
    if (legalMoves == 0)
        return pair<int, stack<Move>*>(isActiveColorInCheck ? -(MATE_IN_ZERO - currentDepth) : 0, variation);
    if (optimalEvaluation <= alpha) 
    {
        if (it == transpositionTable.end() || optimalEvaluation < get<2>(it->second))
            transpositionTable[state] = tuple<int, NodeType, int, Move>(min(depth, MAXIMUM_NEGAMAX_DEPTH) - currentDepth, NodeType::ALL, optimalEvaluation, optimalMove);
    }
    else
        transpositionTable[state] = tuple<int, NodeType, int, Move>(min(depth, MAXIMUM_NEGAMAX_DEPTH) - currentDepth, NodeType::PV, optimalEvaluation, optimalMove);
    return pair<int, stack<Move>*>(optimalEvaluation, variation);
}
pair<int, stack<Move>*> Engine::negamax(State& state, int depth, int alpha, int beta) 
{
    stack<Move>* variation = new stack<Move>;
    if (chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - start).count() >= seconds) 
        return pair<int, stack<Move>*>(TIMEOUT, variation);
    unordered_map<State, tuple<int, NodeType, int, Move>>::iterator it = transpositionTable.find(state);
    if (it != transpositionTable.end() && get<0>(it->second) >= depth) 
    {
        if ((get<1>(it->second) == NodeType::PV || get<1>(it->second) == NodeType::ALL) && get<2>(it->second) <= alpha)
            return pair<int, stack<Move>*>(get<2>(it->second), variation);
        if ((get<1>(it->second) == NodeType::PV || get<1>(it->second) == NodeType::CUT) && get<2>(it->second) >= beta)
            return pair<int, stack<Move>*>(get<2>(it->second), variation);
    }
    bool isActiveColorInCheck = MoveGenerator::isActiveColorInCheck(state);
    int checkExtension = isActiveColorInCheck ? 1 : 0;
    int nullMoveReduction = 0;
    if (!isActiveColorInCheck)
    {
        int possibleEnPassantTargetColumn = state.getPossibleEnPassantTargetColumn();
        makeNullMove(state);
        int R = depth > 6 ? MAXIMUM_R : MINIMUM_R;
        pair<int, stack<Move>*> evaluation = negamax(state, 1, depth - R + checkExtension, -beta, -beta + 1, false, false);
        evaluation.first = -evaluation.first;
        evaluation.second->push(Move(MoveType::NULL_MOVE));
        if (evaluation.first == TIMEOUT)
        {
            killerMoves[1].clear();
            delete variation;
            return pair<int, stack<Move>*>(TIMEOUT, evaluation.second);
        }
        delete evaluation.second;
        unmakeNullMove(state, possibleEnPassantTargetColumn);
        if (evaluation.first >= beta)
        {
            nullMoveReduction = DR;
            if (0 >= min(depth - nullMoveReduction + checkExtension, MAXIMUM_NEGAMAX_DEPTH))
            {
                delete variation;
                return quiescenceSearch(state, 0, alpha, beta);
            }
        }
    }
    vector<Move> activeColorMoves;
    MoveGenerator::getMoves(activeColorMoves, state);
    mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    shuffle(activeColorMoves.begin(), activeColorMoves.end(), rng);
    if (it != transpositionTable.end())
        sort(activeColorMoves.begin(), activeColorMoves.end(), MoveComparator(killerMoves[0], get<3>(it->second)));
    else
        sort(activeColorMoves.begin(), activeColorMoves.end(), MoveComparator(killerMoves[0]));
    Move optimalMove;
    int optimalEvaluation = -INT32_MAX;
    int legalMoves = activeColorMoves.size();
    int searchedMoves = 0;
    int staticEvaluation = Evaluator::getEvaluation(state);
    for (int i = 0; i < activeColorMoves.size(); i++)
    {
        bool couldActiveColorCastleKingside = state.canActiveColorCastleKingside();
        bool couldActiveColorCastleQueenside = state.canActiveColorCastleQueenside();
        int possibleEnPassantTargetColumn = state.getPossibleEnPassantTargetColumn();
        if (!makeMove(state, activeColorMoves[i]))
        {
            legalMoves--;
            unmakeMove(state, activeColorMoves[i], couldActiveColorCastleKingside, couldActiveColorCastleQueenside, possibleEnPassantTargetColumn);
            continue;
        }
        bool isInactiveColorInCheck = MoveGenerator::isActiveColorInCheck(state);
        bool isFutilityPruningOk = depth == 1 && !activeColorMoves[i].isCapture() && !isInactiveColorInCheck && !isActiveColorInCheck && abs(max(alpha, optimalEvaluation) - (MATE_IN_ZERO - depth)) > Evaluator::getCentipawnEquivalent('N') && abs(beta - (MATE_IN_ZERO - depth)) > Evaluator::getCentipawnEquivalent('N') && staticEvaluation + Evaluator::getCentipawnEquivalent('N') <= max(alpha, optimalEvaluation);
        if (isFutilityPruningOk)
        {
            unmakeMove(state, activeColorMoves[i], couldActiveColorCastleKingside, couldActiveColorCastleQueenside, possibleEnPassantTargetColumn);
            continue;
        }
        int lateMoveReduction = searchedMoves >= 3 && activeColorMoves[i].isQuiet() && !isActiveColorInCheck && !isInactiveColorInCheck && depth >= 3 ? 1 : 0;
        pair<int, stack<Move>*> evaluation = negamax(state, 1, depth - lateMoveReduction + checkExtension, -beta, -max(alpha, optimalEvaluation), true, isInactiveColorInCheck);
        evaluation.first = -evaluation.first;
        evaluation.second->push(activeColorMoves[i]);
        if (evaluation.first == TIMEOUT)
        {
            killerMoves[1].clear();
            delete variation;
            return pair<int, stack<Move>*>(TIMEOUT, evaluation.second);
        }
        if (lateMoveReduction == 1 && evaluation.first > max(alpha, optimalEvaluation))
        {
            delete evaluation.second;
            evaluation = negamax(state, 1, depth + checkExtension, -beta, -max(alpha, optimalEvaluation), true, isInactiveColorInCheck);
            evaluation.first = -evaluation.first;
            evaluation.second->push(activeColorMoves[i]);
        }
        if (evaluation.first == TIMEOUT) 
        {
            killerMoves[1].clear();
            delete variation;
            return pair<int, stack<Move>*>(TIMEOUT, evaluation.second);
        }
        unmakeMove(state, activeColorMoves[i], couldActiveColorCastleKingside, couldActiveColorCastleQueenside, possibleEnPassantTargetColumn);
        if (evaluation.first >= beta) 
        {
            if (activeColorMoves[i].isQuiet())
                killerMoves[0].insert(activeColorMoves[i]);
            if (it == transpositionTable.end() || evaluation.first > get<2>(it->second))
                transpositionTable[state] = tuple<int, NodeType, int, Move>(depth, NodeType::CUT, evaluation.first, activeColorMoves[i]);
            killerMoves[1].clear();
            delete variation;
            return pair<int, stack<Move>*>(evaluation.first, evaluation.second);
        }
        if (evaluation.first > optimalEvaluation)
        {
            optimalMove = activeColorMoves[i];
            optimalEvaluation = evaluation.first;
            delete variation;
            variation = evaluation.second;
        }
        else
            delete evaluation.second;
        searchedMoves++;
    }
    if (optimalEvaluation <= alpha) 
    {
        if (it == transpositionTable.end() || optimalEvaluation < get<2>(it->second))
            transpositionTable[state] = tuple<int, NodeType, int, Move>(depth, NodeType::ALL, optimalEvaluation, optimalMove);
    }
    else
        transpositionTable[state] = tuple<int, NodeType, int, Move>(depth, NodeType::PV, optimalEvaluation, optimalMove);
    killerMoves[1].clear();
    return pair<int, stack<Move>*>(optimalEvaluation, variation);
}
int Engine::perft(State& state, int currentDepth, int depth) 
{
    if (currentDepth == depth)
        return 1;
    vector<Move> activeColorMoves;
    MoveGenerator::getMoves(activeColorMoves, state);
    int nodes = 0;
    int legalMoves = activeColorMoves.size();
    for (int i = 0; i < activeColorMoves.size(); i++) 
    {
        bool couldActiveColorCastleKingside = state.canActiveColorCastleKingside();
        bool couldActiveColorCastleQueenside = state.canActiveColorCastleQueenside();
        int possibleEnPassantTargetColumn = state.getPossibleEnPassantTargetColumn();
        if (!makeMove(state, activeColorMoves[i])) 
        {
            legalMoves--;
            unmakeMove(state, activeColorMoves[i], couldActiveColorCastleKingside, couldActiveColorCastleQueenside, possibleEnPassantTargetColumn);
            continue;
        }
        nodes += perft(state, currentDepth + 1, depth);
        unmakeMove(state, activeColorMoves[i], couldActiveColorCastleKingside, couldActiveColorCastleQueenside, possibleEnPassantTargetColumn);
    }
    return nodes;
}
pair<int, stack<Move>*> Engine::quiescenceSearch(State& state, int currentDepth, int alpha, int beta) 
{
    stack<Move>* variation = new stack<Move>;
    if (chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - start).count() >= seconds) 
        return pair<int, stack<Move>*>(-TIMEOUT, variation);
    unordered_map<State, tuple<int, NodeType, int, Move>>::iterator it = transpositionTable.find(state);
    if (it != transpositionTable.end()) 
    {
        if ((get<1>(it->second) == NodeType::PV || get<1>(it->second) == NodeType::ALL) && get<2>(it->second) <= alpha)
            return pair<int, stack<Move>*>(get<2>(it->second), variation);
        if ((get<1>(it->second) == NodeType::PV || get<1>(it->second) == NodeType::CUT) && get<2>(it->second) >= beta)
            return pair<int, stack<Move>*>(get<2>(it->second), variation);
    }
    Move optimalMove;
    int optimalEvaluation = Evaluator::getEvaluation(state);
    vector<Move> activeColorMoves;
    MoveGenerator::getMoves(activeColorMoves, state);
    mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    shuffle(activeColorMoves.begin(), activeColorMoves.end(), rng);
    if (it != transpositionTable.end())
        sort(activeColorMoves.begin(), activeColorMoves.end(), MoveComparator(killerMoves[currentDepth], get<3>(it->second)));
    else
        sort(activeColorMoves.begin(), activeColorMoves.end(), MoveComparator(killerMoves[currentDepth]));
    int legalMoves = activeColorMoves.size();
    bool isActiveColorInCheck = MoveGenerator::isActiveColorInCheck(state);
    for (int i = 0; i < activeColorMoves.size(); i++) 
    {
        bool couldActiveColorCastleKingside = state.canActiveColorCastleKingside();
        bool couldActiveColorCastleQueenside = state.canActiveColorCastleQueenside();
        int possibleEnPassantTargetColumn = state.getPossibleEnPassantTargetColumn();
        if (!makeMove(state, activeColorMoves[i])) 
        {
            legalMoves--;
            unmakeMove(state, activeColorMoves[i], couldActiveColorCastleKingside, couldActiveColorCastleQueenside, possibleEnPassantTargetColumn);
            continue;
        }
        if (currentDepth == MAXIMUM_DEPTH) 
        {
            unmakeMove(state, activeColorMoves[i], couldActiveColorCastleKingside, couldActiveColorCastleQueenside, possibleEnPassantTargetColumn);
            continue;
        }
        if (!activeColorMoves[i].isCapture() && !isActiveColorInCheck) 
        {
            unmakeMove(state, activeColorMoves[i], couldActiveColorCastleKingside, couldActiveColorCastleQueenside, possibleEnPassantTargetColumn);
            continue;
        }
        pair<int, stack<Move>*> evaluation = quiescenceSearch(state, currentDepth + 1, -beta, -max(alpha, optimalEvaluation));
        evaluation.first = -evaluation.first;
        evaluation.second->push(activeColorMoves[i]);
        if (evaluation.first == TIMEOUT) 
        {
            killerMoves[currentDepth + 1].clear();
            delete variation;
            return pair<int, stack<Move>*>(-TIMEOUT, evaluation.second);
        }
        unmakeMove(state, activeColorMoves[i], couldActiveColorCastleKingside, couldActiveColorCastleQueenside, possibleEnPassantTargetColumn);
        if (evaluation.first >= beta) 
        {
            if (activeColorMoves[i].isQuiet())
                killerMoves[currentDepth].insert(activeColorMoves[i]);
            if (it == transpositionTable.end() || evaluation.first > get<2>(it->second))
                transpositionTable[state] = tuple<int, NodeType, int, Move>(0, NodeType::CUT, evaluation.first, activeColorMoves[i]);
            killerMoves[currentDepth + 1].clear();
            delete variation;
            return pair<int, stack<Move>*>(evaluation.first, evaluation.second);
        }
        if (evaluation.first > optimalEvaluation)
        {
            optimalMove = activeColorMoves[i];
            optimalEvaluation = evaluation.first;
            delete variation;
            variation = evaluation.second;
        }
        else
            delete evaluation.second;
    }
    if (currentDepth < MAXIMUM_DEPTH)
        killerMoves[currentDepth + 1].clear();
    if (legalMoves == 0)
        return pair<int, stack<Move>*>(isActiveColorInCheck ? -(MATE_IN_ZERO - currentDepth) : 0, variation);
    if (optimalEvaluation <= alpha) 
    {
        if (it == transpositionTable.end() || optimalEvaluation < get<2>(it->second))
            transpositionTable[state] = tuple<int, NodeType, int, Move>(0, NodeType::ALL, optimalEvaluation, optimalMove);
    }
    else if (optimalEvaluation >= beta) 
    {
        if (it == transpositionTable.end() || optimalEvaluation > get<2>(it->second))
            transpositionTable[state] = tuple<int, NodeType, int, Move>(0, NodeType::CUT, optimalEvaluation, optimalMove);
    }
    else
        transpositionTable[state] = tuple<int, NodeType, int, Move>(0, NodeType::PV, optimalEvaluation, optimalMove);
    return pair<int, stack<Move>*>(optimalEvaluation, variation);
}
Engine::Engine() 
{
    Evaluator::initialize();
    MoveGenerator::initialize();
    State::initialize();
}
void Engine::getOptimalMoveDepthVersion(const State& state, int maximumDepth) 
{
    if (maximumDepth > MAXIMUM_NEGAMAX_DEPTH) 
    {
        cout << "\nDepth greater than maximum depth.\n\n";
        return;
    }
    transpositionTable.clear();
    State s(state);
    this->seconds = INT32_MAX;
    start = chrono::steady_clock::now();
    pair<int, stack<Move>*> optimalEvaluation = quiescenceSearch(s, 0, -INT32_MAX, INT32_MAX);
    for (int depth = 1; depth <= maximumDepth; depth++) 
    {
        delete optimalEvaluation.second;
        int alpha = optimalEvaluation.first - 25;
        int beta = optimalEvaluation.first + 25;
        pair<int, stack<Move>*> evaluation = negamax(s, depth, alpha, beta);
        if (evaluation.first <= alpha || evaluation.first >= beta) 
        {
            delete evaluation.second;
            evaluation = negamax(s, depth, -INT32_MAX, INT32_MAX);
        }
        killerMoves[0].clear();
        optimalEvaluation = evaluation;
        printSearchResult(depth, optimalEvaluation);
    }
    delete optimalEvaluation.second;
}
void Engine::getOptimalMoveMoveTimeVersion(const State& state, int seconds) 
{
    transpositionTable.clear();
    State s(state);
    this->seconds = seconds;
    start = chrono::steady_clock::now();
    pair<int, stack<Move>*> optimalEvaluation = quiescenceSearch(s, 0, -INT32_MAX, INT32_MAX);
    for (int depth = 1; depth <= MAXIMUM_NEGAMAX_DEPTH; depth++) 
    {
        delete optimalEvaluation.second;
        int alpha = optimalEvaluation.first - 25;
        int beta = optimalEvaluation.first + 25;
        pair<int, stack<Move>*> evaluation = negamax(s, depth, alpha, beta);
        if (evaluation.first == TIMEOUT) 
        { 
            killerMoves[0].clear();
            delete evaluation.second;
            return;
        }
        if (evaluation.first <= alpha || evaluation.first >= beta) 
        {
            delete evaluation.second;
            evaluation = negamax(s, depth, -INT32_MAX, INT32_MAX);
        }
        if (evaluation.first == TIMEOUT)
        {
            killerMoves[0].clear();
            delete evaluation.second;
            return;
        }
        killerMoves[0].clear();
        optimalEvaluation = evaluation; 
        printSearchResult(depth, optimalEvaluation);
    }
    delete optimalEvaluation.second;
}
void Engine::perft(const State& state, int depth) 
{
    if (depth > MAXIMUM_PERFT_DEPTH) 
    {
        cout << "\nDepth greater than maximum depth.\n\n";
        return;
    }
    cout << "\n";
    State s(state);
    vector<Move> activeColorMoves;
    MoveGenerator::getMoves(activeColorMoves, state);
    int totalNodes = 0;
    for (int i = 0; i < activeColorMoves.size(); i++) 
    {
        bool couldActiveColorCastleKingside = state.canActiveColorCastleKingside();
        bool couldActiveColorCastleQueenside = state.canActiveColorCastleQueenside();
        int possibleEnPassantTargetColumn = state.getPossibleEnPassantTargetColumn();
        if (!makeMove(s, activeColorMoves[i])) 
        {
            unmakeMove(s, activeColorMoves[i], couldActiveColorCastleKingside, couldActiveColorCastleQueenside, possibleEnPassantTargetColumn);
            continue;
        }
        int nodes = perft(s, 1, depth);
        totalNodes += nodes;
        cout << moveToString(activeColorMoves[i]) << ": " << nodes << "\n";
        unmakeMove(s, activeColorMoves[i], couldActiveColorCastleKingside, couldActiveColorCastleQueenside, possibleEnPassantTargetColumn);
    }
    cout << "\nTotal Nodes: " << totalNodes << "\n\n";
}
