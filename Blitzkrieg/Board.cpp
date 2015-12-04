#include "Board.h"


Board::Board(void)
{
	moves.reserve(128);
}


Board::~Board(void)
{
}

bool Board::makeMove(Move const& m)
{
	if (!pos.makeMove(m))
		return false;
	prevmoves.push_back(m);
	return true;
}

void Board::makeMove(int n)
{
	pos.forceMove(moves.at(n));
	prevmoves.push_back(moves.at(n));
}

void Board::unmakeMove()
{
	if(prevmoves.size()!=0)
	{
		Move m = prevmoves.at(prevmoves.size()-1);
		prevmoves.pop_back();
		pos.unmakeMove(m);
	}
}

void Board::generateMoves()
{
	moves.clear();
	pos.generateMoves(moves);
}

int Board::getGameStatus()
{
	return pos.getGameStatus();
}