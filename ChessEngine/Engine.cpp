#include "Engine.h"
#include <iostream>
#include "MoveComparator.h"
#include "MoveType.h"
#include <random>

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
int Engine::negamax(State& state, int currentDepth, int depth, int alpha, int beta, bool isNullMoveOk, bool isActiveColorInCheck) {
    if (chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - start).count() >= seconds)
        return -TIMEOUT;
    nodesSearched++;
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
    if (currentDepth >= min(depth, MAXIMUM_NEGAMAX_DEPTH)) {
        int evaluation = quiescenceSearch(state, currentDepth, alpha, beta);
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
    int checkExtension = isActiveColorInCheck ? 1 : 0;
    int nullMoveReduction = 0;
    if (!isActiveColorInCheck && isNullMoveOk) {
        state.toggleActiveColor();
        int R = depth - currentDepth > 6 ? MAXIMUM_R : MINIMUM_R;
        int evaluation = -negamax(state, currentDepth + 1, depth - R + checkExtension, -beta, -beta + 1, false, false);
        state.toggleActiveColor();
        if (evaluation >= beta) {
            nullMoveReduction = DR;
            if (currentDepth >= min(depth - nullMoveReduction, MAXIMUM_NEGAMAX_DEPTH)) {
                int evaluation = quiescenceSearch(state, currentDepth, alpha, beta);
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
        int evaluation = -negamax(state, currentDepth + 1, depth - nullMoveReduction - lateMoveReduction + checkExtension, -beta, -max(alpha, optimalEvaluation), true, isInactiveColorInCheck);
        if (evaluation == TIMEOUT) {
            killerMoves[currentDepth + 1].clear();
            return -TIMEOUT;
        }
        if (lateMoveReduction == 1 && evaluation > max(alpha, optimalEvaluation))
            evaluation = -negamax(state, currentDepth + 1, depth - nullMoveReduction + checkExtension, -beta, -max(alpha, optimalEvaluation), true, isInactiveColorInCheck);
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
        else
            transpositionTable[depth - currentDepth][isNullMoveOk][state] = tuple<NodeType, int, Move>(NodeType::PV, evaluation, Move());
        return evaluation;
    }
    if (optimalEvaluation <= alpha) {
        if (it == transpositionTable[depth - currentDepth][isNullMoveOk].end() || optimalEvaluation < get<1>(it->second))
            transpositionTable[depth - currentDepth][isNullMoveOk][state] = tuple<NodeType, int, Move>(NodeType::ALL, optimalEvaluation, optimalMove);
    }
    else
        transpositionTable[depth - currentDepth][isNullMoveOk][state] = tuple<NodeType, int, Move>(NodeType::PV, optimalEvaluation, optimalMove);
    return optimalEvaluation;
}
pair<Move, int> Engine::negamax(State& state, int depth, int alpha, int beta) {
    if (chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - start).count() >= seconds)
        return pair<Move, int>(Move(0, 0, 0, 0, MoveType::TIMEOUT, ' ', ' '), 0);
    nodesSearched++;
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
    bool isActiveColorInCheck = MoveGenerator::isActiveColorInCheck(state);
    int staticEvaluation = Evaluator::getEvaluation(state);
    int checkExtension = isActiveColorInCheck ? 1 : 0;
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
        int evaluation = -negamax(state, 1, depth - lateMoveReduction + checkExtension, -beta, -max(alpha, optimalEvaluation), true, isInactiveColorInCheck);
        if (evaluation == TIMEOUT) {
            killerMoves[1].clear();
            return pair<Move, int>(Move(0, 0, 0, 0, MoveType::TIMEOUT, ' ', ' '), 0);
        }
        if (lateMoveReduction == 1 && evaluation > max(alpha, optimalEvaluation))
            evaluation = -negamax(state, 1, depth + checkExtension, -beta, -max(alpha, optimalEvaluation), true, isInactiveColorInCheck);
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
    if (optimalEvaluation <= alpha) {
        if (it == transpositionTable[depth][false].end() || optimalEvaluation < get<1>(it->second))
            transpositionTable[depth][false][state] = tuple<NodeType, int, Move>(NodeType::ALL, optimalEvaluation, optimalMove);
    }
    else
        transpositionTable[depth][false][state] = tuple<NodeType, int, Move>(NodeType::PV, optimalEvaluation, optimalMove);
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
void Engine::printSearchResult(Move& optimalMove, int evaluation, int depth, int nodesSearched) {
    cout << "\nMove: " << char(optimalMove.getBeginColumn() + 'a') << " " << 8 - optimalMove.getBeginRow() << " " << char(optimalMove.getEndColumn() + 'a') << " " << 8 - optimalMove.getEndRow() << " ";
    if (optimalMove.getMoveType() == MoveType::PROMOTION_TO_BISHOP)
        cout << "Promotion to Bishop";
    else if (optimalMove.getMoveType() == MoveType::PROMOTION_TO_KNIGHT)
        cout << "Promotion to Knight";
    else if (optimalMove.getMoveType() == MoveType::PROMOTION_TO_QUEEN)
        cout << "Promotion to Queen";
    else if (optimalMove.getMoveType() == MoveType::PROMOTION_TO_ROOK)
        cout << "Promotion to Rook";
    cout << "\n";
    cout << "Evaluation: ";
    if (abs(evaluation) > MAXIMUM_EVALUATION)
        cout << "Mate in " << (MATE_IN_ZERO - abs(evaluation) + 1) / 2 << "\n";
    else
        cout << evaluation << "\n";
    cout << "Depth: " << depth << "\n";
    cout << "Nodes Searched: " << nodesSearched << "\n\n";
}
int Engine::quiescenceSearch(State& state, int currentDepth, int alpha, int beta) {
    if (chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - start).count() >= seconds)
        return -TIMEOUT;
    nodesSearched++;
    int standPat = Evaluator::getEvaluation(state);
    alpha = max(alpha, standPat);
    vector<Move> activeColorMoves;
    MoveGenerator::getMoves(activeColorMoves, state);
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
        if (activeColorMoves[i].isQuiet() && !isActiveColorInCheck) {
            unmakeMove(state, activeColorMoves[i], couldActiveColorCastleKingside, couldActiveColorCastleQueenside, possibleEnPassantTargetColumn);
            continue;
        }
        int evaluation = -quiescenceSearch(state, currentDepth + 1, -beta, -alpha);
        if (evaluation == TIMEOUT)
            return -TIMEOUT;
        unmakeMove(state, activeColorMoves[i], couldActiveColorCastleKingside, couldActiveColorCastleQueenside, possibleEnPassantTargetColumn);
        if (evaluation >= beta)
            return evaluation;
        standPat = max(standPat, evaluation);
        alpha = max(alpha, evaluation);
    }
    if (legalMoves == 0)
        return isActiveColorInCheck ? -(MATE_IN_ZERO - currentDepth) : 0;
    return standPat;
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
Engine::Engine() {
    MoveGenerator::initialize();
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
        for (int i = 0; i <= depth - 1; i++)
            for (int j = 0; j < 2; j++)
                transpositionTable[i][j].clear();
        State s(state);
        optimalMove = negamax(s, depth, -INT32_MAX, INT32_MAX);
        printSearchResult(optimalMove.first, optimalMove.second, depth, 0);
    }
}
void Engine::getOptimalMoveMoveTimeVersion(State& state, int seconds) {
    pair<Move, int> optimalMove;
    this->seconds = seconds;
    start = chrono::steady_clock::now();
    for (int depth = 1; depth <= MAXIMUM_NEGAMAX_DEPTH; depth++) {
        for (int i = 0; i <= depth - 1; i++)
            for (int j = 0; j < 2; j++)
                transpositionTable[i][j].clear();
        State s(state);
        nodesSearched = 0;
        pair<Move, int> move = negamax(s, depth, -INT32_MAX, INT32_MAX);
        if (move.first.getMoveType() == MoveType::TIMEOUT)
            return;
        optimalMove = move; 
        printSearchResult(optimalMove.first, optimalMove.second, depth, nodesSearched);
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
        cout << char(activeColorMoves[i].getBeginColumn() + 'a') << " " << 8 - activeColorMoves[i].getBeginRow() << " " << char(activeColorMoves[i].getEndColumn() + 'a') << " " << 8 - activeColorMoves[i].getEndRow() << ": " << nodes << "\n";
        unmakeMove(state, activeColorMoves[i], couldActiveColorCastleKingside, couldActiveColorCastleQueenside, possibleEnPassantTargetColumn);
    }
    cout << "\nTotal Nodes: " << totalNodes << "\n\n";
}
