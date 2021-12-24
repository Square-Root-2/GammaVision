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
    unordered_map<State, tuple<int, NodeType, int, Move>>::iterator it = transpositionTable.find(state);
    if (it != transpositionTable.end() && get<0>(it->second) >= min(depth, MAXIMUM_NEGAMAX_DEPTH) - currentDepth) {
        if ((get<1>(it->second) == NodeType::ALL || get<1>(it->second) == NodeType::PV) && get<2>(it->second) <= alpha)
            return get<2>(it->second);
        if ((get<1>(it->second) == NodeType::CUT || get<1>(it->second) == NodeType::PV) && get<2>(it->second) >= beta)
            return get<2>(it->second);
        if (get<1>(it->second) == NodeType::PV)
            return get<2>(it->second);
    }
    int checkExtension = isActiveColorInCheck ? 1 : 0;
    if (currentDepth >= min(depth + checkExtension, MAXIMUM_NEGAMAX_DEPTH))
        return quiescenceSearch(state, currentDepth, alpha, beta, hasThereBeenNullMove);
    int nullMoveReduction = 0;
    if (!isActiveColorInCheck && isNullMoveOk) {
        int possibleEnPassantTargetColumn = state.getPossibleEnPassantTargetColumn();
        makeNullMove(state);
        int R = min(depth, MAXIMUM_NEGAMAX_DEPTH) - currentDepth > 6 ? MAXIMUM_R : MINIMUM_R;
        int evaluation = -negamax(state, currentDepth + 1, depth - R + checkExtension, -beta, -beta + 1, false, false, true);
        unmakeNullMove(state, possibleEnPassantTargetColumn);
        if (evaluation >= beta) {
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
        bool isFutilityPruningOk = min(depth, MAXIMUM_NEGAMAX_DEPTH) - currentDepth == 1 && !activeColorMoves[i].isCapture() && !isInactiveColorInCheck && !isActiveColorInCheck && abs(max(alpha, optimalEvaluation) - (MATE_IN_ZERO - min(depth, MAXIMUM_NEGAMAX_DEPTH))) > Evaluator::getCentipawnEquivalent('N') && abs(beta - (MATE_IN_ZERO - min(depth, MAXIMUM_NEGAMAX_DEPTH))) > Evaluator::getCentipawnEquivalent('N') && Evaluator::getCentipawnEquivalent('N') <= max(alpha, optimalEvaluation);
        if (isFutilityPruningOk) {
            unmakeMove(state, activeColorMoves[i], couldActiveColorCastleKingside, couldActiveColorCastleQueenside, possibleEnPassantTargetColumn);
            continue;
        }
        int lateMoveReduction = searchedMoves >= 3 && activeColorMoves[i].isQuiet() && !isActiveColorInCheck && !isInactiveColorInCheck && min(depth, MAXIMUM_NEGAMAX_DEPTH) - currentDepth >= 3 ? 1 : 0;
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
            if (it == transpositionTable.end() || evaluation > get<2>(it->second))
                transpositionTable[state] = tuple<int, NodeType, int, Move>(min(depth, MAXIMUM_NEGAMAX_DEPTH) - currentDepth, NodeType::CUT, evaluation, activeColorMoves[i]);
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
    if (legalMoves == 0)
        return isActiveColorInCheck ? -(MATE_IN_ZERO - currentDepth) : 0;
    if (optimalEvaluation <= alpha) {
        if (it == transpositionTable.end() || optimalEvaluation < get<2>(it->second))
            transpositionTable[state] = tuple<int, NodeType, int, Move>(min(depth, MAXIMUM_NEGAMAX_DEPTH) - currentDepth, NodeType::ALL, optimalEvaluation, optimalMove);
    }
    else {
        transpositionTable[state] = tuple<int, NodeType, int, Move>(min(depth, MAXIMUM_NEGAMAX_DEPTH) - currentDepth, NodeType::PV, optimalEvaluation, optimalMove);
        if (optimalEvaluation > alpha && !hasThereBeenNullMove)
            principalVariation[currentDepth][state] = optimalMove;
    }
    return optimalEvaluation;
}
pair<Move, int> Engine::negamax(State& state, int depth, int alpha, int beta) {
    if (chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - start).count() >= seconds)
        return pair<Move, int>(Move(0, 0, 0, 0, MoveType::TIMEOUT, ' ', ' '), 0);
    unordered_map<State, tuple<int, NodeType, int, Move>>::iterator it = transpositionTable.find(state);
    if (it != transpositionTable.end() && get<0>(it->second) >= depth) {
        if ((get<1>(it->second) == NodeType::ALL || get<1>(it->second) == NodeType::PV) && get<2>(it->second) <= alpha)
            return pair<Move, int>(get<3>(it->second), get<2>(it->second));
        if ((get<1>(it->second) == NodeType::CUT || get<1>(it->second) == NodeType::PV) && get<2>(it->second) >= beta)
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
        int lateMoveReduction = searchedMoves >= 3 && activeColorMoves[i].isQuiet() && !isActiveColorInCheck && !isInactiveColorInCheck && depth >= 3 ? 1 : 0;
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
            if (activeColorMoves[i].isQuiet())
                killerMoves[0].insert(activeColorMoves[i]);
            if (it == transpositionTable.end() || evaluation > get<2>(it->second))
                transpositionTable[state] = tuple<int, NodeType, int, Move>(depth, NodeType::CUT, evaluation, activeColorMoves[i]);
            killerMoves[1].clear();
            return pair<Move, int>(activeColorMoves[i], evaluation);
        }
        if (evaluation > optimalEvaluation) {
            optimalMove = activeColorMoves[i];
            optimalEvaluation = evaluation;
        }
        searchedMoves++;
    }
    if (optimalEvaluation <= alpha) {
        if (it == transpositionTable.end() || optimalEvaluation < get<2>(it->second))
            transpositionTable[state] = tuple<int, NodeType, int, Move>(depth, NodeType::ALL, optimalEvaluation, optimalMove);
    }
    else {
        transpositionTable[state] = tuple<int, NodeType, int, Move>(depth, NodeType::PV, optimalEvaluation, optimalMove);
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
    unordered_map<State, tuple<int, NodeType, int, Move>>::iterator it = transpositionTable.find(state);
    if (it != transpositionTable.end()) {
        if ((get<1>(it->second) == NodeType::ALL || get<1>(it->second) == NodeType::PV) && get<2>(it->second) <= alpha)
            return get<2>(it->second);
        if ((get<1>(it->second) == NodeType::CUT || get<1>(it->second) == NodeType::PV) && get<2>(it->second) >= beta)
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
    for (int i = 0; i < activeColorMoves.size(); i++) {
        bool couldActiveColorCastleKingside = state.canActiveColorCastleKingside();
        bool couldActiveColorCastleQueenside = state.canActiveColorCastleQueenside();
        int possibleEnPassantTargetColumn = state.getPossibleEnPassantTargetColumn();
        if (!makeMove(state, activeColorMoves[i])) {
            legalMoves--;
            unmakeMove(state, activeColorMoves[i], couldActiveColorCastleKingside, couldActiveColorCastleQueenside, possibleEnPassantTargetColumn);
            continue;
        }
        if (currentDepth == MAXIMUM_DEPTH) {
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
        if (evaluation >= beta) {
            if (activeColorMoves[i].isQuiet())
                killerMoves[currentDepth].insert(activeColorMoves[i]);
            if (it == transpositionTable.end() || evaluation > get<2>(it->second))
                transpositionTable[state] = tuple<int, NodeType, int, Move>(0, NodeType::CUT, evaluation, activeColorMoves[i]);
            killerMoves[currentDepth + 1].clear();
            return evaluation;
        }
        if (evaluation > optimalEvaluation) {
            optimalMove = activeColorMoves[i];
            optimalEvaluation = evaluation;
        }
    }
    if (currentDepth < MAXIMUM_DEPTH)
        killerMoves[currentDepth + 1].clear();
    if (legalMoves == 0)
        return isActiveColorInCheck ? -(MATE_IN_ZERO - currentDepth) : 0;
    if (optimalEvaluation <= alpha) {
        if (it == transpositionTable.end() || optimalEvaluation < get<2>(it->second))
            transpositionTable[state] = tuple<int, NodeType, int, Move>(0, NodeType::ALL, optimalEvaluation, optimalMove);
    }
    else if (optimalEvaluation >= beta) {
        if (it == transpositionTable.end() || optimalEvaluation > get<2>(it->second))
            transpositionTable[state] = tuple<int, NodeType, int, Move>(0, NodeType::CUT, optimalEvaluation, optimalMove);
    }
    else {
        transpositionTable[state] = tuple<int, NodeType, int, Move>(0, NodeType::PV, optimalEvaluation, optimalMove);
        if (optimalEvaluation > alpha && !hasThereBeenNullMove)
            principalVariation[currentDepth][state] = optimalMove;
    }
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
    transpositionTable.clear();
    if (maximumDepth > MAXIMUM_NEGAMAX_DEPTH) {
        cout << "\nDepth greater than maximum depth.\n\n";
        return;
    }
    pair<Move, int> optimalMove(Move(), quiescenceSearch(state, 0, -INT32_MAX, INT32_MAX, false));
    this->seconds = INT32_MAX;
    start = chrono::steady_clock::now();
    for (int depth = 1; depth <= maximumDepth; depth++) {
        for (int i = 0; i <= MAXIMUM_DEPTH; i++)
            principalVariation[i].clear();
        State s(state);
        int alpha = optimalMove.second - 25;
        int beta = optimalMove.second + 25;
        pair<Move, int> move = negamax(s, depth, alpha, beta);
        if (move.second <= alpha || move.second >= beta)
            move = negamax(s, depth, -INT32_MAX, INT32_MAX);
        killerMoves[0].clear();
        optimalMove = move;
        printSearchResult(depth, optimalMove.first, optimalMove.second, state);
    }
}
void Engine::getOptimalMoveMoveTimeVersion(State& state, int seconds) {
    transpositionTable.clear();
    pair<Move, int> optimalMove(Move(), quiescenceSearch(state, 0, -INT32_MAX, INT32_MAX, false));
    this->seconds = seconds;
    start = chrono::steady_clock::now();
    for (int depth = 1; depth <= MAXIMUM_NEGAMAX_DEPTH; depth++) {
        for (int i = 0; i <= MAXIMUM_DEPTH; i++)
            principalVariation[i].clear();
        State s(state);
        int alpha = optimalMove.second - 25;
        int beta = optimalMove.second + 25;
        pair<Move, int> move = negamax(s, depth, alpha, beta);
        if (move.first.getMoveType() == MoveType::TIMEOUT) {
            killerMoves[0].clear();
            return;
        }
        if (move.second <= alpha || move.second >= beta)
            move = negamax(s, depth, -INT32_MAX, INT32_MAX);
        if (move.first.getMoveType() == MoveType::TIMEOUT) {
            killerMoves[0].clear();
            return;
        }
        killerMoves[0].clear();
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
