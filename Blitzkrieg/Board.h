#pragma once

#include "Position.h"

class Board
{
public:
	std::vector<Move> prevmoves;
	std::vector<Move> moves;
	Position pos;

	Board(void);
	~Board(void);

	void makeMove(Move const& m);
	void makeMove(int n);
	void unmakeMove();
	void generateMoves();
	int getGameStatus();
};

