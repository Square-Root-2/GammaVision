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
        int R = min(depth + checkExtension, MAXIMUM_NEGAMAX_DEPTH) - currentDepth > 6 ? MAXIMUM_R : MINIMUM_R;
        pair<int, stack<Move>*> evaluation = negamax(state, currentDepth + 1, depth - R + checkExtension, -beta, -beta + 1, false, false);
        evaluation.first = -evaluation.first;
        evaluation.second->push(Move(MoveType::NULL_MOVE));
        if (evaluation.first == TIMEOUT)
        {
            unmakeNullMove(state, possibleEnPassantTargetColumn);
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
    int staticEvaluation;
    if (min(depth - nullMoveReduction + checkExtension, MAXIMUM_NEGAMAX_DEPTH) - currentDepth == 1)
        staticEvaluation = Evaluator::getEvaluation(state);
    int i = 0;
    for (; i < activeColorMoves.size(); i++) 
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
        bool isFutilityPruningOk = min(depth - nullMoveReduction + checkExtension, MAXIMUM_NEGAMAX_DEPTH) - currentDepth == 1 && !activeColorMoves[i].isCapture() && !isInactiveColorInCheck && !isActiveColorInCheck && abs(alpha - (MATE_IN_ZERO - (currentDepth + 1))) > Evaluator::getCentipawnEquivalent('N') && abs(beta - (MATE_IN_ZERO - (currentDepth + 1))) > Evaluator::getCentipawnEquivalent('N') && staticEvaluation + Evaluator::getCentipawnEquivalent('N') <= alpha;
        if (isFutilityPruningOk)
        {
            unmakeMove(state, activeColorMoves[i], couldActiveColorCastleKingside, couldActiveColorCastleQueenside, possibleEnPassantTargetColumn);
            i++;
            break;
        }
        pair<int, stack<Move>*> evaluation = negamax(state, currentDepth + 1, depth - nullMoveReduction + checkExtension, -beta, -alpha, true, isInactiveColorInCheck);
        evaluation.first = -evaluation.first;
        evaluation.second->push(activeColorMoves[i]);
        if (evaluation.first == TIMEOUT)
        {
            unmakeMove(state, activeColorMoves[i], couldActiveColorCastleKingside, couldActiveColorCastleQueenside, possibleEnPassantTargetColumn);
            killerMoves[currentDepth + 1].clear();
            delete variation;
            return pair<int, stack<Move>*>(-TIMEOUT, evaluation.second);
        }
        unmakeMove(state, activeColorMoves[i], couldActiveColorCastleKingside, couldActiveColorCastleQueenside, possibleEnPassantTargetColumn);
        if (evaluation.first >= beta)
        {
            if (activeColorMoves[i].isQuiet())
                killerMoves[currentDepth].insert(activeColorMoves[i]);
            if (it == transpositionTable.end() || min(depth, MAXIMUM_NEGAMAX_DEPTH) - currentDepth > get<0>(it->second) || min(depth, MAXIMUM_NEGAMAX_DEPTH) - currentDepth == get<0>(it->second) && evaluation.first > get<2>(it->second))
                transpositionTable[state] = tuple<int, NodeType, int, Move>(min(depth, MAXIMUM_NEGAMAX_DEPTH) - currentDepth, NodeType::CUT, evaluation.first, activeColorMoves[i]);
            killerMoves[currentDepth + 1].clear();
            delete variation;
            return pair<int, stack<Move>*>(evaluation.first, evaluation.second);
        }
        optimalMove = activeColorMoves[i];
        optimalEvaluation = evaluation.first;
        delete variation;
        variation = evaluation.second;
        i++;
        break;
    }
    bool isPVNode = beta - alpha > 1;
    for (; i < activeColorMoves.size(); i++) 
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
        bool isFutilityPruningOk = min(depth - nullMoveReduction + checkExtension, MAXIMUM_NEGAMAX_DEPTH) - currentDepth == 1 && !activeColorMoves[i].isCapture() && !isInactiveColorInCheck && !isActiveColorInCheck && abs(max(alpha, optimalEvaluation) - (MATE_IN_ZERO - (currentDepth + 1))) > Evaluator::getCentipawnEquivalent('N') && abs(beta - (MATE_IN_ZERO - (currentDepth + 1))) > Evaluator::getCentipawnEquivalent('N') && staticEvaluation + Evaluator::getCentipawnEquivalent('N') <= max(alpha, optimalEvaluation);
        if (isFutilityPruningOk) 
        {
            unmakeMove(state, activeColorMoves[i], couldActiveColorCastleKingside, couldActiveColorCastleQueenside, possibleEnPassantTargetColumn);
            continue;
        }
        int lateMoveReduction = legalMoves >= 4 && activeColorMoves[i].isQuiet() && !isActiveColorInCheck && !isInactiveColorInCheck && !isPVNode && min(depth - nullMoveReduction + checkExtension, MAXIMUM_NEGAMAX_DEPTH) - currentDepth >= 3 ? 1 : 0;
        pair<int, stack<Move>*> evaluation = negamax(state, currentDepth + 1, depth - nullMoveReduction - lateMoveReduction + checkExtension, -max(alpha, optimalEvaluation) - 1, -max(alpha, optimalEvaluation), true, isInactiveColorInCheck);
        evaluation.first = -evaluation.first;
        evaluation.second->push(activeColorMoves[i]);
        if (evaluation.first == TIMEOUT) 
        {
            unmakeMove(state, activeColorMoves[i], couldActiveColorCastleKingside, couldActiveColorCastleQueenside, possibleEnPassantTargetColumn);
            killerMoves[currentDepth + 1].clear();
            delete variation;
            return pair<int, stack<Move>*>(-TIMEOUT, evaluation.second);
        }
        if ((!isPVNode && lateMoveReduction || isPVNode && evaluation.first < beta) && evaluation.first > max(alpha, optimalEvaluation))
        {
            delete evaluation.second;
            evaluation = negamax(state, currentDepth + 1, depth - nullMoveReduction + checkExtension, -beta, -max(alpha, optimalEvaluation), true, isInactiveColorInCheck);
            evaluation.first = -evaluation.first;
            evaluation.second->push(activeColorMoves[i]);
        }
        if (evaluation.first == TIMEOUT) 
        {
            unmakeMove(state, activeColorMoves[i], couldActiveColorCastleKingside, couldActiveColorCastleQueenside, possibleEnPassantTargetColumn);
            killerMoves[currentDepth + 1].clear();
            delete variation;
            return pair<int, stack<Move>*>(-TIMEOUT, evaluation.second);
        }
        unmakeMove(state, activeColorMoves[i], couldActiveColorCastleKingside, couldActiveColorCastleQueenside, possibleEnPassantTargetColumn);
        if (evaluation.first >= beta) 
        {
            if (activeColorMoves[i].isQuiet())
                killerMoves[currentDepth].insert(activeColorMoves[i]);
            if (it == transpositionTable.end() || min(depth, MAXIMUM_NEGAMAX_DEPTH) - currentDepth > get<0>(it->second) || min(depth, MAXIMUM_NEGAMAX_DEPTH) - currentDepth == get<0>(it->second) && evaluation.first > get<2>(it->second))
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
    }
    killerMoves[currentDepth + 1].clear();
    if (legalMoves == 0)
        return pair<int, stack<Move>*>(isActiveColorInCheck ? -(MATE_IN_ZERO - currentDepth) : 0, variation);
    if (optimalEvaluation <= alpha) 
    {
        if (it == transpositionTable.end() || min(depth, MAXIMUM_NEGAMAX_DEPTH) - currentDepth > get<0>(it->second) || min(depth, MAXIMUM_NEGAMAX_DEPTH) - currentDepth == get<0>(it->second) && optimalEvaluation < get<2>(it->second))
            transpositionTable[state] = tuple<int, NodeType, int, Move>(min(depth, MAXIMUM_NEGAMAX_DEPTH) - currentDepth, NodeType::ALL, optimalEvaluation, optimalMove);
    }
    else if (it == transpositionTable.end() || min(depth, MAXIMUM_NEGAMAX_DEPTH) - currentDepth > get<0>(it->second))
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
        int R = min(depth + checkExtension, MAXIMUM_NEGAMAX_DEPTH) > 6 ? MAXIMUM_R : MINIMUM_R;
        pair<int, stack<Move>*> evaluation = negamax(state, 1, depth - R + checkExtension, -beta, -beta + 1, false, false);
        evaluation.first = -evaluation.first;
        evaluation.second->push(Move(MoveType::NULL_MOVE));
        if (evaluation.first == TIMEOUT)
        {
            unmakeNullMove(state, possibleEnPassantTargetColumn);
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
    int staticEvaluation;
    if (min(depth - nullMoveReduction + checkExtension, MAXIMUM_NEGAMAX_DEPTH) == 1)
        staticEvaluation = Evaluator::getEvaluation(state);
    int i = 0;
    for (; i < activeColorMoves.size(); i++) 
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
        bool isFutilityPruningOk = min(depth - nullMoveReduction + checkExtension, MAXIMUM_NEGAMAX_DEPTH) == 1 && !activeColorMoves[i].isCapture() && !isInactiveColorInCheck && !isActiveColorInCheck && abs(alpha - (MATE_IN_ZERO - 1)) > Evaluator::getCentipawnEquivalent('N') && abs(beta - (MATE_IN_ZERO - 1)) > Evaluator::getCentipawnEquivalent('N') && staticEvaluation + Evaluator::getCentipawnEquivalent('N') <= alpha;
        if (isFutilityPruningOk)
        {
            unmakeMove(state, activeColorMoves[i], couldActiveColorCastleKingside, couldActiveColorCastleQueenside, possibleEnPassantTargetColumn);
            i++;
            break;
        }
        pair<int, stack<Move>*> evaluation = negamax(state, 1, depth - nullMoveReduction + checkExtension, -beta, -alpha, true, isInactiveColorInCheck);
        evaluation.first = -evaluation.first;
        evaluation.second->push(activeColorMoves[i]);
        if (evaluation.first == TIMEOUT)
        {
            unmakeMove(state, activeColorMoves[i], couldActiveColorCastleKingside, couldActiveColorCastleQueenside, possibleEnPassantTargetColumn);
            killerMoves[1].clear();
            delete variation;
            return pair<int, stack<Move>*>(TIMEOUT, evaluation.second);
        }
        unmakeMove(state, activeColorMoves[i], couldActiveColorCastleKingside, couldActiveColorCastleQueenside, possibleEnPassantTargetColumn);
        if (evaluation.first >= beta)
        {
            if (activeColorMoves[i].isQuiet())
                killerMoves[0].insert(activeColorMoves[i]);
            if (it == transpositionTable.end() || min(depth, MAXIMUM_NEGAMAX_DEPTH) > get<0>(it->second) || min(depth, MAXIMUM_NEGAMAX_DEPTH) == get<0>(it->second) && evaluation.first > get<2>(it->second))
                transpositionTable[state] = tuple<int, NodeType, int, Move>(min(depth, MAXIMUM_NEGAMAX_DEPTH), NodeType::CUT, evaluation.first, activeColorMoves[i]);
            killerMoves[1].clear();
            delete variation;
            return pair<int, stack<Move>*>(evaluation.first, evaluation.second);
        }
        optimalMove = activeColorMoves[i];
        optimalEvaluation = evaluation.first;
        delete variation;
        variation = evaluation.second;
        i++;
        break;
    }
    bool isPVNode = beta - alpha > 1;
    for (; i < activeColorMoves.size(); i++)
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
        bool isFutilityPruningOk = min(depth - nullMoveReduction + checkExtension, MAXIMUM_NEGAMAX_DEPTH) == 1 && !activeColorMoves[i].isCapture() && !isInactiveColorInCheck && !isActiveColorInCheck && abs(max(alpha, optimalEvaluation) - (MATE_IN_ZERO - 1)) > Evaluator::getCentipawnEquivalent('N') && abs(beta - (MATE_IN_ZERO - 1)) > Evaluator::getCentipawnEquivalent('N') && staticEvaluation + Evaluator::getCentipawnEquivalent('N') <= max(alpha, optimalEvaluation);
        if (isFutilityPruningOk)
        {
            unmakeMove(state, activeColorMoves[i], couldActiveColorCastleKingside, couldActiveColorCastleQueenside, possibleEnPassantTargetColumn);
            continue;
        }
        int lateMoveReduction = legalMoves >= 4 && activeColorMoves[i].isQuiet() && !isActiveColorInCheck && !isInactiveColorInCheck && !isPVNode && min(depth - nullMoveReduction + checkExtension, MAXIMUM_NEGAMAX_DEPTH) >= 3 ? 1 : 0;
        pair<int, stack<Move>*> evaluation = negamax(state, 1, depth - nullMoveReduction - lateMoveReduction + checkExtension, -max(alpha, optimalEvaluation) - 1, -max(alpha, optimalEvaluation), true, isInactiveColorInCheck);
        evaluation.first = -evaluation.first;
        evaluation.second->push(activeColorMoves[i]);
        if (evaluation.first == TIMEOUT)
        {
            unmakeMove(state, activeColorMoves[i], couldActiveColorCastleKingside, couldActiveColorCastleQueenside, possibleEnPassantTargetColumn);
            killerMoves[1].clear();
            delete variation;
            return pair<int, stack<Move>*>(TIMEOUT, evaluation.second);
        }
        if ((!isPVNode && lateMoveReduction || isPVNode && evaluation.first < beta) && evaluation.first > max(alpha, optimalEvaluation))
        {
            delete evaluation.second;
            evaluation = negamax(state, 1, depth - nullMoveReduction + checkExtension, -beta, -max(alpha, optimalEvaluation), true, isInactiveColorInCheck);
            evaluation.first = -evaluation.first;
            evaluation.second->push(activeColorMoves[i]);
        }
        if (evaluation.first == TIMEOUT) 
        {
            unmakeMove(state, activeColorMoves[i], couldActiveColorCastleKingside, couldActiveColorCastleQueenside, possibleEnPassantTargetColumn);
            killerMoves[1].clear();
            delete variation;
            return pair<int, stack<Move>*>(TIMEOUT, evaluation.second);
        }
        unmakeMove(state, activeColorMoves[i], couldActiveColorCastleKingside, couldActiveColorCastleQueenside, possibleEnPassantTargetColumn);
        if (evaluation.first >= beta) 
        {
            if (activeColorMoves[i].isQuiet())
                killerMoves[0].insert(activeColorMoves[i]);
            if (it == transpositionTable.end() || min(depth, MAXIMUM_NEGAMAX_DEPTH) > get<0>(it->second) || min(depth, MAXIMUM_NEGAMAX_DEPTH) == get<0>(it->second) && evaluation.first > get<2>(it->second))
                transpositionTable[state] = tuple<int, NodeType, int, Move>(min(depth, MAXIMUM_NEGAMAX_DEPTH), NodeType::CUT, evaluation.first, activeColorMoves[i]);
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
    }
    if (optimalEvaluation <= alpha) 
    {
        if (it == transpositionTable.end() || min(depth, MAXIMUM_NEGAMAX_DEPTH) > get<0>(it->second) || min(depth, MAXIMUM_NEGAMAX_DEPTH) == get<0>(it->second) && optimalEvaluation < get<2>(it->second))
            transpositionTable[state] = tuple<int, NodeType, int, Move>(min(depth, MAXIMUM_NEGAMAX_DEPTH), NodeType::ALL, optimalEvaluation, optimalMove);
    }
    else if (it == transpositionTable.end() || min(depth, MAXIMUM_NEGAMAX_DEPTH) > get<0>(it->second))
        transpositionTable[state] = tuple<int, NodeType, int, Move>(min(depth, MAXIMUM_NEGAMAX_DEPTH), NodeType::PV, optimalEvaluation, optimalMove);
    killerMoves[1].clear();
    return pair<int, stack<Move>*>(optimalEvaluation, variation);
}
int Engine::perft(State& state, int currentDepth, int depth) const
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
void Engine::printSearchResult(int depth, const pair<int, stack<Move>*> optimalEvaluation) const
{
    cout << "\nDepth: " << depth << "\n";
    cout << "Move: " << moveToString(optimalEvaluation.second->top()) << "\n";
    cout << "Evaluation: ";
    if (abs(optimalEvaluation.first) > MAXIMUM_EVALUATION)
        cout << ((MATE_IN_ZERO - abs(optimalEvaluation.first)) % 2 ? '+' : '-') << 'M' << (MATE_IN_ZERO - abs(optimalEvaluation.first) + 1) / 2 << "\n";
    else
        cout << optimalEvaluation.first << "\n";
    cout << "Principal Variation: ";
    stack<Move> principalVariation(*optimalEvaluation.second);
    while (!principalVariation.empty())
    {
        cout << moveToString(principalVariation.top()) << " ";
        principalVariation.pop();
    }
    cout << "\n\n";
}
void Engine::ponder(int seconds) 
{
    this->seconds = seconds;
    start = chrono::steady_clock::now();
    pair<int, stack<Move>*> optimalEvaluation = quiescenceSearch(state, 0, -INT32_MAX, INT32_MAX);
    for (int depth = 1; depth <= MAXIMUM_NEGAMAX_DEPTH; depth++)
    {
        int alpha = optimalEvaluation.first - 25;
        int beta = optimalEvaluation.first + 25;
        pair<int, stack<Move>*> evaluation = negamax(state, depth, alpha, beta);
        if (evaluation.first == TIMEOUT)
        {
            killerMoves[0].clear();
            delete evaluation.second, optimalEvaluation.second;
            return;
        }
        if (evaluation.first <= alpha || evaluation.first >= beta)
        {
            delete evaluation.second;
            evaluation = negamax(state, depth, -INT32_MAX, INT32_MAX);
        }
        if (evaluation.first == TIMEOUT)
        {
            killerMoves[0].clear();
            delete evaluation.second, optimalEvaluation.second;
            return;
        }
        killerMoves[0].clear();
        delete optimalEvaluation.second;
        optimalEvaluation = evaluation;
    }
    delete optimalEvaluation.second;
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
    int i = 0;
    for (; i < activeColorMoves.size(); i++)
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
        if (optimalEvaluation >= beta)
        {
            unmakeMove(state, activeColorMoves[i], couldActiveColorCastleKingside, couldActiveColorCastleQueenside, possibleEnPassantTargetColumn);
            if (it == transpositionTable.end() || 0 > get<0>(it->second) || 0 == get<0>(it->second) && optimalEvaluation > get<2>(it->second))
                transpositionTable[state] = tuple<int, NodeType, int, Move>(0, NodeType::CUT, optimalEvaluation, optimalMove);
            return pair<int, stack<Move>*>(optimalEvaluation, variation);
        }
        if (!activeColorMoves[i].isCapture() && !isActiveColorInCheck) {
            unmakeMove(state, activeColorMoves[i], couldActiveColorCastleKingside, couldActiveColorCastleQueenside, possibleEnPassantTargetColumn);
            continue;
        }
        pair<int, stack<Move>*> evaluation = quiescenceSearch(state, currentDepth + 1, -beta, -alpha);
        evaluation.first = -evaluation.first;
        evaluation.second->push(activeColorMoves[i]);
        if (evaluation.first == TIMEOUT)
        {
            unmakeMove(state, activeColorMoves[i], couldActiveColorCastleKingside, couldActiveColorCastleQueenside, possibleEnPassantTargetColumn);
            killerMoves[currentDepth + 1].clear();
            delete variation;
            return pair<int, stack<Move>*>(-TIMEOUT, evaluation.second);
        }
        unmakeMove(state, activeColorMoves[i], couldActiveColorCastleKingside, couldActiveColorCastleQueenside, possibleEnPassantTargetColumn);
        if (evaluation.first >= beta)
        {
            if (activeColorMoves[i].isQuiet())
                killerMoves[currentDepth].insert(activeColorMoves[i]);
            if (it == transpositionTable.end() || 0 > get<0>(it->second) || 0 == get<0>(it->second) && evaluation.first > get<2>(it->second))
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
        i++;
        break;
    }
    for (; i < activeColorMoves.size(); i++) 
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
        if (!activeColorMoves[i].isCapture() && !isActiveColorInCheck) {
            unmakeMove(state, activeColorMoves[i], couldActiveColorCastleKingside, couldActiveColorCastleQueenside, possibleEnPassantTargetColumn);
            continue;
        }
        pair<int, stack<Move>*> evaluation = quiescenceSearch(state, currentDepth + 1, -max(alpha, optimalEvaluation) - 1, -max(alpha, optimalEvaluation));
        evaluation.first = -evaluation.first;
        evaluation.second->push(activeColorMoves[i]);
        if (evaluation.first == TIMEOUT) 
        {
            unmakeMove(state, activeColorMoves[i], couldActiveColorCastleKingside, couldActiveColorCastleQueenside, possibleEnPassantTargetColumn);
            killerMoves[currentDepth + 1].clear();
            delete variation;
            return pair<int, stack<Move>*>(-TIMEOUT, evaluation.second);
        }
        if (evaluation.first > max(alpha, optimalEvaluation) && evaluation.first < beta)
        {
            delete evaluation.second;
            evaluation = quiescenceSearch(state, currentDepth + 1, -beta, -max(alpha, optimalEvaluation));
            evaluation.first = -evaluation.first;
            evaluation.second->push(activeColorMoves[i]);
        }
        unmakeMove(state, activeColorMoves[i], couldActiveColorCastleKingside, couldActiveColorCastleQueenside, possibleEnPassantTargetColumn);
        if (evaluation.first >= beta) 
        {
            if (activeColorMoves[i].isQuiet())
                killerMoves[currentDepth].insert(activeColorMoves[i]);
            if (it == transpositionTable.end() || 0 > get<0>(it->second) || 0 == get<0>(it->second) && evaluation.first > get<2>(it->second))
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
        if (it == transpositionTable.end() || 0 > get<0>(it->second) || 0 == get<0>(it->second) && optimalEvaluation < get<2>(it->second))
            transpositionTable[state] = tuple<int, NodeType, int, Move>(0, NodeType::ALL, optimalEvaluation, optimalMove);
    }
    else if (optimalEvaluation >= beta) 
    {
        if (it == transpositionTable.end() || 0 > get<0>(it->second) || 0 == get<0>(it->second) && optimalEvaluation > get<2>(it->second))
            transpositionTable[state] = tuple<int, NodeType, int, Move>(0, NodeType::CUT, optimalEvaluation, optimalMove);
    }
    else if (it == transpositionTable.end() || 0 > get<0>(it->second))
        transpositionTable[state] = tuple<int, NodeType, int, Move>(0, NodeType::PV, optimalEvaluation, optimalMove);
    return pair<int, stack<Move>*>(optimalEvaluation, variation);
}
Engine::Engine() 
{
    MoveGenerator::initialize();
}
void Engine::getOptimalMoveDepthVersion(const State& state, int maximumDepth)
{
    if (maximumDepth > MAXIMUM_NEGAMAX_DEPTH)
    {
        cout << "\nDepth greater than maximum depth.\n\n";
        return;
    }
    State s(state);
    this->seconds = INT32_MAX;
    start = chrono::steady_clock::now();
    pair<int, stack<Move>*> optimalEvaluation = quiescenceSearch(s, 0, -INT32_MAX, INT32_MAX);
    for (int depth = 1; depth <= maximumDepth; depth++)
    {
        int dAlpha = -25;
        int dBeta = 25;
        while (true) 
        {
            int alpha = optimalEvaluation.first + dAlpha;
            int beta = optimalEvaluation.first + dBeta;
            pair<int, stack<Move>*> evaluation = negamax(s, depth, alpha, beta);
            if (evaluation.first > alpha && evaluation.first < beta) 
            {
                delete optimalEvaluation.second;
                optimalEvaluation = evaluation;
                printSearchResult(depth, optimalEvaluation);
                break;
            }
            if (evaluation.first <= alpha)
                dAlpha *= 2;
            else
                dBeta *= 2;
            delete evaluation.second;
        }
    }
    transpositionTable.clear();
    for (int i = 0; i < 2; i++)
    {
        if (optimalEvaluation.second->empty())
            break;
        makeMove(s, optimalEvaluation.second->top());
        optimalEvaluation.second->pop();
    }
    this->state = s;
    delete optimalEvaluation.second;
}
void Engine::getOptimalMoveMoveTimeVersion(const State& state, int seconds) 
{
    State s(state);
    this->seconds = seconds;
    start = chrono::steady_clock::now();
    pair<int, stack<Move>*> optimalEvaluation = quiescenceSearch(s, 0, -INT32_MAX, INT32_MAX);
    for (int depth = 1; depth <= MAXIMUM_NEGAMAX_DEPTH; depth++) 
    {
        int dAlpha = -25;
        int dBeta = 25;
        while (true)
        {
            int alpha = optimalEvaluation.first + dAlpha;
            int beta = optimalEvaluation.first + dBeta;
            pair<int, stack<Move>*> evaluation = negamax(s, depth, alpha, beta);
            if (evaluation.first == TIMEOUT)
            {
                killerMoves[0].clear();
                transpositionTable.clear();
                for (int i = 0; i < 2; i++)
                {
                    if (optimalEvaluation.second->empty())
                        break;
                    makeMove(s, optimalEvaluation.second->top());
                    optimalEvaluation.second->pop();
                }
                this->state = s;
                delete evaluation.second, optimalEvaluation.second;
                return;
            }
            if (evaluation.first > alpha && evaluation.first < beta) 
            {
                delete optimalEvaluation.second;
                optimalEvaluation = evaluation;
                printSearchResult(depth, optimalEvaluation);
                break;
            }
            if (evaluation.first <= alpha)
                dAlpha *= 2;
            else
                dBeta *= 2;
            delete evaluation.second;
        }
    }
    transpositionTable.clear();
    for (int i = 0; i < 2; i++)
    {
        if (optimalEvaluation.second->empty())
            break;
        makeMove(s, optimalEvaluation.second->top());
        optimalEvaluation.second->pop();
    }
    this->state = s;
    delete optimalEvaluation.second;
}
void Engine::perft(const State& state, int depth) const
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
