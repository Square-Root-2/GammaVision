#pragma once

#include "ChessBoard.h"
#include "Move.h"
#include <queue>

class MoveGenerator {
    static queue<Move> getBishopMoves(ChessBoard& chessBoard, int i, int j);
    static queue<Move> getKingMoves(ChessBoard& chessBoard, int i, int j);
    static queue<Move> getKnightMoves(ChessBoard& chessBoard, int i, int j);
    static queue<Move> getPawnMoves(ChessBoard& chessBoard, int i, int j);
    static queue<Move> getQueenMoves(ChessBoard& chessBoard, int i, int j);
    static queue<Move> getRookMoves(ChessBoard& chessBoard, int i, int j);
public:
    static vector<Move> getMoves(ChessBoard& chessBoard);
};
