#include "Engine.h"
#include "Evaluator.h"
#include <iostream>
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
bool Engine::makeMove(State& state, Move& move) {
    state.setPiece(move.getEndRow(), move.getEndColumn(), move.getAggressor());
    state.setPiece(move.getBeginRow(), move.getBeginColumn(), '.');
    if (move.getMoveType() == MoveType::KINGSIDE_CASTLE) {
        state.setPiece(move.getBeginRow(), 7, '.');
        state.setPiece(move.getBeginRow(), 5, state.getActiveColor() ? 'r' : 'R');
    }
    else if (move.getMoveType() == MoveType::QUEENSIDE_CASTLE) {
        state.setPiece(move.getBeginRow(), 0, '.');
        state.setPiece(move.getBeginRow(), 3, state.getActiveColor() ? 'r' : 'R');
    }
    else if (move.getMoveType() == MoveType::EN_PASSANT)
        state.setPiece(move.getBeginRow(), move.getEndColumn(), '.');
    else if (move.getMoveType() == MoveType::PROMOTION_TO_BISHOP)
        state.setPiece(move.getEndRow(), move.getEndColumn(), state.getActiveColor() ? 'b' : 'B');
    else if (move.getMoveType() == MoveType::PROMOTION_TO_KNIGHT)
        state.setPiece(move.getEndRow(), move.getEndColumn(), state.getActiveColor() ? 'n' : 'N');
    else if (move.getMoveType() == MoveType::PROMOTION_TO_QUEEN)
        state.setPiece(move.getEndRow(), move.getEndColumn(), state.getActiveColor() ? 'q' : 'Q');
    else if (move.getMoveType() == MoveType::PROMOTION_TO_ROOK)
        state.setPiece(move.getEndRow(), move.getEndColumn(), state.getActiveColor() ? 'r' : 'R');
    if (move.getMoveType() == MoveType::KINGSIDE_CASTLE || move.getMoveType() == MoveType::QUEENSIDE_CASTLE) {
        state.setCanActiveColorCastleKingside(false);
        state.setCanActiveColorCastleQueenside(false);
    }
    else if (move.getAggressor() == 'K' || move.getAggressor() == 'k') {
        state.setCanActiveColorCastleKingside(false);
        state.setCanActiveColorCastleQueenside(false);
    }
    else if ((move.getAggressor() == 'R' || move.getAggressor() == 'r') && abs(move.getBeginRow() - 3.5) == 3.5 && move.getBeginColumn() == 7)
        state.setCanActiveColorCastleKingside(false);
    else if ((move.getAggressor() == 'R' || move.getAggressor() == 'r') && abs(move.getBeginRow() - 3.5) == 3.5 && move.getBeginColumn() == 0)
        state.setCanActiveColorCastleQueenside(false);
    if (move.getMoveType() == MoveType::PAWN_DOUBLE_PUSH)
        state.setPossibleEnPassantTargetColumn(move.getBeginColumn());
    else
        state.setPossibleEnPassantTargetColumn(-1);
    bool isLegal = !MoveGenerator::isActiveColorInCheck(state);
    state.toggleActiveColor();
    return isLegal;
}
void Engine::makeNullMove(State& state) {
    state.setPossibleEnPassantTargetColumn(-1);
    state.toggleActiveColor();
}
string Engine::moveToString(Move& move) {
    return string{ char(move.getBeginColumn() + 'a') }
        + to_string(8 - move.getBeginRow())
        + string{ char(move.getEndColumn() + 'a') }
        + to_string(8 - move.getEndRow())
        + (move.isPromotion() ? promotionToString.find(move.getMoveType())->second : "");
}
int Engine::negamax(State& state, int currentDepth, int depth, int alpha, int beta, bool isNullMoveOk, bool isActiveColorInCheck, bool hasThereBeenNullMove) {
    if (chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - start).count() >= seconds)
        return -TIMEOUT;
    unordered_map<State, tuple<NodeType, int, Move>>::iterator it = transpositionTable[max(depth - currentDepth, 0)][isNullMoveOk].find(state);
    if (it != transpositionTable[max(depth - currentDepth, 0)][isNullMoveOk].end()) {
        if ((get<0>(it->second) == NodeType::ALL || get<0>(it->second) == NodeType::PV) && get<1>(it->second) <= alpha)
            return get<1>(it->second);
        if ((get<0>(it->second) == NodeType::CUT || get<0>(it->second) == NodeType::PV) && get<1>(it->second) >= beta)
            return get<1>(it->second);
        if (get<0>(it->second) == NodeType::PV)
            return get<1>(it->second);
        if (get<0>(it->second) == NodeType::ALL && get<1>(it->second) + 1 > alpha)
            beta = min(beta, get<1>(it->second) + 1);
        else if (get<0>(it->second) == NodeType::CUT && get<1>(it->second) - 1 < beta)
            alpha = max(alpha, get<1>(it->second) - 1);
    }
    int checkExtension = isActiveColorInCheck ? 1 : 0;
    if (currentDepth >= min(depth + checkExtension, MAXIMUM_NEGAMAX_DEPTH)) {
        int evaluation = quiescenceSearch(state, currentDepth, alpha, beta, hasThereBeenNullMove);
        if (evaluation <= alpha) {
            if (it == transpositionTable[0][isNullMoveOk].end() || evaluation < get<1>(it->second))
                transpositionTable[0][isNullMoveOk][state] = tuple<NodeType, int, Move>(NodeType::ALL, evaluation, Move());
        }
        else if (evaluation >= beta) {
            if (it == transpositionTable[0][isNullMoveOk].end() || evaluation > get<1>(it->second))
                transpositionTable[0][isNullMoveOk][state] = tuple<NodeType, int, Move>(NodeType::CUT, evaluation, Move());
        }
        else
            transpositionTable[0][isNullMoveOk][state] = tuple<NodeType, int, Move>(NodeType::PV, evaluation, Move());
        return evaluation;
    }
    int nullMoveReduction = 0;
    if (!isActiveColorInCheck && isNullMoveOk) {
        int possibleEnPassantTargetColumn = state.getPossibleEnPassantTargetColumn();
        makeNullMove(state);
        int R = depth - currentDepth > 6 ? MAXIMUM_R : MINIMUM_R;
        int evaluation = -negamax(state, currentDepth + 1, depth - R + checkExtension, -beta, -beta + 1, false, false, true);
        unmakeNullMove(state, possibleEnPassantTargetColumn);
        if (evaluation >= beta) {
            nullMoveReduction = DR;
            if (currentDepth >= min(depth - nullMoveReduction, MAXIMUM_NEGAMAX_DEPTH)) {
                int evaluation = quiescenceSearch(state, currentDepth, alpha, beta, hasThereBeenNullMove);
                if (evaluation <= alpha) {
                    if (it == transpositionTable[depth - currentDepth][isNullMoveOk].end() || evaluation < get<1>(it->second))
                        transpositionTable[depth - currentDepth][isNullMoveOk][state] = tuple<NodeType, int, Move>(NodeType::ALL, evaluation, Move());
                }
                else if (evaluation >= beta) {
                    if (it == transpositionTable[depth - currentDepth][isNullMoveOk].end() || evaluation > get<1>(it->second))
                        transpositionTable[depth - currentDepth][isNullMoveOk][state] = tuple<NodeType, int, Move>(NodeType::CUT, evaluation, Move());
                }
                else
                    transpositionTable[depth - currentDepth][isNullMoveOk][state] = tuple<NodeType, int, Move>(NodeType::PV, evaluation, Move());
                return evaluation;
            }    
        }
    }
    vector<Move> activeColorMoves;
    MoveGenerator::getMoves(activeColorMoves, state);
    mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    shuffle(activeColorMoves.begin(), activeColorMoves.end(), rng);
    if (it != transpositionTable[max(depth - currentDepth, 0)][isNullMoveOk].end())
        sort(activeColorMoves.begin(), activeColorMoves.end(), MoveComparator(killerMoves[currentDepth], get<2>(it->second)));
    else
        sort(activeColorMoves.begin(), activeColorMoves.end(), MoveComparator(killerMoves[currentDepth]));
    Move optimalMove;
    int optimalEvaluation = -INT32_MAX;
    int legalMoves = activeColorMoves.size();
    int searchedMoves = 0;
    int staticEvaluation = Evaluator::getEvaluation(state);
    for (int i = 0; i < activeColorMoves.size(); i++) {
        bool couldActiveColorCastleKingside = state.canActiveColorCastleKingside();
        bool couldActiveColorCastleQueenside = state.canActiveColorCastleQueenside();
        int possibleEnPassantTargetColumn = state.getPossibleEnPassantTargetColumn();
        if (!makeMove(state, activeColorMoves[i])) {
            legalMoves--;
            unmakeMove(state, activeColorMoves[i], couldActiveColorCastleKingside, couldActiveColorCastleQueenside, possibleEnPassantTargetColumn);
            continue;
        }
        bool isInactiveColorInCheck = MoveGenerator::isActiveColorInCheck(state);
        bool isFutilityPruningOk = depth - currentDepth == 1 && !activeColorMoves[i].isCapture() && !isInactiveColorInCheck && !isActiveColorInCheck && abs(max(alpha, optimalEvaluation) - (MATE_IN_ZERO - depth)) > Evaluator::getCentipawnEquivalent('N') && abs(beta - (MATE_IN_ZERO - depth)) > Evaluator::getCentipawnEquivalent('N') && Evaluator::getCentipawnEquivalent('N') <= max(alpha, optimalEvaluation);
        if (isFutilityPruningOk) {
            unmakeMove(state, activeColorMoves[i], couldActiveColorCastleKingside, couldActiveColorCastleQueenside, possibleEnPassantTargetColumn);
            continue;
        }
        int lateMoveReduction = searchedMoves >= 4 && activeColorMoves[i].isQuiet() && !isActiveColorInCheck && !isInactiveColorInCheck && depth - currentDepth >= 3 ? 1 : 0;
        int evaluation = -negamax(state, currentDepth + 1, depth - nullMoveReduction - lateMoveReduction + checkExtension, -beta, -max(alpha, optimalEvaluation), true, isInactiveColorInCheck, hasThereBeenNullMove);
        if (evaluation == TIMEOUT) {
            killerMoves[currentDepth + 1].clear();
            return -TIMEOUT;
        }
        if (lateMoveReduction == 1 && evaluation > max(alpha, optimalEvaluation))
            evaluation = -negamax(state, currentDepth + 1, depth - nullMoveReduction + checkExtension, -beta, -max(alpha, optimalEvaluation), true, isInactiveColorInCheck, hasThereBeenNullMove);
        if (evaluation == TIMEOUT) {
            killerMoves[currentDepth + 1].clear();
            return -TIMEOUT;
        }
        unmakeMove(state, activeColorMoves[i], couldActiveColorCastleKingside, couldActiveColorCastleQueenside, possibleEnPassantTargetColumn);
        if (evaluation >= beta) {
            if (activeColorMoves[i].isQuiet())
                killerMoves[currentDepth].insert(activeColorMoves[i]);
            if (it == transpositionTable[depth - currentDepth][isNullMoveOk].end() || evaluation > get<1>(it->second))
                transpositionTable[depth - currentDepth][isNullMoveOk][state] = tuple<NodeType, int, Move>(NodeType::CUT, evaluation, activeColorMoves[i]);
            killerMoves[currentDepth + 1].clear();
            return evaluation;
        }
        if (evaluation > optimalEvaluation) {
            optimalMove = activeColorMoves[i];
            optimalEvaluation = evaluation;
        }
        searchedMoves++;
    }
    killerMoves[currentDepth + 1].clear();
    if (legalMoves == 0) {
        int evaluation = isActiveColorInCheck ? -(MATE_IN_ZERO - currentDepth) : 0;
        if (evaluation <= alpha) {
            if (it == transpositionTable[depth - currentDepth][isNullMoveOk].end() || evaluation < get<1>(it->second))
                transpositionTable[depth - currentDepth][isNullMoveOk][state] = tuple<NodeType, int, Move>(NodeType::ALL, evaluation, Move());
        }
        else if (evaluation >= beta) {
            if (it == transpositionTable[depth - currentDepth][isNullMoveOk].end() || evaluation > get<1>(it->second))
                transpositionTable[depth - currentDepth][isNullMoveOk][state] = tuple<NodeType, int, Move>(NodeType::CUT, evaluation, Move());
        }
        else {
            transpositionTable[depth - currentDepth][isNullMoveOk][state] = tuple<NodeType, int, Move>(NodeType::PV, evaluation, Move());
            if (!hasThereBeenNullMove)
                principalVariation[currentDepth][state] = Move();
        }
        return evaluation;
    }
    if (optimalEvaluation <= alpha) {
        if (it == transpositionTable[depth - currentDepth][isNullMoveOk].end() || optimalEvaluation < get<1>(it->second))
            transpositionTable[depth - currentDepth][isNullMoveOk][state] = tuple<NodeType, int, Move>(NodeType::ALL, optimalEvaluation, optimalMove);
    }
    else {
        transpositionTable[depth - currentDepth][isNullMoveOk][state] = tuple<NodeType, int, Move>(NodeType::PV, optimalEvaluation, optimalMove);
        if (!hasThereBeenNullMove)
            principalVariation[currentDepth][state] = optimalMove;
    }
    return optimalEvaluation;
}
pair<Move, int> Engine::negamax(State& state, int depth, int alpha, int beta) {
    if (chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - start).count() >= seconds)
        return pair<Move, int>(Move(0, 0, 0, 0, MoveType::TIMEOUT, ' ', ' '), 0);
    unordered_map<State, tuple<NodeType, int, Move>>::iterator it = transpositionTable[depth][false].find(state);
    if (it != transpositionTable[depth][false].end()) {
        if ((get<0>(it->second) == NodeType::ALL || get<0>(it->second) == NodeType::PV) && get<1>(it->second) <= alpha)
            return pair<Move, int>(get<2>(it->second), get<1>(it->second));
        if ((get<0>(it->second) == NodeType::CUT || get<0>(it->second) == NodeType::PV) && get<1>(it->second) >= beta)
            return pair<Move, int>(get<2>(it->second), get<1>(it->second));
        if (get<0>(it->second) == NodeType::PV)
            return pair<Move, int>(get<2>(it->second), get<1>(it->second));
        if (get<0>(it->second) == NodeType::ALL && get<1>(it->second) + 1 > alpha)
            beta = min(beta, get<1>(it->second) + 1);
        else if (get<0>(it->second) == NodeType::CUT && get<1>(it->second) - 1 < beta)
            alpha = max(alpha, get<1>(it->second) - 1);
    }
    bool isActiveColorInCheck = MoveGenerator::isActiveColorInCheck(state);
    int checkExtension = isActiveColorInCheck ? 1 : 0;
    vector<Move> activeColorMoves;
    MoveGenerator::getMoves(activeColorMoves, state);
    mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    shuffle(activeColorMoves.begin(), activeColorMoves.end(), rng);
    if (it != transpositionTable[depth][false].end())
        sort(activeColorMoves.begin(), activeColorMoves.end(), MoveComparator(get<2>(it->second)));
    else
        sort(activeColorMoves.begin(), activeColorMoves.end(), MoveComparator());
    Move optimalMove;
    int optimalEvaluation = -INT32_MAX;
    int legalMoves = activeColorMoves.size();
    int searchedMoves = 0;
    int staticEvaluation = Evaluator::getEvaluation(state);
    for (int i = 0; i < activeColorMoves.size(); i++) {
        bool couldActiveColorCastleKingside = state.canActiveColorCastleKingside();
        bool couldActiveColorCastleQueenside = state.canActiveColorCastleQueenside();
        int possibleEnPassantTargetColumn = state.getPossibleEnPassantTargetColumn();
        if (!makeMove(state, activeColorMoves[i])) {
            legalMoves--;
            unmakeMove(state, activeColorMoves[i], couldActiveColorCastleKingside, couldActiveColorCastleQueenside, possibleEnPassantTargetColumn);
            continue;
        }
        bool isInactiveColorInCheck = MoveGenerator::isActiveColorInCheck(state);
        bool isFutilityPruningOk = depth == 1 && !activeColorMoves[i].isCapture() && !isInactiveColorInCheck && !isActiveColorInCheck && abs(max(alpha, optimalEvaluation) - (MATE_IN_ZERO - depth)) > Evaluator::getCentipawnEquivalent('N') && abs(beta - (MATE_IN_ZERO - depth)) > Evaluator::getCentipawnEquivalent('N') && staticEvaluation + Evaluator::getCentipawnEquivalent('N') <= max(alpha, optimalEvaluation);
        if (isFutilityPruningOk) {
            unmakeMove(state, activeColorMoves[i], couldActiveColorCastleKingside, couldActiveColorCastleQueenside, possibleEnPassantTargetColumn);
            continue;
        }
        int lateMoveReduction = searchedMoves >= 4 && activeColorMoves[i].isQuiet() && !isActiveColorInCheck && !isInactiveColorInCheck && depth >= 3 ? 1 : 0;
        int evaluation = -negamax(state, 1, depth - lateMoveReduction + checkExtension, -beta, -max(alpha, optimalEvaluation), true, isInactiveColorInCheck, false);
        if (evaluation == TIMEOUT) {
            killerMoves[1].clear();
            return pair<Move, int>(Move(0, 0, 0, 0, MoveType::TIMEOUT, ' ', ' '), 0);
        }
        if (lateMoveReduction == 1 && evaluation > max(alpha, optimalEvaluation))
            evaluation = -negamax(state, 1, depth + checkExtension, -beta, -max(alpha, optimalEvaluation), true, isInactiveColorInCheck, false);
        if (evaluation == TIMEOUT) {
            killerMoves[1].clear();
            return pair<Move, int>(Move(0, 0, 0, 0, MoveType::TIMEOUT, ' ', ' '), 0);
        }
        unmakeMove(state, activeColorMoves[i], couldActiveColorCastleKingside, couldActiveColorCastleQueenside, possibleEnPassantTargetColumn);
        if (evaluation >= beta) {
            if (it == transpositionTable[depth][false].end() || evaluation > get<1>(it->second))
                transpositionTable[depth][false][state] = tuple<NodeType, int, Move>(NodeType::CUT, evaluation, activeColorMoves[i]);
            killerMoves[1].clear();
            return pair<Move, int>(activeColorMoves[i], evaluation);
        }
        if (evaluation > optimalEvaluation) {
            optimalMove = activeColorMoves[i];
            optimalEvaluation = evaluation;
        }
        searchedMoves++;
    }
    if (optimalEvaluation < alpha) {
        if (it == transpositionTable[depth][false].end() || optimalEvaluation < get<1>(it->second))
            transpositionTable[depth][false][state] = tuple<NodeType, int, Move>(NodeType::ALL, optimalEvaluation, optimalMove);
    }
    else {
        transpositionTable[depth][false][state] = tuple<NodeType, int, Move>(NodeType::PV, optimalEvaluation, optimalMove);
        principalVariation[0][state] = optimalMove;
    }
    killerMoves[1].clear();
    return pair<Move, int>(optimalMove, optimalEvaluation);
}
int Engine::perft(State& state, int currentDepth, int depth) {
    if (currentDepth == depth)
        return 1;
    vector<Move> activeColorMoves;
    MoveGenerator::getMoves(activeColorMoves, state);
    int nodes = 0;
    int legalMoves = activeColorMoves.size();
    for (int i = 0; i < activeColorMoves.size(); i++) {
        bool couldActiveColorCastleKingside = state.canActiveColorCastleKingside();
        bool couldActiveColorCastleQueenside = state.canActiveColorCastleQueenside();
        int possibleEnPassantTargetColumn = state.getPossibleEnPassantTargetColumn();
        if (!makeMove(state, activeColorMoves[i])) {
            legalMoves--;
            unmakeMove(state, activeColorMoves[i], couldActiveColorCastleKingside, couldActiveColorCastleQueenside, possibleEnPassantTargetColumn);
            continue;
        }
        nodes += perft(state, currentDepth + 1, depth);
        unmakeMove(state, activeColorMoves[i], couldActiveColorCastleKingside, couldActiveColorCastleQueenside, possibleEnPassantTargetColumn);
    }
    return nodes;
}
string Engine::principalVariationToString(State& state) {
    State s(state);
    string pv;
    int depth = 0;
    while (true) {
        unordered_map<State, Move>::iterator it = principalVariation[depth].find(s);
        if (it == principalVariation[depth].end() || it->second.getMoveType() == MoveType::NO_MOVE)
            break;
        pv += moveToString(it->second) + " ";
        makeMove(s, it->second);
        depth++;
    }
    return pv.substr(0, pv.size() - 1);
}
void Engine::printSearchResult(int depth, Move& optimalMove, int evaluation, State& state) {
    cout << "\nDepth: " << depth << "\n";
    cout << "Move: " << moveToString(optimalMove) << "\n";
    cout << "Evaluation: ";
    if (abs(evaluation) > MAXIMUM_EVALUATION)
        cout << (MATE_IN_ZERO - abs(evaluation) % 2 ? '+' : '-') << 'M' << (MATE_IN_ZERO - abs(evaluation) + 1) / 2 << "\n";
    else
        cout << evaluation << "\n";
    cout << "Principal Variation: " << principalVariationToString(state) << "\n\n";
}
int Engine::quiescenceSearch(State& state, int currentDepth, int alpha, int beta, bool hasThereBeenNullMove) {
    if (chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - start).count() >= seconds)
        return -TIMEOUT;
    Move optimalMove;
    int optimalEvaluation = Evaluator::getEvaluation(state);
    vector<Move> activeColorMoves;
    MoveGenerator::getMoves(activeColorMoves, state);
    mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    shuffle(activeColorMoves.begin(), activeColorMoves.end(), rng);
    sort(activeColorMoves.begin(), activeColorMoves.end(), MoveComparator());
    int legalMoves = activeColorMoves.size();
    bool isActiveColorInCheck = MoveGenerator::isActiveColorInCheck(state);
    for (int i = 0; i < activeColorMoves.size(); i++) {
        bool couldActiveColorCastleKingside = state.canActiveColorCastleKingside();
        bool couldActiveColorCastleQueenside = state.canActiveColorCastleQueenside();
        int possibleEnPassantTargetColumn = state.getPossibleEnPassantTargetColumn();
        if (!makeMove(state, activeColorMoves[i])) {
            legalMoves--;
            unmakeMove(state, activeColorMoves[i], couldActiveColorCastleKingside, couldActiveColorCastleQueenside, possibleEnPassantTargetColumn);
            continue;
        }
        if (!activeColorMoves[i].isCapture() && !isActiveColorInCheck) {
            unmakeMove(state, activeColorMoves[i], couldActiveColorCastleKingside, couldActiveColorCastleQueenside, possibleEnPassantTargetColumn);
            continue;
        }
        int evaluation = -quiescenceSearch(state, currentDepth + 1, -beta, -max(alpha, optimalEvaluation), hasThereBeenNullMove);
        if (evaluation == TIMEOUT)
            return -TIMEOUT;
        unmakeMove(state, activeColorMoves[i], couldActiveColorCastleKingside, couldActiveColorCastleQueenside, possibleEnPassantTargetColumn);
        if (evaluation >= beta)
            return evaluation;
        if (evaluation > optimalEvaluation) {
            optimalMove = activeColorMoves[i];
            optimalEvaluation = evaluation;
        }
    }
    if (legalMoves == 0) {
        int evaluation = isActiveColorInCheck ? -(MATE_IN_ZERO - currentDepth) : 0;
        if (evaluation > alpha && !hasThereBeenNullMove)
            principalVariation[currentDepth][state] = Move();
        return evaluation;
    }
    if (optimalEvaluation > alpha && !hasThereBeenNullMove)
        principalVariation[currentDepth][state] = optimalMove;
    return optimalEvaluation;
}
void Engine::unmakeMove(State& state, Move& move, bool couldActiveColorCastleKingside, bool couldActiveColorCastleQueenside, int possibleEnPassantTargetColumn) {
    state.toggleActiveColor();
    state.setPossibleEnPassantTargetColumn(possibleEnPassantTargetColumn);
    state.setCanActiveColorCastleKingside(couldActiveColorCastleKingside);
    state.setCanActiveColorCastleQueenside(couldActiveColorCastleQueenside);
    if (move.getMoveType() == MoveType::KINGSIDE_CASTLE) {
        state.setPiece(move.getBeginRow(), 5, '.');
        state.setPiece(move.getBeginRow(), 7, state.getActiveColor() ? 'r' : 'R');
    }
    else if (move.getMoveType() == MoveType::QUEENSIDE_CASTLE) {
        state.setPiece(move.getBeginRow(), 3, '.');
        state.setPiece(move.getBeginRow(), 0, state.getActiveColor() ? 'r' : 'R');
    }
    else if (move.getMoveType() == MoveType::EN_PASSANT)
        state.setPiece(move.getBeginRow(), move.getEndColumn(), move.getVictim());
    else if (move.isPromotion())
        state.setPiece(move.getEndRow(), move.getEndColumn(), move.getAggressor());
    if (move.getMoveType() == MoveType::EN_PASSANT)
        state.setPiece(move.getEndRow(), move.getEndColumn(), '.');
    else
        state.setPiece(move.getEndRow(), move.getEndColumn(), move.getVictim());
    state.setPiece(move.getBeginRow(), move.getBeginColumn(), move.getAggressor());
}
void Engine::unmakeNullMove(State& state, int possibleEnPassantTargetColumn) {
    state.toggleActiveColor();
    state.setPossibleEnPassantTargetColumn(possibleEnPassantTargetColumn);
}
Engine::Engine() {
    Evaluator::initialize();
    MoveGenerator::initialize();
    State::initialize();
}
void Engine::getOptimalMoveDepthVersion(State& state, int maximumDepth) {
    if (maximumDepth > MAXIMUM_NEGAMAX_DEPTH) {
        cout << "\nDepth greater than maximum depth.\n\n";
        return;
    }
    pair<Move, int> optimalMove;
    this->seconds = INT32_MAX;
    start = chrono::steady_clock::now();
    for (int depth = 1; depth <= maximumDepth; depth++) {
        for (int i = 0; i <= MAXIMUM_NEGAMAX_DEPTH; i++)
            for (int j = 0; j < 2; j++)
                transpositionTable[i][j].clear();
        for (int i = 0; i <= MAXIMUM_NEGAMAX_DEPTH + MAXIMUM_QUIESCENCE_DEPTH; i++)
            principalVariation[i].clear();
        State s(state);
        optimalMove = negamax(s, depth, -INT32_MAX, INT32_MAX);
        printSearchResult(depth, optimalMove.first, optimalMove.second, state);
    }
}
void Engine::getOptimalMoveMoveTimeVersion(State& state, int seconds) {
    pair<Move, int> optimalMove;
    this->seconds = seconds;
    start = chrono::steady_clock::now();
    for (int depth = 1; depth <= MAXIMUM_NEGAMAX_DEPTH; depth++) {
        for (int i = 0; i <= MAXIMUM_NEGAMAX_DEPTH; i++)
            for (int j = 0; j < 2; j++)
                transpositionTable[i][j].clear();
        for (int i = 0; i <= MAXIMUM_NEGAMAX_DEPTH + MAXIMUM_QUIESCENCE_DEPTH; i++)
            principalVariation[i].clear();
        State s(state);
        pair<Move, int> move = negamax(s, depth, -INT32_MAX, INT32_MAX);
        if (move.first.getMoveType() == MoveType::TIMEOUT)
            return;
        optimalMove = move; 
        printSearchResult(depth, optimalMove.first, optimalMove.second, state);
    }
}
void Engine::perft(State& state, int depth) {
    if (depth > MAXIMUM_PERFT_DEPTH) {
        cout << "\nDepth greater than maximum depth.\n\n";
        return;
    }
    cout << "\n";
    vector<Move> activeColorMoves;
    MoveGenerator::getMoves(activeColorMoves, state);
    int totalNodes = 0;
    for (int i = 0; i < activeColorMoves.size(); i++) {
        bool couldActiveColorCastleKingside = state.canActiveColorCastleKingside();
        bool couldActiveColorCastleQueenside = state.canActiveColorCastleQueenside();
        int possibleEnPassantTargetColumn = state.getPossibleEnPassantTargetColumn();
        if (!makeMove(state, activeColorMoves[i])) {
            unmakeMove(state, activeColorMoves[i], couldActiveColorCastleKingside, couldActiveColorCastleQueenside, possibleEnPassantTargetColumn);
            continue;
        }
        int nodes = perft(state, 1, depth);
        totalNodes += nodes;
        cout << moveToString(activeColorMoves[i]) << ": " << nodes << "\n";
        unmakeMove(state, activeColorMoves[i], couldActiveColorCastleKingside, couldActiveColorCastleQueenside, possibleEnPassantTargetColumn);
    }
    cout << "\nTotal Nodes: " << totalNodes << "\n\n";
}
