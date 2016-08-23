#include "TranspositionTable.h"

Bitset TT_PieceKey[2][6][64];
Bitset TT_ColorKey;
Bitset TT_CastlingKey[2][2];
Bitset TT_EPKey[64];

int CONS_TTUNKNOWN = -32767;


TranspositionTable Table(4194304);
//TranspositionTable Table(65536);
int SizeMinusOne = 4194303;

TranspositionTable::TranspositionTable(unsigned long long s)
{
	size = s;
	entries = new HashEntry[size];
	hits = 0;
}

TranspositionTable::~TranspositionTable()
{
	delete [] entries;
}

void TranspositionTable::Save(HashEntry const& entry)
{
	HashEntry* hash = &entries[entry.key&SizeMinusOne];

	if(hash->key!=entry.key 
		//|| entry.depth >= hash->depth
		)
	{
		hash->key = entry.key;
		hash->score = entry.score;
		hash->depth = entry.depth;
		hash->bound = entry.bound;
		hash->bestmove = entry.bestmove;
	}
}

void TranspositionTable::Save(Bitset key, int depth, int score, int bound, Move bestmove)
{
	HashEntry* hash = &entries[key&SizeMinusOne];

	if (hash->key != key 
		//|| depth >= hash->depth
		|| ((bound != TT_EXACT && hash->bound != TT_EXACT) || (bound==TT_EXACT && hash->bound==TT_EXACT)
			|| (bound==TT_EXACT && hash->bound!=TT_EXACT))
		//|| !(!hash->bestmove.isNullMove() && bestmove.isNullMove())
		)
	{
		hash->key = key;
		hash->score = score;
		hash->depth = depth;
		hash->bound = bound;
		hash->bestmove = bestmove;
	}
}

ProbeStruct TranspositionTable::Probe(Bitset key,int depth,int alpha,int beta)
{
	ProbeStruct pb;
    HashEntry* hash = &entries[key&SizeMinusOne];
	pb.found = false;
	pb.avoidnull = false;
    if(hash->key == key)
    {
		pb.entry = hash;
		hits++;
        if((hash->depth >= depth))
        {
			if (hash->bound == TT_EXACT 
				&& hash->score > alpha && hash->score < beta
				)
			{
				//return hash->score;
				pb.score = hash->score;
				pb.found = true;
			}
               
			if (hash->bound == TT_ALPHA && hash->score <= alpha)
			{
				//return hash->score;
				pb.score = hash->score;
				pb.found = true;
			}
               
			if (hash->bound == TT_BETA && hash->score >= beta)
			{
				//return hash->score;
				pb.score = hash->score;
				pb.found = true;
			}
        }

		if (hash->score < beta
			&& depth >= 3
			&& hash->bound == TT_ALPHA
			&& pb.found == true
			)
		{
			pb.avoidnull = true;
		}
    }
    return pb;
}

Move TranspositionTable::getBestMove(Bitset key)
{
	HashEntry* hash = &entries[key&SizeMinusOne];
	if(key==hash->key)
		return hash->bestmove;
	return CONS_NULLMOVE;
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
	unsigned long long r0 = rand(), r1 = rand(), r2 = rand(), r3 = rand();
	return (r0 | (r1 << 16ULL) | (r2 << 32ULL) | (r3 << 48ULL));
	//return (rand() | (rand()<<16) | (rand()<<32) | (rand()<<48));
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

