#include "Engine.h"

#include <ctime>

Engine::Engine()
{
	initialize();
}

Engine::Engine(int col)
{
	initialize();
	myColor = col;
	pos = Position();
}

Engine::Engine(int col,Position const& p)
{
	initialize();
	myColor = col;
    pos = p;
}

Engine::Engine(int col, Position const& p, Move const& prevmove)
{
	initialize();
	myColor = col;
	pos = p;
}

Engine::~Engine()
{
}

void Engine::initialize()
{
	myColor = COLOR_WHITE;
	pos = Position();

	alphafirst_sum = 0;
	alphalast_sum = 0;
	alpha_counter = 0;
	betacutoff_counter = 0;
	betacutoff_sum = 0;

	ply = 0;

	for (int i = 0;i<2;i++)
	{
		for (int j = 0;j<100;j++)
		{
			KillerMoves[i][j] = CONS_NULLMOVE;
			/*if(i!=2)
			KillerScores[i][j] = CONS_NEGINF;*/
		}
	}
	for (int i = 0;i < 100;i++)
	{
		Threats[i] = CONS_NULLMOVE;
	}
	for (unsigned int i = 0;i<64;i++) //ages the history table
	{
		for (unsigned int j = 0;j<64;j++)
		{
			HistoryScores[i][j] = 0;
		}
	}
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