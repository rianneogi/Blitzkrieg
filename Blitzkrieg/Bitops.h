#ifndef BITOPS_H_INCLUDED
#define BITOPS_H_INCLUDED

#include <iostream>
#include <conio.h>
#include <string>
#include <sstream>
#include <algorithm>
#include <assert.h>

using namespace std;

typedef unsigned long long Bitset;

enum PIECES {PIECE_PAWN,PIECE_KNIGHT,PIECE_BISHOP,PIECE_ROOK,PIECE_QUEEN,PIECE_KING,PIECE_NONE};
enum SQUARES{SQUARE_EMPTY,SQUARE_WHITEPAWN,SQUARE_WHITEKNIGHT,SQUARE_WHITEBISHOP,SQUARE_WHITEROOK,
SQUARE_WHITEQUEEN,SQUARE_WHITEKING,SQUARE_BLACKPAWN,SQUARE_BLACKKNIGHT,
SQUARE_BLACKBISHOP,SQUARE_BLACKROOK,SQUARE_BLACKQUEEN,SQUARE_BLACKKING};
enum COLORS {COLOR_WHITE,COLOR_BLACK,COLOR_NONE};
enum OUTPUTTYPE {OUTPUT_CONSOLE,OUTPUT_XBOARD,OUTPUT_UCI};

extern Bitset RookAttacks[64][4096];
extern Bitset BishopAttacks[64][512];

struct Magic
{
	Bitset mask;
	Bitset magic;
};

extern Magic BishopMagicTable[64];
extern Magic RookMagicTable[64];

extern int OUTPUT;
extern bool DEBUG;

extern stringstream OutStream;
extern stringstream InStream;
extern string debugstring;

extern Bitset ColoredSquares[2]; //light squares and dark squares
extern Bitset EnemyTerritory[2]; //opponent's side of the board
extern Bitset CenterBits;
extern Bitset CenterBorderBits;

extern Bitset Pos2Bit[64];

extern int Mirror[64]; //mirrors board horizontally
extern int ColorMirror[2][64]; //doesn't mirror board for white and mirrors board for black

extern int turn90[64]; //equivalent to turning the board 90 degrees clockwise
extern int turn45[64];
extern int turn135[64];
extern int turn180[64]; //equivalent to turning the board 180 degrees clockwise
extern int turn270[64]; //equivalent to turning the board 270 degrees clockwise
extern int flipA1H8[64];

extern int Rank[64];
extern int File[64];
extern int Diagonal[64];
extern int RankOffset[64];
extern int FileOffset[64];

extern int Opponent[2];
extern int Square2Piece[13];
extern int Square2Color[13];
extern int Piece2Square[2][7];
extern int Plus8[64];
extern int Minus8[64];

extern int SquareColor[64];

extern Bitset KnightMoves[64];
extern Bitset WhitePawnMoves[64];
extern Bitset BlackPawnMoves[64];
extern Bitset PawnMoves[2][64];
extern Bitset WhitePawnAttacks[64];
extern Bitset BlackPawnAttacks[64];
extern Bitset PawnAttacks[2][64];
extern Bitset KingMoves[64];
extern Bitset SlidingMoves[8][256];
extern Bitset RookRankMoves[64][256];
extern Bitset RookFileMoves[64][256];
extern Bitset BishopA1H8Moves[64][256];
extern Bitset BishopA8H1Moves[64][256];

extern Bitset AboveBits[2][64];
extern Bitset AboveSideBits[2][64];
extern Bitset AboveAndAboveSideBits[2][64];
extern Bitset FileBits[64];
extern Bitset SideBits[64];
extern Bitset FileAndSideBits[64];

extern Bitset KingShield1[2][64];
extern Bitset KingShield2[2][64];
extern Bitset KingField[2][64];

string Int2Sq(int n);
int Sq2Int(string s);

int getRank(Bitset b);
int getFile(Bitset b);
int getSquare(int file,int rank);

void datainit();

int firstOf(Bitset b);
int lastOf(Bitset b);
int popcnt2(Bitset b);

void printBitset(Bitset b);
void printBitsetDiag(Bitset b);

char Int2Char(int n);
std::string Long2Str(unsigned long long n);
std::string Int2Str(int n);

inline bool isValidSquare(int n)
{
	return (n < 64 && n >= 0);
}

inline int getSquareColor(int n)
{
	return SquareColor[n];
}

inline Bitset getPos2Bit(int n)
{
    //return Pos2Bit[n];
	return (1ULL << n);
}

inline int getOpponent(int n)
{
	//return Opponent[n];
	return !n;
}

inline int getSquare2Piece(int n)
{
	return Square2Piece[n];
}

inline int getSquare2Color(int n)
{
	return Square2Color[n];
}

inline int getPiece2Square(int n,int turn)
{
	return Piece2Square[turn][n];
}

inline int getPlus8(int n)
{
	return Plus8[n];
	//return n + 8;
}

inline int getMinus8(int n)
{
	//return n - 8;
	return Minus8[n];
}

inline int getMirror(int n)
{
	return Mirror[n];
}

inline int getColorMirror(int c,int n)
{
	return ColorMirror[c][n];
}

inline int getturn90(int n)
{
    return turn90[n];
}

inline int getturn45(int n)
{
    return turn45[n];
}

inline int getturn135(int n)
{
    return turn135[n];
}

inline int getturn180(int n)
{
    return turn180[n];
	//return 63 - n;
}

inline int getturn270(int n)
{
    return turn270[n];
}

inline int getflipA1H8(int n)
{
    return flipA1H8[n];
}


inline int getFile(unsigned long n)
{
    return File[n];
	//return (n & 0x7);
}

inline int getFile(int n)
{
    return File[n];
	//return (n & 0x7);
}

inline int getRank(unsigned long n)
{
    //return Rank[n];
	return (n >> 3);
}

inline int getRank(int n)
{
    //return Rank[n];
	return ((unsigned)(n) >> 3);
}

inline int getDiag(int n)
{
    return Diagonal[n];
}

inline int getRankOffset(int n)
{
    return RankOffset[n];
}

inline int getFileOffset(int n)
{
    return FileOffset[n];
}

inline Bitset getKnightMoves(int n)
{
    return KnightMoves[n];
}

inline Bitset getPawnMoves(int color,int n)
{
    return PawnMoves[color][n];
}

inline Bitset getPawnAttacks(int color,int n)
{
    return PawnAttacks[color][n];
}

inline Bitset getWhitePawnMoves(int n)
{
    return WhitePawnMoves[n];
}

inline Bitset getBlackPawnMoves(int n)
{
    return BlackPawnMoves[n];
}

inline Bitset getKingMoves(int n)
{
    return KingMoves[n];
}

inline Bitset getSlidingMoves(int n,int m)
{
    return SlidingMoves[n][m];
}

inline Bitset getRookRankMoves(int n,int m)
{
    return RookRankMoves[n][m];
}

inline Bitset getRookFileMoves(int n,int m)
{
    return RookFileMoves[n][m];
}

inline Bitset getBishopA1H8Moves(int n,int m)
{
    return BishopA1H8Moves[n][m];
}

inline Bitset getBishopA8H1Moves(int n,int m)
{
    return BishopA8H1Moves[n][m];
}

inline Bitset getAboveBits(int col,int n)
{
    return AboveBits[col][n];
}

inline Bitset getAboveSideBits(int col,int n)
{
    return AboveSideBits[col][n];
}

inline Bitset getAboveAndAboveSideBits(int col,int n)
{
    return AboveAndAboveSideBits[col][n];
}

inline Bitset getFileBits(int n)
{
    return FileBits[n];
}

inline Bitset getSideBits(int n)
{
	return SideBits[n];
}

inline Bitset getFileAndSideBits(int n)
{
    return FileAndSideBits[n];
}

inline Bitset getKingShield1(int color,int n)
{
    return KingShield1[color][n];
}

inline Bitset getKingShield2(int color,int n)
{
    return KingShield2[color][n];
}

inline Bitset getKingField(int color,int n)
{
    return KingField[color][n];
}

inline unsigned long long popcnt(Bitset b)
{
	return _mm_popcnt_u64(b);
}



#endif // BITOPS_H_INCLUDED
