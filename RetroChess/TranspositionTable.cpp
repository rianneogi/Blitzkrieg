#include "TranspositionTable.h"

Bitset TT_PieceKey[2][6][64];
Bitset TT_ColorKey;
Bitset TT_CastlingKey[2][2];
Bitset TT_EPKey[64];

int CONS_TTUNKNOWN = -32767;


TranspositionTable Table(4194304);
int SizeMinusOne = 4194303;

TranspositionTable::TranspositionTable(unsigned long long s)
{
	size = s;
	entries = new HashEntry[size];
}

TranspositionTable::~TranspositionTable()
{
	delete [] entries;
}

void TranspositionTable::Save(HashEntry const& entry)
{
	HashEntry* hash = &entries[entry.key&SizeMinusOne];

	if(hash->key!=entry.key || entry.depth >= hash->depth)
	{
		hash->key = entry.key;
		hash->score = entry.score;
		hash->depth = entry.depth;
		hash->bound = entry.bound;
		hash->bestmove = entry.bestmove;
	}
}

void TranspositionTable::Save(Bitset key,int depth,int score,int bound,Move bestmove)
{
	HashEntry* hash = &entries[key&SizeMinusOne];

	if(hash->key!=key || depth >= hash->depth)
	{
		hash->key = key;
		hash->score = score;
		hash->depth = depth;
		hash->bound = bound;
		hash->bestmove = bestmove;
	}
}

int TranspositionTable::Probe(Bitset key,int depth,int alpha,int beta)
{
    HashEntry* hash = &entries[key&SizeMinusOne];
    if(hash->key == key)
    {
        if((hash->depth >= depth))
        {
            if(hash->bound == TT_EXACT)
                return hash->score;
            if(hash->bound == TT_ALPHA && hash->score <= alpha)
                return alpha;
            if(hash->bound == TT_BETA && hash->score >= beta)
                return beta;
        }
    }
    return CONS_TTUNKNOWN;
}

Move TranspositionTable::getBestMove(Bitset key)
{
	HashEntry* hash = &entries[key&SizeMinusOne];
	return hash->bestmove;
}

void TTinit()
{
	for (int i=0; i< 2; i++)
    {
        for (int j=0; j<6; j++)
        {
            for (int k=0; k<64; k++)
            {
                TT_PieceKey[i][j][k] = TTRandom();
            }
        }
    }

    TT_ColorKey = TTRandom();

    for (int i=0; i<2; i++)
    {
		for(int j=0;j<2;j++)
		{
            TT_CastlingKey[i][j] = TTRandom();
		}
    }

    for (int i=0; i<64; i++)
    {
        TT_EPKey[i] = TTRandom();
    }
}

Bitset TTRandom()
{
	return (rand() | (rand()<<16) | (rand()<<32) | (rand()<<48));
}

HashEntry::HashEntry()
{
	key = 0;
	depth = 0;
	score = CONS_TTUNKNOWN;
	bound = TT_EXACT;
}

HashEntry::HashEntry(Bitset k,int d,int s,int b,Move m)
{
	key = k;
	depth = d;
	score = s;
	bound = b;
	bestmove = m;
}

