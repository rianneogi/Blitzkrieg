#pragma once
#include "Position.h"

extern int CONS_TTUNKNOWN;

enum BOUNDTYPE{TT_EXACT,TT_ALPHA,TT_BETA};

extern Bitset TT_PieceKey[2][6][64];
extern Bitset TT_ColorKey;
extern Bitset TT_CastlingKey[2][2];
extern Bitset TT_EPKey[64];

struct HashEntry
{
	Bitset key;
	int depth;
	int score;
	int bound;
	Move bestmove;

	HashEntry();
	HashEntry(Bitset k,int d,int s,int b,Move m);
};

class TranspositionTable
{
public:
	HashEntry* entries;
	unsigned long long size;

	TranspositionTable(unsigned long long s);
	~TranspositionTable();

	void Save(HashEntry const& entry);
	void Save(Bitset key,int depth,int score,int bound,Move bestmove);
	int Probe(Bitset key,int depth,int alpha,int beta);
	Move getBestMove(Bitset key);
};


//HashEntry createHashEntry(Position const& pos);
void TTinit();
Bitset TTRandom();

extern TranspositionTable Table;

