#pragma once

#include "Bitops.h"



extern Bitset RookAttacks[64][4096];
extern Bitset BishopAttacks[64][512];

struct Magic
{
	Bitset mask;
	Bitset magic;
	int shift;
};

extern Magic BishopMagicTable[64];
extern Magic RookMagicTable[64];

Bitset generateRookMask(int sq);
Bitset generateBishopMask(int sq);
Bitset generateRookAttacks(int sq, Bitset block);
Bitset generateBishopAttacks(int sq, Bitset block);

Bitset find_magic(int sq, int m, int bishop);

void attacksinit();
void magicinit();
void generateMagics();