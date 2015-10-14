#include "Move.h"

Move CONS_NULLMOVE = Move();

int CapturedPiecePriority[13] = {0,2,3,4,5,6,1,2,3,4,5,6,1};
int MovingPiecePriority[13][6] = {{2,5,5,3,4,1},   //empty
                                  {6,4,4,3,2,1},   //white pawn
                                  {6,4,4,3,2,1},   //white knight
								  {6,4,4,3,2,1},   //white bishop
                                  {6,4,4,3,2,1},   //white rook
                                  {6,4,4,3,2,1},   //white queen
                                  {6,4,4,3,2,1},   //white king
                                  {6,4,4,3,2,1},   //black pawn
                                  {6,4,4,3,2,1},   //black knight
                                  {6,4,4,3,2,1},   //black bishop
                                  {6,4,4,3,2,1},   //black rook
                                  {6,4,4,3,2,1},   //black queen
                                  {6,4,4,3,2,1}};  //black king
int ToPriority[64] = { 1, 2, 3, 5, 5, 3, 2, 1,
					   2, 3, 5, 6, 6, 5, 3, 2,
					   3, 5, 6, 7, 7, 6, 5, 3,
					   5, 6, 7, 8, 8, 7, 6, 5,
					   5, 6, 7, 8, 8, 7, 6, 5,
					   3, 5, 6, 7, 7, 6, 5, 3,
					   2, 3, 5, 6, 6, 5, 3, 2,
					   1, 2, 3, 5, 5, 3, 2, 1};

int CapturedPieceBonus = 1000;
int SpecialBonus[7] = {4000,-1000,-2000,-2000,3000,2000,0};
int MovingPieceBonus[13][6] = {{   2,   5,   5,   3,   4,   1},   //empty
                               {   0,-200,-233,-400,-800, 100},   //white pawn
                               { 200,   0, -33,-200,-600, 300},   //white knight
							   { 233,  33,   0,-167,-567, 333},   //white bishop
                               { 400, 200, 167,   0,-400, 500},   //white rook
                               { 800, 600, 567, 400,   0, 900},   //white queen
							   {1000,1000,1000,1000,1000,1000},   //white king
							   {   0,-200,-233,-400,-800, 100},   //black pawn
                               { 200,   0, -33,-200,-600, 300},   //black  knight
							   { 233,  33,   0,-167,-567, 333},   //black bishop
                               { 400, 200, 167,   0,-400, 500},   //black rook
                               { 800, 600, 567, 400,   0, 900},   //black queen
                               {1000,1000,1000,1000,1000,1000}};  //white king
int SquareBonus[6][64] = 	
{
    {   0,   0,   0,   0,   0,   0,   0,   0, //pawn
	    0,   0, -10, -20, -19, -15,   0,   0,
	    1,   1,   2,  10,  10,   5,   1,   1,
	    2,   2,   5,  20,  20,  10,   2,   2,
	    3,   3,   6,  24,  24,  11,   3,   3,
	   40,  50,  50,  50,  50,  50,  50,  40,
	   70,  50,  50,  50,  50,  50,  50,  70,
	    0,   0,   0,   0,   0,   0,   0,   0},

	{ -20, -18, -15, -10, -10, -15, -18, -20, //knight
	   -5,   0,   0,   0,   0,   0,   0,  -5,
	   -5,   0,  10,   3,   3,  10,   0,  -5,
	   -5,   0,   3,  15,  15,   3,   0,  -5,
	   -5,   0,   3,  15,  15,   3,   0,  -5,
	   25,  30,  50,  50,  50,  40,  30,  25,
	   10,  15,  20,  25,  25,  20,  15,  10,
	    0,   0,   0,   0,   0,   0,   0,   0},

	{   0,  -5, -10,  -5,  -5, -10,  -5,   0, //bishop
	    0,  10,   0,   0,   0,   0,  10,   0,
	    0,   0,  10,   5,   5,  10,   0,   0,
	    0,   0,   8,  12,  12,   8,   0,   0,
	    0,   2,   5,  12,  12,   5,   2,   0,
	    0,   0,  10,   5,   5,  10,   0,   0,
	    0,  10,   0,   0,   0,   0,  10,   0,
	    5,   0,   0,   0,   0,   0,   0,   5},

	{  0,  0, 10, 10, 10, 10,  0,  0, //rook
	   0,  0,  0, 10, 10, 10,  0,  0,
	   0,  0,  0, 10, 10, 10,  0,  0,
	   0,  0,  0, 10, 10, 10,  0,  0,
	   0,  0,  0, 10, 10, 10,  0,  0,
	   0,  0,  0, 10, 10, 10,  0,  0,
	  10, 10, 10, 20, 20, 20, 10, 10,
	  10, 10, 10, 20, 20, 20, 10, 10},

	{  0,  0,  0,  0, -1,  0,  0,  0, //queen
	   0,  0,  0,  0,  2,  3,  0,  0,
	   0,  0,  3,  1,  1,  3,  3,  0,
	   0,  0,  1,  5,  5,  1,  0,  0,
	   0,  0,  1,  5,  5,  1,  0,  0,
	   0,  0,  3,  1,  1,  3,  0,  0,
	   0,  0,  0,  0,  0,  0,  0,  0,
	   0,  0,  0,  0,  0,  0,  0,  0},

	{  0,  0,  0,  0,  0,  0,  0,  0, //king
	   0,  0,  0,  0,  0,  0,  0,  0, 
       0,  0,  0,  0,  0,  0,  0,  0, 
	   0,  0,  0,  0,  0,  0,  0,  0, 
	   0,  0,  0,  0,  0,  0,  0,  0, 
	   0,  0,  0,  0,  0,  0,  0,  0, 
	   0,  0,  0,  0,  0,  0,  0,  0, 
	   0,  0,  0,  0,  0,  0,  0,  0}
	   
};

void sortMoves(vector<Move>& moves,int turn)
{
	vector<int> vals;
	int val = 0;
	int capturedpiece = SQUARE_EMPTY;
	int movingpiece = 0;
	for(int i = 0;i<moves.size();i++)
	{
		val = 0;
		capturedpiece = moves.at(i).getCapturedPiece();
		movingpiece = moves.at(i).getMovingPiece();
		if(capturedpiece)
		{
			val += CapturedPieceBonus;
		}
		val += MovingPieceBonus[capturedpiece][movingpiece];
		val += SpecialBonus[moves.at(i).getSpecial()];
		if(turn==COLOR_WHITE)
		{
			val += SquareBonus[movingpiece][moves.at(i).getTo()];
			val += -SquareBonus[movingpiece][moves.at(i).getFrom()];
		}
		else
		{
			val += SquareBonus[movingpiece][getturn180(moves.at(i).getTo())];
			val += -SquareBonus[movingpiece][getturn180(moves.at(i).getFrom())];
		}
		vals.push_back(val);
	}

	/*for(int i = 0;i<vals.size();i++)
	{
		cout << vals.at(i) << endl;
	}*/

	for(unsigned int i = 0;i<moves.size();i++)
	{
		for(unsigned int j = 0; j<moves.size()-1-i;j++)
		{
			if(vals.at(j)<vals.at(j+1))
			{
				Move tmp = moves.at(j);
				moves.at(j) = moves.at(j+1);
				moves.at(j+1) = tmp;
				int x = vals.at(j);
				vals.at(j) = vals.at(j+1);
				vals.at(j+1) = x;
			}
		}
	}
}

Move::Move()
{
    data = 0;
}

Move::Move(Bitset n)
{
	data = n;
}

Move::Move(Bitset f,Bitset t,Bitset m,Bitset c,Bitset s,Bitset wk,Bitset wq,Bitset bk,Bitset bq,Bitset ep)
{
    data = f;
    data |= t << 6;
    data |= m << 12;
    data |= c << 16;
	data |= s << 20;
    data |= wk << 24;
    data |= wq << 25;
    data |= bk << 26;
    data |= bq << 27;
	data |= ep << 28;
}

bool Move::operator==(Move const& m) const
{
	return (data==m.data);
}

bool Move::operator!=(Move const& m) const
{
	return (data!=m.data);
}

bool Move::isEquivalent(Move const& m) const
{
	if(m.getTo()==getTo() && m.getFrom()==getFrom() && m.getSpecial()==getSpecial())
	{
		return true;
	}
	return false;
}

bool Move::operator<(Move const& m) const
{
	int sp = getSpecial();
	int msp = m.getSpecial();
	if(sp==PIECE_KNIGHT || sp==PIECE_ROOK || sp==PIECE_BISHOP || msp==PIECE_QUEEN || msp==PIECE_KING || msp==PIECE_PAWN)
	{
		return true;
	}
	else if(sp==PIECE_QUEEN || sp==PIECE_KING || sp==PIECE_PAWN || msp==PIECE_KNIGHT || msp==PIECE_ROOK || msp==PIECE_BISHOP)
	{
		return false;
	}
	int c = getCapturedPiece();
	int mc = m.getCapturedPiece();
	int cap = CapturedPiecePriority[c];
	int mcap = CapturedPiecePriority[mc];
	if(cap<mcap)
	{
		return true;
	}
	else if(cap>mcap)
	{
		return false;
	}
	else
	{
		int mov = MovingPiecePriority[c][getMovingPiece()];
		int mmov = MovingPiecePriority[mc][m.getMovingPiece()];
		if(mov<mmov)
		{
			return true;
		}
		else if(mmov>mov)
		{
			return false;
		}
		else
		{
			if(ToPriority[getTo()]<ToPriority[m.getTo()])
			{
				return true;
			}
			else
				return false;
		}
	}
	return false;
}

bool Move::operator>(Move const& m) const
{
	int sp = getSpecial();
	if(sp==PIECE_QUEEN || sp==PIECE_KING || sp==PIECE_PAWN)
	{
		return true;
	}
	else if(sp==PIECE_KNIGHT || sp==PIECE_ROOK || sp==PIECE_BISHOP)
	{
		return false;
	}
	int c = getCapturedPiece();
	int mc = m.getCapturedPiece();
	int cap = CapturedPiecePriority[c];
	int mcap = CapturedPiecePriority[mc];
	if(cap>mcap)
	{
		return true;
	}
	else if(cap<mcap)
	{
		return false;
	}
	else
	{
		int mov = MovingPiecePriority[c][getMovingPiece()];
		int mmov = MovingPiecePriority[mc][m.getMovingPiece()];
		if(mov>mmov)
		{
			return true;
		}
		else if(mmov>mov)
		{
			return false;
		}
		else
		{
			if(ToPriority[getTo()]>ToPriority[m.getTo()])
			{
				return true;
			}
			else
				return false;
		}
	}
	return false;
}

Bitset Move::getFrom() const
{
	return (data & 0x3f);
}

Bitset Move::getTo() const
{
	return ((data>>6) & 0x3f);
}

Bitset Move::getMovingPiece() const
{
	return ((data>>12) & 0xf);
}

Bitset Move::getCapturedPiece() const
{
	return ((data>>16) & 0xf);
}

Bitset Move::getSpecial() const
{
	return ((data>>20) & 0xf);
}

Bitset Move::getWKC() const
{
	return ((data>>24) & 0x1);
}

Bitset Move::getWQC() const
{
	return ((data>>25) & 0x1);
}

Bitset Move::getBKC() const
{
	return ((data>>26) & 0x1);
}

Bitset Move::getBQC() const
{
	return ((data>>27) & 0x1);
}

Bitset Move::getEP() const
{
	return ((data>>28) & 0x3f);
}


Bitset Move::getbit() const
{
    return getPos2Bit(getTo()) | getPos2Bit(getFrom());
}

Bitset Move::getbit90() const
{
    return getPos2Bit(getturn90(getTo())) | getPos2Bit(getturn90(getFrom()));
}

Bitset Move::getbit45() const
{
    return getPos2Bit(getturn45(getTo())) | getPos2Bit(getturn45(getFrom()));
}

Bitset Move::getbit135() const
{
    return getPos2Bit(getturn135(getTo())) | getPos2Bit(getturn135(getFrom()));
}

string Move::toString() const
{
	string s = "";
	int i = getSpecial();
	if(i==PIECE_QUEEN) s="Q";
	else if(i==PIECE_KNIGHT) s="N";
	else if(i==PIECE_ROOK) s="R";
	else if(i==PIECE_BISHOP) s="B";
	return (Int2Sq(getFrom()) + Int2Sq(getTo()) + s);
}

Move String2Move(const string& s)
{
	int to=0,from=0,sp=PIECE_NONE;
	if(s.length()>4)
	{
		if(s.at(4)=='Q' || s.at(4)=='q')
		{
			sp=PIECE_QUEEN;
		}
		else if(s.at(4)=='N' || s.at(4)=='n')
		{
			sp=PIECE_KNIGHT;
		}
		else if(s.at(4)=='R' || s.at(4)=='r')
		{
			sp=PIECE_ROOK;
		}
		else if(s.at(4)=='B' || s.at(4)=='b')
		{
			sp=PIECE_BISHOP;
		}
	}
	string s_to = "";
	s_to += s.at(2);
	s_to += s.at(3);
	string s_from = "";
	s_from += s.at(0);
	s_from += s.at(1);
	Move m(Sq2Int(s_from),Sq2Int(s_to),0,0,sp,0,0,0,0,0);
	return m;
}