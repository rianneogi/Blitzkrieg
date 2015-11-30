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
    Bitset OccupiedSq90;
    Bitset OccupiedSq45;
    Bitset OccupiedSq135;
    
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
	void generateMoves2(vector<Move>& moves);
	bool isLegal(Move const& m);
    void addMove(std::vector<Move>& vec,Move const& m) const;
    void forceMove(Move const& m);
    bool makeMove(Move const& m);
    void unmakeMove(Move const& m);
    Move* getBestMove();
    bool checkLegal(Move* m);
    bool isAttacked(int turn,int n) const;
	Move getSmallestAttacker(int turn,int n);
	Move getSmallestAttacker(int turn,int n,unsigned long long occ,unsigned long long occ90,unsigned long long occ45,
								   unsigned long long occ135);
	int getSmallestAttacker2(int turn,int n);
	Move makeCapture(int piece,int n); //makes the first legal move by piece to square n, returns the move made
    bool underCheck(int turn);
    int getGameStatus();
	int getColorPieces(int turn);
	void display(int flip);
	void takebackMove();
	bool isRepetition();
};
unsigned long long getRookAttacks(int sq,unsigned long long occ,unsigned long long occ90);
unsigned long long getBishopAttacks(int sq,unsigned long long occ45,unsigned long long occ135);

string getStringToken(string str, char delimiter, int token);
int getStringTokenPosition(string str, char delimiter, int token);

//int getSquare2Piece(int sq);
//int getSquare2Color(int sq);
//int getPiece2Square(int p,int color);
#endif // POSITION_H_INCLUDED
