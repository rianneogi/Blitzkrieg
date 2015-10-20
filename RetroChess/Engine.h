#ifndef ENGINE_H_INCLUDED
#define ENGINE_H_INCLUDED

//extern "C"
//{
//	#include "lua.h"
//	#include "lualib.h"
//	#include "lauxlib.h"
//}

#include "Position.h"
#include "TranspositionTable.h"
#include "Clock.h"

extern int PieceMaterial[7];

enum EvalConstants{CONS_INF = 20000,CONS_NEGINF = -20000, CONS_DRAW = 0, CONS_MATED = -10000};

extern int EndgameMaterial;
extern int MAXTIME;
extern int MAXDEPTH;
extern int CheckupNodeCount;

class Engine
{
    public:
    Position pos;
    Move historymove;

	int myColor;
	vector<Move> PrincipalVariation;
	Move KillerMoves[2][100];
	int ply;

	int nodes;
	int prunednodes;
	int futilitynodes;
	int betacutoff_counter;
	int betacutoff_sum;
	int alphaimprovement_counter;
	int alphaimprovement_sum;

	Clock timer;

	Clock evaltime;
	Clock sorttime;
	Clock movegentime;
	Clock quisctime;

	//Engine.cpp
    Engine();
	Engine(int col);
    Engine(int col,Position const& p);
	Engine(int col,Position const& p,Move const& prevmove);
    ~Engine();
	void makeMove(int n);
	void makeMove(Move const& m);
	void unmakeMove(Move const& m);
	
	//Search.cpp
	Move IterativeDeepening(int movetime);
	int think(int depth,int alpha,int beta,vector<Move>* variation);
	int AlphaBeta(int depth,int alpha,int beta,Move lastmove,vector<Move>* variation,bool cannull,bool dopv);
	void movesort(vector<Move>& moves,int depth);
	Move getHighestScoringMove(vector<Move>& moves,int currentmove);
	unsigned long long getMoveScore(const Move& m);
	void generateCaptureScores(vector<Move>& moves, vector<int>& scores);
	//void ageHistoryTable();
	void checkup();
	void setKiller(Move m,int depth);

	//Quiescence.cpp
	//int QuiescenceSearch(int alpha,int beta,Move lastmove);
	int QuiescenceSearchStandPat(int alpha,int beta,Move lastmove);
	int StaticExchangeEvaluation(int to, int from,int movpiece,int capt);
	int StaticExchangeEvaluation2(Move m);
	int StaticExchangeEvaluation(Move m);
	int StaticExchangeEvaluation(int square,int side);

	//Evaluation.cpp
	int LeafEval(int alpha,int beta);
	int EvalKingSafety(int turn,bool isEG);
	int FastEval();
	int getBoardMaterial();
	int Trace(int alpha,int beta);
	//int loadFromLua(std::string path);
};

void evalinit();
void searchinit();
int getNPS(int nodes,int milliseconds);
//int loadFromLua(std::string path);
#endif // ENGINE_H_INCLUDED
