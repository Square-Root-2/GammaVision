#include "Engine.h"
#include "MoveComparator.h"
#include "MoveType.h"
#include <random>
#include <iostream>

void Engine::makeMove(State& state, Move& move) {
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
    state.toggleActiveColor();
}
pair<Move, int> Engine::negamax(State& state, int depth, int alpha, int beta) {
    if (chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - start).count() >= seconds)
        return pair<Move, int>(Move(0, 0, 0, 0, MoveType::TIMEOUT, ' ', ' '), 0);
    vector<Move> activeColorMoves = moveGenerator.getMoves(state);
    tuple<string, bool, int, int> hashCode = state.getHashCode();
    map<tuple<string, bool, int, int>, tuple<int, NodeType, int, Move>>::iterator it = transpositionTable.find(hashCode);
    bool isActiveColorInCheck = state.isActiveColorInCheck();
    mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    shuffle(activeColorMoves.begin(), activeColorMoves.end(), rng);
    sort(activeColorMoves.begin(), activeColorMoves.end(), MoveComparator(killerMoves[0], it != transpositionTable.end() ? get<3>(it->second) : move));
    Move optimalMove;
    int staticEvaluation = Evaluator::getEvaluation(state);
    int searchedMoves = 0;
    for (int i = 0; i < activeColorMoves.size(); i++) {
        bool couldActiveColorCastleKingside = state.canActiveColorCastleKingside();
        bool couldActiveColorCastleQueenside = state.canActiveColorCastleQueenside();
        int possibleEnPassantTargetColumn = state.getPossibleEnPassantTargetColumn();
        makeMove(state, activeColorMoves[i]);
        bool isInactiveColorInCheck = state.isActiveColorInCheck();
        if (depth == 1 && !activeColorMoves[i].isCapture() && !isInactiveColorInCheck && !isActiveColorInCheck && abs(alpha) < mateValue - MAXIMUM_NEGAMAX_DEPTH - MAXIMUM_QUIESCENCE_DEPTH && abs(beta) < mateValue - MAXIMUM_NEGAMAX_DEPTH - MAXIMUM_QUIESCENCE_DEPTH && staticEvaluation + 320 <= alpha) {
            state.setHashCode(hashCode);
            continue;
        }
        vector<Move> inactiveColorMoves = moveGenerator.getMoves(state);
        int evaluation = -negamax(state, 1, depth - (searchedMoves++ >= 4 && activeColorMoves[i].isQuiet() && !isActiveColorInCheck && !isInactiveColorInCheck && inactiveColorMoves.size() != 1 && depth >= 3), -beta, -alpha, true, inactiveColorMoves, isInactiveColorInCheck);
        if (evaluation == mateValue + 1)
            return pair<Move, int>(Move(0, 0, 0, 0, MoveType::TIMEOUT, ' ', ' '), 0);
        if (evaluation > alpha)
            evaluation = -negamax(state, 1, depth, -beta, -alpha, true, inactiveColorMoves, isInactiveColorInCheck);
        if (evaluation == mateValue + 1)
            return pair<Move, int>(Move(0, 0, 0, 0, MoveType::TIMEOUT, ' ', ' '), 0);
        unmakeMove(state, activeColorMoves[i], couldActiveColorCastleKingside, couldActiveColorCastleQueenside, possibleEnPassantTargetColumn);
        if (evaluation >= beta) {
            transpositionTable[hashCode] = tuple<int, NodeType, int, Move>(depth, NodeType::CUT_NODE, beta, activeColorMoves[i]);
            return pair<Move, int>(activeColorMoves[i], beta);
        }
        if (evaluation > alpha) {
            optimalMove = activeColorMoves[i];
            alpha = evaluation;
        }
    }
    killerMoves[1].clear();
    if (optimalMove.getMoveType() == MoveType::NULL_MOVE)
        transpositionTable[hashCode] = tuple<int, NodeType, int, Move>(depth, NodeType::ALL_NODE, alpha, optimalMove);
    else
        transpositionTable[hashCode] = tuple<int, NodeType, int, Move>(depth, NodeType::PV_NODE, alpha, optimalMove);
    return pair<Move, int>(optimalMove, alpha);
}
int Engine::negamax(State& state, int currentDepth, int depth, int alpha, int beta, bool isNullOk, vector<Move>& activeColorMoves, bool isActiveColorInCheck) {
    if (chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - start).count() >= seconds)
        return -(mateValue + 1);
    tuple<string, bool, int, int> hashCode = state.getHashCode();
    map<tuple<string, bool, int, int>, tuple<int, NodeType, int, Move>>::iterator it = transpositionTable.find(hashCode);
    if (it != transpositionTable.end() && get<0>(it->second) >= depth - currentDepth) {
        if ((get<1>(it->second) == NodeType::PV_NODE || get<1>(it->second) == NodeType::CUT_NODE) && get<2>(it->second) >= beta)
            return beta;
        if ((get<1>(it->second) == NodeType::PV_NODE || get<1>(it->second) == NodeType::ALL_NODE) && get<2>(it->second) <= alpha)
            return alpha;
        if (get<1>(it->second) == NodeType::PV_NODE)
            return get<2>(it->second);
        if (get<1>(it->second) == NodeType::CUT_NODE)
            alpha = max(alpha, get<2>(it->second) - 1);
        else if (get<1>(it->second) == NodeType::ALL_NODE)
            beta = min(beta, get<2>(it->second) + 1);
    }
    if (activeColorMoves.empty()) {
        transpositionTable[hashCode] = tuple<int, NodeType, int, Move>(MAXIMUM_NEGAMAX_DEPTH, NodeType::PV_NODE, isActiveColorInCheck ? -(mateValue - currentDepth) : 0, move);
        return isActiveColorInCheck ? -(mateValue - currentDepth) : 0;
    }
    if (currentDepth >= depth) {
        int evaluation = quiescenceSearch(state, currentDepth, alpha, beta, activeColorMoves);
        transpositionTable[hashCode] = tuple<int, NodeType, int, Move>(0, NodeType::PV_NODE, evaluation, move);
        return evaluation;
    }
    if (isNullOk && !isActiveColorInCheck) {
        state.toggleActiveColor();
        vector<Move> inactiveColorMoves = moveGenerator.getMoves(state);
        int evaluation = -negamax(state, currentDepth + 1, depth - (depth - currentDepth > 6 ? MAXIMUM_R : MINIMUM_R), -beta, -beta + 1, false, inactiveColorMoves, false);
        if (evaluation == mateValue + 1)
            return -(mateValue + 1);
        state.toggleActiveColor();
        if (evaluation == beta) {
            depth -= DR;
            if (currentDepth >= depth) {
                int evaluation = quiescenceSearch(state, currentDepth, alpha, beta, activeColorMoves);
                transpositionTable[hashCode] = tuple<int, NodeType, int, Move>(0, NodeType::PV_NODE, evaluation, move);
                killerMoves[currentDepth + 1].clear();
                return evaluation;
            }
        }
    }
    sort(activeColorMoves.begin(), activeColorMoves.end(), MoveComparator(killerMoves[currentDepth], it != transpositionTable.end() ? get<3>(it->second) : move));
    Move optimalMove;
    int searchedMoves = 0;
    int staticEvaluation = Evaluator::getEvaluation(state);
    for (int i = 0; i < activeColorMoves.size(); i++) {
        bool couldActiveColorCastleKingside = state.canActiveColorCastleKingside();
        bool couldActiveColorCastleQueenside = state.canActiveColorCastleQueenside();
        int possibleEnPassantTargetColumn = state.getPossibleEnPassantTargetColumn();
        makeMove(state, activeColorMoves[i]);
        bool isInactiveColorInCheck = state.isActiveColorInCheck();
        if (depth - currentDepth == 1 && !activeColorMoves[i].isCapture() && !isInactiveColorInCheck && !isActiveColorInCheck && abs(alpha) < mateValue - MAXIMUM_NEGAMAX_DEPTH - MAXIMUM_QUIESCENCE_DEPTH && abs(beta) < mateValue - MAXIMUM_NEGAMAX_DEPTH - MAXIMUM_QUIESCENCE_DEPTH && staticEvaluation + 320 <= alpha) {
            state.setHashCode(hashCode);
            continue;
        }
        vector<Move> inactiveColorMoves = moveGenerator.getMoves(state);
        int evaluation = -negamax(state, currentDepth + 1, depth - (searchedMoves++ >= 4 && activeColorMoves[i].isQuiet() && !isActiveColorInCheck && !isInactiveColorInCheck && inactiveColorMoves.size() != 1 && depth - currentDepth >= 3), -beta, -alpha, true, inactiveColorMoves, isInactiveColorInCheck);
        if (evaluation == mateValue + 1)
            return -(mateValue + 1);
        if (evaluation > alpha)
            evaluation = -negamax(state, currentDepth + 1, depth, -beta, -alpha, true, inactiveColorMoves, isInactiveColorInCheck);
        if (evaluation == mateValue + 1)
            return -(mateValue + 1);
        unmakeMove(state, activeColorMoves[i], couldActiveColorCastleKingside, couldActiveColorCastleQueenside, possibleEnPassantTargetColumn);
        if (evaluation >= beta) {
            if (activeColorMoves[i].isQuiet())
                killerMoves[currentDepth].insert(activeColorMoves[i]);
            killerMoves[currentDepth + 1].clear();
            if (it == transpositionTable.end() || depth - currentDepth > get<0>(it->second))
                transpositionTable[hashCode] = tuple<int, NodeType, int, Move>(depth - currentDepth, NodeType::CUT_NODE, beta, activeColorMoves[i]);
            return beta;
        }
        if (evaluation > alpha) {
            alpha = evaluation;
            optimalMove = activeColorMoves[i];
        }
    }
    killerMoves[currentDepth + 1].clear();
    if (it == transpositionTable.end() || depth - currentDepth > get<0>(it->second))
        if (optimalMove.getMoveType() == MoveType::NULL_MOVE)
            transpositionTable[hashCode] = tuple<int, NodeType, int, Move>(depth - currentDepth, NodeType::ALL_NODE, alpha, optimalMove);
        else
            transpositionTable[hashCode] = tuple<int, NodeType, int, Move>(depth - currentDepth, NodeType::PV_NODE, alpha, optimalMove);
    return alpha;
}
int Engine::perft(State& state, int currentDepth, int depth) {
    if (currentDepth == depth)
        return 1;
    vector<Move> activeColorMoves = moveGenerator.getMoves(state);
    if (activeColorMoves.empty())
        return 0;
    if (depth - currentDepth == 1)
        return activeColorMoves.size();
    int nodes = 0;
    tuple<string, bool, int, int> hashCode = state.getHashCode();
    for (int i = 0; i < activeColorMoves.size(); i++) {
        makeMove(state, activeColorMoves[i]);
        nodes += perft(state, currentDepth + 1, depth);
        state.setHashCode(hashCode);
    }
    return nodes;
}
int Engine::quiescenceSearch(State& state, int currentDepth, int alpha, int beta, vector<Move>& activeColorMoves) {
    if (chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - start).count() >= seconds)
        return -(mateValue + 1);
    if (activeColorMoves.empty())
        return state.isActiveColorInCheck() ? -(mateValue - currentDepth) : 0;
    int standPat = Evaluator::getEvaluation(state);
    if (standPat >= beta)
        return beta;
    alpha = max(alpha, standPat);
    sort(activeColorMoves.begin(), activeColorMoves.end(), MoveComparator(killerMoves[0], move));
    for (int i = 0; i < activeColorMoves.size(); i++) {
        if (!activeColorMoves[i].isCapture())
            break;
        bool couldActiveColorCastleKingside = state.canActiveColorCastleKingside();
        bool couldActiveColorCastleQueenside = state.canActiveColorCastleQueenside();
        int possibleEnPassantTargetColumn = state.getPossibleEnPassantTargetColumn();
        makeMove(state, activeColorMoves[i]);
        vector<Move> inactiveColorMoves = moveGenerator.getMoves(state);
        int evaluation = -quiescenceSearch(state, currentDepth + 1, -beta, -alpha, inactiveColorMoves);
        if (evaluation == mateValue + 1)
            return -(mateValue + 1);
        unmakeMove(state, activeColorMoves[i], couldActiveColorCastleKingside, couldActiveColorCastleQueenside, possibleEnPassantTargetColumn);
        if (evaluation >= beta)
            return beta;
        alpha = max(alpha, evaluation);
    }
    return alpha;
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
int Engine::getMateValue() {
    return mateValue;
}
tuple<Move, int, int> Engine::getOptimalMove(string& FEN, int seconds) {
    transpositionTable.clear();
    pair<Move, int> optimalMove;
    this->seconds = seconds;
    start = chrono::steady_clock::now();
    State state(FEN);
    for (int depth = 1; depth <= MAXIMUM_NEGAMAX_DEPTH; depth++) {
        State state(FEN);
        pair<Move, int> move = negamax(state, depth, -INT32_MAX, INT32_MAX);
        if (move.first.getMoveType() == MoveType::TIMEOUT)
            return tuple<Move, int, int>(optimalMove.first, optimalMove.second, depth - 1);
        optimalMove = move;
    }
    return tuple<Move, int, int>(optimalMove.first, optimalMove.second, MAXIMUM_NEGAMAX_DEPTH);
}
void Engine::perft(State& state, int depth) {
    vector<Move> activeColorMoves = moveGenerator.getMoves(state);
    int totalNodes = 0;
    for (int i = 0; i < activeColorMoves.size(); i++) {
        bool couldActiveColorCastleKingside = state.canActiveColorCastleKingside();
        bool couldActiveColorCastleQueenside = state.canActiveColorCastleQueenside();
        int possibleEnPassantTargetColumn = state.getPossibleEnPassantTargetColumn();
        makeMove(state, activeColorMoves[i]);
        int nodes = perft(state, 1, depth);
        totalNodes += nodes;
        cout << char(activeColorMoves[i].getBeginColumn() + 'a') << " " << 8 - activeColorMoves[i].getBeginRow() << " " << char(activeColorMoves[i].getEndColumn() + 'a') << " " << 8 - activeColorMoves[i].getEndRow() << ": " << nodes << "\n";
        unmakeMove(state, activeColorMoves[i], couldActiveColorCastleKingside, couldActiveColorCastleQueenside, possibleEnPassantTargetColumn);
    }
    cout << "\nTotal Nodes: " << totalNodes << "\n\n";
}
