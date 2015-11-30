#include "PawnHashTable.h"

PawnHashTable PawnTable(32768);
int PawnSizeMinusOne = 32767;

PawnHashTable::PawnHashTable(unsigned long long s)
{
	size = s;
	entries = new PawnHashEntry[size];
}

PawnHashTable::~PawnHashTable()
{
	delete [] entries;
}

void PawnHashTable::Save(Bitset key, int score, Bitset wp, Bitset bp)
{
	PawnHashEntry* hash = &entries[key&PawnSizeMinusOne];

	if (hash->key != key)
	{
		hash->key = key;
		hash->score = score;
		hash->whitepawns = wp;
		hash->blackpawns = bp;
	}
}

int PawnHashTable::Probe(Bitset key)
{
	PawnHashEntry* hash = &entries[key&PawnSizeMinusOne];
	if (hash->key == key)
	{
		return hash->score;
	}
	return CONS_TTUNKNOWN;
}

PawnHashEntry::PawnHashEntry(): key(0), score(0), whitepawns(0), blackpawns(0)
{
}

PawnHashEntry::PawnHashEntry(Bitset k, int s, Bitset wp, Bitset bp): key(k), score(s), whitepawns(wp), blackpawns(bp)
{
}
