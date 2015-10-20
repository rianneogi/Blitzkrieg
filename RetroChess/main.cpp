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
// Search doesn't consider enpassant moves even though they are generated my movegen
// FIXED - Engine crashes when near mate
// FIXED - Bug where after calculating upto depth 6 then calculating upto depth 5 after e2e4 e7e5 leads to messed up engine position
// FIXED - Bug where calculation in depth 6 or above leads to messed up engine position
// FIXED - Bug where white king is being replaced by black king during engine search
// FIXED - Bug regarding sliding piece move generation
///BUGS

string ENGINENAME = "RetroChess";
string ENGINEAUTHOR = "Rian Neogi";
const int ENGINEVERSION = 61;

///BUILDS
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
	try{
    i.start();
	}catch(exception e) 
	{
		cout << e.what() << endl;
		cout << "info string " << e.what() << endl;
	}
    return 0;
}
