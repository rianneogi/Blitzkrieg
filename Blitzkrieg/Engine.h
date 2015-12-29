#ifndef ENGINE_H_INCLUDED
#define ENGINE_H_INCLUDED

#include "Clock.h"
#include "Score.h"

#define BLITZKRIEG_STATS
#define BLITZKRIEG_DEBUG

extern int PieceMaterial[7];
extern int PieceSq[13][64];
extern int PieceSqEG[13][64];
extern int PieceSqValues[6][64];
extern int SquareValues[64];

enum EvalConstants{CONS_INF = 20000,CONS_NEGINF = -20000, CONS_DRAW = 0, CONS_MATED = -10000};
enum SortPhases{SORTPHASE_NONE,SORTPHASE_PV,SORTPHASE_HASH,SORTPHASE_GOODCAP,SORTPHASE_KILLER,SORTPHASE_HISTORY,SORTPHASE_BADCAP};
enum TimeMode{MODE_DEFAULT,MODE_MOVETIME,MODE_INF};

extern const int EndgameMaterial;
extern int MAXDEPTH;
extern const unsigned long long CheckupNodeCount;

class Engine
{
    public:
    Position pos;

	int timeMode;

	int myColor;
	vector<Move> PrincipalVariation;
	Move currentVariation[128];
	int PvSize;
	int PvPly;
	Move KillerMoves[2][100];
	Move CounterMoves[6][64][2];
	Move FollowupMoves[6][64][2];
	long HistoryScores[64][64];
	Move Threats[100];
	bool incheck[100];
	int Evaluation[100];
	//int KillerScores[2][100];
	int ply;
	int SelectiveDepth;
	int SortPhase;
	uint64_t AllocatedTime;

	//stats
	uint64_t nodes;
	uint64_t prunednodes;
	uint64_t futilitynodes;
	uint64_t betacutoff_counter;
	uint64_t firstbetacutoffcount;
	uint64_t betacutoff_sum;
	uint64_t alpha_counter;
	uint64_t alphalast_sum;
	uint64_t alphafirst_sum;
	uint64_t tthitcount;
	uint64_t latemoveresearch;
	uint64_t pvresearch;
	uint64_t aspirationresets;
	uint64_t nullcutoffs;
	//uint64_t badavoidnull;

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
	Move IterativeDeepening(int mode, uint64_t wtime, uint64_t btime, uint64_t winc, uint64_t binc, bool print);
	int AlphaBeta(int depth,int alpha,int beta,vector<Move>* variation,bool cannull,bool dopv);
	Move getHighestScoringMove(vector<Move>& moves,int currentmove);
	long long getMoveScore(const Move& m);
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
	bool isDraw();

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
