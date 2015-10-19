#include "Bitops.h"
#include <iostream>
#include <string>
#include <sstream>

using namespace std;

stringstream OutStream("");
stringstream InStream("");
string debugstring;

int OUTPUT = OUTPUT_CONSOLE;
bool DEBUG = false;

Bitset Pos2Bit[64];

Bitset KnightMoves[64];
Bitset WhitePawnMoves[64];
Bitset BlackPawnMoves[64];
Bitset PawnMoves[2][64];
Bitset WhitePawnAttacks[64];
Bitset BlackPawnAttacks[64];
Bitset PawnAttacks[2][64];
Bitset KingMoves[64];
Bitset SlidingMoves[8][256];
Bitset RookRankMoves[64][256];
Bitset RookFileMoves[64][256];
Bitset BishopA1H8Moves[64][256];
Bitset BishopA8H1Moves[64][256];
Bitset AboveBits[2][64];
Bitset AboveSideBits[2][64];
Bitset AboveAndAboveSideBits[2][64];
Bitset FileBits[64];
Bitset SideBits[64];
Bitset FileAndSideBits[64];
Bitset KingShield1[2][64];
Bitset KingShield2[2][64];
Bitset KingField[2][64];

int Opponent[2] = {COLOR_BLACK,COLOR_WHITE};

int Square2Piece[13] = {PIECE_NONE,PIECE_PAWN,PIECE_KNIGHT,PIECE_BISHOP,PIECE_ROOK,PIECE_QUEEN,
	PIECE_KING,PIECE_PAWN,PIECE_KNIGHT,PIECE_BISHOP,PIECE_ROOK,PIECE_QUEEN,PIECE_KING};
int Square2Color[13] = {COLOR_NONE,COLOR_WHITE,COLOR_WHITE,COLOR_WHITE,COLOR_WHITE,COLOR_WHITE,
	COLOR_WHITE,COLOR_BLACK,COLOR_BLACK,COLOR_BLACK,COLOR_BLACK,COLOR_BLACK,COLOR_BLACK};
int Piece2Square[2][7] = {{SQUARE_WHITEPAWN,SQUARE_WHITEKNIGHT,SQUARE_WHITEBISHOP,SQUARE_WHITEROOK,SQUARE_WHITEQUEEN,SQUARE_WHITEKING,
SQUARE_EMPTY},{SQUARE_BLACKPAWN,SQUARE_BLACKKNIGHT,SQUARE_BLACKBISHOP,SQUARE_BLACKROOK,SQUARE_BLACKQUEEN,SQUARE_BLACKKING,SQUARE_EMPTY}};

Bitset ColoredSquares[2] = { 0xAA55AA55AA55AA55, 0x55AA55AA55AA55AA };

int Plus8[64] = { 8, 9,10,11,12,13,14,15,
                 16,17,18,19,20,21,22,23,
                 24,25,26,27,28,29,30,31,
                 32,33,34,35,36,37,38,39,
                 40,41,42,43,44,45,46,47,
                 48,49,50,51,52,53,54,55,
                 56,57,58,59,60,61,62,63,
                 63,63,63,63,63,63,63,63};

int Minus8[64] = { 0, 0, 0, 0, 0, 0, 0, 0,
                   0, 1, 2, 3, 4, 5, 6, 7,
                   8, 9,10,11,12,13,14,15,
                  16,17,18,19,20,21,22,23,
                  24,25,26,27,28,29,30,31,
                  32,33,34,35,36,37,38,39,
                  40,41,42,43,44,45,46,47,
				  48,49,50,51,52,53,54,55};
int Mirror[64] = {56,57,58,59,60,61,62,63,
                  48,49,50,51,52,53,54,55,
                  40,41,42,43,44,45,46,47,
                  32,33,34,35,36,37,38,39,
                  24,25,26,27,28,29,30,31,
                  16,17,18,19,20,21,22,23,
                   8, 9,10,11,12,13,14,15,
                   0, 1, 2, 3, 4, 5, 6, 7};
int ColorMirror[2][64];
int turn180[64] = {63,62,61,60,59,58,57,56,
                   55,54,53,52,51,50,49,48,
                   47,46,45,44,43,42,41,40,
                   39,38,37,36,35,34,33,32,
                   31,30,29,28,27,26,25,24,
                   23,22,21,20,19,18,17,16,
                   15,14,13,12,11,10, 9, 8,
                    7, 6, 5, 4, 3, 2, 1, 0};
int turn270[64] = { 7,15,23,31,39,47,55,63,
                    6,14,22,30,38,46,54,62,
                    5,13,21,29,37,45,53,61,
                    4,12,20,28,36,44,52,60,
                    3,11,19,27,35,43,51,59,
                    2,10,18,26,34,42,50,58,
                    1, 9,17,25,33,41,49,57,
                    0, 8,16,24,32,40,48,56};
int turn90[64] =  {56,48,40,32,24,16, 8, 0,
                   57,49,41,33,25,17, 9, 1,
                   58,50,42,34,26,18,10, 2,
                   59,51,43,35,27,19,11, 3,
                   60,52,44,36,28,20,12, 4,
                   61,53,45,37,29,21,13, 5,
                   62,54,46,38,30,22,14, 6,
                   63,55,47,39,31,23,15, 7};
int turn45[64] =  {28,21,15,10, 6, 3, 1, 0,
                   36,29,22,16,11, 7, 4, 2,
                   43,37,30,23,17,12, 8, 5,
                   49,44,38,31,24,18,13, 9,
                   54,50,45,39,32,25,19,14,
                   58,55,51,46,40,33,26,20,
                   61,59,56,52,47,41,34,27,
                   63,62,60,57,53,48,42,35};
/*int turn45[64] =  {35,27,20,14, 9, 5, 2, 0,
                   42,34,26,19,13, 8, 4, 1,
                   48,41,33,25,18,12, 7, 3,
                   53,47,40,32,24,17,11, 6,
                   57,52,46,39,31,23,16,10,
                   60,56,51,45,38,30,22,15,
                   62,59,55,50,44,37,29,21,
                   63,61,58,54,49,43,36,28};*/
int turn135[64] = { 0, 2, 5, 9,14,20,27,35,
                    1, 4, 8,13,19,26,34,42,
                    3, 7,12,18,25,33,41,48,
                    6,11,17,24,32,40,47,53,
                   10,16,23,31,39,46,52,57,
                   15,22,30,38,45,51,56,60,
                   21,29,37,44,50,55,59,62,
                   28,36,43,49,54,58,61,63};
int flipA1H8[64] = {63,55,47,39,31,23,15, 7,
                    62,54,46,38,30,22,14, 6,
                    61,53,45,37,29,21,13, 5,
                    60,52,44,36,28,20,12, 4,
                    59,51,43,35,27,19,11, 3,
                    58,50,42,34,26,18,10, 2,
                    57,49,41,33,25,17, 9, 1,
                    56,48,40,32,24,16, 8, 0};
int Rank[64] = {0,0,0,0,0,0,0,0,
                1,1,1,1,1,1,1,1,
                2,2,2,2,2,2,2,2,
                3,3,3,3,3,3,3,3,
                4,4,4,4,4,4,4,4,
                5,5,5,5,5,5,5,5,
                6,6,6,6,6,6,6,6,
                7,7,7,7,7,7,7,7};
int File[64] = {7,6,5,4,3,2,1,0,
                7,6,5,4,3,2,1,0,
                7,6,5,4,3,2,1,0,
                7,6,5,4,3,2,1,0,
                7,6,5,4,3,2,1,0,
                7,6,5,4,3,2,1,0,
                7,6,5,4,3,2,1,0,
                7,6,5,4,3,2,1,0};
int Diagonal[64] = { 0, 1, 1, 3, 3, 3, 6, 6,
                     6, 6,10,10,10,10,10,15,
                    15,15,15,15,15,21,21,21,
                    21,21,21,21,28,28,28,28,
                    28,28,28,28,36,36,36,36,
                    36,36,36,43,43,43,43,43,
                    43,49,49,49,49,49,54,54,
                    54,54,58,58,58,61,61,63};
int RankOffset[64] = { 0, 0, 0, 0, 0, 0, 0, 0,
			           8, 8, 8, 8, 8, 8, 8, 8,
                      16,16,16,16,16,16,16,16,
                      24,24,24,24,24,24,24,24,
                      32,32,32,32,32,32,32,32,
                      40,40,40,40,40,40,40,40,
                      48,48,48,48,48,48,48,48,
                      56,56,56,56,56,56,56,56};
int FileOffset[64] = {56,48,40,32,24,16, 8, 0,
			          56,48,40,32,24,16, 8, 0,
                      56,48,40,32,24,16, 8, 0,
                      56,48,40,32,24,16, 8, 0,
                      56,48,40,32,24,16, 8, 0,
                      56,48,40,32,24,16, 8, 0,
                      56,48,40,32,24,16, 8, 0,
                      56,48,40,32,24,16, 8, 0};

int SquareColor[64] = { 0, 1, 0, 1, 0, 1, 0, 1,
			           1, 0, 1, 0, 1, 0, 1, 0,
                       0, 1, 0, 1, 0, 1, 0, 1,
			           1, 0, 1, 0, 1, 0, 1, 0,
                       0, 1, 0, 1, 0, 1, 0, 1,
			           1, 0, 1, 0, 1, 0, 1, 0,
                       0, 1, 0, 1, 0, 1, 0, 1,
					   1, 0, 1, 0, 1, 0, 1, 0};



string Int2Sq(int n)
{
    string s = "";
    char c = getFile(n)+97;
    char d = getRank(n)+49;
    s+=c;
    s+=d;
    return s;
}

int Sq2Int(string s)
{
    int n = 0;
    n+=(s.at(1)-49)*8;
    n+=7-(s.at(0)-97);
    if(n < 0 || n > 63)
    {
        return -1;
    }
    return n;
}

int getFile(Bitset b)
{
    Bitset a = 0x8080808080808080;
    int c = 0;
    while(true)
    {
        if((b & a)==0x0)
        {
            c++;
            b = b << 1;
        }
        else
        {
            break;
        }
    }
    return c;
}

int getRank(Bitset b)
{
    Bitset a = 0x00000000000000ff;
    int c = 0;
    while(true)
    {
        if((b & a)==0x0)
        {
            c++;
            b = b >> 8;
        }
        else
        {
            break;
        }
    }
    return c;
}

int getSquare(int file,int rank)
{
	return (8*rank + (7-file));
}

void datainit()
{
    for(int i = 0;i<64;i++)
    {
        if(i==0)
            Pos2Bit[0] = 0x1;
        else
            Pos2Bit[i] = Pos2Bit[i-1] << 1;
        Bitset b = Pos2Bit[i];
        int f = getFile(b);
        int r = getRank(b);

        //King Moves
        if(f>0 && f<7)
        {
            if(r>0 && r<7)
                KingMoves[i] = (b<<9) |  (b<<8) | (b<<7) | (b<<1) | (b>>1) | (b>>7) | (b>>8) | (b>>9);
            else if(r==0)
                KingMoves[i] = (b<<9) |  (b<<8) | (b<<7) | (b<<1) | (b>>1);
            else if(r==7)
                KingMoves[i] = (b<<1) | (b>>1) | (b>>7) | (b>>8) | (b>>9);
        }
        else if(f==0)
        {
            if(r>0 && r<7)
                KingMoves[i] = (b<<8) | (b<<7) | (b>>1) | (b>>8) | (b>>9);
            else if(r==0)
                KingMoves[i] = (b<<8) | (b<<7) | (b>>1);
            else if(r==7)
                KingMoves[i] = (b>>1) | (b>>8) | (b>>9);
        }
        else if(f==7)
        {
            if(r>0 && r<7)
                KingMoves[i] = (b<<9) | (b<<8) | (b<<1) | (b>>7) | (b>>8);
            else if(r==0)
                KingMoves[i] = (b<<9) | (b<<8) | (b<<1);
            else if(r==7)
                KingMoves[i] = (b<<1) | (b>>7) | (b>>8);
        }

        //Pawn Moves
        PawnMoves[COLOR_WHITE][i] = (b<<8);
        PawnMoves[COLOR_BLACK][i] = (b>>8);

        //Pawn Attacks
        if(f==0)
        {
            PawnAttacks[COLOR_WHITE][i] = (b<<7);
            PawnAttacks[COLOR_BLACK][i] = (b>>9);
        }
        else if(f==7)
        {
            PawnAttacks[COLOR_WHITE][i] = (b<<9);
            PawnAttacks[COLOR_BLACK][i] = (b>>7);
        }
        else
        {
            PawnAttacks[COLOR_WHITE][i] = (b<<9) | (b<<7);
            PawnAttacks[COLOR_BLACK][i] = (b>>9) | (b>>7);
        }

        //Knight Moves
        if(f==0)
        {
            if(r==0)
                KnightMoves[i] = (b<<15) | (b<<6);
            else if(r==1)
                KnightMoves[i] = (b<<15) | (b<<6) | (b>>10);
            else if(r==7)
                KnightMoves[i] = (b>>17) | (b>>10);
            else if(r==6)
                KnightMoves[i] = (b>>17) | (b>>10) | (b<<6);
            else
                KnightMoves[i] = (b<<15) | (b<<6) | (b>>10) | (b>>17);
        }
        else if(f==1)
        {
            if(r==0)
                KnightMoves[i] = (b<<17) | (b<<15) | (b<<6);
            else if(r==1)
                KnightMoves[i] = (b<<17) | (b<<15) | (b<<6) | (b>>10);
            else if(r==7)
                KnightMoves[i] = (b>>17) | (b>>15) | (b>>10);
            else if(r==6)
                KnightMoves[i] = (b>>17) | (b>>15) | (b>>10) | (b<<6);
            else
                KnightMoves[i] = (b<<17) | (b<<15) | (b<<6) | (b>>10) | (b>>15) | (b>>17);
        }
        else if(f==7)
        {
            if(r==0)
                KnightMoves[i] = (b<<17) | (b<<10);
            else if(r==1)
                KnightMoves[i] = (b<<17) | (b<<10) | (b>>6);
            else if(r==7)
                KnightMoves[i] = (b>>15) | (b>>6);
            else if(r==6)
                KnightMoves[i] = (b>>15) | (b>>6) | (b<<10);
            else
                KnightMoves[i] = (b<<17) | (b<<10) | (b>>6) | (b>>15);
        }
        else if(f==6)
        {
            if(r==0)
                KnightMoves[i] = (b<<15) | (b<<17) | (b<<10);
            else if(r==1)
                KnightMoves[i] = (b<<15) | (b<<17) | (b<<10) | (b>>6);
            else if(r==7)
                KnightMoves[i] = (b>>15) | (b>>17) | (b>>6);
            else if(r==6)
                KnightMoves[i] = (b>>15) | (b>>17) | (b>>6) | (b<<10);
            else
                KnightMoves[i] = (b<<15) | (b<<17) | (b<<10) | (b>>6) | (b>>17) | (b>>15);
        }
        else
        {
            if(r==0)
                KnightMoves[i] = (b<<17) | (b<<15) | (b<<10) | (b<<6);
            else if(r==1)
                KnightMoves[i] = (b<<17) | (b<<15) | (b<<10) | (b<<6) | (b>>6) | (b>>10);
            else if(r==7)
                KnightMoves[i] = (b>>6) | (b>>10) | (b>>15) | (b>>17);
            else if(r==6)
                KnightMoves[i] = (b<<10) | (b<<6) | (b>>6) | (b>>10) | (b>>15) | (b>>17);
            else
                KnightMoves[i] = (b<<17) | (b<<15) | (b<<10) | (b<<6) | (b>>6) | (b>>10) | (b>>15) | (b>>17);
        }
    }
    //Sliding Moves init
    for(int i = 0;i<8;i++)
    {
        for(int j = 0;j<256;j++)
        {
            Bitset b = 0x0;
            for(int x = 0;x<8-i;x++)
            {
                if(i+x+1==8)
                {
                    break;
                }
                b |= Pos2Bit[i+x+1];
                if((j>>(i+x+1))%2==1)
                {
                    break;
                }
            }
            for(int x = 0;x<i;x++)
            {
                b |= Pos2Bit[i-x-1];
                if((j>>(i-x-1))%2==1)
                {
                    break;
                }
            }
            SlidingMoves[i][j] = b;
        }
    }
    for(int i = 0;i<64;i++)
    {
         for(int j = 0;j<256;j++)
         {
             //Rook Rank Moves
             RookRankMoves[i][j] = SlidingMoves[i%8][j] << (8*(i/8));
             //Rook File Moves
             Bitset b = SlidingMoves[getRank(i)][j];
             Bitset p = 0x0;
             for(int k = 0;k<8;k++)
             {
                 Bitset s = (b>>k)%2;
                 p |= Pos2Bit[turn270[k]]*s;
             }
             RookFileMoves[i][j] = (p>>(getFile(i)));

             //Bishop A1H8 Moves
             int flag = 0;
             if(turn135[i]>=32)
             {
                 b = SlidingMoves[7-getRank(i)][j];
                 flag = 1;
             }
             else
                 b = SlidingMoves[7-getFile(i)][j];
             p = 0x0;
             int m = i;
             if(getFile(m)!=0 && getRank(m)!=0)
             {
                 while(true)
                 {
                     m = m-7;
                     if(m<0 || m>63)
                     {
                         break;
                     }
                     int k = 0;
                     if(flag==1)
                         k = 7-getRank(m);
                     else
                         k = 7-getFile(m);
                     Bitset s = (b>>k)%2;
                     p |= Pos2Bit[m]*s;
                     if(getFile(m)==0 || getRank(m)==0)
                     {
                          break;
                     }
                 }
             }
             m = i;
             if(getFile(m)!=7 && getRank(m)!=7)
             {
                 while(true)
                 {
                     m = m+7;
                     if(m<0 || m>63)
                     {
                         break;
                     }
                     int k = 0;
                     if(flag==1)
                         k = 7-getRank(m);
                     else
                         k = 7-getFile(m);
                     Bitset s = (b>>k)%2;
                     p |= Pos2Bit[m]*s;
                     if(getFile(m)==7 || getRank(m)==7)
                     {
                          break;
                     }
                 }
             }
             BishopA1H8Moves[i][j] = p;

             //Bishop A8H1 Moves
             flag = 0;
             if(turn45[i]>=32)
             {
                 b = SlidingMoves[7-getFile(i)][j];
                 flag = 1;
             }
             else
                 b = SlidingMoves[getRank(i)][j];
             p = 0x0;
             m = i;
             if(getFile(m)!=7 && getRank(m)!=0)
             {
                 while(true)
                 {
                     m = m-9;
                     if(m<0 || m>63)
                     {
                         break;
                     }
                     int k = 0;
                     if(flag==1)
                         k = 7-getFile(m);
                     else
                         k = getRank(m);
                     Bitset s = (b>>k)%2;
                     p |= Pos2Bit[m]*s;
                     if(getFile(m)==7 || getRank(m)==0)
                     {
                          break;
                     }
                 }
             }
             m = i;
             if(getFile(m)!=0 && getRank(m)!=7)
             {
                 while(true)
                 {
                     m = m+9;
                     if(m<0 || m>63)
                     {
                         break;
                     }
                     int k = 0;
                     if(flag==1)
                         k = 7-getFile(m);
                     else
                         k = getRank(m);
                     Bitset s = (b>>k)%2;
                     p |= Pos2Bit[m]*s;
                     if(getFile(m)==0 || getRank(m)==7)
                     {
                          break;
                     }
                 }
             }
             BishopA8H1Moves[i][j] = p;
         }
    }
	for(int col = 0;col < 2;col++)
	{
		for(int i = 0;i<64;i++)
		{
			AboveBits[col][i] = 0x0;
			AboveSideBits[col][i] = 0x0;
			AboveAndAboveSideBits[col][i] = 0x0;
			if(col==0)
			{
				for(int j = i+8;j<64;j=j+8) //above
				{
					AboveBits[col][i] |= Pos2Bit[j];
					AboveAndAboveSideBits[col][i] |= Pos2Bit[j];
					if(j%8!=0)
					{
						AboveSideBits[col][i] |= Pos2Bit[j-1];
						AboveAndAboveSideBits[col][i] |= Pos2Bit[j-1];
					}
					if(j%8!=7)
					{
						AboveSideBits[col][i] |= Pos2Bit[j+1];
						AboveAndAboveSideBits[col][i] |= Pos2Bit[j+1];
					}
				}
			}
			else
			{
				for(int j = i-8;j>=0;j=j-8) //below
				{
					AboveBits[col][i] |= Pos2Bit[j];
					AboveAndAboveSideBits[col][i] |= Pos2Bit[j];
					if(j%8!=0)
					{
						AboveSideBits[col][i] |= Pos2Bit[j-1];
						AboveAndAboveSideBits[col][i] |= Pos2Bit[j-1];
					}
					if(j%8!=7)
					{
						AboveSideBits[col][i] |= Pos2Bit[j+1];
						AboveAndAboveSideBits[col][i] |= Pos2Bit[j+1];
					}
				}
			}
		}
	}
	for(int i = 0;i<64;i++)
	{
		FileBits[i] = 0x0;
		FileAndSideBits[i] = 0x0;
		SideBits[i] = 0x0;
		int add = i%8;
		for(int j = 0;j<8;j++)
		{
			FileBits[i] |= Pos2Bit[j*8+add];
			FileAndSideBits[i] |= Pos2Bit[j*8+add];
			if(add!=0)
			{
				FileAndSideBits[i] |= Pos2Bit[j*8+add-1];
				SideBits[i] |= Pos2Bit[j*8+add-1];
			}
			if(add!=7)
			{
				FileAndSideBits[i] |= Pos2Bit[j*8+add+1];
				SideBits[i] |= Pos2Bit[j*8+add+1];
			}
		}
	}
	for(int col = 0;col<2;col++) //king shields
	{
		for(int i = 0;i<64;i++)
		{
			KingShield1[col][i] = KingMoves[i];
			KingShield2[col][i] = KingMoves[i];
			if(col==COLOR_WHITE)
			{
				if(i<55)
				{
					KingShield1[col][i] |= Pos2Bit[i+8];
					if(i%8!=0)
					{
						KingShield1[col][i] |= Pos2Bit[i+7];
						KingShield1[col][i] |= Pos2Bit[i-1];
					}
					if(i%8!=7)
					{
						KingShield1[col][i] |= Pos2Bit[i+9];
						KingShield1[col][i] |= Pos2Bit[i+1];
					}
				}
				if(i<47)
				{
					KingShield2[col][i] |= Pos2Bit[i+16];
					if(i%8!=0)
					{
						KingShield2[col][i] |= Pos2Bit[i+15];
					}
					if(i%8!=7)
					{
						KingShield2[col][i] |= Pos2Bit[i+17];
					}
				}
			}
			else if(col==COLOR_BLACK)
			{
				if(i>=8)
				{
					KingShield1[col][i] |= Pos2Bit[i-8];
					if(i%8!=0)
					{
						KingShield1[col][i] |= Pos2Bit[i-9];
						KingShield1[col][i] |= Pos2Bit[i-1];
					}
					if(i%8!=7)
					{
						KingShield1[col][i] |= Pos2Bit[i-7];
						KingShield1[col][i] |= Pos2Bit[i+1];
					}
				}
				if(i>=16)
				{
					KingShield2[col][i] |= Pos2Bit[i-16];
					if(i%8!=0)
					{
						KingShield2[col][i] |= Pos2Bit[i-17];
					}
					if(i%8!=7)
					{
						KingShield2[col][i] |= Pos2Bit[i-15];
					}
				}
			}
		}
	}
	for(int col=0;col<2;col++)
	{
		for(int i = 0;i<64;i++)
		{
			KingField[col][i] = KingShield1[col][i]|KingShield2[col][i]|KingShield1[Opponent[col]][i];
		}
	}
	for(int i = 0;i<64;i++)
	{
		ColorMirror[COLOR_WHITE][i] = i;
		ColorMirror[COLOR_BLACK][i] = Mirror[i];
	}
}

int popcnt2(Bitset b)
{
	int c = 0;
	while(b)
	{
		b &= b-1; //clear the last most significant bit
		c++;
	}
	return _mm_popcnt_u64(b);
}

int firstOf(Bitset b)
{
    for(int i = 0;i<64;i++)
    {
        if((b&1)==1)
        {
            return i;
        }
        b = b >> 1;
    }
    return -1;
}

int lastOf(Bitset b)
{
    for(int i = 0;i<64;i++)
    {
        if((b&0x8000000000000000)==1)
        {
            return 63-i;
        }
        b << 1;
    }
    return -1;
}

void printBitset(Bitset b)
{
    for(int i = 0;i<64;i++)
    {
        if(i%8==0 && i!=0)
            cout << endl;
        cout << (b>>63);
        b = (b<<1);
    }
    cout << endl;
}

void printBitsetDiag(Bitset b)
{
    int sp = 7;
    for(int j = 0;j<sp;j++)
        cout << " ";
    for(int i = 0;i<64;i++)
    {
        if(i==1 || i==3 || i==6 || i==10 || i==15 || i==21 || i==28 || i==36 || i==43 || i==49 || i==54 || i==58 || i==61 || i==63)
        {
            if(i<32)
                sp--;
            else
                sp++;
            cout << endl;
            for(int j = 0;j<sp;j++)
                cout << " ";
        }
        cout << (b>>63) << " ";
        b = (b<<1);
    }
    cout << endl;
}

std::string Int2Str(int n)
{
	return static_cast<std::ostringstream*>( &(std::ostringstream() << n) )->str();
}

char Int2Char(int n)
{
	return (char(n)+48);
}

std::string Long2Str(unsigned long long n)
{
	std::string s = "";
	for(int i = 0;n!=0;i++)
	{
		int a = n%10;
	    s = Int2Char(a) + s;
		n = n/10;
	}
	return s;
}
