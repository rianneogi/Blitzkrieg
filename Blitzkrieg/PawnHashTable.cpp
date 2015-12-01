#include "PawnHashTable.h"

PawnHashTable PawnTable(65536);
int PawnSizeMinusOne = 65535;

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

Bitset PawnHashTable::getPawns(Bitset key, int col)
{
	PawnHashEntry* hash = &entries[key&PawnSizeMinusOne];
	if (hash->key == key)
	{
		if (col == COLOR_WHITE)
			return hash->whitepawns;
		return hash->blackpawns;
	}
	return 1;
}

PawnHashEntry::PawnHashEntry(): key(0), score(0), whitepawns(0), blackpawns(0)
{
}

PawnHashEntry::PawnHashEntry(Bitset k, int s, Bitset wp, Bitset bp): key(k), score(s), whitepawns(wp), blackpawns(bp)
{
}
