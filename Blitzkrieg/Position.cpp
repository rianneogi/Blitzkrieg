#include "Position.h"
#include <vector>

string pieceStrings[] = {"- ","P ","N ","B ","R ","Q ","K ","p ","n ","b ","r ","q ","k "};

Position::Position()
{
	setStartPos();
}

Position::Position(Position const& pos,Move const& m)
{
    for(int i = 0;i<2;i++)
    {
        for(int j = 0;j<6;j++)
        {
            Pieces[i][j] = pos.Pieces[i][j];
        }
    }
    OccupiedSq = pos.OccupiedSq;
    /*OccupiedSq90 = pos.OccupiedSq90;
    OccupiedSq45 = pos.OccupiedSq45;
    OccupiedSq135 = pos.OccupiedSq135;*/

    for(int i = 0;i<64;i++)
    {
        Squares[i] = pos.Squares[i];
    }

    turn = pos.turn;

    for(int i = 0;i<2;i++)
    {
        for(int j = 0;j<2;j++)
        {
            castling[i][j] = pos.castling[i][j];
        }
    }
    epsquare = pos.epsquare;

	TTKey = pos.TTKey;
	PawnKey = pos.PawnKey;

	movelist = pos.movelist;
	hashlist = pos.hashlist;

    forceMove(m);
}

Position::~Position()
{
}

void Position::setStartPos()
{
	Pieces[COLOR_WHITE][PIECE_PAWN] = 0x000000000000ff00;
	Pieces[COLOR_BLACK][PIECE_PAWN] = 0x00ff000000000000;
	Pieces[COLOR_WHITE][PIECE_ROOK] = 0x0000000000000081;
	Pieces[COLOR_BLACK][PIECE_ROOK] = 0x8100000000000000;
	Pieces[COLOR_WHITE][PIECE_KNIGHT] = 0x0000000000000042;
	Pieces[COLOR_BLACK][PIECE_KNIGHT] = 0x4200000000000000;
	Pieces[COLOR_WHITE][PIECE_BISHOP] = 0x0000000000000024;
	Pieces[COLOR_BLACK][PIECE_BISHOP] = 0x2400000000000000;
	Pieces[COLOR_WHITE][PIECE_QUEEN] = 0x0000000000000010;
	Pieces[COLOR_BLACK][PIECE_QUEEN] = 0x1000000000000000;
	Pieces[COLOR_WHITE][PIECE_KING] = 0x0000000000000008;
	Pieces[COLOR_BLACK][PIECE_KING] = 0x0800000000000000;
	OccupiedSq = 0xffff00000000ffff;
	/*OccupiedSq90 = 0xc3c3c3c3c3c3c3c3;
	OccupiedSq45 = 0x0;
	OccupiedSq135 = 0x0;*/
	for (int i = 0;i<64;i++)
	{
		/*OccupiedSq45 |= getPos2Bit(getturn45(i))*((OccupiedSq >> i) % 2);
		OccupiedSq135 |= getPos2Bit(getturn135(i))*((OccupiedSq >> i) % 2);*/
		Squares[i] = SQUARE_EMPTY;
	}

	for (int i = 0;i<2;i++)
	{
		for (int j = 0;j<6;j++)
		{
			Bitset b = Pieces[i][j];
			while (b != 0)
			{
				int x = firstOf(b);
				if (x == -1)
				{
					break;
				}
				Squares[x] = i * 6 + j + 1;
				b ^= getPos2Bit(x);
			}
		}
	}

	turn = COLOR_WHITE;

	for (int i = 0;i<2;i++)
	{
		for (int j = 0;j<2;j++)
		{
			castling[i][j] = 1;
		}
	}
	epsquare = 0;

	TTKey = 0x0;
	PawnKey = 0x0;
	for (int i = 0;i<64;i++)
	{
		if (Squares[i] != SQUARE_EMPTY)
		{
			TTKey ^= TT_PieceKey[getSquare2Color(Squares[i])][getSquare2Piece(Squares[i])][i];
			if (getSquare2Piece(Squares[i]) == PIECE_PAWN)
			{
				PawnKey ^= TT_PieceKey[getSquare2Color(Squares[i])][PIECE_PAWN][i];
			}
		}
	}
	for (int i = 0;i<2;i++)
	{
		for (int j = 0;j<2;j++)
		{
			if (castling[i][j] == 1)
				TTKey ^= TT_CastlingKey[i][j];
		}
	}
	TTKey ^= TT_EPKey[epsquare];

	movelist = vector<Move>(0);
	hashlist = vector<Bitset>(0);
}

void Position::clearBoard()
{
	Pieces[COLOR_WHITE][PIECE_PAWN] = 0;
	Pieces[COLOR_BLACK][PIECE_PAWN] = 0;
	Pieces[COLOR_WHITE][PIECE_ROOK] = 0;
	Pieces[COLOR_BLACK][PIECE_ROOK] = 0;
	Pieces[COLOR_WHITE][PIECE_KNIGHT] = 0;
	Pieces[COLOR_BLACK][PIECE_KNIGHT] = 0;
	Pieces[COLOR_WHITE][PIECE_BISHOP] = 0;
	Pieces[COLOR_BLACK][PIECE_BISHOP] = 0;
	Pieces[COLOR_WHITE][PIECE_QUEEN] = 0;
	Pieces[COLOR_BLACK][PIECE_QUEEN] = 0;
	Pieces[COLOR_WHITE][PIECE_KING] = 0;
	Pieces[COLOR_BLACK][PIECE_KING] = 0;
	OccupiedSq = 0;
	/*OccupiedSq90 = 0;
	OccupiedSq45 = 0;
	OccupiedSq135 = 0;*/
	for (int i = 0;i<64;i++)
	{
		Squares[i] = SQUARE_EMPTY;
	}

	turn = COLOR_WHITE;

	for (int i = 0;i<2;i++)
	{
		for (int j = 0;j<2;j++)
		{
			castling[i][j] = 0;
		}
	}
	epsquare = 0;

	TTKey = 0x0;
	PawnKey = 0x0;
	for (int i = 0;i<64;i++)
	{
		if (Squares[i] != SQUARE_EMPTY)
		{
			TTKey ^= TT_PieceKey[getSquare2Color(Squares[i])][getSquare2Piece(Squares[i])][i];
			if (Squares[i] == SQUARE_WHITEPAWN || Squares[i] == SQUARE_BLACKPAWN)
			{
				PawnKey ^= TT_PieceKey[getSquare2Color(Squares[i])][PIECE_PAWN][i];
			}
		}
	}
	for (int i = 0;i<2;i++)
	{
		for (int j = 0;j<2;j++)
		{
			if (castling[i][j] == 1)
				TTKey ^= TT_CastlingKey[i][j];
		}
	}
	TTKey ^= TT_EPKey[epsquare];

	movelist = vector<Move>(0);
	hashlist = vector<Bitset>(0);
}

void Position::initializeBitsets()
{
	OccupiedSq = 0;
	/*OccupiedSq90 = 0;
	OccupiedSq45 = 0;
	OccupiedSq135 = 0;*/
	for (int i = 0;i < 2;i++)
	{
		for (int j = 0;j < 6;j++)
		{
			Pieces[i][j] = 0;
		}
	}
	for (int i = 0;i < 64;i++)
	{
		if (Squares[i] != SQUARE_EMPTY)
		{
			Pieces[getSquare2Color(Squares[i])][getSquare2Piece(Squares[i])] |= getPos2Bit(i);
			OccupiedSq |= getPos2Bit(i);
		}
		
	}
	/*for (int i = 0;i<64;i++)
	{
		OccupiedSq45 |= getPos2Bit(getturn45(i))*((OccupiedSq >> i) % 2);
		OccupiedSq135 |= getPos2Bit(getturn135(i))*((OccupiedSq >> i) % 2);
		OccupiedSq90 |= getPos2Bit(getturn90(i))*((OccupiedSq >> i) % 2);
	}*/
	TTKey = 0x0;
	PawnKey = 0x0;
	for (int i = 0;i<64;i++)
	{
		if (Squares[i] != SQUARE_EMPTY)
		{
			TTKey ^= TT_PieceKey[getSquare2Color(Squares[i])][getSquare2Piece(Squares[i])][i];
			if (Squares[i] == SQUARE_WHITEPAWN || Squares[i] == SQUARE_BLACKPAWN)
			{
				TTKey ^= TT_PieceKey[getSquare2Color(Squares[i])][PIECE_PAWN][i];
			}
		}
	}
	for (int i = 0;i<2;i++)
	{
		for (int j = 0;j<2;j++)
		{
			if(castling[i][j]==1)
				TTKey ^= TT_CastlingKey[i][j];
		}
	}
	TTKey ^= TT_EPKey[epsquare];
	if (turn == COLOR_BLACK)
		TTKey ^= TT_ColorKey;
}

void Position::placePiece(int square, int location)
{
	Squares[location] = square;
}

void Position::loadFromFEN(string fen)
{
	clearBoard();
	string s = getStringToken(fen, ' ', 1);
	int currsquare = 63;
	for (int i = 0;i < s.size();i++)
	{
		//cout << "info string fen " << s.at(i) << endl;
		if (s.at(i) >= '0' && s.at(i) <= '9')
		{
			currsquare -= s.at(i) - 48;
		}
		else if (s.at(i) == 'K')
		{
			placePiece(SQUARE_WHITEKING, currsquare);
			currsquare--;
		}
		else if (s.at(i) == 'k')
		{
			placePiece(SQUARE_BLACKKING, currsquare);
			currsquare--;
		}
		else if (s.at(i) == 'Q')
		{
			placePiece(SQUARE_WHITEQUEEN, currsquare);
			currsquare--;
		}
		else if (s.at(i) == 'q')
		{
			placePiece(SQUARE_BLACKQUEEN, currsquare);
			currsquare--;
		}
		else if (s.at(i) == 'R')
		{
			placePiece(SQUARE_WHITEROOK, currsquare);
			currsquare--;
		}
		else if (s.at(i) == 'r')
		{
			placePiece(SQUARE_BLACKROOK, currsquare);
			currsquare--;
		}
		else if (s.at(i) == 'N')
		{
			placePiece(SQUARE_WHITEKNIGHT, currsquare);
			currsquare--;
		}
		else if (s.at(i) == 'n')
		{
			placePiece(SQUARE_BLACKKNIGHT, currsquare);
			currsquare--;
		}
		else if (s.at(i) == 'B')
		{
			placePiece(SQUARE_WHITEBISHOP, currsquare);
			currsquare--;
		}
		else if (s.at(i) == 'b')
		{
			placePiece(SQUARE_BLACKBISHOP, currsquare);
			currsquare--;
		}
		else if (s.at(i) == 'P')
		{
			placePiece(SQUARE_WHITEPAWN, currsquare);
			currsquare--;
		}
		else if (s.at(i) == 'p')
		{
			placePiece(SQUARE_BLACKPAWN, currsquare);
			currsquare--;
		}
	}

	s = getStringToken(fen, ' ', 2);
	if (s == "b")
	{
		turn = COLOR_BLACK;
	}
	else
	{
		turn = COLOR_WHITE;
	}

	s = getStringToken(fen, ' ', 3);
	for (int i = 0;i < 2;i++)
	{
		for (int j = 0;j < 2;j++)
		{
			castling[i][j] = 0;
		}
	}
	if (s != "-")
	{
		for (int i = 0;i < s.size();i++)
		{
			if (s.at(i) == 'K')
			{
				castling[0][0] = 1;
			}
			if (s.at(i) == 'Q')
			{
				castling[0][1] = 1;
			}
			if (s.at(i) == 'k')
			{
				castling[1][0] = 1;
			}
			if (s.at(i) == 'q')
			{
				castling[1][1] = 1;
			}
		}
	}

	s = getStringToken(fen, ' ', 4);
	if (s != "-")
	{
		epsquare = Sq2Int(s);
	}
	else
	{
		epsquare = 0;
	}

	initializeBitsets();
}

void Position::forceMove(Move const& m)
{
	/*if(DEBUG)
		cout << "made " << m.toString() << endl;*/
	movelist.push_back(m);
	hashlist.push_back(TTKey);
	if(m.isNullMove()) //nullmove
	{
		turn = getOpponent(turn);
		if(epsquare!=0)
		{
			TTKey ^= TT_EPKey[epsquare];
			TTKey ^= TT_EPKey[0];
			epsquare = 0;
		}
		TTKey ^= TT_ColorKey;
		return;
	}
	Bitset from = m.getFrom();
	Bitset to = m.getTo();
	Bitset movingpiece = m.getMovingPiece();
	Bitset capturedpiece = m.getCapturedPiece();
	Bitset special = m.getSpecial();
	int cast[2][2];
	cast[0][0] = castling[0][0];
	cast[0][1] = castling[0][1];
	cast[1][0] = castling[1][0];
	cast[1][1] = castling[1][1];
	if(capturedpiece) //if a piece is captured update captured player's pieces
	{
		if(special==PIECE_QUEEN || special==PIECE_KNIGHT || special==PIECE_ROOK || special==PIECE_BISHOP) //promotion
		{
			OccupiedSq ^= getPos2Bit(from);
			/*OccupiedSq45 ^= getPos2Bit(getturn45(from));
			OccupiedSq135 ^= getPos2Bit(getturn135(from));
			OccupiedSq90 ^= getPos2Bit(getturn90(from));*/
			Pieces[turn][movingpiece] ^= getPos2Bit(from);
			Pieces[getOpponent(turn)][getSquare2Piece(capturedpiece)] ^= getPos2Bit(to);
			Pieces[turn][special] |= getPos2Bit(to);
			Squares[to] = getPiece2Square(special,turn);
			Squares[from] = SQUARE_EMPTY;
			TTKey ^= TT_PieceKey[getOpponent(turn)][getSquare2Piece(capturedpiece)][to];
			TTKey ^= TT_PieceKey[turn][movingpiece][from];
			TTKey ^= TT_PieceKey[turn][special][to];
		}
		else
		{
			OccupiedSq ^= getPos2Bit(from);
			/*OccupiedSq45 ^= getPos2Bit(getturn45(from));
			OccupiedSq135 ^= getPos2Bit(getturn135(from));
			OccupiedSq90 ^= getPos2Bit(getturn90(from));*/
			Pieces[turn][movingpiece] ^= m.getbit();
			Pieces[getOpponent(turn)][getSquare2Piece(capturedpiece)] ^= getPos2Bit(to);
			Squares[to] = Squares[from];
			Squares[from] = SQUARE_EMPTY;
			TTKey ^= TT_PieceKey[getOpponent(turn)][getSquare2Piece(capturedpiece)][to];
			TTKey ^= TT_PieceKey[turn][movingpiece][from];
			TTKey ^= TT_PieceKey[turn][movingpiece][to];
		}
	}
	else if(special==PIECE_QUEEN || special==PIECE_KNIGHT || special==PIECE_ROOK || special==PIECE_BISHOP) //promotion
	{
		OccupiedSq ^= m.getbit();
		/*OccupiedSq45 ^= m.getbit45();
		OccupiedSq135 ^= m.getbit135();
		OccupiedSq90 ^= m.getbit90();*/
		Pieces[turn][movingpiece] ^= getPos2Bit(from);
		Pieces[turn][special] |= getPos2Bit(to);
		Squares[to] = getPiece2Square(special,turn);
		Squares[from] = SQUARE_EMPTY;
		TTKey ^= TT_PieceKey[turn][movingpiece][from];
		TTKey ^= TT_PieceKey[turn][special][to];
	}
	else
	{
		Bitset bit = m.getbit();
		OccupiedSq ^= bit;
		/*OccupiedSq45 ^= m.getbit45();
		OccupiedSq135 ^= m.getbit135();
		OccupiedSq90 ^= m.getbit90();*/
		Pieces[turn][movingpiece] ^= bit;
		Squares[to] = Squares[from];
		Squares[from] = SQUARE_EMPTY;
		TTKey ^= TT_PieceKey[turn][movingpiece][from];
		TTKey ^= TT_PieceKey[turn][movingpiece][to];
	}
	//cout << m.getFrom() << " " << m.getTo() << " " << m.getMovingPiece() << " " << m.getCapturedPiece() << " " << m.getSpecial() << endl;

    if(special==PIECE_KING) //castling
    {
        int f = -1;
        int t = -1;
        if(turn==COLOR_WHITE)
        {
            if(to == 1)
            {
                f = 0;
                t = 2;
            }
            if(to == 5)
            {
                f = 7;
                t = 4;
            }
        }
        else
        {
            if(to == 57)
            {
                f = 56;
                t = 58;
            }
            if(to == 61)
            {
                f = 63;
                t = 60;
            }
        }
        Bitset cas = getPos2Bit(f) | getPos2Bit(t);
        Bitset cas45 = getPos2Bit(getturn45(f)) | getPos2Bit(getturn45(t));
        Bitset cas135 = getPos2Bit(getturn135(f)) | getPos2Bit(getturn135(t));
        Bitset cas90 = getPos2Bit(getturn90(f)) | getPos2Bit(getturn90(t));
        OccupiedSq ^= cas;
        /*OccupiedSq45 ^= cas45;
        OccupiedSq135 ^= cas135;
        OccupiedSq90 ^= cas90;*/
        Pieces[turn][PIECE_ROOK] ^= cas;
        Squares[t] = Squares[f];
        Squares[f] = SQUARE_EMPTY;

		TTKey ^= TT_PieceKey[turn][PIECE_ROOK][f];
		TTKey ^= TT_PieceKey[turn][PIECE_ROOK][t];
    }
	else if(special==PIECE_PAWN) //en passant
    {
        int x = 0;
        if(turn==COLOR_WHITE)
        {
            x = getMinus8(to);
			Pieces[COLOR_BLACK][PIECE_PAWN] ^= getPos2Bit(x);
			TTKey ^= TT_PieceKey[COLOR_BLACK][PIECE_PAWN][x];
        }
        else
        {
            x = getPlus8(to);
			Pieces[COLOR_WHITE][PIECE_PAWN] ^= getPos2Bit(x);
			TTKey ^= TT_PieceKey[COLOR_WHITE][PIECE_PAWN][x];
        }
        OccupiedSq ^= getPos2Bit(x);
        /*OccupiedSq45 ^= getPos2Bit(getturn45(x));
        OccupiedSq135 ^= getPos2Bit(getturn135(x));
        OccupiedSq90 ^= getPos2Bit(getturn90(x));*/
        Squares[x] = SQUARE_EMPTY;
    }

    if(movingpiece==PIECE_KING) //removing castling rights
    {
        if(turn==COLOR_WHITE)
        {
            castling[0][0] = 0;
            castling[0][1] = 0;
        }
        else
        {
            castling[1][0] = 0;
            castling[1][1] = 0;
        }
    }

    if(movingpiece==PIECE_ROOK) //removing castling rights
    {
        if(from == 0)
        {
            castling[0][0] = 0;
        }
        if(from == 7)
        {
            castling[0][1] = 0;
        }
        if(from == 56)
        {
            castling[1][0] = 0;
        }
        if(from == 63)
        {
            castling[1][1] = 0;
        }
    }

	if(capturedpiece==SQUARE_WHITEROOK) //removing castling rights if rook gets captured
	{
		if(to == 0)
		{
			castling[0][0] = 0;
		}
		else if(to == 7)
		{
			castling[0][1] = 0;
		}
	}
	if(capturedpiece==SQUARE_BLACKROOK) //removing castling rights if rook gets captured
	{
		if(to == 56)
		{
			castling[1][0] = 0;
		}
		else if(to == 63)
		{
			castling[1][1] = 0;
		}
	}

    if(movingpiece==PIECE_PAWN) //set ep square
    {
		if(turn==COLOR_WHITE && getRank(int(from))==1 && getRank(int(to))==3)
		{
			TTKey ^= TT_EPKey[epsquare];
			epsquare = getPlus8(from);
			TTKey ^= TT_EPKey[epsquare];
		}
		else if(turn==COLOR_BLACK && getRank(int(from))==6 && getRank(int(to))==4)
		{
			TTKey ^= TT_EPKey[epsquare];
			epsquare = getPlus8(to);
			TTKey ^= TT_EPKey[epsquare];
		}
		else if(epsquare!=0)
		{
			TTKey ^= TT_EPKey[epsquare];
			epsquare = 0;
			TTKey ^= TT_EPKey[0];
		}
    }
    else if(epsquare!=0)
    {
		TTKey ^= TT_EPKey[epsquare];
        epsquare = 0;
		TTKey ^= TT_EPKey[0];
    }

	if(cast[0][0]!=castling[0][0]) //check if castling values have changed
	{
		TTKey ^= TT_CastlingKey[0][0];
	}
	if(cast[0][1]!=castling[0][1])
	{
		TTKey ^= TT_CastlingKey[0][1];
	}
	if(cast[1][0]!=castling[1][0])
	{
		TTKey ^= TT_CastlingKey[1][0];
	}
	if(cast[1][1]!=castling[1][1])
	{
		TTKey ^= TT_CastlingKey[1][1];
	}

	if (movingpiece == PIECE_PAWN)
	{
		PawnKey ^= TT_PieceKey[turn][movingpiece][from];
		if(special==PIECE_NONE || special==PIECE_PAWN)
			PawnKey ^= TT_PieceKey[turn][movingpiece][to];
	}
	if (getSquare2Piece(capturedpiece) == PIECE_PAWN)
	{
		if (special == PIECE_PAWN)
		{
			if(turn==COLOR_WHITE)
				PawnKey ^= TT_PieceKey[getOpponent(turn)][PIECE_PAWN][getMinus8(to)];
			else
				PawnKey ^= TT_PieceKey[getOpponent(turn)][PIECE_PAWN][getPlus8(to)];
		}
		else
		{
			PawnKey ^= TT_PieceKey[getOpponent(turn)][PIECE_PAWN][to];
		}
	}

    turn = getOpponent(turn);
	TTKey ^= TT_ColorKey;	
}

bool Position::makeMove(Move const& m)
{
    forceMove(m);
	if(!underCheck(getOpponent(turn)))
	{
		return true;
	}
	unmakeMove(m);
	return false;
}

void Position::unmakeMove(Move const& m)
{
	/*if(DEBUG)
		cout << "unmade " << m.toString() << endl;*/
	movelist.pop_back();
	hashlist.pop_back();
	if(m.isNullMove()) //nullmove
	{
		turn = getOpponent(turn);
		TTKey ^= TT_ColorKey;
		int ep = m.getEP(); //setting ep square
		if(epsquare != ep)
		{
			TTKey ^= TT_EPKey[epsquare];
			TTKey ^= TT_EPKey[ep];
			epsquare = ep;
		}
		return;
	}
	Bitset from = m.getFrom();
	Bitset to = m.getTo();
	Bitset movingpiece = m.getMovingPiece();
	Bitset capturedpiece = m.getCapturedPiece();
	Bitset special = m.getSpecial();
	Bitset bit = m.getbit();
	if(capturedpiece)
	{
		if(special==PIECE_QUEEN || special==PIECE_KNIGHT || special==PIECE_ROOK || special==PIECE_BISHOP) //promotion
		{
			OccupiedSq |= getPos2Bit(from);
			/*OccupiedSq45 |= getPos2Bit(getturn45(from));
			OccupiedSq135 |= getPos2Bit(getturn135(from));
			OccupiedSq90 |= getPos2Bit(getturn90(from));*/
			Pieces[turn][getSquare2Piece(capturedpiece)] |= getPos2Bit(to);

			TTKey ^= TT_PieceKey[turn][getSquare2Piece(capturedpiece)][to];

			turn = getOpponent(turn);
			TTKey ^= TT_ColorKey;

			Pieces[turn][movingpiece] |= getPos2Bit(from);
			Pieces[turn][special] ^= getPos2Bit(to);

			Squares[from] = getPiece2Square(PIECE_PAWN,turn);
			Squares[to] = capturedpiece;

			TTKey ^= TT_PieceKey[turn][movingpiece][from];
			TTKey ^= TT_PieceKey[turn][special][to];
		}
		else
		{
			OccupiedSq |= getPos2Bit(from);
			/*OccupiedSq45 |= getPos2Bit(getturn45(from));
			OccupiedSq135 |= getPos2Bit(getturn135(from));
			OccupiedSq90 |= getPos2Bit(getturn90(from));*/
			Pieces[turn][getSquare2Piece(capturedpiece)] |= getPos2Bit(to);

			TTKey ^= TT_PieceKey[turn][getSquare2Piece(capturedpiece)][to];

			turn = getOpponent(turn);
			TTKey ^= TT_ColorKey;
			Pieces[turn][movingpiece] ^= bit;
		
			Squares[from] = getPiece2Square(movingpiece,turn);
			Squares[to] = capturedpiece;

			TTKey ^= TT_PieceKey[turn][movingpiece][from];
			TTKey ^= TT_PieceKey[turn][movingpiece][to];
		}
	}
	else if(special==PIECE_QUEEN || special==PIECE_KNIGHT || special==PIECE_ROOK || special==PIECE_BISHOP) //promotion
	{
		Bitset bit = m.getbit();
		OccupiedSq ^= bit;
		/*OccupiedSq45 ^= m.getbit45();
		OccupiedSq135 ^= m.getbit135();
		OccupiedSq90 ^= m.getbit90();*/

		turn = getOpponent(turn);
		TTKey ^= TT_ColorKey;

		Pieces[turn][movingpiece] |= getPos2Bit(from);
		Pieces[turn][special] ^= getPos2Bit(to);

		Squares[from] = getPiece2Square(PIECE_PAWN,turn);
		Squares[to] = capturedpiece;

		TTKey ^= TT_PieceKey[turn][movingpiece][from];
		TTKey ^= TT_PieceKey[turn][special][to];
	}
	else
	{
		OccupiedSq ^= bit;
		/*OccupiedSq45 ^= m.getbit45();
		OccupiedSq135 ^= m.getbit135();
		OccupiedSq90 ^= m.getbit90();*/

		turn = getOpponent(turn);
		TTKey ^= TT_ColorKey;
		Pieces[turn][movingpiece] ^= bit;

		Squares[from] = getPiece2Square(movingpiece,turn);
		Squares[to] = capturedpiece;

		TTKey ^= TT_PieceKey[turn][movingpiece][from];
		TTKey ^= TT_PieceKey[turn][movingpiece][to];
	}

    if(special==PIECE_KING) //castling
    {
        int f = -1;
        int t = -1;
        if(turn==COLOR_WHITE)
        {
            if(to == 1)
            {
                f = 0;
                t = 2;
            }
            if(to == 5)
            {
                f = 7;
                t = 4;
            }
        }
        else
        {
            if(to == 57)
            {
                f = 56;
                t = 58;
            }
            if(to == 61)
            {
                f = 63;
                t = 60;
            }
        }
        Bitset cas = getPos2Bit(f) | getPos2Bit(t);
        Bitset cas45 = getPos2Bit(getturn45(f)) | getPos2Bit(getturn45(t));
        Bitset cas135 = getPos2Bit(getturn135(f)) | getPos2Bit(getturn135(t));
        Bitset cas90 = getPos2Bit(getturn90(f)) | getPos2Bit(getturn90(t));
        OccupiedSq ^= cas;
        /*OccupiedSq45 ^= cas45;
        OccupiedSq135 ^= cas135;
        OccupiedSq90 ^= cas90;*/
        Pieces[turn][PIECE_ROOK] ^= cas;
        Squares[f] = Squares[t];
        Squares[t] = SQUARE_EMPTY;

		TTKey ^= TT_PieceKey[turn][PIECE_ROOK][f];
		TTKey ^= TT_PieceKey[turn][PIECE_ROOK][t];
    }
	else if(special==PIECE_PAWN) //en passant
    {
        int x = 0;
        if(turn==COLOR_WHITE)
        {
            x = getMinus8(to);
			Pieces[COLOR_BLACK][PIECE_PAWN] ^= getPos2Bit(x);
			Squares[x] = SQUARE_BLACKPAWN;
			TTKey ^= TT_PieceKey[COLOR_BLACK][PIECE_PAWN][x];
        }
        else
        {
            x = getPlus8(to);
			Pieces[COLOR_WHITE][PIECE_PAWN] ^= getPos2Bit(x);
			Squares[x] = SQUARE_WHITEPAWN;
			TTKey ^= TT_PieceKey[COLOR_WHITE][PIECE_PAWN][x];
        }
        OccupiedSq ^= getPos2Bit(x);
        /*OccupiedSq45 ^= getPos2Bit(getturn45(x));
        OccupiedSq135 ^= getPos2Bit(getturn135(x));
        OccupiedSq90 ^= getPos2Bit(getturn90(x));*/
    }

	int wkc = m.getWKC();
	int wqc = m.getWQC();
	int bkc = m.getBKC();
	int bqc = m.getBQC();
	if(castling[COLOR_WHITE][CASTLE_KINGSIDE] != wkc) //reset castling
	{
		castling[COLOR_WHITE][CASTLE_KINGSIDE] = wkc;
		TTKey ^= TT_CastlingKey[COLOR_WHITE][CASTLE_KINGSIDE];
	}
	if(castling[COLOR_WHITE][CASTLE_QUEENSIDE] != wqc)
	{
		castling[COLOR_WHITE][CASTLE_QUEENSIDE] = wqc;
		TTKey ^= TT_CastlingKey[COLOR_WHITE][CASTLE_QUEENSIDE];
	}
	if(castling[COLOR_BLACK][CASTLE_KINGSIDE] != bkc)
	{
		castling[COLOR_BLACK][CASTLE_KINGSIDE] = bkc;
		TTKey ^= TT_CastlingKey[COLOR_BLACK][CASTLE_KINGSIDE];
	}
	if(castling[COLOR_BLACK][CASTLE_QUEENSIDE] != bqc)
	{
		castling[COLOR_BLACK][CASTLE_QUEENSIDE] = bqc;
		TTKey ^= TT_CastlingKey[COLOR_BLACK][CASTLE_QUEENSIDE];
	}
	int ep = m.getEP(); //setting ep square
	if(epsquare != ep)
	{
		TTKey ^= TT_EPKey[epsquare];
		TTKey ^= TT_EPKey[ep];
		epsquare = ep;
	}

	if (movingpiece == PIECE_PAWN)
	{
		PawnKey ^= TT_PieceKey[turn][movingpiece][from];
		if (special == PIECE_NONE || special == PIECE_PAWN)
			PawnKey ^= TT_PieceKey[turn][movingpiece][to];
	}
	if (getSquare2Piece(capturedpiece) == PIECE_PAWN)
	{
		if (special == PIECE_PAWN)
		{
			if (turn == COLOR_WHITE)
				PawnKey ^= TT_PieceKey[getOpponent(turn)][PIECE_PAWN][getMinus8(to)];
			else
				PawnKey ^= TT_PieceKey[getOpponent(turn)][PIECE_PAWN][getPlus8(to)];
		}
		else
			PawnKey ^= TT_PieceKey[getOpponent(turn)][PIECE_PAWN][to];
	}
}

void Position::generateMoves(vector<Move>& moves) const
{
    //std::vector<Move> moves(0);
	//moves.reserve(128);
	//moves.clear();

    Bitset ColorPieces[2];

    ColorPieces[COLOR_WHITE] = Pieces[COLOR_WHITE][PIECE_PAWN] | Pieces[COLOR_WHITE][PIECE_KNIGHT] |
                         Pieces[COLOR_WHITE][PIECE_BISHOP] | Pieces[COLOR_WHITE][PIECE_ROOK] |
                         Pieces[COLOR_WHITE][PIECE_QUEEN] | Pieces[COLOR_WHITE][PIECE_KING];
    ColorPieces[COLOR_BLACK] = Pieces[COLOR_BLACK][PIECE_PAWN] | Pieces[COLOR_BLACK][PIECE_KNIGHT] |
                         Pieces[COLOR_BLACK][PIECE_BISHOP] | Pieces[COLOR_BLACK][PIECE_ROOK] |
                         Pieces[COLOR_BLACK][PIECE_QUEEN] | Pieces[COLOR_BLACK][PIECE_KING];

    Bitset b;
    //Pawn Moves
    b = Pieces[turn][PIECE_PAWN];
    while(b)
    {
        unsigned long n = 0;
		_BitScanForward64(&n,b);
		b ^= getPos2Bit(n);
        Bitset m = PawnMoves[turn][n]&(PawnMoves[turn][n]^OccupiedSq);
        while(m)
        {
			unsigned long k = 0;
			_BitScanForward64(&k,m);
			m ^= getPos2Bit(k);
			if(getRank(k)==7 || getRank(k)==0) //promotion
			{
				Move movq(n,k,PIECE_PAWN,Squares[k],PIECE_QUEEN,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
				addMove(moves,movq);
				Move movn(n,k,PIECE_PAWN,Squares[k],PIECE_KNIGHT,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
				addMove(moves,movn);
				Move movr(n,k,PIECE_PAWN,Squares[k],PIECE_ROOK,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
				addMove(moves,movr);
				Move movb(n,k,PIECE_PAWN,Squares[k],PIECE_BISHOP,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
				addMove(moves,movb);
			}
			else
			{
				Move mov(n,k,PIECE_PAWN,Squares[k],PIECE_NONE,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
				addMove(moves,mov);
			}
        }
		//double moves
		if(turn==COLOR_WHITE)
		{
			if(n==8 && Squares[16]==SQUARE_EMPTY && Squares[24]==SQUARE_EMPTY)
			{
				Move mov(n,24,PIECE_PAWN,SQUARE_EMPTY,PIECE_NONE,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
				addMove(moves,mov);
			}
			else if(n==9 && Squares[17]==SQUARE_EMPTY && Squares[25]==SQUARE_EMPTY)
			{
				Move mov(n,25,PIECE_PAWN,SQUARE_EMPTY,PIECE_NONE,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
				addMove(moves,mov);
			}
			else if(n==10 && Squares[18]==SQUARE_EMPTY && Squares[26]==SQUARE_EMPTY)
			{
				Move mov(n,26,PIECE_PAWN,SQUARE_EMPTY,PIECE_NONE,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
				addMove(moves,mov);
			}
			else if(n==11 && Squares[19]==SQUARE_EMPTY && Squares[27]==SQUARE_EMPTY)
			{
				Move mov(n,27,PIECE_PAWN,SQUARE_EMPTY,PIECE_NONE,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
				addMove(moves,mov);
			}
			else if(n==12 && Squares[20]==SQUARE_EMPTY && Squares[28]==SQUARE_EMPTY)
			{
				Move mov(n,28,PIECE_PAWN,SQUARE_EMPTY,PIECE_NONE,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
				addMove(moves,mov);
			}
			else if(n==13 && Squares[21]==SQUARE_EMPTY && Squares[29]==SQUARE_EMPTY)
			{
				Move mov(n,29,PIECE_PAWN,SQUARE_EMPTY,PIECE_NONE,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
				addMove(moves,mov);
			}
			else if(n==14 && Squares[22]==SQUARE_EMPTY && Squares[30]==SQUARE_EMPTY)
			{
				Move mov(n,30,PIECE_PAWN,SQUARE_EMPTY,PIECE_NONE,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
				addMove(moves,mov);
			}
			else if(n==15 && Squares[23]==SQUARE_EMPTY && Squares[31]==SQUARE_EMPTY)
			{
				Move mov(n,31,PIECE_PAWN,SQUARE_EMPTY,PIECE_NONE,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
				addMove(moves,mov);
			}
		}
		else
		{
			if(n==48 && Squares[40]==SQUARE_EMPTY && Squares[32]==SQUARE_EMPTY)
			{
				Move mov(n,32,PIECE_PAWN,SQUARE_EMPTY,PIECE_NONE,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
				addMove(moves,mov);
			}
			else if(n==49 && Squares[41]==SQUARE_EMPTY && Squares[33]==SQUARE_EMPTY)
			{
				Move mov(n,33,PIECE_PAWN,SQUARE_EMPTY,PIECE_NONE,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
				addMove(moves,mov);
			}
			else if(n==50 && Squares[42]==SQUARE_EMPTY && Squares[34]==SQUARE_EMPTY)
			{
				Move mov(n,34,PIECE_PAWN,SQUARE_EMPTY,PIECE_NONE,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
				addMove(moves,mov);
			}
			else if(n==51 && Squares[43]==SQUARE_EMPTY && Squares[35]==SQUARE_EMPTY)
			{
				Move mov(n,35,PIECE_PAWN,SQUARE_EMPTY,PIECE_NONE,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
				addMove(moves,mov);
			}
			else if(n==52 && Squares[44]==SQUARE_EMPTY && Squares[36]==SQUARE_EMPTY)
			{
				Move mov(n,36,PIECE_PAWN,SQUARE_EMPTY,PIECE_NONE,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
				addMove(moves,mov);
			}
			else if(n==53 && Squares[45]==SQUARE_EMPTY && Squares[37]==SQUARE_EMPTY)
			{
				Move mov(n,37,PIECE_PAWN,SQUARE_EMPTY,PIECE_NONE,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
				addMove(moves,mov);
			}
			else if(n==54 && Squares[46]==SQUARE_EMPTY && Squares[38]==SQUARE_EMPTY)
			{
				Move mov(n,38,PIECE_PAWN,SQUARE_EMPTY,PIECE_NONE,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
				addMove(moves,mov);
			}
			else if(n==55 && Squares[47]==SQUARE_EMPTY && Squares[39]==SQUARE_EMPTY)
			{
				Move mov(n,39,PIECE_PAWN,SQUARE_EMPTY,PIECE_NONE,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
				addMove(moves,mov);
			}
		}

        //Pawn Attacks
        Bitset x = 0x0;
        /*if(turn==COLOR_WHITE)
		{
			if(epsquare==0)
				x = getPawnAttacks(turn,n)&(ColorPieces[COLOR_BLACK]);
			else
				x = getPawnAttacks(turn,n)&(ColorPieces[COLOR_BLACK] | getPos2Bit(epsquare));
		}
        else
		{
			if(epsquare==0)
				x = getPawnAttacks(turn,n)&(ColorPieces[COLOR_WHITE]);
			else
				x = getPawnAttacks(turn,n)&(ColorPieces[COLOR_WHITE] | getPos2Bit(epsquare));
		}*/
		if(epsquare==0)
			x = PawnAttacks[turn][n]&(ColorPieces[getOpponent(turn)]);
		else
			x = PawnAttacks[turn][n]&(ColorPieces[getOpponent(turn)] | getPos2Bit(epsquare));
        while(x)
        {
            unsigned long k = 0;
			_BitScanForward64(&k,x);
			x ^= getPos2Bit(k);
			if(getRank(int(k))==7 || getRank(int(k))==0) //promotion
			{
				Move movq(n,k,PIECE_PAWN,Squares[k],PIECE_QUEEN,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
				addMove(moves,movq);
				Move movn(n,k,PIECE_PAWN,Squares[k],PIECE_KNIGHT,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
				addMove(moves,movn);
				Move movr(n,k,PIECE_PAWN,Squares[k],PIECE_ROOK,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
				addMove(moves,movr);
				Move movb(n,k,PIECE_PAWN,Squares[k],PIECE_BISHOP,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
				addMove(moves,movb);
			}
			else
			{
				Move mov(n,k,PIECE_PAWN,Squares[k],PIECE_NONE,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
				if(k==epsquare && epsquare!=0)
					mov = Move(n,k,PIECE_PAWN,Squares[k],PIECE_PAWN,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
				addMove(moves,mov);
			}
        }
		/*if(epsquare!=0)
		{ 
			if((getPawnAttacks(turn,n)&getPos2Bit(epsquare)))
			{
				Move mov(n,epsquare,PIECE_PAWN,SQUARE_EMPTY,PIECE_PAWN,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
				addMove(moves,mov);
			}
		}*/
    }
    //Knight Moves
    b = Pieces[turn][PIECE_KNIGHT];
    while(b)
    {
        unsigned long n = 0;
		_BitScanForward64(&n,b);
		b ^= getPos2Bit(n);
        Bitset m = KnightMoves[n]&(KnightMoves[n]^ColorPieces[turn]);
        while(m)
        {
            unsigned long k = 0;
			_BitScanForward64(&k,m);
			m ^= getPos2Bit(k);
            Move mov(n,k,PIECE_KNIGHT,Squares[k],PIECE_NONE,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
            addMove(moves,mov);
        }
    }
    //King Moves
    b = Pieces[turn][PIECE_KING];
    while(b)
    {
        unsigned long n = 0;
		_BitScanForward64(&n,b);
		b ^= getPos2Bit(n);
        Bitset m = KingMoves[n]&(KingMoves[n]^ColorPieces[turn]);
        while(m)
        {
			unsigned long k = 0;
			_BitScanForward64(&k,m);
			m ^= getPos2Bit(k);
            Move mov(n,k,PIECE_KING,Squares[k],PIECE_NONE,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
            addMove(moves,mov);
        }
    }

    //Rook Moves
    b = Pieces[turn][PIECE_ROOK];
    while(b)
    {
        unsigned long n = 0;
		_BitScanForward64(&n,b);
		b ^= getPos2Bit(n);
        /*Bitset m = getRookRankMoves(n,(OccupiedSq>>(getRankOffset(n)))&0xff);
        m |= getRookFileMoves(n,(OccupiedSq90>>(getFileOffset(n)))&0xff);*/
		Bitset m = getRookAttacks(n,OccupiedSq);
        m &= m^ColorPieces[turn];
        while(m)
        {
            unsigned long k = 0;
			_BitScanForward64(&k,m);
			m ^= getPos2Bit(k);
            Move mov(n,k,PIECE_ROOK,Squares[k],PIECE_NONE,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
            addMove(moves,mov);
        }
    }

    //Bishop Moves
    b = Pieces[turn][PIECE_BISHOP];
    while(b)
    {
        unsigned long n = 0;
		_BitScanForward64(&n,b);
		b ^= getPos2Bit(n);
		Bitset m = getBishopAttacks(n,OccupiedSq);
        m &= m^ColorPieces[turn];
        while(m)
        {
            unsigned long k = 0;
			_BitScanForward64(&k,m);
            m^=getPos2Bit(k);
            Move mov(n,k,PIECE_BISHOP,Squares[k],PIECE_NONE,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
            addMove(moves,mov);
        }
    }

    //Queen Moves
    b = Pieces[turn][PIECE_QUEEN];
    while(b)
    {
        unsigned long n = 0;
		_BitScanForward64(&n,b);
		b ^= getPos2Bit(n);
        /*Bitset m = getRookRankMoves(n,(OccupiedSq>>(getRankOffset(n)))&0xff);
        m |= getRookFileMoves(n,(OccupiedSq90>>(getFileOffset(n)))&0xff);
        m |= getBishopA1H8Moves(n,(OccupiedSq135>>getDiag(getturn135(n)))&0xff);
        m |= getBishopA8H1Moves(n,(OccupiedSq45>>getDiag(getturn45(n)))&0xff);*/
		Bitset m = getQueenAttacks(n,OccupiedSq);
        m &= m^ColorPieces[turn];
        while(m)
        {
            unsigned long k = 0;
			_BitScanForward64(&k,m);
			m ^= getPos2Bit(k);
            Move mov(n,k,PIECE_QUEEN,Squares[k],PIECE_NONE,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
            addMove(moves,mov);
        }
    }

    //Castling
    if(turn==COLOR_WHITE)
    {
        if(castling[turn][CASTLE_KINGSIDE] && !isAttacked(turn,3) && !isAttacked(turn,2) && !isAttacked(turn,1) && Squares[2]==SQUARE_EMPTY && Squares[1]==SQUARE_EMPTY)
        {
            Move mov(3,1,PIECE_KING,SQUARE_EMPTY,PIECE_KING,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);

            addMove(moves,mov);
        }
        if(castling[turn][CASTLE_QUEENSIDE] && !isAttacked(turn,3) && !isAttacked(turn,4) && !isAttacked(turn,5) && Squares[4]==SQUARE_EMPTY && Squares[5]==SQUARE_EMPTY && Squares[6]==SQUARE_EMPTY)
        {
            Move mov(3,5,PIECE_KING,SQUARE_EMPTY,PIECE_KING,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
            addMove(moves,mov);
        }
    }
    else
    {
        if(castling[turn][CASTLE_KINGSIDE] && !isAttacked(turn,59) && !isAttacked(turn,58) && !isAttacked(turn,57) && Squares[58]==SQUARE_EMPTY && Squares[57]==SQUARE_EMPTY)
        {
            Move mov(59,57,PIECE_KING,SQUARE_EMPTY,PIECE_KING,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
            addMove(moves,mov);
        }
        if(castling[turn][CASTLE_QUEENSIDE] && !isAttacked(turn,59) && !isAttacked(turn,60) && !isAttacked(turn,61) && Squares[60]==SQUARE_EMPTY && Squares[61]==SQUARE_EMPTY && Squares[62]==SQUARE_EMPTY)
        {
            Move mov(59,61,PIECE_KING,SQUARE_EMPTY,PIECE_KING,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
            addMove(moves,mov);
        }
    }

	//sort
	/*for(unsigned int i = 0;i<moves.size();i++)
	{
		for(unsigned int j = 0; j<moves.size()-1-i;j++)
		{
			Move a = moves.at(j);
			Move b = moves.at(j+1);
			if(a<b)
			{
				moves.at(j) = b;
				moves.at(j+1) = a;
			}
		}
	}*/

	//sortMoves(moves,turn);
	//return moves;
}

void Position::generateCaptures(vector<Move>& moves) const
{
    //std::vector<Move> moves(0);
	//moves.reserve(128);
	//moves.clear();

    Bitset ColorPieces[2];

    ColorPieces[COLOR_WHITE] = Pieces[COLOR_WHITE][PIECE_PAWN] | Pieces[COLOR_WHITE][PIECE_KNIGHT] |
                         Pieces[COLOR_WHITE][PIECE_BISHOP] | Pieces[COLOR_WHITE][PIECE_ROOK] |
                         Pieces[COLOR_WHITE][PIECE_QUEEN] | Pieces[COLOR_WHITE][PIECE_KING];
    ColorPieces[COLOR_BLACK] = Pieces[COLOR_BLACK][PIECE_PAWN] | Pieces[COLOR_BLACK][PIECE_KNIGHT] |
                         Pieces[COLOR_BLACK][PIECE_BISHOP] | Pieces[COLOR_BLACK][PIECE_ROOK] |
                         Pieces[COLOR_BLACK][PIECE_QUEEN] | Pieces[COLOR_BLACK][PIECE_KING];

    Bitset b;
    //Pawn Moves
    b = Pieces[turn][PIECE_PAWN];
    while(b)
    {
        unsigned long n = 0;
		_BitScanForward64(&n,b);
		b ^= getPos2Bit(n);

        //Pawn Attacks
        Bitset x = 0x0;
		if(epsquare==0)
			x = PawnAttacks[turn][n]&(ColorPieces[getOpponent(turn)]);
		else
			x = PawnAttacks[turn][n]&(ColorPieces[getOpponent(turn)] | getPos2Bit(epsquare));
        while(x)
        {
            unsigned long k = 0;
			_BitScanForward64(&k,x);
			x ^= getPos2Bit(k);
			if(getRank(k)==7 || getRank(k)==0) //promotion
			{
				Move movq(n,k,PIECE_PAWN,Squares[k],PIECE_QUEEN,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
				addMove(moves,movq);
				Move movn(n,k,PIECE_PAWN,Squares[k],PIECE_KNIGHT,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
				addMove(moves,movn);
				Move movr(n,k,PIECE_PAWN,Squares[k],PIECE_ROOK,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
				addMove(moves,movr);
				Move movb(n,k,PIECE_PAWN,Squares[k],PIECE_BISHOP,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
				addMove(moves,movb);
			}
			else
			{
				Move mov(n,k,PIECE_PAWN,Squares[k],PIECE_NONE,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
				if(epsquare!=0 && k==epsquare)
					mov = Move(n,k,PIECE_PAWN,Squares[k],PIECE_PAWN,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
				addMove(moves,mov);
			}
        }
    }
    //Knight Moves
    b = Pieces[turn][PIECE_KNIGHT];
    while(b)
    {
        unsigned long n = 0;
		_BitScanForward64(&n,b);
        b^=getPos2Bit(n);
        Bitset m = getKnightMoves(n)&ColorPieces[getOpponent(turn)];
        while(m)
        {
            unsigned long k = 0;
			_BitScanForward64(&k,m);
            m^=getPos2Bit(k);
            Move mov(n,k,PIECE_KNIGHT,Squares[k],PIECE_NONE,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
            addMove(moves,mov);
        }
    }
    //King Moves
    b = Pieces[turn][PIECE_KING];
    while(b)
    {
        unsigned long n = 0;
		_BitScanForward64(&n,b);
		b ^= getPos2Bit(n);
        Bitset m = KingMoves[n]&ColorPieces[getOpponent(turn)];
        while(m)
        {
			unsigned long k = 0;
			_BitScanForward64(&k,m);
			m ^= getPos2Bit(k);
            Move mov(n,k,PIECE_KING,Squares[k],PIECE_NONE,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
            addMove(moves,mov);
        }
    }

    //Rook Moves
    b = Pieces[turn][PIECE_ROOK];
    while(b)
    {
        unsigned long n = 0;
		_BitScanForward64(&n,b);
		b ^= getPos2Bit(n);
        /*Bitset m = getRookRankMoves(n,(OccupiedSq>>(getRankOffset(n)))&0xff);
        m |= getRookFileMoves(n,(OccupiedSq90>>(getFileOffset(n)))&0xff);*/
		Bitset m = getRookAttacks(n,OccupiedSq);
        m &= ColorPieces[getOpponent(turn)];
        while(m)
        {
            unsigned long k = 0;
			_BitScanForward64(&k,m);
			m ^= getPos2Bit(k);
            Move mov(n,k,PIECE_ROOK,Squares[k],PIECE_NONE,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
            addMove(moves,mov);
        }
    }

    //Bishop Moves
    b = Pieces[turn][PIECE_BISHOP];
    while(b)
    {
        unsigned long n = 0;
		_BitScanForward64(&n,b);
		b ^= getPos2Bit(n);
        /*Bitset m = getBishopA1H8Moves(n,(OccupiedSq135>>getDiag(getturn135(n)))&0xff);
        m |= getBishopA8H1Moves(n,(OccupiedSq45>>getDiag(getturn45(n)))&0xff);*/
		Bitset m = getBishopAttacks(n, OccupiedSq);
        m &= ColorPieces[getOpponent(turn)];
        while(m)
        {
            unsigned long k = 0;
			_BitScanForward64(&k,m);
			m ^= getPos2Bit(k);
            Move mov(n,k,PIECE_BISHOP,Squares[k],PIECE_NONE,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
            addMove(moves,mov);
        }
    }

    //Queen Moves
    b = Pieces[turn][PIECE_QUEEN];
    while(b)
    {
        unsigned long n = 0;
		_BitScanForward64(&n,b);
		b ^= getPos2Bit(n);
		Bitset m = getQueenAttacks(n, OccupiedSq);
        m &= ColorPieces[getOpponent(turn)];
        while(m)
        {
            unsigned long k = 0;
			_BitScanForward64(&k,m);
			m ^= getPos2Bit(k);
            Move mov(n,k,PIECE_QUEEN,Squares[k],PIECE_NONE,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
            addMove(moves,mov);
        }
    }

	//sort
	/*for(unsigned int i = 0;i<moves.size();i++)
	{
		for(unsigned int j = 0; j<moves.size()-1-i;j++)
		{
			Move a = moves.at(j);
			Move b = moves.at(j+1);
			if(a<b)
			{
				moves.at(j) = b;
				moves.at(j+1) = a;
			}
		}
	}*/

	//sortMoves(moves,turn);
	//return moves;
}

void Position::addMove(std::vector<Move>& vec,Move const& m) const
{
	/*forceMove(m);
	if(!underCheck(getOpponent(turn)))
	{*/
		vec.push_back(m);
	/*}
	unmakeMove(m);*/
}

bool Position::isLegal(Move const& m)
{
	forceMove(m);
	if(!underCheck(getOpponent(turn)))
	{
		unmakeMove(m);
		return true;
	}
	unmakeMove(m);
	return false;
}

bool Position::isAttacked(int turn,int n) const
{
    int opp = getOpponent(turn);
	Bitset b = getBishopAttacks(n, OccupiedSq)&(Pieces[opp][PIECE_BISHOP] | Pieces[opp][PIECE_QUEEN]);
	if (b != 0)
        return true;
	b = getRookAttacks(n, OccupiedSq)&(Pieces[opp][PIECE_ROOK] | Pieces[opp][PIECE_QUEEN]);
	if (b != 0)
		return true;
	b = PawnAttacks[turn][n] & Pieces[opp][PIECE_PAWN];
	if (b != 0)
		return true;
	b = KnightMoves[n] & Pieces[opp][PIECE_KNIGHT];
	if (b != 0)
		return true;
	b = KingMoves[n] & Pieces[opp][PIECE_KING];
	if (b != 0)
		return true;
    return false;
}

int Position::getSmallestAttacker2(int turn,int n)
{
    int opp = getOpponent(turn);
	Bitset b = PawnAttacks[opp][n]&Pieces[turn][PIECE_PAWN];
	if(b!=0)
	{
		return PIECE_PAWN;
	}
	b = KnightMoves[n]&Pieces[turn][PIECE_KNIGHT];
	if(b!=0)
	{
		return PIECE_KNIGHT;
	}
	b = getBishopAttacks(n, OccupiedSq)&Pieces[turn][PIECE_BISHOP];
	/*b |= BishopA1H8Moves[n][(OccupiedSq135>>Diagonal[turn135[n]])&0xff]&Pieces[turn][PIECE_BISHOP];
    b |= BishopA8H1Moves[n][(OccupiedSq45>>Diagonal[turn45[n]])&0xff]&Pieces[turn][PIECE_BISHOP];*/
	if(b!=0)
	{
		return PIECE_BISHOP;
	}
	b = getRookAttacks(n, OccupiedSq)&Pieces[turn][PIECE_ROOK];
    /*b |= RookRankMoves[n][(OccupiedSq>>(RankOffset[n]))&0xff]&Pieces[turn][PIECE_ROOK];
    b |= RookFileMoves[n][(OccupiedSq90>>(FileOffset[n]))&0xff]&Pieces[turn][PIECE_ROOK];*/
	if(b!=0)
	{
		return PIECE_ROOK;
	}
	/*b |= BishopA1H8Moves[n][(OccupiedSq135>>Diagonal[turn135[n]])&0xff]&Pieces[turn][PIECE_QUEEN];
    b |= BishopA8H1Moves[n][(OccupiedSq45>>Diagonal[turn45[n]])&0xff]&Pieces[turn][PIECE_QUEEN];
	b |= RookRankMoves[n][(OccupiedSq>>(RankOffset[n]))&0xff]&Pieces[turn][PIECE_QUEEN];
    b |= RookFileMoves[n][(OccupiedSq90>>(FileOffset[n]))&0xff]&Pieces[turn][PIECE_QUEEN];*/
	b = getQueenAttacks(n, OccupiedSq)&Pieces[turn][PIECE_QUEEN];
	if(b!=0)
	{
		return PIECE_QUEEN;
	}
	b = KingMoves[n]&Pieces[turn][PIECE_KING];
    if(b!=0)
	{
        return PIECE_KING;
	}
    return PIECE_NONE;
}

Move Position::getSmallestAttacker(int turn,int n)
{
	return getSmallestAttacker(turn,n,OccupiedSq);
}

Move Position::getSmallestAttacker(int turn,int n,unsigned long long occ)
{
    int opp = getOpponent(turn);
	Bitset b = 0x0;
	b = PawnAttacks[opp][n]&Pieces[turn][PIECE_PAWN];
	if(b!=0)
	{
		unsigned long k = 0;
		_BitScanForward64(&k,b);  //we don't care if this is a promotion or en passant since this function is only going to be called in SEE
		Move mov(k,n,PIECE_PAWN,Squares[n],PIECE_NONE,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
		//if(isLegal(mov))
			return mov;
	}
	b |= KnightMoves[n]&Pieces[turn][PIECE_KNIGHT];
	if(b!=0)
	{
		unsigned long k = 0;
		_BitScanForward64(&k,b);
		Move mov(k,n,PIECE_KNIGHT,Squares[n],PIECE_NONE,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
		//if(isLegal(mov))
			return mov;
	}
	b |= getBishopAttacks(n,occ);
	b &= Pieces[turn][PIECE_BISHOP];
	if(b!=0)
	{
		unsigned long k = 0;
		_BitScanForward64(&k,b);
		Move mov(k,n,PIECE_BISHOP,Squares[n],PIECE_NONE,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
		//if(isLegal(mov))
			return mov;
	}
    b |= getRookAttacks(n,occ);
	b &= Pieces[turn][PIECE_ROOK];
	if(b!=0)
	{
		unsigned long k = 0;
		_BitScanForward64(&k,b);
		Move mov(k,n,PIECE_ROOK,Squares[n],PIECE_NONE,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
		//if(isLegal(mov))
			return mov;
	}
	b |= getBishopAttacks(n, occ);
	b |= getRookAttacks(n, occ);
	b &= Pieces[turn][PIECE_QUEEN];
	if(b!=0)
	{
		unsigned long k = 0;
		_BitScanForward64(&k,b);
		Move mov(k,n,PIECE_QUEEN,Squares[n],PIECE_NONE,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
		//if(isLegal(mov))
			return mov;
	}
	b |= getKingMoves(n)&Pieces[turn][PIECE_KING];
    if(b!=0)
	{
        unsigned long k = 0;
		_BitScanForward64(&k,b);
		Move mov(k,n,PIECE_KING,Squares[n],PIECE_NONE,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
		//if(isLegal(mov))
			return mov;
	}
    return CONS_NULLMOVE;
}

bool Position::underCheck(int turn)
{
	unsigned long k = 0;
	_BitScanForward64(&k, Pieces[turn][PIECE_KING]);
    return isAttacked(turn, k);
}

Move Position::makeCapture(int piece, int n)
{
	std::vector<Move> vec;
	vec.reserve(128);
	generateCaptures(vec);
	for(int i = 0;i<vec.size();i++)
	{
	    Move m = vec.at(i);
		if(m.getMovingPiece()==piece && m.getTo()==n)
		{
			//forceMove(m);
			return m;
		}
	}
	return CONS_NULLMOVE;
	/*int opp = getOpponent(turn);
	Bitset b = 0x0;
	if(piece==PIECE_PAWN)
	{
		b = getPawnAttacks(turn,n)&Pieces[opp][PIECE_PAWN];
	}
	else if(piece==PIECE_KNIGHT)
	{
		b = getKnightMoves(n)&Pieces[opp][PIECE_KNIGHT];
	}
	else if(piece==PIECE_BISHOP)
	{
		b = getBishopA1H8Moves(n,(OccupiedSq135>>getDiag(getturn135(n)))&0xff)&Pieces[opp][PIECE_BISHOP];
		b |= getBishopA8H1Moves(n,(OccupiedSq45>>getDiag(getturn45(n)))&0xff)&Pieces[opp][PIECE_BISHOP];
	}
	else if(piece==PIECE_ROOK)
	{
		b = getRookRankMoves(n,(OccupiedSq>>(8*getRank(n)))&0xff)&Pieces[opp][PIECE_ROOK];
		b |= getRookFileMoves(n,(OccupiedSq90>>(8*getFile(n)))&0xff)&Pieces[opp][PIECE_ROOK];
	}
	else if(piece==PIECE_QUEEN)
	{
		b = getBishopA1H8Moves(n,(OccupiedSq135>>getDiag(getturn135(n)))&0xff)&Pieces[opp][PIECE_QUEEN];
		b |= getBishopA8H1Moves(n,(OccupiedSq45>>getDiag(getturn45(n)))&0xff)&Pieces[opp][PIECE_QUEEN];
		b |= getRookRankMoves(n,(OccupiedSq>>(8*getRank(n)))&0xff)&Pieces[opp][PIECE_QUEEN];
		b |= getRookFileMoves(n,(OccupiedSq90>>(8*getFile(n)))&0xff)&Pieces[opp][PIECE_QUEEN];
	}
	else if(piece==PIECE_KING)
	{
		b = getKingMoves(n)&Pieces[opp][PIECE_KING];
	}
    return CONS_NULLMOVE;*/
}

int Position::getGameStatus()
{
	if(isRepetition())
	{
		return STATUS_3FOLDREP;
	}
	vector<Move> vec;
	vec.reserve(128);
	generateMoves(vec);
	if(vec.size()==0)
	{
		if(underCheck(turn) && turn==COLOR_WHITE)
		{
			return STATUS_WHITEMATED;
		}
		else if(underCheck(turn) && turn==COLOR_BLACK)
		{
			return STATUS_BLACKMATED;
		}
		else
		{
			return STATUS_STALEMATE;
		}
	}
	return STATUS_NOTOVER;
}

bool Position::isRepetition()
{
	int rep = 0;
	int i;
	int size = hashlist.size();
	for(i = 1;i<=size;i++)
	{
		Move m = movelist.at(size-i);
		if(m!=CONS_NULLMOVE && m.getMovingPiece()!=PIECE_PAWN && m.getCapturedPiece()==SQUARE_EMPTY && m.getSpecial()!=PIECE_PAWN)
		{
			if(hashlist.at(size-i)==TTKey)
			{
				rep++;
				if(rep>=1)
				{
					return true;
				}
			}
		}
		else
		{
			return false;
		}
	}
	return false;
}

Bitset getRookAttacks(int sq, Bitset occ)
{
	/*Bitset m = RookRankMoves[sq][(occ>>(RankOffset[sq]))&0xff];
    m |= RookFileMoves[sq][(occ90>>(FileOffset[sq]))&0xff];
    return m;*/
	/*occ &= RookMagicTable[sq].mask;
	occ *= RookMagicTable[sq].magic;
	occ >>= RookMagicTable[sq].shift;*/
	occ = ((occ&RookMagicTable[sq].mask)*RookMagicTable[sq].magic) >> RookMagicTable[sq].shift;
	return RookAttacks[sq][occ];
}

Bitset getBishopAttacks(int sq, Bitset occ)
{
	/*Bitset m = BishopA1H8Moves[sq][(occ135>>Diagonal[turn135[sq]])&0xff];
    m |= BishopA8H1Moves[sq][(occ45>>Diagonal[turn45[sq]])&0xff];*/
	//return m;
	/*occ &= BishopMagicTable[sq].mask;
	occ *= BishopMagicTable[sq].magic;
	occ >>= BishopMagicTable[sq].shift;*/
	occ = ((occ&BishopMagicTable[sq].mask)*BishopMagicTable[sq].magic) >> BishopMagicTable[sq].shift;
	return BishopAttacks[sq][occ];
}

Bitset getQueenAttacks(int sq, Bitset occ)
{
	Bitset m = getRookAttacks(sq, occ);
	m |= getBishopAttacks(sq, occ);
	return m;
}

int Position::getColorPieces(int turn)
{
	return (Pieces[turn][PIECE_PAWN] | Pieces[turn][PIECE_KNIGHT] |
            Pieces[turn][PIECE_BISHOP] | Pieces[turn][PIECE_ROOK] |
            Pieces[turn][PIECE_QUEEN] | Pieces[turn][PIECE_KING]);
}

void Position::takebackMove()
{
	unmakeMove(movelist.at(movelist.size()-1));
}

void Position::display(int flip)
{
    for(int i = 0;i<64;i++)
    {
        if(i%8==0)
            cout << endl;
        if(flip==1)
            cout << pieceStrings[Squares[i]];
        else
            cout << pieceStrings[Squares[63-i]];
    }
    cout << endl;
}

//int getSquare2Piece(int sq)
//{
//	return ((sq-1)%6);
//}
//
//int getPiece2Square(int p,int color)
//{
//	if(color==COLOR_WHITE)
//	{
//		return p+1;
//	}
//	else
//	{
//		return p+7;
//	}
//}
//
//int getSquare2Color(int sq)
//{
//	return ((sq-1)/6);
//}

///OLD FUNCTIONS
//std::vector<Move> Position::generateMoves2()
//{
//    std::vector<Move> moves(0);
//
//    Bitset ColorPieces[2];
//
//    ColorPieces[COLOR_WHITE] = Pieces[COLOR_WHITE][PIECE_PAWN] | Pieces[COLOR_WHITE][PIECE_KNIGHT] |
//                         Pieces[COLOR_WHITE][PIECE_BISHOP] | Pieces[COLOR_WHITE][PIECE_ROOK] |
//                         Pieces[COLOR_WHITE][PIECE_QUEEN] | Pieces[COLOR_WHITE][PIECE_KING];
//    ColorPieces[COLOR_BLACK] = Pieces[COLOR_BLACK][PIECE_PAWN] | Pieces[COLOR_BLACK][PIECE_KNIGHT] |
//                         Pieces[COLOR_BLACK][PIECE_BISHOP] | Pieces[COLOR_BLACK][PIECE_ROOK] |
//                         Pieces[COLOR_BLACK][PIECE_QUEEN] | Pieces[COLOR_BLACK][PIECE_KING];
//
//    Bitset b;
//    //Pawn Moves
//    b = Pieces[turn][PIECE_PAWN];
//    while(b)
//    {
//        int n = firstOf(b);
//        if(n==-1)
//        {
//            break;
//        }
//        b^=getPos2Bit(n);
//        Bitset m = getPawnMoves(turn,n)&(getPawnMoves(turn,n)^OccupiedSq);
//        while(m)
//        {
//            int k = firstOf(m);
//            if(k==-1)
//            {
//                break;
//            }
//            m^=getPos2Bit(k);
//			if(getRank(k)==7 || getRank(k)==0) //promotion
//			{
//				Move movq(n,k,PIECE_PAWN,Squares[k],PIECE_QUEEN,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
//				addMove(moves,movq);
//				Move movn(n,k,PIECE_PAWN,Squares[k],PIECE_KNIGHT,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
//				addMove(moves,movn);
//				Move movr(n,k,PIECE_PAWN,Squares[k],PIECE_ROOK,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
//				addMove(moves,movr);
//				Move movb(n,k,PIECE_PAWN,Squares[k],PIECE_BISHOP,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
//				addMove(moves,movb);
//			}
//			else
//			{
//				Move mov(n,k,PIECE_PAWN,Squares[k],PIECE_NONE,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
//				addMove(moves,mov);
//			}
//        }
//		//double moves
//		if(turn==COLOR_WHITE)
//		{
//			if(n==8 && Squares[16]==SQUARE_EMPTY && Squares[24]==SQUARE_EMPTY)
//			{
//				Move mov(n,24,PIECE_PAWN,SQUARE_EMPTY,PIECE_NONE,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
//				addMove(moves,mov);
//			}
//			else if(n==9 && Squares[17]==SQUARE_EMPTY && Squares[25]==SQUARE_EMPTY)
//			{
//				Move mov(n,25,PIECE_PAWN,SQUARE_EMPTY,PIECE_NONE,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
//				addMove(moves,mov);
//			}
//			else if(n==10 && Squares[18]==SQUARE_EMPTY && Squares[26]==SQUARE_EMPTY)
//			{
//				Move mov(n,26,PIECE_PAWN,SQUARE_EMPTY,PIECE_NONE,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
//				addMove(moves,mov);
//			}
//			else if(n==11 && Squares[19]==SQUARE_EMPTY && Squares[27]==SQUARE_EMPTY)
//			{
//				Move mov(n,27,PIECE_PAWN,SQUARE_EMPTY,PIECE_NONE,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
//				addMove(moves,mov);
//			}
//			else if(n==12 && Squares[20]==SQUARE_EMPTY && Squares[28]==SQUARE_EMPTY)
//			{
//				Move mov(n,28,PIECE_PAWN,SQUARE_EMPTY,PIECE_NONE,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
//				addMove(moves,mov);
//			}
//			else if(n==13 && Squares[21]==SQUARE_EMPTY && Squares[29]==SQUARE_EMPTY)
//			{
//				Move mov(n,29,PIECE_PAWN,SQUARE_EMPTY,PIECE_NONE,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
//				addMove(moves,mov);
//			}
//			else if(n==14 && Squares[22]==SQUARE_EMPTY && Squares[30]==SQUARE_EMPTY)
//			{
//				Move mov(n,30,PIECE_PAWN,SQUARE_EMPTY,PIECE_NONE,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
//				addMove(moves,mov);
//			}
//			else if(n==15 && Squares[23]==SQUARE_EMPTY && Squares[31]==SQUARE_EMPTY)
//			{
//				Move mov(n,31,PIECE_PAWN,SQUARE_EMPTY,PIECE_NONE,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
//				addMove(moves,mov);
//			}
//		}
//		else
//		{
//			if(n==48 && Squares[40]==SQUARE_EMPTY && Squares[32]==SQUARE_EMPTY)
//			{
//				Move mov(n,32,PIECE_PAWN,SQUARE_EMPTY,PIECE_NONE,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
//				addMove(moves,mov);
//			}
//			else if(n==49 && Squares[41]==SQUARE_EMPTY && Squares[33]==SQUARE_EMPTY)
//			{
//				Move mov(n,33,PIECE_PAWN,SQUARE_EMPTY,PIECE_NONE,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
//				addMove(moves,mov);
//			}
//			else if(n==50 && Squares[42]==SQUARE_EMPTY && Squares[34]==SQUARE_EMPTY)
//			{
//				Move mov(n,34,PIECE_PAWN,SQUARE_EMPTY,PIECE_NONE,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
//				addMove(moves,mov);
//			}
//			else if(n==51 && Squares[43]==SQUARE_EMPTY && Squares[35]==SQUARE_EMPTY)
//			{
//				Move mov(n,35,PIECE_PAWN,SQUARE_EMPTY,PIECE_NONE,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
//				addMove(moves,mov);
//			}
//			else if(n==52 && Squares[44]==SQUARE_EMPTY && Squares[36]==SQUARE_EMPTY)
//			{
//				Move mov(n,36,PIECE_PAWN,SQUARE_EMPTY,PIECE_NONE,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
//				addMove(moves,mov);
//			}
//			else if(n==53 && Squares[45]==SQUARE_EMPTY && Squares[37]==SQUARE_EMPTY)
//			{
//				Move mov(n,37,PIECE_PAWN,SQUARE_EMPTY,PIECE_NONE,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
//				addMove(moves,mov);
//			}
//			else if(n==54 && Squares[46]==SQUARE_EMPTY && Squares[38]==SQUARE_EMPTY)
//			{
//				Move mov(n,38,PIECE_PAWN,SQUARE_EMPTY,PIECE_NONE,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
//				addMove(moves,mov);
//			}
//			else if(n==55 && Squares[47]==SQUARE_EMPTY && Squares[39]==SQUARE_EMPTY)
//			{
//				Move mov(n,39,PIECE_PAWN,SQUARE_EMPTY,PIECE_NONE,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
//				addMove(moves,mov);
//			}
//		}
//
//        //Pawn Attacks
//        Bitset x = 0x0;
//        if(turn==COLOR_WHITE)
//		{
//			if(epsquare==0)
//				x = getPawnAttacks(turn,n)&(ColorPieces[COLOR_BLACK]);
//			else
//				x = getPawnAttacks(turn,n)&(ColorPieces[COLOR_BLACK] | getPos2Bit(epsquare));
//		}
//        else
//		{
//			if(epsquare==0)
//				x = getPawnAttacks(turn,n)&(ColorPieces[COLOR_WHITE]);
//			else
//				x = getPawnAttacks(turn,n)&(ColorPieces[COLOR_WHITE] | getPos2Bit(epsquare));
//		}
//        while(x)
//        {
//            int k = firstOf(x);
//            if(k==-1)
//            {
//                break;
//            }
//            x^=getPos2Bit(k);
//			if(getRank(k)==7 || getRank(k)==0) //promotion
//			{
//				Move movq(n,k,PIECE_PAWN,Squares[k],PIECE_QUEEN,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
//				addMove(moves,movq);
//				Move movn(n,k,PIECE_PAWN,Squares[k],PIECE_KNIGHT,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
//				addMove(moves,movn);
//				Move movr(n,k,PIECE_PAWN,Squares[k],PIECE_ROOK,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
//				addMove(moves,movr);
//				Move movb(n,k,PIECE_PAWN,Squares[k],PIECE_BISHOP,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
//				addMove(moves,movb);
//			}
//			else
//			{
//				Move mov(n,k,PIECE_PAWN,Squares[k],PIECE_NONE,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
//				if(k==epsquare && epsquare!=0)
//					mov = Move(n,k,PIECE_PAWN,Squares[k],PIECE_PAWN,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
//				addMove(moves,mov);
//			}
//        }
//    }
//    //Knight Moves
//    b = Pieces[turn][PIECE_KNIGHT];
//    while(b)
//    {
//        int n = firstOf(b);
//        if(n==-1)
//        {
//            break;
//        }
//        b^=getPos2Bit(n);
//        Bitset m = getKnightMoves(n)&(getKnightMoves(n)^ColorPieces[turn]);
//        while(m)
//        {
//            int k = firstOf(m);
//            if(k==-1)
//            {
//                break;
//            }
//            m^=getPos2Bit(k);
//            Move mov(n,k,PIECE_KNIGHT,Squares[k],PIECE_NONE,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
//            addMove(moves,mov);
//        }
//    }
//    //King Moves
//    b = Pieces[turn][PIECE_KING];
//    while(b)
//    {
//        int n = firstOf(b);
//        if(n==-1)
//        {
//            break;
//        }
//        b^=getPos2Bit(n);
//        Bitset m = getKingMoves(n)&(getKingMoves(n)^ColorPieces[turn]);
//        while(m)
//        {
//            int k = firstOf(m);
//            if(k==-1)
//            {
//                break;
//            }
//            m^=getPos2Bit(k);
//            Move mov(n,k,PIECE_KING,Squares[k],PIECE_NONE,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
//            addMove(moves,mov);
//        }
//    }
//
//    //Rook Moves
//    b = Pieces[turn][PIECE_ROOK];
//    while(b)
//    {
//        int n = firstOf(b);
//        if(n==-1)
//        {
//            break;
//        }
//        b^=getPos2Bit(n);
//        Bitset m = getRookRankMoves(n,(OccupiedSq>>(getRankOffset(n)))&0xff);
//        m |= getRookFileMoves(n,(OccupiedSq90>>(getFileOffset(n)))&0xff);
//        m &= m^ColorPieces[turn];
//        while(m)
//        {
//            int k = firstOf(m);
//            if(k==-1)
//            {
//                break;
//            }
//            m^=getPos2Bit(k);
//            Move mov(n,k,PIECE_ROOK,Squares[k],PIECE_NONE,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
//            addMove(moves,mov);
//        }
//    }
//
//    //Bishop Moves
//    b = Pieces[turn][PIECE_BISHOP];
//    while(b)
//    {
//        int n = firstOf(b);
//        if(n==-1)
//        {
//            break;
//        }
//        b^=getPos2Bit(n);
//        Bitset m = getBishopA1H8Moves(n,(OccupiedSq135>>getDiag(getturn135(n)))&0xff);
//        m |= getBishopA8H1Moves(n,(OccupiedSq45>>getDiag(getturn45(n)))&0xff);
//        m = m&(m^ColorPieces[turn]);
//        while(m)
//        {
//            int k = firstOf(m);
//            if(k==-1)
//            {
//                break;
//            }
//            m^=getPos2Bit(k);
//            Move mov(n,k,PIECE_BISHOP,Squares[k],PIECE_NONE,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
//            addMove(moves,mov);
//        }
//    }
//
//    //Queen Moves
//    b = Pieces[turn][PIECE_QUEEN];
//    while(b)
//    {
//        int n = firstOf(b);
//        if(n==-1)
//        {
//            break;
//        }
//        b^=getPos2Bit(n);
//        Bitset m = getRookRankMoves(n,(OccupiedSq>>(getRankOffset(n)))&0xff);
//        m |= getRookFileMoves(n,(OccupiedSq90>>(getFileOffset(n)))&0xff);
//        m |= getBishopA1H8Moves(n,(OccupiedSq135>>getDiag(getturn135(n)))&0xff);
//        m |= getBishopA8H1Moves(n,(OccupiedSq45>>getDiag(getturn45(n)))&0xff);
//        m = m&(m^ColorPieces[turn]);
//        while(m)
//        {
//            int k = firstOf(m);
//            if(k==-1)
//            {
//                break;
//            }
//            m^=getPos2Bit(k);
//            Move mov(n,k,PIECE_QUEEN,Squares[k],PIECE_NONE,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
//            addMove(moves,mov);
//        }
//    }
//
//    //Castling
//    if(turn==COLOR_WHITE)
//    {
//        if(castling[turn][CASTLE_KINGSIDE] && !isAttacked(turn,3) && !isAttacked(turn,2) && !isAttacked(turn,1) && 
//			Squares[2]==SQUARE_EMPTY && Squares[1]==SQUARE_EMPTY)
//        {
//            Move mov(3,1,PIECE_KING,SQUARE_EMPTY,PIECE_KING,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
//            addMove(moves,mov);
//        }
//        if(castling[turn][CASTLE_QUEENSIDE] && !isAttacked(turn,3) && !isAttacked(turn,4) && !isAttacked(turn,5) && 
//			Squares[4]==SQUARE_EMPTY && Squares[5]==SQUARE_EMPTY)
//        {
//            Move mov(3,5,PIECE_KING,SQUARE_EMPTY,PIECE_KING,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
//            addMove(moves,mov);
//        }
//    }
//    else
//    {
//        if(castling[turn][CASTLE_KINGSIDE] && !isAttacked(turn,59) && !isAttacked(turn,58) && !isAttacked(turn,57) && Squares[58]==SQUARE_EMPTY && Squares[57]==SQUARE_EMPTY)
//        {
//            Move mov(59,57,PIECE_KING,SQUARE_EMPTY,PIECE_KING,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
//            addMove(moves,mov);
//        }
//        if(castling[turn][CASTLE_QUEENSIDE] && !isAttacked(turn,59) && !isAttacked(turn,60) && !isAttacked(turn,61) && Squares[60]==SQUARE_EMPTY && Squares[61]==SQUARE_EMPTY)
//        {
//            Move mov(59,61,PIECE_KING,SQUARE_EMPTY,PIECE_KING,castling[0][0],castling[0][1],castling[1][0],castling[1][1],epsquare);
//            addMove(moves,mov);
//        }
//    }
//
//	//sort
//	/*for(unsigned int i = 0;i<moves.size();i++)
//	{
//		for(unsigned int j = 0; j<moves.size()-1-i;j++)
//		{
//			Move a = moves.at(j);
//			Move b = moves.at(j+1);
//			if(a<b)
//			{
//				moves.at(j) = b;
//				moves.at(j+1) = a;
//			}
//		}
//	}*/
//	return moves;
//}