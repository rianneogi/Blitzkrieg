#ifndef ENGINE_H_INCLUDED
#define ENGINE_H_INCLUDED

//extern "C"
//{
//	#include "lua.h"
//	#include "lualib.h"
//	#include "lauxlib.h"
//}

#include "Position.h"
#include "Clock.h"

extern int PieceMaterial[7];
extern int PieceSq[13][64];
extern int PieceSqEG[13][64];
extern int PieceSqValues[6][64];
extern int SquareValues[64];

enum EvalConstants{CONS_INF = 20000,CONS_NEGINF = -20000, CONS_DRAW = 0, CONS_MATED = -10000};
enum SortPhases{SORTPHASE_NONE,SORTPHASE_PV,SORTPHASE_HASH,SORTPHASE_GOODCAP,SORTPHASE_KILLER,SORTPHASE_HISTORY,SORTPHASE_BADCAP};

extern int EndgameMaterial;
extern int MAXTIME;
extern int MAXDEPTH;
extern const int CheckupNodeCount;

class Engine
{
    public:
    Position pos;

	int myColor;
	vector<Move> PrincipalVariation;
	//Move PrincipalVariation[128];
	int PvSize;
	int PvPly;
	Move KillerMoves[2][100];
	unsigned int HistoryScores[64][64];
	Move Threats[100];
	bool incheck[100];
	//int KillerScores[2][100];
	int ply;
	int SelectiveDepth;
	int SortPhase;

	//stats
	unsigned long long nodes;
	unsigned long long prunednodes;
	unsigned long long futilitynodes;
	unsigned long long betacutoff_counter;
	unsigned long long firstbetacutoffcount;
	unsigned long long betacutoff_sum;
	unsigned long long alpha_counter;
	unsigned long long alphalast_sum;
	unsigned long long alphafirst_sum;
	unsigned long long tthitcount;
	unsigned long long latemoveresearch;
	unsigned long long pvresearch;

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
	Move IterativeDeepening(int movetime, bool print);
	int AlphaBeta(int depth,int alpha,int beta,vector<Move>* variation,bool cannull,bool dopv);
	Move getHighestScoringMove(vector<Move>& moves,int currentmove);
	unsigned long long getMoveScore(const Move& m);
	void generateCaptureScores(vector<Move>& moves, vector<int>& scores);
	void checkup();
	void setKiller(Move m,int depth,int score);

	unsigned long long perft(int depth);

	//Quiescence.cpp
	int QuiescenceSearchStandPat(int alpha,int beta);
	int StaticExchangeEvaluation(int to, int from,int movpiece,int capt);

	//Evaluation.cpp
	template<bool Trace> int LeafEval();
	template<int Color> int getBoardMaterial();

	void prepareSearch();
	void initialize();
};

void evalinit();
void searchinit();
int getNPS(int nodes,int milliseconds);
template int Engine::LeafEval<true>();
template int Engine::LeafEval<false>();
template int Engine::getBoardMaterial<COLOR_WHITE>();
template int Engine::getBoardMaterial<COLOR_BLACK>();

//int loadFromLua(std::string path);
#endif // ENGINE_H_INCLUDED
