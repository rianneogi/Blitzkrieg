#include "Interface.h"
#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <conio.h>
#include "Clock.h"

#define _CRT_SECURE_NO_WARNINGS

using namespace std;

Interface::Interface()
{
    board = Board();
    e1 = Engine(COLOR_WHITE,board.pos);
    //e2 = Engine(COLOR_BLACK,board.pos);
}

Interface::~Interface()
{
}

void Interface::UCI()
{
	cout << "id name " << ENGINENAME << endl;
	cout << "id author " << ENGINEAUTHOR << endl;
	cout << "uciok" << endl;
	cout << "info string " << ENGINENAME << " " << ENGINEVERSION << endl;
	string str = "";
	char cp[5000];
	while(true)
	{
		str = "";
		fgets(cp,sizeof(cp),stdin);
		for(int i = 0;cp[i]!='\0' && cp[i]!='\n';i++)
		{
			str += cp[i];
		}
		//cin >> str;
		//cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		string s = getStringToken(str,' ',1);
		if(s=="isready")
		{
			cout << "readyok" << endl;
		}
		else if(s=="quit")
		{
			break;
		}
		else if(s=="go")
		{
			int time = 0;
			for (int i = 2;;i++)
			{
				s = getStringToken(str, ' ', i);
				if (s == "infinite")
				{
					time = -1;
				}
				if (s == "movetime")
				{
					time = +atoi(getStringToken(str, ' ', i+1).c_str());
					i++;
				}
				if (s == "wtime")
				{
					if (e1.pos.turn == 0)
					{
						time += atoi(getStringToken(str, ' ', i + 1).c_str())/30;
					}
					i++;
				}
				if (s == "btime")
				{
					if (e1.pos.turn == 1)
					{
						time += atoi(getStringToken(str, ' ', i + 1).c_str())/30;
					}
					i++;
				}
				if (s == "winc")
				{
					if (e1.pos.turn == 0)
					{
						time += (2*atoi(getStringToken(str, ' ', i + 1).c_str()))/4;
					}
					i++;
				}
				if (s == "binc")
				{
					if (e1.pos.turn == 1)
					{
						time += (2*atoi(getStringToken(str, ' ', i + 1).c_str()))/4;
					}
					i++;
				}
				if (s == "")
					break;
			}
			
			if(time==0)
				time = 1000;

			Move m = e1.IterativeDeepening(time, true);
			cout << "bestmove " << m.toString() << endl;
			//e1.pos.forceMove(m);
		}
		else if(s=="position")
		{
			s = getStringToken(str,' ',2);
			int tokennumber = 3;
			if(s=="startpos")
			{
				//e1.pos = Position();
				e1.pos.setStartPos();
			}
			else if (s == "fen")
			{
				//cout << "info string fen enter" << endl;
				e1.pos.loadFromFEN(str.substr(getStringTokenPosition(str,' ',3)));
				tokennumber = 9;
			}

			s = getStringToken(str,' ',tokennumber);
			if(s=="moves")
			{
				int i = tokennumber+1;
				s = getStringToken(str,' ',i);

				while(s!="")
				{
					vector<Move> v;
					v.reserve(128);
					e1.pos.generateMoves(v);
					Move m = String2Move(s);
					int flag = 0;
					for(int j = 0;j<v.size();j++)
					{
						if(v.at(j).getFrom() == m.getFrom() && v.at(j).getTo() ==  m.getTo() && 
							(
								(m.getSpecial() == v.at(j).getSpecial() && 
									(
										m.getSpecial()==PIECE_BISHOP || m.getSpecial()==PIECE_KNIGHT || 
										m.getSpecial()==PIECE_QUEEN || m.getSpecial()==PIECE_ROOK
									)
								)
								|| m.getSpecial()==PIECE_PAWN || m.getSpecial()==PIECE_KING || m.getSpecial()==PIECE_NONE
							)
						)
						{
							e1.makeMove(v.at(j));
							flag = 1;
							//cout << v.at(j).toString() << endl;
						}
					}
					//cout << s << endl;
					//cout << m.getFrom() << " " << m.getTo() << endl;
					if (flag == 0)
					{
						cout << "info string ERROR: Legal move not found: " << m.toString() << ", move count: " << e1.pos.movelist.size() << endl;
					}
					i++;
					s = getStringToken(str,' ',i);
				}
			}
		}
		else if(s=="display")
		{
			e1.pos.display(0);
		}
		else if(s=="debug")
		{
			DEBUG = !DEBUG;
		}
		else if (s == "trace")
		{
			cout << e1.LeafEval<true>() << endl;
		}
		else if (s == "eval")
		{
			cout << e1.LeafEval<false>() << endl;
		}
		else if (s == "see")
		{
			string s2;
			cin >> s2;
			int m1 = Sq2Int(s2);
			cin >> s2;
			int m2 = Sq2Int(s2);
			cout << m1 << " " << m2 << " " << Square2Piece[board.pos.Squares[m1]] << Square2Piece[board.pos.Squares[m2]] << endl;
			cout << e1.StaticExchangeEvaluation(m2, m1, Square2Piece[board.pos.Squares[m1]], board.pos.Squares[m2]) << endl;
		}
		else if (s == "info" || s == "information")
		{
			info();
		}
		fflush(stdin);
	}
}

void Interface::Winboard()
{
	OUTPUT = OUTPUT_XBOARD;
	cout << endl;
	string s;
	bool side = COLOR_WHITE;
	while(true)
	{
		cin >> s;
		if(s=="new")
		{
			e1.pos = Position();
		}
		else if(s=="go")
		{
			side = e1.pos.turn;
			Move m = e1.IterativeDeepening(1000, true);
			cout << "move " << m.toString() << endl;
		}
		else if(s=="quit")
		{
			break;
		}
		else if(s=="ping")
		{
			int d;
			cin >> d;
			cout << "pong " << d << endl;
		}
		else
		{
			Move m = String2Move(s);
			if(!m.isNullMove())
			{
				vector<Move> moves;
				moves.reserve(128);
				e1.pos.generateMoves(moves);
				for(int i = 0;i<moves.size();i++)
				{
					if(m==moves.at(i))
					{
						e1.pos.forceMove(m);
					}
				}
			}
		}
	}
}

void Interface::start()
{
    string s;
    while(true)
    {
        cout << "System is ready..." << endl;
        if(board.pos.turn == COLOR_WHITE)
        {
            cout << "White to move" << endl;
        }
        else
        {
            cout << "Black to move" << endl;
        }
        cin >> s;
        if(s=="disp" || s=="display")
        {
            display(0);
        }
        else if(s=="dispflip" || s=="displayflip" || s=="displayflipped")
        {
            display(1);
        }
        else if(s=="move" || s=="makemove" || s=="playmove")
        {
            makeMove();
        }
		else if(s=="unmakemove" || s=="unmake" || s=="unmove" || s=="undo")
		{
			unmakeMove();
		}
        else if(s=="moves" || s=="getmoves")
        {
            getMoves();
        }
        else if(s=="countmoves" || s=="movecount" || s=="noofmoves")
        {
            movecount();
        }
		else if(s=="think" || s=="analyse" || s=="analyze" || s=="go")
		{
			think();
		}
		else if (s == "movesort")
		{
			vector<Move> vec;
			e1.pos.generateMoves(vec);
			for (int i = 0;i < vec.size();i++)
			{
				Move bm = e1.getHighestScoringMove(vec, i);
				cout << bm.toString() << " " << e1.getMoveScore(bm) << endl;
			}
		}
		else if (s == "pv")
		{
			cout << "ply = " << e1.ply << endl;
			cout << "pv size: " << e1.PrincipalVariation.size() << endl;
			for (int i = 0;i < e1.PrincipalVariation.size();i++)
			{
				cout << e1.PrincipalVariation.at(i).toString() << endl;
			}
		}
		else if (s == "loadfen")
		{
			fstream fen("fen.txt", ios::in);
			string fenstr = "";
			getline(fen, fenstr);
			e1.pos.loadFromFEN(fenstr);
			board.pos.loadFromFEN(fenstr);
			display(0);
		}
		else if (s == "piecesq")
		{
			string s2;
			cin >> s2;
			if (s2 == "wp")
			{
				for (int i = 0;i < 64;i++)
				{
					if (i % 8 == 0)
					{
						cout << endl;
					}
					cout << PieceSq[SQUARE_WHITEPAWN][i] << " ";
				}
			}
			if (s2 == "bp")
			{
				for (int i = 0;i < 64;i++)
				{
					if (i % 8 == 0)
					{
						cout << endl;
					}
					cout << PieceSq[SQUARE_BLACKPAWN][i] << " ";
				}
			}
			if (s2 == "wn")
			{
				for (int i = 0;i < 64;i++)
				{
					if (i % 8 == 0)
					{
						cout << endl;
					}
					cout << PieceSq[SQUARE_WHITEKNIGHT][i] << " ";
				}
			}
			if (s2 == "bn")
			{
				for (int i = 0;i < 64;i++)
				{
					if (i % 8 == 0)
					{
						cout << endl;
					}
					cout << PieceSq[SQUARE_BLACKKNIGHT][i] << " ";
				}
			}
			if (s2 == "wb")
			{
				for (int i = 0;i < 64;i++)
				{
					if (i % 8 == 0)
					{
						cout << endl;
					}
					cout << PieceSq[SQUARE_WHITEBISHOP][i] << " ";
				}
			}
			if (s2 == "bb")
			{
				for (int i = 0;i < 64;i++)
				{
					if (i % 8 == 0)
					{
						cout << endl;
					}
					cout << PieceSq[SQUARE_BLACKBISHOP][i] << " ";
				}
			}
		}
		else if(s=="leafeval" || s=="leafevaluate")
		{
			cout << e1.LeafEval<false>() << endl;
		}
		else if(s=="trace")
		{
			cout << e1.LeafEval<true>() << endl;
		}
		else if(s=="getmovelist" || s=="movelist" || s=="getprevmoves" || s=="prevmoves")
		{
			for(int i = 0;i<board.prevmoves.size();i++)
			{
				cout << board.prevmoves.at(i).toString() << " ";
			}
			cout << endl;
		}
		else if(s=="see")
		{
			string s2;
			cin >> s2;
			int m1 = Sq2Int(s2);
			cin >> s2;
			int m2 = Sq2Int(s2);
			cout << m1 << " " << m2 << " " << Square2Piece[board.pos.Squares[m1]] << Square2Piece[board.pos.Squares[m2]] << endl;
			cout << e1.StaticExchangeEvaluation(m2,m1,Square2Piece[board.pos.Squares[m1]],board.pos.Squares[m2]) << endl;
		}
        else if(s=="help" || s=="?")
        {
            help();
        }
		else if (s == "perft")
		{
			string s2;
			cin >> s2;
			int pdepth = atoi(s2.c_str());
			cout << e1.perft(pdepth) << endl;
		}
        else if(s=="exit" || s=="quit")
        {
            exit();
            break;
        }
		else if(s=="getattacker")
		{
			int d = 0;
			cin >> d;
			cout << e1.pos.getSmallestAttacker(e1.pos.turn,d).toString() << endl;
		}
		else if(s=="depth")
		{
			int d = 0;
			cin >> d;
			MAXDEPTH = d;
		}
        else if(s=="info" || s=="information")
        {
            info();
        }
		else if(s=="restartengine")
		{
			e1 = Engine(COLOR_WHITE,board.pos);
		}
		else if(s=="debug")
		{
			DEBUG = !DEBUG;
		}
		else if(s=="uci")
		{
			UCI();
			break;
		}
		//else if(s=="xboard") //xboard doesn't work for some reason and bugs with arena so im commenting it out
		//{
			//Winboard();
			//break;
		//}
        else
        {
            cout << "Invalid command" << endl;
        }
    }
}

void Interface::info()
{
	string s;
	cin >> s;
	if(s=="os")
	{
		printBitset(board.pos.OccupiedSq);
	}
	if(s=="eos")
	{
		printBitset(e1.pos.OccupiedSq);
	}
	/*if(s=="os90")
	{
		printBitset(board.pos.OccupiedSq90);
	}
	if(s=="os45")
	{
		printBitsetDiag(board.pos.OccupiedSq45);
	}
	if(s=="os135")
	{
		printBitsetDiag(board.pos.OccupiedSq135);
	}*/
	if(s=="wp")
	{
		printBitset(board.pos.Pieces[COLOR_WHITE][PIECE_PAWN]);
	}
	if(s=="ewp")
	{
		printBitset(e1.pos.Pieces[COLOR_WHITE][PIECE_PAWN]);
	}
	if(s=="bp")
	{
		printBitset(board.pos.Pieces[COLOR_BLACK][PIECE_PAWN]);
	}
	if(s=="wn")
	{
		printBitset(board.pos.Pieces[COLOR_WHITE][PIECE_KNIGHT]);
	}
	if(s=="bn")
	{
		printBitset(board.pos.Pieces[COLOR_BLACK][PIECE_KNIGHT]);
	}
	if(s=="wb")
	{
		printBitset(board.pos.Pieces[COLOR_WHITE][PIECE_BISHOP]);
	}
	if(s=="bb")
	{
		printBitset(board.pos.Pieces[COLOR_BLACK][PIECE_BISHOP]);
	}
	if(s=="wr")
	{
		printBitset(board.pos.Pieces[COLOR_WHITE][PIECE_ROOK]);
	}
	if(s=="br")
	{
		printBitset(board.pos.Pieces[COLOR_BLACK][PIECE_ROOK]);
	}
	if(s=="wq")
	{
		printBitset(board.pos.Pieces[COLOR_WHITE][PIECE_QUEEN]);
	}
	if(s=="bq")
	{
		printBitset(board.pos.Pieces[COLOR_BLACK][PIECE_QUEEN]);
	}
	if(s=="wk")
	{
		printBitset(e1.pos.Pieces[COLOR_WHITE][PIECE_KING]);
	}
	if(s=="bk")
	{
		printBitset(e1.pos.Pieces[COLOR_BLACK][PIECE_KING]);
	}
	if(s=="epsq")
	{
		cout << Int2Sq(board.pos.epsquare) << "(" << board.pos.epsquare << ")" << endl;
	}
	if (s == "castling")
	{
		cout << e1.pos.castling[0][0] << " " << e1.pos.castling[0][1] << " " << e1.pos.castling[1][0] << " " << e1.pos.castling[1][1] << endl;
	}
	if(s=="epos")
	{
		for(int i = 0;i<64;i++)
		{
		    if(i%8==0)
			    cout << endl;
			cout << pieceStrings[e1.pos.Squares[63-i]];
		}
		cout << endl;
	}
	if (s == "threat")
	{
		cout << "Threat: " << e1.Threats[0].toString() << ", Killer: " << e1.KillerMoves[0][0].toString() << " " << e1.KillerMoves[1][0].toString() << endl;
	}
	if(s=="bb")
	{
		for(int i = 0;i<10;i++)
		{
			printBitset(getAboveSideBits(COLOR_WHITE,i));
			cout << endl;
			printBitset(getAboveSideBits(COLOR_BLACK,i));
			cout << endl;
		}
	}
	if(s=="key")
	{
		cout << board.pos.TTKey << endl;
	}
	if(s=="ekey")
	{
		cout << e1.pos.TTKey << endl;
	}
	if (s == "pkey")
	{
		cout << e1.pos.PawnKey << endl;
	}
	if(s=="rep")
	{
		cout << board.pos.isRepetition() << endl;
	}
	if (s == "piecesq")
	{
		for (int i = 0;i < 64;i++)
		{
			if (i % 8 == 0) cout << endl;
			cout << PieceSq[SQUARE_WHITEPAWN][i] << " ";
		}
	}
}

void Interface::help()
{
    cout << "List of commands:" << endl;
    cout << "disp : Displays the board" << endl;
    cout << "dispflip : Displays the board flipped" << endl;
    cout << "move : Lets you make a move" << endl;
	cout << "think : Makes computer think with given depth" << endl;
	cout << "leafeval : Computer evaluates the position directly" << endl;
	cout << "trace : Computer evaluates the position directly and gives reasoning" << endl;
    cout << "moves : Displays list of possible moves" << endl;
    cout << "movecount : Displays number of moves" << endl;
	cout << "movelist : Displays all past moves" << endl;
    cout << "help : Displays list of commands" << endl;
    cout << "exit : Quits the program" << endl;
    cout << "info : Prints debugging information" << endl;
	cout << "debug : Goes into debug mode" << endl;
}

void Interface::think()
{
	Clock c;
	c.Start();
	e1.IterativeDeepening(1000, true);
	c.Stop();
	cout << "Time taken: " << c.ElapsedMilliseconds() << endl;
}

//void Interface::thinkdepth()
//{
//	string s;
//	cin >> s;
//	int x = atoi(s.c_str());
//	Clock c;
//	c.Start();
//	e1.IterativeDeepeningDepth(x);
//	c.Stop();
//	cout << "Time taken: " << c.ElapsedMilliseconds() << endl;
//}

void Interface::makeMove()
{
    string s;
    //cout << "Enter move:" << endl;
    cin >> s;
	if(s=="null")
	{
		board.makeMove(createNullMove(board.pos.epsquare));
		e1.makeMove(createNullMove(e1.pos.epsquare));
		return;
	}
    //cout << endl;
    int from = 0;
    int to = 0;
	int sp = PIECE_NONE;

	if(s.size()<4)
	{
		cout << "Invalid format" << endl;
		return;
	}
    from+=(s.at(1)-49)*8;
    to+=(s.at(3)-49)*8;
    from+=7-(s.at(0)-97);
    to+=7-(s.at(2)-97);
	if(s.size()>4)
	{
		char spc = s.at(4);
		if(spc=='Q' || spc=='q')
		{
			sp = PIECE_QUEEN;
		}
		else if(spc=='N' || spc=='n')
		{
			sp = PIECE_KNIGHT;
		}
		else if(spc=='R' || spc=='r')
		{
			sp = PIECE_ROOK;
		}
		else if(spc=='B' || spc=='b')
		{
			sp = PIECE_BISHOP;
		}
	}

    if(from < 0 || from > 63 || to < 0 || to > 63)
    {
        cout << "Invalid format" << endl;
        return;
    }

    board.generateMoves();

    int flag = 0;
    for(unsigned int i = 0;i<board.moves.size();i++)
    {
		Move m = board.moves.at(i);
        if(m.getTo() == to && m.getFrom() == from && (m.getSpecial() == sp
			|| m.getSpecial() == PIECE_KING || m.getSpecial()==PIECE_PAWN))
        {
            flag = 1;
            if(!board.makeMove(m))
				continue;
			e1.makeMove(m);
			int status = board.getGameStatus();
			if(status==STATUS_WHITEMATED) //post the game end status
			{
				cout << "White is checkmated!" << endl;
			}
			else if(status==STATUS_BLACKMATED)
			{
				cout << "Black is checkmated!" << endl;
			}
			else if(status==STATUS_STALEMATE)
			{
				cout << "Its a stalemate!" << endl;
			}
			else if(status==STATUS_INSUFFICIENTMAT)
			{
				cout << "Its a draw due to insufficient material!" << endl;
			}
			else if(status==STATUS_3FOLDREP)
			{
				cout << "Its a draw due to 3-fold repetition!" << endl;
			}
        }
    }
    if(flag == 0)
    {
        cout << "Illegal Move!" << endl;
    }
}

void Interface::unmakeMove()
{
	board.unmakeMove();
	cout << "Restarting engine..." << endl;
	e1 = Engine(COLOR_WHITE,board.pos);
}

void Interface::getMoves()
{
    board.generateMoves();
    for(int i = 0;i<board.moves.size();i++)
    {
        Move m = board.moves.at(i);
        string s = Int2Sq(m.getFrom());
        s+=Int2Sq(m.getTo());
        cout << s << endl;
    }
}

void Interface::movecount()
{
    board.generateMoves();
    cout << (board.moves.size()) << endl;
}

void Interface::exit()
{
    cout << "Happy Coding !!";
	//getch();
}

void Interface::display(int flip)
{
    board.pos.display(flip);
}

string getStringToken(string str, char delimiter, int token)
{
	int x = 1;
	string s = "";
	for(int i = 0;i<str.size();i++)
	{
		if(str.at(i)==delimiter && i!=0 && str.at(i-1)!=delimiter)
		{
			x++;
		}
		else if(x==token && i==str.size()-1)
		{
			s += str.at(i);
			return s;
		}
		else if(x>token)
		{
			return s;
		}
		else if(x==token)
		{
			s += str.at(i);
		}
		else if(i==str.size()-1 && x<token)
		{
			return "";
		}
	}
	return "";
}

int getStringTokenPosition(string str, char delimiter, int token)
{
	int x = 1;
	for (int i = 0;i<str.size();i++)
	{
		if (str.at(i) == delimiter)
		{
			x++;
		}
		if (x == token)
		{
			return i+1;
		}
		else if (i == str.size() - 1 && x<token)
		{
			return -1;
		}
	}
	return -1;
}
