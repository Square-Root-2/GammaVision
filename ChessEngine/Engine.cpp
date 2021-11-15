#include "Engine.h"
#include "Evaluator.h"
#include "MoveGenerator.h"
#include "MoveType.h"
#include <random>

void Engine::makeMove(State& state, Move move) {
    state.setPiece(move.getEndRow(), move.getEndColumn(), state.getPiece(move.getBeginRow(), move.getBeginColumn()));
    state.setPiece(move.getBeginRow(), move.getBeginColumn(), '.');
    if (move.getType() == MoveType::CASTLE_KINGSIDE) {
        state.setPiece(move.getBeginRow(), 7, '.');
        state.setPiece(move.getBeginRow(), 5, state.getActiveColor() ? 'r' : 'R');
    }
    else if (move.getType() == MoveType::CASTLE_QUEENSIDE) {
        state.setPiece(move.getBeginRow(), 0, '.');
        state.setPiece(move.getBeginRow(), 3, state.getActiveColor() ? 'r' : 'R');
    }
    else if (move.getType() == MoveType::EN_PASSANT)
        state.setPiece(move.getBeginRow(), move.getEndColumn(), '.');
    else if (move.getType() == MoveType::PROMOTION_TO_BISHOP)
        state.setPiece(move.getEndRow(), move.getEndColumn(), state.getActiveColor() ? 'b' : 'B');
    else if (move.getType() == MoveType::PROMOTION_TO_KNIGHT)
        state.setPiece(move.getEndRow(), move.getEndColumn(), state.getActiveColor() ? 'n' : 'N');
    else if (move.getType() == MoveType::PROMOTION_TO_QUEEN)
        state.setPiece(move.getEndRow(), move.getEndColumn(), state.getActiveColor() ? 'q' : 'Q');
    else if (move.getType() == MoveType::PROMOTION_TO_ROOK)
        state.setPiece(move.getEndRow(), move.getEndColumn(), state.getActiveColor() ? 'r' : 'R');
    if (move.getType() == MoveType::CASTLE_KINGSIDE || move.getType() == MoveType::CASTLE_QUEENSIDE) {
        state.setCanActiveColorCastleKingside(false);
        state.setCanActiveColorCastleQueenside(false);
    }
    else if (move.getType() == MoveType::KING_MOVE) {
        state.setCanActiveColorCastleKingside(false);
        state.setCanActiveColorCastleQueenside(false);
    }
    else if (move.getType() == MoveType::ROOK_MOVE && abs(move.getBeginRow() - 3.5) == 3.5 && move.getBeginColumn() == 7)
        state.setCanActiveColorCastleKingside(false);
    else if (move.getType() == MoveType::ROOK_MOVE && abs(move.getBeginRow() - 3.5) == 3.5 && move.getBeginColumn() == 0)
        state.setCanActiveColorCastleQueenside(false);
    if (move.getType() == MoveType::PAWN_FORWARD_TWO)
        state.setPossibleEnPassantTargetColumn(move.getBeginColumn());
    else
        state.setPossibleEnPassantTargetColumn(-1);
    state.toggleActiveColor();
}
pair<Move, Evaluation> Engine::negamax(State& state, int depths) {
    if (chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - start).count() >= seconds)
        return pair<Move, Evaluation>(Move(0, 0, 0, 0, MoveType::TIMEOUT), Evaluation(0, 0));
    vector<Move> moves = MoveGenerator::getMoves(state);
    mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    shuffle(moves.begin(), moves.end(), rng);
    Move optimalMove;
    Evaluation alpha(-INFINITY, 0);
    tuple<string, bool, int, int> hashCode = state.getHashCode();
    for (int i = 0; i < moves.size(); i++) {
        makeMove(state, moves[i]);
        Evaluation evaluation = negamax(state, depths - 1, Evaluation(-INFINITY, 0), Evaluation(-alpha.getPawns(), abs(alpha.getPawns()) == INFINITY ? alpha.getMoves() - 1 : INT32_MAX));
        if (evaluation.getMoves() == INT32_MIN)
            return pair<Move, Evaluation>(Move(0, 0, 0, 0, MoveType::TIMEOUT), Evaluation(0, 0));
        evaluation.negatePawns();
        evaluation.incrementMoves();
        if (evaluation > alpha) {
            optimalMove = moves[i];
            alpha = evaluation;
        }
        state.setHashCode(hashCode);
    }
    return pair<Move, Evaluation>(optimalMove, alpha);
}
Evaluation Engine::negamax(State& state, int depths, Evaluation alpha, Evaluation beta) {
    if (chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - start).count() >= seconds)
        return Evaluation(0, INT32_MIN);
    vector<Move> moves = MoveGenerator::getMoves(state);
    if (moves.empty())
        return Evaluation(state.isActiveColorInCheck() ? -INFINITY : 0, 0);
    if (depths == 0)
        return Evaluation(Evaluator::getEvaluation(state), INT32_MAX);
    mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    shuffle(moves.begin(), moves.end(), rng);
    tuple<string, bool, int, int> hashCode = state.getHashCode();
    for (int i = 0; i < moves.size(); i++) {
        makeMove(state, moves[i]);
        Evaluation evaluation = negamax(state, depths - 1, Evaluation(-beta.getPawns(), abs(beta.getPawns()) == INFINITY ? beta.getMoves() - 1 : INT32_MAX), Evaluation(-alpha.getPawns(), abs(alpha.getPawns()) == INFINITY ? alpha.getMoves() - 1 : INT32_MAX));
        if (evaluation.getMoves() == INT32_MIN)
            return Evaluation(0, INT32_MIN);
        evaluation.negatePawns();
        evaluation.incrementMoves();
        // alpha-beta pruning
        if (evaluation >= beta)
            return beta;
        if (evaluation > alpha)
            alpha = evaluation;
        state.setHashCode(hashCode);
    }
    return alpha;
}
tuple<Move, Evaluation, int> Engine::getOptimalMove(string FEN, int seconds) {
    pair<Move, Evaluation> optimalMove;
    this->seconds = seconds;
    start = chrono::steady_clock::now();
    for (int depths = 1; ; depths++) {
        State state(FEN);
        pair<Move, Evaluation> move = negamax(state, depths);
        if (move.first.getType() == MoveType::TIMEOUT)
            return tuple<Move, Evaluation, int>(optimalMove.first, optimalMove.second, depths - 1);
        optimalMove = move;
    }
}
