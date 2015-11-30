#pragma once

#include "TranspositionTable.h"

class PawnHashEntry
{
public:
	Bitset key;
	int score;
	Bitset whitepawns;
	Bitset blackpawns;

	PawnHashEntry();
	PawnHashEntry(Bitset k, int s, Bitset wp, Bitset bp);
};

class PawnHashTable
{
public:
	PawnHashEntry* entries;
	unsigned long long size;

	PawnHashTable(unsigned long long s);
	~PawnHashTable();

	void Save(Bitset key, int score, Bitset wp, Bitset bp);
	int Probe(Bitset key);
};

extern PawnHashTable PawnTable;

