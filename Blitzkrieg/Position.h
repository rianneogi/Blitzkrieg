#ifndef POSITION_H_INCLUDED
#define POSITION_H_INCLUDED

#include <vector>
#include "Move.h"
#include "PawnHashTable.h"

enum CASTLE {CASTLE_KINGSIDE,CASTLE_QUEENSIDE};
enum GAMESTATUS{STATUS_NOTOVER,STATUS_WHITEMATED,STATUS_BLACKMATED,STATUS_STALEMATE,STATUS_INSUFFICIENTMAT,STATUS_3FOLDREP};

extern string pieceStrings[13];

class Position
{
public:
	Bitset TTKey;
	Bitset PawnKey;
	int turn;

    int Squares[64];
    Bitset Pieces[2][6];
    Bitset OccupiedSq;
    /*Bitset OccupiedSq90;
    Bitset OccupiedSq45;
    Bitset OccupiedSq135;*/
    
    Bitset castling[2][2];
    Bitset epsquare;

	vector<Move> movelist;
	vector<Bitset> hashlist;

    Position();
    Position(Position const& pos,Move const& m);
    ~Position();

	void initializeBitsets();
	void setStartPos();
	void clearBoard();
	void placePiece(int square, int location);
	void loadFromFEN(string fen);

    void generateMoves(vector<Move>& moves) const;
	void generateCaptures(vector<Move>& moves) const;
	void generatePawnPushes(vector<Move>& moves) const;
	bool isLegal(Move const& m);
    void addMove(std::vector<Move>& vec,Move const& m) const;
    void forceMove(Move const& m);
    bool makeMove(Move const& m);
    void unmakeMove(Move const& m);
    bool isAttacked(int turn,int n) const;
	Move getSmallestAttacker(int turn,int n);
	Move getSmallestAttacker(int turn,int n,unsigned long long occ);
	int getSmallestAttacker2(int turn,int n);
	Move makeCapture(int piece,int n); //makes the first legal move by piece to square n, returns the move made
    bool underCheck(int turn);
    int getGameStatus();
	int getColorPieces(int turn);
	void display(int flip);
	void takebackMove();
	bool isRepetition();
};
//Bitset getRookAttacks(int sq,Bitset occ);
//Bitset getBishopAttacks(int sq, Bitset occ);
//Bitset getQueenAttacks(int sq, Bitset occ);

inline Bitset getRookAttacks(int sq, Bitset occ)
{
	occ = ((occ&RookMagicTable[sq].mask)*RookMagicTable[sq].magic) >> RookMagicTable[sq].shift;
	return RookAttacks[sq][occ];
}

inline Bitset getBishopAttacks(int sq, Bitset occ)
{
	occ = ((occ&BishopMagicTable[sq].mask)*BishopMagicTable[sq].magic) >> BishopMagicTable[sq].shift;
	return BishopAttacks[sq][occ];
}

inline Bitset getQueenAttacks(int sq, Bitset occ)
{
	Bitset m = getRookAttacks(sq, occ);
	m |= getBishopAttacks(sq, occ);
	return m;
}

string getStringToken(string str, char delimiter, int token);
int getStringTokenPosition(string str, char delimiter, int token);

//int getSquare2Piece(int sq);
//int getSquare2Color(int sq);
//int getPiece2Square(int p,int color);
#endif // POSITION_H_INCLUDED
