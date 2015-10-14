#include "Engine.h"

#include <ctime>

Engine::Engine()
{
	myColor = COLOR_WHITE;
    pos = Position();
	historymove = Move();
}

Engine::Engine(int col)
{
	myColor = col;
	pos = Position();
	historymove = Move();
}

Engine::Engine(int col,Position const& p)
{
	myColor = col;
    pos = p;
	historymove = Move();
}

Engine::Engine(int col,Position const& p,Move const& prevmove)
{
	myColor = col;
    pos = p;
	historymove = Move();
}

Engine::~Engine()
{
}

void Engine::makeMove(int i)
{
	vector<Move> v;
	v.reserve(128);
	pos.generateMoves(v);
	Move m = v.at(i);
	pos.forceMove(m);
}

void Engine::makeMove(Move const& m)
{
	pos.forceMove(m);
}

void Engine::unmakeMove(Move const& m) //needs testing
{
	pos.unmakeMove(m);
}