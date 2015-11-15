#include <iostream>
#include <conio.h>
#include <stdlib.h>
#include <time.h>
#include "Interface.h"

using namespace std;

///TODO
// Use arrays instead of vectors to improve speed
// check ep
// find a way to make PV generation faster my making lineptr a pointer instead of an object
// add enpassant values to Hash Entries
///TODO

///BUGS
// Rare crash
// FIXED - Search doesn't consider enpassant moves even though they are generated my movegen
// FIXED - Engine crashes when near mate
// FIXED - Bug where after calculating upto depth 6 then calculating upto depth 5 after e2e4 e7e5 leads to messed up engine position
// FIXED - Bug where calculation in depth 6 or above leads to messed up engine position
// FIXED - Bug where white king is being replaced by black king during engine search
// FIXED - Bug regarding sliding piece move generation
///BUGS

string ENGINENAME = "Blitzkrieg";
string ENGINEAUTHOR = "Rian Neogi";
const int ENGINEVERSION = 102;

///BUILDS
// Build 102 - 15-11-2015 - Undid Build 100
// Build 101 - 15-11-2015 - Added backward pawn and rook behind passer evaluation
// Build 100 - 14-11-2015 - New cutoff now only replaces a killer move if its score is higher
// Build 99 - 14-11-2015 - Removed PieceSq values for move sorting (Build 81)
// Build 98 - 14-11-2015 - Undid Build 97 and Build 95, changed LMR depth to 1
// Build 97 - 14-11-2015 - Does not reduce pawn moves past 6th rank
// Build 96 - 14-11-2015 - Made an optimization in LMR if-condition
// Build 95 - 13-11-2015 - Does not reduce pawn moves in endgame
// Build 94 - 13-11-2015 - Undid last change, undid change in Build 86
// Build 93 - 13-11-2015 - Reverted PieceSq values to what they were in Build 87
// Build 92 - 13-11-2015 - Optimized a bit more
// Build 91 - 13-11-2015 - Optimized a bit by reducing number of arguments passed
// Build 90 - 13-11-2015 - Improved zugzwang check for nullmove
// Build 89 - 13-11-2015 - Fixed a bug with passed pawn evaluation
// Build 88 - 13-11-2015 - Reduced Queen Piece Square values a bit
// Build 87 - 13-11-2015 - Balanced out Piece Square values a bit
// Build 86 - 12-11-2015 - Limited null move to only once per variation
// Build 85 - 12-11-2015 - Changed LMR depth to max(3,d-2)
// Build 84 - 12-11-2015 - Changed LMR depth to scale with movenumber
// Build 83 - 10-11-2015 - Removed small forward razoring
// Build 82 - 10-11-2015 - Increased latemove reduction from 1 to 2
// Build 81 - 10-11-2015 - Improved move sorting by adding PieceSq value difference to move score
// Build 80 - 09-11-2015 - Undid last change, made a few optimizations
// Build 79 - 01-11-2015 - Removed Piece Square Bonus
// Build 78 - 31-10-2015 - Undid last change, reduced attack weights of bishop and knight by 1
// Build 77 - 31-10-2015 - getRank is now a shift instead of a table lookup
// Build 76 - 31-10-2015 - Added lategame scaling to evaluation
// Build 75 - 29-10-2015 - Improved endgame draw evaluation
// Build 74 - 29-10-2015 - Fixed a bug with evaluation, reduced king safety parameters a bit
// Build 73 - 27-10-2015 - Now does not do null move pruning and razoring at PV nodes, improved blocked pawn evaluation
// Build 72 - 26-10-2015 - Changed PV back to vector
// Build 71 - 25-10-2015 - Fixed endgame draw evaluation
// Build 70 - 25-10-2015 - Improved speed a bit, added mobility bonus for queen and piece adjustment for number of enemy pawns
// Build 69 - 24-10-2015 - Added null move extensions and singular extensions, increase AttackWeights of minor pieces and rook by 1
// Build 68 - 23-10-2015 - Fixed a bug regarding TTKey changing with null moves
// Build 67 - 23-10-2015 - Now does razoring and pruning before null move search
// Build 66 - 23-10-2015 - A Hash hit will now return the probe even if best move is null
// Build 65 - 22-10-2015 - Made Principal Variation an array instead of vector
// Build 64 - 22-10-2015 - Made everything fail soft in AlphaBeta function, readded razoring, removed futility capture pruning
// Build 63 - 21-10-2015 - Fixed a bug with forward pruning, reduced reduction depth in LMR, removed negative history bonus for quiet moves
// Build 62 - 20-10-2015 - LMR now happens even if alpha was raised and current move count is greater than or equal to 6
//							, now uses probe as leafeval(in margin checking) if possible
// Build 61 - 19-10-2015 - Improved futility threshold
// Build 60 - 19-10-2015 - Changed everything to fail soft, improved beta futility pruning and changed LMR to be constant 2
// Build 59 - 19-10-2015 - Added forward pruning w.r.t. beta
// Build 58 - 19-10-2015 - Changed aspiration search algorithm, undid last change
// Build 57 - 19-10-2015 - Increase attack weights alot
// Build 56 - 19-10-2015 - Reduced attack weights a bit
// Build 55 - 19-10-2015 - Readded Killer Moves
// Build 54 - 19-10-2015 - Removed Killer Moves, fixed ColoredSquares and added bonus eval for outposts protected by a pawn
// Build 53 - 18-10-2015 - Added time management
// Build 52 - 18-10-2015 - Now evaluates outposts depending on rank, changed king field to include squares adj. to king also
// Build 51 - 17-10-2015 - Move ordering now orders Killer Move 1 above Killer Move 2 instead of ordering both equally
// Build 50 - 17-10-2015 - Undid last change, reduced latemove reduction a bit
// Build 49 - 17-10-2015 - Undid last change, increased latemove reduction base depth from 1 to 2
// Build 48 - 17-10-2015 - Now does a depth reduction in pvs even if alpha isnt raised
// Build 47 - 16-10-2015 - Increased max depth for futility and large forward pruning to 4
// Build 46 - 16-10-2015 - Added futility pruning for depth==1 and removed the SEE evaluations vector optimization
// Build 45 - 16-10-2015 - Removed futility pruning in favor of small forward pruning and improved evaluation for minor piece outposts
// Build 44 - 16-10-2015 - Added forward pruning
// Build 43 - 15-10-2015 - Now stores already-calculated SEE evaluations in a vector so we dont need to recalculate
// Build 42 - 14-10-2015 - Improved move sorting for captures
// Build 41 - 14-10-2015 - Fixed a bug regarding PV generation
// Build 40 - 14-10-2015 - Added latemove reduction to pvs, moderate increase in search depth
// Build 39 - 14-10-2015 - Made modifications to reduction depth in latemove reduction
// Build 38 - 09-07-2015 - Added better filters for hash move
// Build 37 - 08-11-2014 - Optimized a little bit more
// Build 36 - 08-11-2014 - Optimized a little bit
// Build 35 - 08-11-2014 - Fixed 3-fold repetition bug by making evaluation return 0 once 2 similar positions occur(instead of 3)
// Build 34 - 06-04-2014 - Added bishop pawn same color adjustment
// Build 33 - 19-01-2014 - Fixed SEE
// Build 32 - 04-01-2014 - Added epsquare to TTKey
// Build 31 - 29-12-2013 - Fixed 3-fold repetition check
// Build 30 - 28-12-2013 - Added Castling to TTKey
// Build 29 - 28-12-2013 - Reduced queen out early penalty, improved Qsearch by increase margin for big delta pruning, added 3-fold repetition check
// Build 28 - Improved eval, added huge penalty for bringing the queen out early, now beats Faile consistently
// Build 27 - Slighty improved eval, now gives bonus to connected rooks
// Build 26 - Now checks for mate/stalemate after searching all moves rather than before(this allows us to generate only capture moves if necessary)
//                approx. 50% improvement in nps
// Build 25 - GenerateMoves() now generates pseudo-legal moves and move legality checking happens while making the move
//                fixed a bug where ply wasnt incrementing in quiescence (a massive approx. 100% improvement in nps)
// Build 24 - Made SEE faster by making it iterative (approx. 10% improvement in nps)
// Build 23 - Added mobility factor in evaluation and improved king safety evaluation
// Build 22 - Disabled lazy eval and basic move ordering
// Build 21 - Fixed a few bugs with move ordering that caused some moves to not be considered at all
// Build 20 - Fixed en passant, fixed some problems with TT(ep and castle moves were not properly updating the key)
// Build 19 - Improved move ordering with changes to SEE
// Build 18 - Improved king safety evaluation
// Build 17 - Fixed some bugs with leaf evaluation
// Build 16 - Minor performance improvement
// Build 15 - Increased TT size to 128 MB
// Build 14 - Improved king evaluation, added outpost evaluation, added piece attack evaluation
// Build 13 - Added King Safety Evaluation, engine plays much more safer now.
// Build 12 - Increased Endgame Material, engine should now take its king out in endgames.
// Build 11 - Added Static Exchange Evaluation for move sorting
// Build 10 - Added Hash Table Heuristic for move sorting
// Build 9 - Added basic lazy eval, improved evaluation
// Build 8 - Made sorting more retroactive, now sorts every time a child node is searched instead of sorting only once at the start of the search.
//             This is a big improvement over the previous builds, it goes a full extra depth at start position in 10 seconds.
// Build 7 - Readded basic move sort, added killer move heuristic and reimplemented PV search first
// Build 6 - Added killer move heuristic and PV is now searched first
// Build 5 - Removed basic move sort, moves are sorted only by history heuristic
// Build 4 - Engine no longer searches PV first. This fixed a number of bugs. I'll add a proper implementation of PV later.
// Build 3 - Uses mirror instead of turn180 in eval
// Build 2 - Does null move searching in pv searches and fails
// Build 1 - Original Build
///BUILDS

///HISTORY
// 26-11-2013 - Build 9 : Added Lazy Eval and Killer Move Heuristic
// 24-11-2013 - Divided Engine.cpp into three parts:- Search.cpp, Quiescence.cpp and Evaluation.cpp
// 21-11-2013 - Added UCI support
// 15-09-2013 - Added razoring
// 19-07-2013 - Added Stand Pat Quiescence Search
// 14-07-2013 - Massive increase in speed, depth 6 gives d7d5 after e2e4 in ~2.79 seconds
// 11-07-2013 - Added turn to the TTKey
// 09-07-2013 - Program now searches Principal Variation first
// 08-07-2013 - Program now prints Principal Variation
// 30-06-2013 - Added Principal Variation Search
// 28-06-2013 - Added Relative History Heuristic and Late Move Reductions
// 28-06-2013 - Added extended bounds Aspiration Search
// 27-06-2013 - Added Aspiration Search and Null Move Pruning
// 07-05-2013 - Improved move ordering
// 07-05-2013 - Improved move generation speed
// 07-05-2013 - Added Endgame Evaluation
// 07-05-2013 - Added Transposition Table
// 23-04-2013 - Improved leaf evaluation, now it takes into account pawn structure and rook mobility
// 22-04-2013 - Improved move ordering
// 21-04-2013 - Added move ordering
// 21-04-2013 - Added quiescence search
// 21-04-2013 - Improved memory management
// 21-04-2013 - Improved speed of generateMoves() and forceMove()
// 19-04-2013 - Engine now does not take alot of time to evaluate beyond depth 2
// 19-04-2013 - Engine now works! *Celebration* Although it takes a lot of time to evaluate beyond depth 2
// 18-04-2013 - Added conditions for check, checkmate and stalemate
// 17-04-2013 - Added unmakeMove()
// 17-04-2013 - Basic Move generation and enpassant done
// 16-04-2013 - Made a couple of bitops functions inline
// 27-11-2012 - Made a move list to trace back with unmakeMove()
//   -Doesn't work because when I regenerate moves after coming back to root position, already analysed memory is lost
// Original BetaChess with only prevmove to trace back for unmakeMove()
//   -Doesn't work because after using unmakeMove() to go back, you don't know the previous move
///HISTORY

string convertMoveNotation(string move, const Position& pos) //converts move notation (eg. Nf3 into g1f3) 
{
	vector<Move> moves;
	pos.generateMoves(moves);

	if (move.at(move.size() - 1) == ';')
	{
		move = move.substr(0, move.size() - 1);
	}

	if (move.at(move.size() - 1) == '+' || move.at(move.size() - 1) == '#')
	{
		move = move.substr(0, move.size() - 1);
	}

	for (int i = 0;i < moves.size();i++)
	{
		Move m = moves.at(i);
		string s = m.toString();
		if (Int2Sq(m.getTo()) == move.substr(move.size() - 2))
		{
			if (pieceStrings[m.getMovingPiece() + 1].at(0) == move.at(0) && move.size()>2) //piece
			{
				if (move.size() >= 4 && move.at(1) != 'x')
				{
					if (move.at(1) >= '1' && move.at(1) <= '8')
					{
						int rank = move.at(1) - 49;
						if (m.getFrom() >= rank && m.getFrom() <= rank + 7)
						{
							return s;
						}
					}
					else
					{
						int file = move.at(1) - 97;
						if (m.getFrom() % 8 == (7 - file))
						{
							return s;
						}
					}
				}
				else
				{
					return s;
				}
			}
			else if (move.size() == 2 || (move.size()==4 && move.at(1)=='x'))
			{
				return s;
			}
		}
	}
	return "error";
}

void testpositions(string test, int fenformat, int onlyfailed, int time, Engine& e)
{
	vector<int> oldfailed;
	vector<int> newfailed;

	fstream f("Test Suites//"+test+".txt", ios::in);
	if (!f.is_open())
	{
		cout << "Cant open test suite" << endl;
		return;
	}
	string s;

	if (onlyfailed)
	{
		fstream f3("Test Suites//" + test + "_results.txt", ios::in);
		if (!f3.is_open())
		{
			cout << "Cant open test suite results" << endl;
			return;
		}
		getline(f3, s);
		getline(f3, s);
		int i = 1;
		while (true)
		{
			string s2 = getStringToken(s, ' ', i);
			if (s2 == "") break;
			oldfailed.push_back(atoi(s2.c_str()));
			//cout << s2 << endl;
			i++;
		}
		f3.close();
	}

	int oldfailedcount = oldfailed.size();

	int error = 0;
	int count = 1;
	int passed = 0;
	while (!f.eof())
	{
		getline(f, s);
		if (onlyfailed && (oldfailed.size() == 0 || count != oldfailed.at(0)))
		{
			count++;
			continue;
		}
		e.pos.loadFromFEN(s);
		Move m = e.IterativeDeepening(time, false);

		string bm;
		if (fenformat == 1)
		{
			bm = getStringToken(s, ' ', 8);
		}
		else
		{
			bm = getStringToken(s, ' ', 6);
		}

		string bmx = convertMoveNotation(bm, e.pos);
		if (bmx == "error")
		{
			cout << "ERROR" << endl;
			error++;
		}

		if (m.toString() == bmx)
		{
			cout << "test " << count << ": success, suggested move : " << m.toString() << ", best move : " << bmx << endl;
			passed++;
		}
		else
		{
			cout << "test " << count << ": failed, suggested move: " << m.toString() << ", best move: " << bmx << endl;
			newfailed.push_back(count);
		}
		cout << endl;

		if (onlyfailed) oldfailed.erase(oldfailed.begin());
		count++;
	}

	f.close();

	fstream f2("Test Suites//"+test+"_results.txt", ios::out | ios::trunc);
	if (!f2.is_open())
	{
		cout << "Cant open test suite results" << endl;
		return;
	}

	if(onlyfailed)
		f2 << "Total passed: " << passed << "/" << oldfailedcount << " ";
	else
		f2 << "Total passed: " << passed << "/" << (count - 1) << " ";
	f2 << ", Time per position: " << time << " ms" << endl;

	if (error)
	{
		cout << error << " errors have occured" << endl;
	}
	for (int i = 0;i < newfailed.size();i++)
	{
		f2 << newfailed.at(i);
		if (i != newfailed.size() - 1)
			f2 << " ";
	}

	f2.close();
}

int main(int argc, char* args[])
{
	srand(time(0));
	//loadFromLua("Personalities\\Wierd.lua");
    datainit();
	searchinit();
	evalinit();
	TTinit();
    cout << "Initialization done" << endl;

	Interface i = Interface();
	
	//testpositions("wac", 0, 0, 1000, i.e1);
    
	try{
    i.start();
	}catch(exception e) 
	{
		cout << e.what() << endl;
		cout << "info string " << e.what() << endl;
	}
    return 0;
}
