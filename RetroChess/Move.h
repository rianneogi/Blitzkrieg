#ifndef MOVE_H_INCLUDED
#define MOVE_H_INCLUDED

#include "Bitops.h"
#include <vector>
#include <deque>

class Move
{
    public:
    Bitset data;

    Move();
	Move(Bitset n);
    Move(Bitset f,Bitset t,Bitset m,Bitset c,Bitset s,Bitset wk,Bitset wq,Bitset bk,Bitset bq,Bitset ep);
	
	bool operator==(Move const& m) const;
	bool operator!=(Move const& m) const;
	bool isEquivalent(Move const& m) const; //checks whether to, from and special are equal
	bool operator<(Move const& m) const;
	bool operator>(Move const& m) const;
    Bitset getbit() const;
    Bitset getbit90() const;
    Bitset getbit45() const;
    Bitset getbit135() const;
    Bitset getFrom() const; //int type
    Bitset getTo() const; //int type
    Bitset getMovingPiece() const; //Piece type
    Bitset getSpecial() const; //Piece type
    Bitset getCapturedPiece() const; //Square type
    Bitset getWKC() const;
    Bitset getWQC() const;
    Bitset getBKC() const;
    Bitset getBQC() const;
	Bitset getEP() const;
	string toString() const;
};

struct MoveReturn
{
	Move move;
	int moveid;
	int eval;
};

void sortMoves(vector<Move>& moves,int turn);
Move String2Move(const string& s); 

extern Move CONS_NULLMOVE;

#endif // MOVE_H_INCLUDED
