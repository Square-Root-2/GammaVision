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
int Engine::negamax(State& state, int currentDepth, int depth, int alpha, int beta, bool isNullMoveOk, bool isActiveColorInCheck, bool hasThereBeenNullMove) 
{
    if (chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - start).count() >= seconds)
        return -TIMEOUT;
    unordered_map<State, tuple<int, NodeType, int, Move>>::iterator it = transpositionTable.find(state);
    if (it != transpositionTable.end() && get<0>(it->second) >= min(depth, MAXIMUM_NEGAMAX_DEPTH) - currentDepth) 
    {
        if ((get<1>(it->second) == NodeType::PV || get<1>(it->second) == NodeType::ALL) && get<2>(it->second) <= alpha)
            return get<2>(it->second);
        if ((get<1>(it->second) == NodeType::PV || get<1>(it->second) == NodeType::CUT) && get<2>(it->second) >= beta)
            return get<2>(it->second);
        if (get<1>(it->second) == NodeType::PV)
            return get<2>(it->second);
    }
    int checkExtension = isActiveColorInCheck ? 1 : 0;
    if (currentDepth >= min(depth + checkExtension, MAXIMUM_NEGAMAX_DEPTH))
        return quiescenceSearch(state, currentDepth, alpha, beta, hasThereBeenNullMove);
    int nullMoveReduction = 0;
    if (!isActiveColorInCheck && isNullMoveOk) 
    {
        int possibleEnPassantTargetColumn = state.getPossibleEnPassantTargetColumn();
        makeNullMove(state);
        int R = min(depth, MAXIMUM_NEGAMAX_DEPTH) - currentDepth > 6 ? MAXIMUM_R : MINIMUM_R;
        int evaluation = -negamax(state, currentDepth + 1, depth - R + checkExtension, -beta, -beta + 1, false, false, true);
        unmakeNullMove(state, possibleEnPassantTargetColumn);
        if (evaluation >= beta) 
        {
            nullMoveReduction = DR;
            if (currentDepth >= min(depth - nullMoveReduction + checkExtension, MAXIMUM_NEGAMAX_DEPTH))
                return quiescenceSearch(state, currentDepth, alpha, beta, hasThereBeenNullMove);
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
        int evaluation = -negamax(state, currentDepth + 1, depth - nullMoveReduction - lateMoveReduction + checkExtension, -beta, -max(alpha, optimalEvaluation), true, isInactiveColorInCheck, hasThereBeenNullMove);
        if (evaluation == TIMEOUT) 
        {
            killerMoves[currentDepth + 1].clear();
            return -TIMEOUT;
        }
        if (lateMoveReduction == 1 && evaluation > max(alpha, optimalEvaluation))
            evaluation = -negamax(state, currentDepth + 1, depth - nullMoveReduction + checkExtension, -beta, -max(alpha, optimalEvaluation), true, isInactiveColorInCheck, hasThereBeenNullMove);
        if (evaluation == TIMEOUT) 
        {
            killerMoves[currentDepth + 1].clear();
            return -TIMEOUT;
        }
        unmakeMove(state, activeColorMoves[i], couldActiveColorCastleKingside, couldActiveColorCastleQueenside, possibleEnPassantTargetColumn);
        if (evaluation >= beta) 
        {
            if (activeColorMoves[i].isQuiet())
                killerMoves[currentDepth].insert(activeColorMoves[i]);
            if (it == transpositionTable.end() || evaluation > get<2>(it->second))
                transpositionTable[state] = tuple<int, NodeType, int, Move>(min(depth, MAXIMUM_NEGAMAX_DEPTH) - currentDepth, NodeType::CUT, evaluation, activeColorMoves[i]);
            killerMoves[currentDepth + 1].clear();
            return evaluation;
        }
        if (evaluation > optimalEvaluation) 
        {
            optimalMove = activeColorMoves[i];
            optimalEvaluation = evaluation;
        }
        searchedMoves++;
    }
    killerMoves[currentDepth + 1].clear();
    if (legalMoves == 0)
        return isActiveColorInCheck ? -(MATE_IN_ZERO - currentDepth) : 0;
    if (optimalEvaluation <= alpha) 
    {
        if (it == transpositionTable.end() || optimalEvaluation < get<2>(it->second))
            transpositionTable[state] = tuple<int, NodeType, int, Move>(min(depth, MAXIMUM_NEGAMAX_DEPTH) - currentDepth, NodeType::ALL, optimalEvaluation, optimalMove);
    }
    else
        transpositionTable[state] = tuple<int, NodeType, int, Move>(min(depth, MAXIMUM_NEGAMAX_DEPTH) - currentDepth, NodeType::PV, optimalEvaluation, optimalMove);
    return optimalEvaluation;
}
pair<Move, int> Engine::negamax(State& state, int depth, int alpha, int beta) 
{
    if (chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - start).count() >= seconds)
        return pair<Move, int>(Move(MoveType::TIMEOUT), 0);
    unordered_map<State, tuple<int, NodeType, int, Move>>::iterator it = transpositionTable.find(state);
    if (it != transpositionTable.end() && get<0>(it->second) >= depth) 
    {
        if ((get<1>(it->second) == NodeType::PV || get<1>(it->second) == NodeType::ALL) && get<2>(it->second) <= alpha)
            return pair<Move, int>(get<3>(it->second), get<2>(it->second));
        if ((get<1>(it->second) == NodeType::PV || get<1>(it->second) == NodeType::CUT) && get<2>(it->second) >= beta)
            return pair<Move, int>(get<3>(it->second), get<2>(it->second));
        if (get<1>(it->second) == NodeType::PV)
            return pair<Move, int>(get<3>(it->second), get<2>(it->second));
    }
    bool isActiveColorInCheck = MoveGenerator::isActiveColorInCheck(state);
    int checkExtension = isActiveColorInCheck ? 1 : 0;
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
        int evaluation = -negamax(state, 1, depth - lateMoveReduction + checkExtension, -beta, -max(alpha, optimalEvaluation), true, isInactiveColorInCheck, false);
        if (evaluation == TIMEOUT) 
        {
            killerMoves[1].clear();
            return pair<Move, int>(Move(MoveType::TIMEOUT), 0);
        }
        if (lateMoveReduction == 1 && evaluation > max(alpha, optimalEvaluation))
            evaluation = -negamax(state, 1, depth + checkExtension, -beta, -max(alpha, optimalEvaluation), true, isInactiveColorInCheck, false);
        if (evaluation == TIMEOUT) 
        {
            killerMoves[1].clear();
            return pair<Move, int>(Move(MoveType::TIMEOUT), 0);
        }
        unmakeMove(state, activeColorMoves[i], couldActiveColorCastleKingside, couldActiveColorCastleQueenside, possibleEnPassantTargetColumn);
        if (evaluation >= beta) 
        {
            if (activeColorMoves[i].isQuiet())
                killerMoves[0].insert(activeColorMoves[i]);
            if (it == transpositionTable.end() || evaluation > get<2>(it->second))
                transpositionTable[state] = tuple<int, NodeType, int, Move>(depth, NodeType::CUT, evaluation, activeColorMoves[i]);
            killerMoves[1].clear();
            return pair<Move, int>(activeColorMoves[i], evaluation);
        }
        if (evaluation > optimalEvaluation) 
        {
            optimalMove = activeColorMoves[i];
            optimalEvaluation = evaluation;
        }
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
    return pair<Move, int>(optimalMove, optimalEvaluation);
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
int Engine::quiescenceSearch(State& state, int currentDepth, int alpha, int beta, bool hasThereBeenNullMove) 
{
    if (chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - start).count() >= seconds)
        return -TIMEOUT;
    unordered_map<State, tuple<int, NodeType, int, Move>>::iterator it = transpositionTable.find(state);
    if (it != transpositionTable.end()) 
    {
        if ((get<1>(it->second) == NodeType::PV || get<1>(it->second) == NodeType::ALL) && get<2>(it->second) <= alpha)
            return get<2>(it->second);
        if ((get<1>(it->second) == NodeType::PV || get<1>(it->second) == NodeType::CUT) && get<2>(it->second) >= beta)
            return get<2>(it->second);
        if (get<1>(it->second) == NodeType::PV)
            return get<2>(it->second);
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
        int evaluation = -quiescenceSearch(state, currentDepth + 1, -beta, -max(alpha, optimalEvaluation), hasThereBeenNullMove);
        if (evaluation == TIMEOUT)
            return -TIMEOUT;
        unmakeMove(state, activeColorMoves[i], couldActiveColorCastleKingside, couldActiveColorCastleQueenside, possibleEnPassantTargetColumn);
        if (evaluation >= beta) 
        {
            if (activeColorMoves[i].isQuiet())
                killerMoves[currentDepth].insert(activeColorMoves[i]);
            if (it == transpositionTable.end() || evaluation > get<2>(it->second))
                transpositionTable[state] = tuple<int, NodeType, int, Move>(0, NodeType::CUT, evaluation, activeColorMoves[i]);
            killerMoves[currentDepth + 1].clear();
            return evaluation;
        }
        if (evaluation > optimalEvaluation) 
        {
            optimalMove = activeColorMoves[i];
            optimalEvaluation = evaluation;
        }
    }
    if (currentDepth < MAXIMUM_DEPTH)
        killerMoves[currentDepth + 1].clear();
    if (legalMoves == 0)
        return isActiveColorInCheck ? -(MATE_IN_ZERO - currentDepth) : 0;
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
    return optimalEvaluation;
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
    pair<Move, int> optimalMove(Move(), quiescenceSearch(s, 0, -INT32_MAX, INT32_MAX, false));
    for (int depth = 1; depth <= maximumDepth; depth++) 
    {
        int alpha = optimalMove.second - 25;
        int beta = optimalMove.second + 25;
        pair<Move, int> move = negamax(s, depth, alpha, beta);
        if (move.second <= alpha || move.second >= beta) 
        {
            move = negamax(s, depth, -INT32_MAX, INT32_MAX);
        }
        killerMoves[0].clear();
        optimalMove = move;
        printSearchResult(depth, optimalMove.first, optimalMove.second);
    }
}
void Engine::getOptimalMoveMoveTimeVersion(const State& state, int seconds) 
{
    transpositionTable.clear();
    State s(state);
    this->seconds = seconds;
    start = chrono::steady_clock::now();
    pair<Move, int> optimalMove(Move(), quiescenceSearch(s, 0, -INT32_MAX, INT32_MAX, false));
    for (int depth = 1; depth <= MAXIMUM_NEGAMAX_DEPTH; depth++) 
    {
        int alpha = optimalMove.second - 25;
        int beta = optimalMove.second + 25;
        pair<Move, int> move = negamax(s, depth, alpha, beta);
        if (move.first == Move(MoveType::TIMEOUT)) 
        {
            killerMoves[0].clear();
            return;
        }
        if (move.second <= alpha || move.second >= beta)
            move = negamax(s, depth, -INT32_MAX, INT32_MAX);
        if (move.first == Move(MoveType::TIMEOUT)) 
        {
            killerMoves[0].clear();
            return;
        }
        killerMoves[0].clear();
        optimalMove = move; 
        printSearchResult(depth, optimalMove.first, optimalMove.second);
    }
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
// 2K3k1/1p3ppp/2p2b2/1r6/8/p7/8/8 b - - 0 1
