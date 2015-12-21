#include "Engine.h"

#define S(x,y) Score(x,y)

string PlayerStrings[2] = { "White","Black" };

int ColorFactor[2] = {1,-1};

int PieceMaterial[7] = {100,325,335,500,975,-CONS_MATED,0};
int MaterialValues[13] = {0,100,325,335,500,975,-CONS_MATED,-100,-325,-335,-500,-975,CONS_MATED};
int TotalMaterial = (8*PieceMaterial[0] + 2*PieceMaterial[1] + 2*PieceMaterial[2] + 2*PieceMaterial[3] + PieceMaterial[4]);
int TotalMaterialBothSides = TotalMaterial * 2;

int EndgameMaterial = 3800;

int LazyEval1 = 400;

//int CenterSquareBonus = 4;
//int CenterBorderSquareBonus = 2;
//int EnemyTerritorySquareBonus = 3;

Scale KingSafetyFactor(1, 0);
Scale PawnStructureFactor(0.5, 0.7);
Scale PassedPawnFactor(1, 1.5);
Scale MobilityFactor(1, 1);
Scale OutpostFactor(1, 0.5);

Scale BishopFactor(1, 1);
Scale KnightFactor(1, 1);
Scale RookFactor(1, 1);
Scale QueenFactor(1, 1);

//Mobility
Score BishopMobility[16] = { -24, -8,  0,  4,  8,  8,  12,  12,  16,  16, 20, 20, 24, 28, 32 };
Score RookMobility[16] = { -16,-8, 0, 4, 4, 4, 8, 8, 8, 12, 12, 12, 16, 16, 16 };
Score KnightMobility[9] = { -12, -8,  0,  4,  8, 10, 12, 14, 16 };
Score QueenMobility[32] = { -10,-9,-8,-7,-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21 };

//Outposts
Score KnightOutpostBonus[64] = { 0, 0, 0, 0, 0, 0, 0, 0,
								 0, 0, 0, 0, 0, 0, 0, 0,
								 0, 0, 0, 5, 5, 0, 0, 0,
								 0, 0, 5,10,10, 5, 0, 0,
								 0, 5,10,15,15,10, 5, 0,
								 5,10,15,25,25,15,10, 5,
								 0, 0, 0, 0, 0, 0, 0, 0,
								 0, 0, 0, 0, 0, 0, 0, 0}; //non-negative values always(code will bug otherwise)

Score BishopOutpostBonus[64] = { 0, 0, 0, 0, 0, 0, 0, 0,
								 0, 0, 0, 0, 0, 0, 0, 0, 
								 0, 0, 0, 4, 4, 0, 0, 0, 
								 0, 0, 4, 6, 6, 4, 0, 0, 
								 0, 4, 6, 8, 8, 6, 4, 0, 
								 4, 6, 8,10,10, 8, 6, 4, 
								 0, 0, 0, 0, 0, 0, 0, 0, 
								 0, 0, 0, 0, 0, 0, 0, 0}; //non-negative values always(code will bug otherwise)

//Knights
Score KnightPairBonus = S(-5, -10);
Score KnightPawnAdj[9] = { -20,-16,-12, -8, -4,  0,  4,  8, 12 };
Score KnightOppPawnAdj[9] = { -20,-16,-12,-8,-4,0,4,8,12 };

//Bishops
Score BishopPairBonus = S(45, 55);
Score BishopPawnAdj[9] = { 0,0,0,0,0,0,0,0,0 };
Score BishopOppPawnAdj[9] = { 0,0,0,0,0,0,0,0,0 };
Score BishopPawnSameColor[9] = { 15,12,9,6,3,0,-3,-6,-9 };
Score BishopOppPawnSameColor[9] = { -9,-6,-3,0,3,6,9,12,15 };

//Rooks
Score RookPairBonus = S(10, 40);
Score RookPawnAdj[9] = { 15,12, 9, 6, 3, 0,-3,-6,-9 };
Score RookOppPawnAdj[9] = { -8,-4, 0, 4, 8,12,16,20,24 };
Score RookHalfOpenBonus[8] = { 5,5,8,10,10,8,5,5 };
Score RookOpenBonus[8] = { 10,10,15,20,20,15,10,10 };
Score RookConnectedBonus = S(5, 15);

//Queen
Score QueenOutEarlyPenalty = S(4,0); //penalty for queens not on back rank for every minor on back rank

//extra bonus based on tension between minor pieces
Score KnightAttacksBishopBonus = S(6, 12);
Score BishopAttacksKnightBonus = S(2, 6);

//King Safety
Score PawnShield1Bonus = 20;
Score PawnShield2Bonus = 10;
Score BishopShieldBonus = 15;
Score KingOnHalfOpenFilePenalty = 15; //penalty for king being on half open files
Score KingOnOpenFilePenalty = 25; //penalty for king being on open files
Score KingAdjHalfOpenFilePenalty = 10; //penalty for king being adjacent to half open files
Score KingAdjOpenFilePenalty = 15; //penalty for king being adjacent to open files
Score KingOnRookFilePenalty = 10; //penalty for king being on an opponent semiopen file with a rook on it
Score KingAdjRookFilePenalty = 5; //penalty for king being adjacent an opponent semiopen file with a rook on it
Score KingBetweenRooksPenalty = 10; //not implemented

const int AttackWeights[6] = {1,2,2,3,4,0};
Score SafetyTable[100] = {
    0,  0,   1,   2,   3,   5,   7,   9,  12,  15,
  18,  22,  26,  30,  35,  39,  44,  50,  56,  62,
  68,  75,  82,  85,  89,  97, 105, 113, 122, 131,
 140, 150, 169, 180, 191, 202, 213, 225, 237, 248,
 260, 272, 283, 295, 307, 319, 330, 342, 354, 366,
 377, 389, 401, 412, 424, 436, 448, 459, 471, 483,
 494, 500, 500, 500, 500, 500, 500, 500, 500, 500,
 500, 500, 500, 500, 500, 500, 500, 500, 500, 500,
 500, 500, 500, 500, 500, 500, 500, 500, 500, 500,
 500, 500, 500, 500, 500, 500, 500, 500, 500, 500
};

//Pawn Structure
Score PawnsE4D4Bonus = S(15, 0);
Score PawnsC4D4Bonus = S(10, 0);
Score NoPawnsPenalty = S(32,32);
Score PawnPressureBonus[9] = { 0,3,7,12,18,25,33,42,52 };
Score DoubledPawnPenalty[8] = { 6,8,10,15,15,10,8,6 };
Score IsolatedPawnPenalty[8] = { 9,12,18,30,30,18,12,9 };
Score BackwardPawnPenalty[8] = { 6,8,10,12,12,10,8,6 };
Score BlockedPawnPenalty[64] = {   0,  0,  0,  0,  0,  0,  0,  0,
	                             1,  1, 10, 16, 16, 10,  1,  1,
								 0,  0,  0,  4,  4,  0,  0,  0,
								 0,  0,  0,  0,  0,  0,  0,  0,
								 2,  2,  2,  2,  2,  2,  2,  2,
								10, 10, 10, 10, 10, 10, 10, 10,
								20, 20, 20, 20, 20, 20, 20, 20,
								 0,  0,  0,  0,  0,  0,  0,  0};

//Passed Pawns
Score PassedPawnBonus[64] = {  0,  0,  0,  0,  0,  0,  0,  0, //non-negative values always(code will bug otherwise)
						     5, 10, 10, 10, 10, 10, 10,  5,
						    10, 20, 20, 20, 20, 20, 20, 10,
                            20, 40, 40, 40, 40, 40, 40, 20,
                            30, 60, 60, 60, 60, 60, 60, 30,
							40, 80, 80, 80, 80, 80, 80, 40,
						    90,120,120,120,120,120,120, 90,
						   200,200,200,200,200,200,200,200};
Score RookBehindPassedPawnBonus = S(10, 20);
Score OppRookBehindPassedPawnPenalty = S(5, 15);

int PieceSq[13][64];
int PieceSqEG[13][64];

int SquareValues[64] = { 1, 1, 1, 1, 1, 1, 1, 1,
						 1, 1, 1, 1, 1, 1, 1, 1,
						 1, 1, 2, 2, 2, 2, 1, 1,
						 1, 1, 2, 4, 4, 2, 1, 1,
						 2, 2, 4, 8, 8, 4, 2, 2,
						 2, 2, 4, 4, 4, 4, 2, 2,
						 2, 2, 2, 2, 2, 2, 2, 2,
						 2, 2, 2, 2, 2, 2, 2, 2 };

int PieceSqValues[6][64] =
{
	//{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //empty
	{  0,  0,  0,  0,  0,  0,  0,  0, //pawn
	 -12, -4, -4,  0,  0, -4, -4,-12,
	 -12, -4,  8,  8,  8,  8, -4,-12,
	  -8,  8, 24, 32, 32, 24,  8, -8,
	  -8,  8,  8, 16, 16,  8,  8, -8,
	  -8,  0,  0,  0,  0,  0,  0, -8,
	  -8,  0,  0,  0,  0,  0,  0, -8,
	   0,  0,  0,  0,  0,  0,  0,  0},

	{  2,  4,  6,  6,  6,  6,  4,  2, //knight, non-negative
	   4,  6, 10, 12, 12, 10,  6,  4,
	   6, 14, 20, 20, 20, 20, 14,  6,
	  10, 18, 22, 20, 20, 22, 18, 10,
	  10, 16, 18, 18, 18, 18, 16, 10,
	  10, 18, 24, 20, 20, 24, 18, 10,
	   8, 12, 20, 22, 22, 20, 12,  8,
	   4,  8, 10, 12, 12, 10,  8,  4},

	{ 16, 10,  6,  6,  6,  6, 10, 16, //bishop, non-negative
	  14, 16, 12, 10, 10, 12, 16, 14,
	  10, 14, 18, 14, 14, 18, 14, 10,
	  16, 12, 18, 22, 22, 18, 12, 16,
	  16, 12, 14, 18, 18, 14, 12, 16,
	  16, 14, 18, 18, 18, 18, 14, 16,
	  10, 18, 16, 14, 14, 16, 18, 10,
	  14, 14, 10, 10, 10, 10, 14, 14},

	{  8,  8, 10, 12, 12, 10,  8,  8, //rook
	   8,  8, 10, 12, 12, 10,  8,  8,
	   8,  8, 10, 12, 12, 10,  8,  8,
	  10, 10, 12, 12, 12, 12, 10, 10,
	  16, 16, 16, 16, 16, 16, 16, 16,
	  12, 12, 14, 16, 16, 14, 12, 12,
	  18, 18, 20, 22, 22, 20, 18, 18,
	  10, 10, 12, 14, 14, 12, 10, 10},

	{  8,  6,  8,  8,  8,  8,  6,  8, //queen
	   8,  8,  8, 10, 10,  8,  8,  8,
	   8,  8, 10, 10, 10, 10,  8,  8,
	   8, 10, 12, 12, 12, 12, 10,  8,
	  10, 12, 12, 14, 14, 12, 12, 10,
	   8, 10, 12, 14, 14, 12, 10,  8,
	   8, 10, 10, 12, 12, 10, 10,  8,
	  10,  8, 10, 10, 10, 10,  8, 10},

	{ 20, 20,-10,-20,-20, 10, 20, 20, //king
	  10, 20,  0,-30,-30,  0, 15, 10,
	   0,-10,-20,-40,-40,-20,-10,  0,
	 -10,-20,-40,-50,-50,-40,-20,-10,
	 -60,-60,-60,-60,-60,-60,-60,-60,
	 -80,-80,-80,-80,-80,-80,-80,-80,
	 -80,-80,-80,-80,-80,-80,-80,-80,
	 -80,-80,-80,-80,-80,-80,-80,-80}
};

int PieceSqValuesEG[6][64] = 
{
	//{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //empty
	{  0,  0,  0,  0,  0,  0,  0,  0, //pawn
	   0,  0,  0,  0,  0,  0,  0,  0,
	   1,  2,  4, 10, 10,  4,  2,  1,
	   2,  4,  8, 20, 20,  8,  4,  2,
	   3,  6, 12, 30, 30, 12,  6,  3,
	   4,  8, 16, 35, 35, 16,  8,  4,
	   5, 10, 20, 40, 40, 20, 10,  5,
	   0,  0,  0,  0,  0,  0,  0,  0},

	{ 
	2,  4,  6,  6,  6,  6,  4,  2, //knight
	4,  6, 10, 12, 12, 10,  6,  4,
	6, 14, 20, 20, 20, 20, 14,  6,
	10, 18, 22, 20, 20, 22, 18, 10,
	10, 16, 18, 18, 18, 18, 16, 10,
	10, 18, 24, 20, 20, 24, 18, 10,
	8, 12, 20, 22, 22, 20, 12,  8,
	4,  8, 10, 12, 12, 10,  8,  4 },

	{
	16, 10,  6,  6,  6,  6, 10, 16, //bishop
	14, 16, 12, 10, 10, 12, 16, 14,
	10, 14, 18, 14, 14, 18, 14, 10,
	16, 12, 18, 22, 22, 18, 12, 16,
	16, 12, 14, 18, 18, 14, 12, 16,
	16, 14, 18, 18, 18, 18, 14, 16,
	10, 18, 16, 14, 14, 16, 18, 10,
	14, 14, 10, 10, 10, 10, 14, 14 },

	{ 
	8,  8, 10, 12, 12, 10,  8,  8, //rook
	8,  8, 10, 12, 12, 10,  8,  8,
	8,  8, 10, 12, 12, 10,  8,  8,
	10, 10, 12, 12, 12, 12, 10, 10,
	16, 16, 16, 16, 16, 16, 16, 16,
	12, 12, 14, 16, 16, 14, 12, 12,
	18, 18, 20, 22, 22, 20, 18, 18,
	10, 10, 12, 14, 14, 12, 10, 10 },

	{
	8,  6,  8,  8,  8,  8,  6,  8, //queen
	8,  8,  8, 10, 10,  8,  8,  8,
	8,  8, 10, 10, 10, 10,  8,  8,
	8, 10, 12, 12, 12, 12, 10,  8,
	10, 12, 12, 14, 14, 12, 12, 10,
	8, 10, 12, 14, 14, 12, 10,  8,
	8, 10, 10, 12, 12, 10, 10,  8,
	10,  8, 10, 10, 10, 10,  8, 10 },

	{-40,-30,-20,-10,-10,-20,-30,-40, //king
	 -30,  0,  0,  0,  0,  0,  0,-30,
	 -20,  0, 10, 20, 20, 10,  0,-20,
	 -10,  0, 20, 40, 40, 20,  0,-10,
	 -10,  0, 20, 40, 40, 20,  0,-10,
	 -20,  0, 10, 20, 20, 10,  0,-20,
	 -30,  0,  0,  0,  0,  0,  0,-30,
	 -40,-30,-20,-10,-10,-20,-30,-40}
};

//int luaGetInteger(lua_State* L,std::string name)
//{
//	lua_getglobal(L,name.c_str());
//	int val = lua_tointeger(L,-1);
//	lua_pop(L,1);
//	return val;
//}
//
//int loadFromLua(std::string path)
//{
//	lua_State* L = luaL_newstate();
//	luaL_openlibs(L);
//	if (luaL_loadfile(L, path.c_str()) || lua_pcall(L, 0, 0, 0)) 
//	{
//        std::cout<<"Error: failed to load " << path <<std::endl;
//		std::cout << lua_tostring(L,-1) << "\n";
//		return NULL;
//    }
//
//	lua_getglobal(L,"PieceMaterial");
//	for(int i = 0;i<6;i++)
//	{
//		lua_pushinteger(L,i+1);
//		lua_gettable(L,1);
//		PieceMaterial[i] = lua_tointeger(L,-1);
//		MaterialValues[i+1] = PieceMaterial[i];
//		MaterialValues[i+7] = PieceMaterial[i];
//		lua_pop(L,1);
//	}
//	lua_pop(L,1);
//
//	EndgameMaterial = luaGetInteger(L,"EndgameMaterial");
//	BishopPairBonus = luaGetInteger(L,"BishopPairBonus");
//	KnightPairBonus = luaGetInteger(L,"KnightPairBonus");
//	RookPairBonus = luaGetInteger(L,"RookPairBonus");
//	BishopOutpostBonus = luaGetInteger(L,"BishopOutpostBonus");
//	KnightOutpostBonus = luaGetInteger(L,"KnightOutpostBonus");
//	QueenOutEarlyPenalty = luaGetInteger(L,"QueenOutEarlyPenalty");
//	RookConnectedBonus = luaGetInteger(L,"RookConnectedBonus");
//	NoPawnsPenalty = luaGetInteger(L,"NoPawnsPenalty");
//
//	lua_close(L);
//}

template<bool Trace> int Engine::LeafEval()
{
	nodes++;
	if (nodes%CheckupNodeCount == 0)
	{
		checkup();
		//nodes = 0;
	}
	//evaltime.Start();

	Bitset ColorPieces[2];
	long long ColorPiecesCount[2];
	for (int i = 0;i < 2;i++)
	{
		ColorPieces[i] = pos.Pieces[i][PIECE_PAWN] | pos.Pieces[i][PIECE_KNIGHT] |
			pos.Pieces[i][PIECE_BISHOP] | pos.Pieces[i][PIECE_ROOK] |
			pos.Pieces[i][PIECE_QUEEN] | pos.Pieces[i][PIECE_KING];
		ColorPiecesCount[i] = popcnt(ColorPieces[i]);
	}

	if (ColorPiecesCount[COLOR_WHITE] == 2 && ColorPiecesCount[COLOR_BLACK] == 2) //2 piece endgame
	{
		if (popcnt(pos.Pieces[COLOR_WHITE][PIECE_PAWN]) == 0 && popcnt(pos.Pieces[COLOR_BLACK][PIECE_PAWN]) == 0) //no pawns
		{
			if (popcnt(pos.Pieces[COLOR_WHITE][PIECE_QUEEN]) == 0 && popcnt(pos.Pieces[COLOR_BLACK][PIECE_QUEEN]) == 0) //no queens
			{
				if (Trace)
					cout << "draw, since there are no pawns and no queens" << endl;
				return 0; //draw
			}
		}
	}
	if (ColorPiecesCount[COLOR_WHITE] == 1 && ColorPiecesCount[COLOR_BLACK] == 3 && popcnt(pos.Pieces[COLOR_BLACK][PIECE_KNIGHT]) == 2)
	{
		if (Trace)
			cout << "draw, 2 knights cant force checkmate" << endl;
		return 0; //2 knights cant force checkmate
	}
	if (ColorPiecesCount[COLOR_BLACK] == 1 && ColorPiecesCount[COLOR_WHITE] == 3 && popcnt(pos.Pieces[COLOR_WHITE][PIECE_KNIGHT]) == 2)
	{
		if (Trace)
			cout << "draw, 2 knights cant force checkmate" << endl;
		return 0; //2 knights cant force checkmate
	}

	Score TotalEval = S(0,0);
	int Material[2] = { getBoardMaterial<COLOR_WHITE>(),getBoardMaterial<COLOR_BLACK>() };
	Score KingSafety[2] = { S(0,0),S(0,0) };
	Score PawnStructure[2] = { S(0,0),S(0,0) };
	Score PieceActivity[2] = { S(0,0),S(0,0) };

	int currentMaterial = Material[COLOR_WHITE] + Material[COLOR_BLACK];

	//int sideeval[2]={Material[0],Material[1]};
	if (Trace)
	{
		cout << "White material: " << Material[0] << endl;
		cout << "Black material: " << Material[1] << endl;
	}

	//Bitset Pieces = pos.OccupiedSq ^ pos.Pieces[COLOR_WHITE][PIECE_PAWN] ^ pos.Pieces[COLOR_BLACK][PIECE_PAWN];
	//int pieceCount = popcnt(Pieces);
	/*bool isEG = false;
	if (currentMaterial <= EndgameMaterial)
	{
		isEG = true;
		if (Trace)
			cout << "We are in the endgame" << endl;
	}*/

	//Piece Sq eval
	for (int i = 0;i < 64;i++)
	{
		if (pos.Squares[i] != SQUARE_EMPTY)
		{
			TotalEval.mg += PieceSq[pos.Squares[i]][i];
			TotalEval.eg += PieceSqEG[pos.Squares[i]][i];
			if (Trace && pos.Squares[i] != SQUARE_EMPTY)
				cout << PieceSq[pos.Squares[i]][i] << " for piece on " << Int2Sq(i) << endl;
		}
	}

	/*if (!isEG)
	{
		if (pos.Squares[27] == SQUARE_WHITEPAWN && pos.Squares[28] == SQUARE_WHITEPAWN)
		{
			PieceActivity[COLOR_WHITE] += PawnsE4D4Bonus;
			if (Trace)
				cout << "Bonus for E4D4 Pawns for White: " << PawnsE4D4Bonus << endl;
		}
		if (pos.Squares[28] == SQUARE_WHITEPAWN && pos.Squares[29] == SQUARE_WHITEPAWN)
		{
			PieceActivity[COLOR_WHITE] += PawnsC4D4Bonus;
			if (Trace)
				cout << "Bonus for C4D4 Pawns for White: " << PawnsC4D4Bonus << endl;
		}
		if (pos.Squares[35] == SQUARE_BLACKPAWN && pos.Squares[36] == SQUARE_BLACKPAWN)
		{
			PieceActivity[COLOR_BLACK] += PawnsE4D4Bonus;
			if (Trace)
				cout << "Bonus for E4D4 Pawns for Black: " << PawnsE4D4Bonus << endl;
		}
		if (pos.Squares[36] == SQUARE_BLACKPAWN && pos.Squares[37] == SQUARE_BLACKPAWN)
		{
			PieceActivity[COLOR_BLACK] += PawnsC4D4Bonus;
			if (Trace)
				cout << "Bonus for C4D4 Pawns for Black: " << PawnsC4D4Bonus << endl;
		}
	}*/

	unsigned long k = 0;
	/*for (int i = 0;i < 2;i++)
	{
		for (int j = 0;j < 6;j++)
		{
			Bitset b = pos.Pieces[i][j];
			while (b)
			{
				_BitScanForward64(&k, b);
				b ^= getPos2Bit(k);
				PieceActivity[i] += PieceSqValues[j][getColorMirror(i,k)];
				if (Trace)
					cout << PieceSqValues[j][getColorMirror(i, k)] << " for piece on " << Int2Sq(k) << endl;
			}
		}
	}*/

	Bitset b = 0x0;
	int KingSquare[2];
	Bitset KingField[2];
	int KingAttackUnits[2] = { 0,0 };
	int MinorsOnBackRank[2] = { 0,0 };
	int PawnCount[2] = { 0,0 }; //number of pawns per side
	int PawnCountColor[2][2] = { {0,0},{0,0} }; //number of pawns on a square of a certain color per side, PawnCountColor[side][squarecolor]

	if (Trace)
		cout << endl << "King Safety:" << endl;
	for (int i = 0;i < 2;i++) //king safety
	{
		b = pos.Pieces[i][PIECE_KING];
		//unsigned long k = 0;
		_BitScanForward64(&k, b);
		KingSquare[i] = k;
		KingField[i] = getKingField(i, k);

		KingSafety[i] += PawnShield1Bonus*(int)popcnt(KingShield1[i][k] & pos.Pieces[i][PIECE_PAWN]);
		if (Trace)
		{
			cout << "Bonus for pawns in front of king for " << PlayerStrings[i] << ": " << PawnShield1Bonus*(int)popcnt(KingShield1[i][k] & pos.Pieces[i][PIECE_PAWN]) << endl;
		}
		KingSafety[i] += PawnShield2Bonus*(int)popcnt(KingShield2[i][k] & pos.Pieces[i][PIECE_PAWN]);
		if (Trace)
		{
			cout << "Bonus for pawns 2 steps in front of king for " << PlayerStrings[i] << ": " << PawnShield2Bonus*(int)popcnt(KingShield2[i][k] & pos.Pieces[i][PIECE_PAWN]) << endl;
		}
		if (getPawnMoves(i, k) & pos.Pieces[i][PIECE_BISHOP])
		{
			KingSafety[i] += BishopShieldBonus;
			if (Trace)
			{
				cout << "Bonus for Bishop in front of king for " << PlayerStrings[i] << ": " << BishopShieldBonus << endl;
			}
		}

		if ((getAboveBits(i, k)&pos.Pieces[i][PIECE_PAWN]) == 0) //checks if there arent friendly pawns in the same file
		{
			if (getAboveBits(i, k)&pos.Pieces[getOpponent(i)][PIECE_PAWN]) //checks if there are opponent pawns in the same file
			{
				KingSafety[i] -= KingOnHalfOpenFilePenalty;
				if (Trace)
					cout << "Penalty for King on half open file for " << PlayerStrings[i] << ": " << KingOnHalfOpenFilePenalty << endl;
				//KingAttackUnits[i] += 4; 
			}
			else
			{
				KingSafety[i] -= KingOnOpenFilePenalty;
				if (Trace)
					cout << "Penalty for King on open file for " << PlayerStrings[i] << ": " << KingOnOpenFilePenalty << endl;
				//KingAttackUnits[i] += 8;
			}
		}
		if ((getAboveBits(i, k)&pos.Pieces[getOpponent(i)][PIECE_PAWN]) == 0 && (getAboveBits(i, k)&pos.Pieces[getOpponent(i)][PIECE_ROOK]) != 0)
		{ //checks if there are no opponent pawns on the file and there are rooks on the file
			KingSafety[i] -= KingOnRookFilePenalty;
			if (Trace)
				cout << "Penalty for King on rook file for " << PlayerStrings[i] << ": " << KingOnRookFilePenalty << endl;
			//KingAttackUnits[i] += 2;
		}
		if (getFile(k) != 0)
		{
			if ((getAboveBits(i, k + 1)&pos.Pieces[i][PIECE_PAWN]) == 0) //checks if there arent friendly pawns in the same file
			{
				if (getAboveBits(i, k + 1)&pos.Pieces[getOpponent(i)][PIECE_PAWN]) //checks if there are opponent pawns in the same file
				{
					KingSafety[i] -= KingAdjHalfOpenFilePenalty;
					if (Trace)
						cout << "Penalty for King adj to half open file for " << PlayerStrings[i] << ": " << KingAdjHalfOpenFilePenalty << endl;
					//KingAttackUnits[i] += 3;
				}
				else
				{
					KingSafety[i] -= KingAdjOpenFilePenalty;
					if (Trace)
						cout << "Penalty for King adj to open file for " << PlayerStrings[i] << ": " << KingAdjOpenFilePenalty << endl;
					//KingAttackUnits[i] += 6;
				}
			}
			if ((getAboveBits(i, k + 1)&pos.Pieces[getOpponent(i)][PIECE_PAWN]) == 0 && (getAboveBits(i, k + 1)&pos.Pieces[getOpponent(i)][PIECE_ROOK]) != 0)
			{ //checks if there are no opponent pawns on the file and there are rooks on the file
				KingSafety[i] -= KingAdjRookFilePenalty;
				if (Trace)
					cout << "Penalty for King adj to rook file for " << PlayerStrings[i] << ": " << KingAdjRookFilePenalty << endl;
				//KingAttackUnits[i] += 1;
			}
		}
		if (getFile(k) != 7)
		{
			if ((getAboveBits(i, k - 1)&pos.Pieces[i][PIECE_PAWN]) == 0) //checks if there arent friendly pawns in the same file
			{
				if (getAboveBits(i, k - 1)&pos.Pieces[getOpponent(i)][PIECE_PAWN]) //checks if there are opponent pawns in the same file
				{
					KingSafety[i] -= KingAdjHalfOpenFilePenalty;
					if (Trace)
						cout << "Penalty for King adj to half open file for " << PlayerStrings[i] << ": " << KingAdjHalfOpenFilePenalty << endl;
					//KingAttackUnits[i] += 3;
				}
				else
				{
					KingSafety[i] -= KingAdjOpenFilePenalty;
					if (Trace)
						cout << "Penalty for King adj to open file for " << PlayerStrings[i] << ": " << KingAdjOpenFilePenalty << endl;
					//KingAttackUnits[i] += 6;
				}
			}
			if ((getAboveBits(i, k - 1)&pos.Pieces[getOpponent(i)][PIECE_PAWN]) == 0 && (getAboveBits(i, k - 1)&pos.Pieces[getOpponent(i)][PIECE_ROOK]) != 0)
			{ //checks if there are no opponent pawns on the file and there are rooks on the file
				KingSafety[i] -= KingAdjRookFilePenalty;
				if (Trace)
					cout << "Penalty for King adj to rook file for " << PlayerStrings[i] << ": " << KingAdjRookFilePenalty << endl;
				//KingAttackUnits[i] += 1;
			}
		}
	}

	//Pawn Evaluation
	if (Trace)
		cout << endl << "Pawn Structure:" << endl;
	//int pawnprobe = PawnTable.Probe(pos.PawnKey);

	for (int i = 0;i < 2;i++)
	{
		b = pos.Pieces[i][PIECE_PAWN];
		PawnCount[i] = popcnt(b);
		PawnCountColor[i][COLOR_WHITE] = popcnt(b&ColoredSquares[COLOR_WHITE]);
		PawnCountColor[i][COLOR_BLACK] = popcnt(b&ColoredSquares[COLOR_BLACK]);
	}

	Bitset WeakPawns[2] = { pos.Pieces[COLOR_WHITE][PIECE_PAWN],pos.Pieces[COLOR_BLACK][PIECE_PAWN] };

	//if(pawnprobe!=CONS_TTUNKNOWN)
	for (int i = 0;i < 2;i++)
	{
		b = pos.Pieces[i][PIECE_PAWN];
		if (!b) //penalty for having no pawns
		{
			PawnStructure[i] -= NoPawnsPenalty;
			if (Trace)
				cout << "Penalty for having no pawns for " << PlayerStrings[i] << ": " << NoPawnsPenalty << endl;
		}
		while (b)
		{
			//int k = firstOf(b);
			//unsigned long k = 0;
			_BitScanForward64(&k, b);
			b ^= getPos2Bit(k);

			if (getPawnAttacks(getOpponent(i), k)&pos.Pieces[COLOR_WHITE][PIECE_PAWN]) //checks if this pawn is defended by a friendly pawn
			{
				WeakPawns[i] ^= getPos2Bit(k);
			}

			if (getAboveBits(i, k)&pos.Pieces[i][PIECE_PAWN]) //checks if there are friendly pawns in the same file
			{
				PawnStructure[i] -= DoubledPawnPenalty[getFile(getColorMirror(i, k))];
				if (Trace)
					cout << "Penalty for doubled pawn on " << Int2Sq(k) << " for " << PlayerStrings[i] << ": " << DoubledPawnPenalty[getFile(getColorMirror(i, k))] << endl;
			}

			if ((getSideBits(k)&pos.Pieces[i][PIECE_PAWN]) == 0) //checks if there are no friendly pawns on the adjacent files
			{
				PawnStructure[i] -= IsolatedPawnPenalty[getFile(getColorMirror(i, k))];
				if (Trace)
					cout << "Penalty for isolated pawn on " << Int2Sq(k) << " for " << PlayerStrings[i] << ": " << IsolatedPawnPenalty[getFile(getColorMirror(i, k))] << endl;
			}

			if ((getAboveAndAboveSideBits(i, k)&pos.Pieces[getOpponent(i)][PIECE_PAWN]) == 0) //checks if the pawn is a passer
			{
				unsigned int passerbonus = PassedPawnBonus[getColorMirror(i, k)];
				int rank = getRank(getColorMirror(i, k))-2;
				if (getAboveBits(i, k)&pos.OccupiedSq) //blocked by a piece
				{
					passerbonus /= 2;
				}
				if (getPawnAttacks(getOpponent(i), k)&pos.Pieces[i][PIECE_PAWN]) //protected passer
				{
					passerbonus += passerbonus / 2;
				}

				PawnStructure[i] += (int)passerbonus;
				PawnStructure[i].eg -= getSquareDistance(KingSquare[i], k)*rank;
				PawnStructure[i].eg += 2 * getSquareDistance(KingSquare[getOpponent(i)], k)*rank;

				if (Trace)
					cout << "Bonus for Passed pawn on " << Int2Sq(k) << " for " << PlayerStrings[i] << ": " << passerbonus << endl;
			}
			//if (pos.OccupiedSq&getPos2Bit(getColorMirror(i,getPlus8(getColorMirror(i, k))))) //checks if pawn is blocked
			//{
			//	PawnStructure[i] -= BlockedPawnPenalty[getColorMirror(i, k)];
			//	if (Trace)
			//		cout << "Penalty for blocked pawn on " << Int2Sq(k) << " for " << PlayerStrings[i] << ": " << BlockedPawnPenalty[getColorMirror(i, k)] << endl;
			//}

			if ((getAboveSideBits(getOpponent(i), k)&pos.Pieces[i][PIECE_PAWN]) == 0) //checks if pawn is backward
			{
				if (getRank(getColorMirror(i, k)) < 5)
				{
					if (getPawnAttacks(i, getColorMirror(i, getPlus8(getColorMirror(i, k))))&pos.Pieces[getOpponent(i)][PIECE_PAWN]
						//&& pos.Squares[getColorMirror(i, getPlus8(getColorMirror(i, k)))] == SQUARE_EMPTY
						)
					{
						PawnStructure[i] -= BackwardPawnPenalty[getFile(k)];
						if (Trace)
							cout << "Penalty for backward pawn on " << Int2Sq(k) << " for " << PlayerStrings[i] << ": " << BackwardPawnPenalty[getFile(k)] << endl;
					}
				}
			}
		}
	}

	/*PawnTable.Save(pos.PawnKey, PawnStructure[COLOR_WHITE] - PawnStructure[COLOR_BLACK], pos.Pieces[COLOR_WHITE][PIECE_PAWN], pos.Pieces[COLOR_BLACK][PIECE_PAWN]);
	pawnprobe = PawnTable.Probe(pos.PawnKey);
	if ((PawnStructure[COLOR_WHITE] - PawnStructure[COLOR_BLACK] != pawnprobe) && (pawnprobe != CONS_TTUNKNOWN))
	{
		cout << "no match " << (PawnStructure[COLOR_WHITE] - PawnStructure[COLOR_BLACK]) << " " << pawnprobe << endl;
		
	}
	if ((PawnTable.getPawns(pos.PawnKey, COLOR_WHITE) != pos.Pieces[COLOR_WHITE][PIECE_PAWN]
		|| PawnTable.getPawns(pos.PawnKey, COLOR_BLACK) != pos.Pieces[COLOR_BLACK][PIECE_PAWN]) && (pawnprobe != CONS_TTUNKNOWN))
	{
		cout << "wrong pawns " << pos.movelist.at(pos.movelist.size() - 1).getMovingPiece() 
			<< " " << pos.movelist.at(pos.movelist.size() - 1).getCapturedPiece() << " " << pos.movelist.at(pos.movelist.size() - 1).getSpecial() << endl;
		cout << pos.movelist.at(pos.movelist.size() - 1).toString() << endl;
		cout << pos.PawnKey << " " << (PawnTable.entries[pos.PawnKey & 65535]).key << endl;
		printBitset(PawnTable.getPawns(pos.PawnKey, COLOR_WHITE));
		cout << endl;
		printBitset(pos.Pieces[COLOR_WHITE][PIECE_PAWN]);
		cout << endl;
		printBitset(PawnTable.getPawns(pos.PawnKey, COLOR_BLACK));
		cout << endl;
		printBitset(pos.Pieces[COLOR_BLACK][PIECE_PAWN]);
		cout << endl;
	}*/

	//extra pawn evaluation
	for (int i = 0;i < 2;i++)
	{
		b = pos.Pieces[i][PIECE_PAWN];
		while (b)
		{
			_BitScanForward64(&k, b);
			b ^= getPos2Bit(k);
			//if (pos.Squares[getColorMirror(i, getPlus8(getColorMirror(i, k)))] != SQUARE_EMPTY) //checks if pawn is blocked
			if(pos.OccupiedSq&getPawnMoves(i,k)) //checks if pawn is blocked
			{
				PawnStructure[i] -= BlockedPawnPenalty[getColorMirror(i, k)];
				if (Trace)
					cout << "Penalty for blocked pawn on " << Int2Sq(k) << " for " << PlayerStrings[i] << ": " << BlockedPawnPenalty[getColorMirror(i, k)] << endl;
			}
			if ((getAboveAndAboveSideBits(i, k)&pos.Pieces[getOpponent(i)][PIECE_PAWN]) == 0) //checks if the pawn is a passer
			{
				if (getAboveBits(getOpponent(i), k)&pos.Pieces[i][PIECE_ROOK]) //rook behind passer
				{
					PieceActivity[i] += RookBehindPassedPawnBonus;
					if (Trace)
						cout << "Bonus for Rook behind Passed pawn on " << Int2Sq(k) << " for " << PlayerStrings[i] << ": " << RookBehindPassedPawnBonus << endl;
				}
				if (getAboveBits(getOpponent(i), k)&pos.Pieces[getOpponent(i)][PIECE_ROOK])
				{
					PieceActivity[i] -= OppRookBehindPassedPawnPenalty;
					if (Trace)
						cout << "Penalty for Rook behind Passed pawn on " << Int2Sq(k) << " for " << PlayerStrings[i] << ": " << OppRookBehindPassedPawnPenalty << endl;
				}
			}
			//pawn attacks near opposing king
			//eval += ColorFactor[i]*popcnt(KingField[getOpponent(i)]&getPawnAttacks(i,k))*AttackWeights[PIECE_PAWN];
			KingAttackUnits[getOpponent(i)] += popcnt(KingField[getOpponent(i)] & getPawnAttacks(i, k))*AttackWeights[PIECE_PAWN];
			if (Trace && popcnt(KingField[getOpponent(i)] & getPawnAttacks(i, k)) != 0)
				cout << "King attack units penalty for pawn on " << Int2Sq(k) << " for " << PlayerStrings[getOpponent(i)] << ": " << popcnt(KingField[getOpponent(i)] & getPawnAttacks(i, k))*AttackWeights[PIECE_PAWN] << endl;

			//PieceActivity[i] += popcnt(getPawnAttacks(i, k)&CenterBits)*CenterSquareBonus;
			//PieceActivity[i] += popcnt(getPawnAttacks(i, k)&CenterBorderBits)*CenterBorderSquareBonus;
			//PieceActivity[i] += popcnt(getPawnAttacks(i, k)&EnemyTerritory[i])*EnemyTerritorySquareBonus;
		}
	}

	//Other pieces
	if (Trace)
		cout << endl << "Piece Activity:" << endl;
	for (int i = 0;i < 2;i++)
	{
		//Rooks
		b = pos.Pieces[i][PIECE_ROOK];
		int rookcount = popcnt(b);
		PieceActivity[i] += RookPawnAdj[PawnCount[i]]*rookcount; //adjustment for pawns
		PieceActivity[i] += RookOppPawnAdj[PawnCount[getOpponent(i)]] * rookcount;
		if (Trace)
		{
			cout << "Adj for Rooks based on number of own pawns for " << PlayerStrings[i] << ": " << RookPawnAdj[PawnCount[i]] * rookcount << endl;
			cout << "Adj for Rooks based on number of opponent pawns for " << PlayerStrings[i] << ": " << RookOppPawnAdj[PawnCount[getOpponent(i)]] * rookcount << endl;
		}
			
		if(rookcount >= 2) //Pair Bonus
		{
			PieceActivity[i] += RookPairBonus;
			if (Trace)
				cout << "Bonus for Rook Pair for " << PlayerStrings[i] << ": " << RookPairBonus << endl;
		}
		while(b)
		{
			//int k = firstOf(b);
			//unsigned long k = 0;
			_BitScanForward64(&k,b);
			b ^= getPos2Bit(k);
			if((getAboveBits(i,k)&pos.Pieces[i][PIECE_PAWN])==0) //checks if there arent friendly pawns in the same file
			{
				if(getAboveBits(i,k)&pos.Pieces[getOpponent(i)][PIECE_PAWN]) //checks if there are opponent pawns in the same file
				{
					PieceActivity[i] += RookHalfOpenBonus[getFile(k)];
					if(Trace)
						cout << "Bonus for Rook on " << Int2Sq(k) << " on half open for " << PlayerStrings[i] << ": " << RookHalfOpenBonus[getFile(k)] << endl;
				}
				else
				{
					PieceActivity[i] += RookOpenBonus[getFile(k)];
					if (Trace)
						cout << "Bonus for Rook on " << Int2Sq(k) << " on open for " << PlayerStrings[i] << ": " << RookOpenBonus[getFile(k)] << endl;
				}
			}

			//Rook Attacks
			/*Bitset m = getRookRankMoves(k,(pos.OccupiedSq>>(getRankOffset(k)))&0xff);
			m |= getRookFileMoves(k,(pos.OccupiedSq90>>(getFileOffset(k)))&0xff);*/
			Bitset m = getRookAttacks(k, pos.OccupiedSq);
			if (m&b)
			{
				PieceActivity[i] += RookConnectedBonus;
				if (Trace)
					cout << "Bonus for Rooks being connected for " << PlayerStrings[i] << ": " << RookConnectedBonus << endl;
			}

			//PieceActivity[i] += popcnt(m&CenterBits)*CenterSquareBonus;
			//PieceActivity[i] += popcnt(m&CenterBorderBits)*CenterBorderSquareBonus;
			//PieceActivity[i] += popcnt(m&EnemyTerritory[i])*EnemyTerritorySquareBonus;

			m &= m^ColorPieces[i];
			PieceActivity[i] += RookMobility[popcnt(m)];
			if (Trace)
				cout << "Rook on " << Int2Sq(k) << " mobility bonus for " << PlayerStrings[i] << ": " << RookMobility[popcnt(m)] << endl;

			PieceActivity[i] += PawnPressureBonus[popcnt(m&WeakPawns[getOpponent(i)])];
			if (Trace)
			{
				cout << "Pawn pressure bonus for rook on " << Int2Sq(k) << ": " << PawnPressureBonus[popcnt(m&WeakPawns[getOpponent(i)])] << endl;
			}
			
			//eval += ColorFactor[i]*popcnt(KingField[getOpponent(i)]&m)*AttackWeights[PIECE_ROOK];
			KingAttackUnits[getOpponent(i)] += popcnt(KingField[getOpponent(i)]&m)*AttackWeights[PIECE_ROOK];
			if (Trace && popcnt(KingField[getOpponent(i)] & m) != 0)
				cout << "King attack units penalty for rook on " << Int2Sq(k) << " for " << PlayerStrings[getOpponent(i)] << ": " << popcnt(KingField[getOpponent(i)] & m)*AttackWeights[PIECE_ROOK] << endl;
		}

		//Knights
		b = pos.Pieces[i][PIECE_KNIGHT];
		int knightcount = popcnt(b);
		PieceActivity[i] += KnightPawnAdj[PawnCount[i]]*knightcount; //adjustment for pawns
		PieceActivity[i] += KnightOppPawnAdj[PawnCount[getOpponent(i)]] * knightcount;
		if (Trace)
		{
			cout << "Adj for Knights based on number of own pawns for " << PlayerStrings[i] << ": " << KnightPawnAdj[PawnCount[i]] * knightcount << endl;
			cout << "Adj for Knights based on number of opponent pawns for " << PlayerStrings[i] << ": " << KnightOppPawnAdj[PawnCount[getOpponent(i)]] * knightcount << endl;
		}
		if(popcnt(b) >= 2)
		{
			PieceActivity[i] += KnightPairBonus;
			if (Trace)
				cout << "Bonus for Knight Pair for " << PlayerStrings[i] << ": " << KnightPairBonus << endl;
		}
		while(b)
		{
			//unsigned long k = 0;
			_BitScanForward64(&k,b);
			b ^= getPos2Bit(k);
			if((getAboveSideBits(i,k)&pos.Pieces[getOpponent(i)][PIECE_PAWN])==0) //checks if there are no enemy pawns on the adjacent files
			{
				unsigned int outpostbonus = KnightOutpostBonus[getColorMirror(i, k)];
				//unsigned int outpostbonus = PieceSqValues[PIECE_KNIGHT][getColorMirror(i, k)];
				int piececolor = SquareColor[k];
				if (pos.Pieces[getOpponent(i)][PIECE_KNIGHT] == 0 && (pos.Pieces[getOpponent(i)][PIECE_BISHOP] & ColoredSquares[piececolor]) == 0)
				{
					outpostbonus *= 2; //double bonus if there are no opponent minor pieces
				}
				if (pos.Pieces[i][PIECE_PAWN] & PawnAttacks[getOpponent(i)][k] != 0)
				{
					outpostbonus += outpostbonus/2; //extra bonus if defended by a pawn
				}
				PieceActivity[i] += (int)outpostbonus;
				if (Trace)
					cout << "Outpost bonus for Knight on " << Int2Sq(k) << " for " << PlayerStrings[i] << ": " << outpostbonus << endl;
			}

			//knight attacks near opposing king
			//eval += ColorFactor[i]*popcnt(KingField[getOpponent(i)]&getKnightMoves(k))*AttackWeights[PIECE_KNIGHT];
			//PieceActivity[i] += popcnt(getKnightMoves(k)&CenterBits)*CenterSquareBonus;
			//PieceActivity[i] += popcnt(getKnightMoves(k)&CenterBorderBits)*CenterBorderSquareBonus;
			//PieceActivity[i] += popcnt(getKnightMoves(k)&EnemyTerritory[i])*EnemyTerritorySquareBonus;

			Bitset m = getKnightMoves(k)&(getKnightMoves(k)^ColorPieces[i]);
			PieceActivity[i] += KnightMobility[popcnt(m)];
			if (Trace)
				cout << "Knight on " << Int2Sq(k) << " mobility bonus for " << PlayerStrings[i] << ": " << KnightMobility[popcnt(m)] << endl;

			if (m&pos.Pieces[getOpponent(i)][PIECE_BISHOP])
			{
				PieceActivity[i] += KnightAttacksBishopBonus;
				if (Trace)
					cout << "Bonus for knight on " << Int2Sq(k) << " attacking a bishop: " << KnightAttacksBishopBonus << endl;
 			}

			PieceActivity[i] += PawnPressureBonus[popcnt(m&WeakPawns[getOpponent(i)])];
			if (Trace)
			{
				cout << "Pawn pressure bonus for knight on " << Int2Sq(k) << ": " << PawnPressureBonus[popcnt(m&WeakPawns[getOpponent(i)])] << endl;
			}

			KingAttackUnits[getOpponent(i)] += popcnt(KingField[getOpponent(i)]&m)*AttackWeights[PIECE_KNIGHT];

			if (Trace && popcnt(KingField[getOpponent(i)] & m) != 0)
				cout << "King attack units penalty for knight on " << Int2Sq(k) << " for " << PlayerStrings[getOpponent(i)] << ": " << popcnt(KingField[getOpponent(i)] & m)*AttackWeights[PIECE_KNIGHT] << endl;

			if(getRank(getColorMirror(i,k))==0)
			{
				MinorsOnBackRank[i]++;
			}
		}

		//Bishops
		b = pos.Pieces[i][PIECE_BISHOP];
		int bishopcount = popcnt(b);
		PieceActivity[i] += BishopPawnAdj[PawnCount[i]]*bishopcount; //adjustment for pawns
		PieceActivity[i] += BishopOppPawnAdj[PawnCount[getOpponent(i)]] * bishopcount;
		if (Trace)
		{
			cout << "Adj for Bishops based on number of own pawns for " << PlayerStrings[i] << ": " << BishopPawnAdj[PawnCount[i]] * bishopcount << endl;
			cout << "Adj for Bishops based on number of opponent pawns for " << PlayerStrings[i] << ": " << BishopOppPawnAdj[PawnCount[getOpponent(i)]] * bishopcount << endl;
		}
		if(popcnt(b) >= 2)
		{
			PieceActivity[i] += BishopPairBonus;
			if (Trace)
				cout << "Bonus for Bishop Pair for " << PlayerStrings[i] << ": " << BishopPairBonus << endl;
		}
		while(b)
		{
			//unsigned long k = 0;
			_BitScanForward64(&k,b);
			b ^= getPos2Bit(k);
			if((getAboveSideBits(i,k)&pos.Pieces[getOpponent(i)][PIECE_PAWN])==0) //checks if there are no enemy pawns on the adjacent files
			{
				unsigned int outpostbonus = BishopOutpostBonus[getColorMirror(i, k)];
				//unsigned int outpostbonus = PieceSqValues[PIECE_BISHOP][getColorMirror(i, k)];
				int piececolor = SquareColor[k];
				if (pos.Pieces[getOpponent(i)][PIECE_KNIGHT] == 0 && (pos.Pieces[getOpponent(i)][PIECE_BISHOP]&ColoredSquares[piececolor])==0)
				{
					outpostbonus *= 2; //double bonus if there are no opponent minor pieces
				}
				if (pos.Pieces[i][PIECE_PAWN] & PawnAttacks[getOpponent(i)][k] != 0)
				{
					outpostbonus += outpostbonus/2; //extra bonus if defended by a pawn
				}
				PieceActivity[i] += (int)outpostbonus;
				if (Trace)
					cout << "Outpost bonus for Bishop on " << Int2Sq(k) << " for " << PlayerStrings[i] << ": " << outpostbonus << endl;
			}

			//bishop attacks near opposing king
			/*Bitset m = getBishopA1H8Moves(k,(pos.OccupiedSq135>>getDiag(getturn135(k)))&0xff);
			m |= getBishopA8H1Moves(k,(pos.OccupiedSq45>>getDiag(getturn45(k)))&0xff);*/
			Bitset m = getBishopAttacks(k, pos.OccupiedSq);

			//PieceActivity[i] += popcnt(m&CenterBits)*CenterSquareBonus;
			//PieceActivity[i] += popcnt(m&CenterBorderBits)*CenterBorderSquareBonus;
			//PieceActivity[i] += popcnt(m&EnemyTerritory[i])*EnemyTerritorySquareBonus;

			m &= m^ColorPieces[i];

			PieceActivity[i] += BishopMobility[popcnt(m)];
			if (Trace)
				cout << "Bishop on " << Int2Sq(k) << " mobility bonus for " << PlayerStrings[i] << ": " << BishopMobility[popcnt(m)] << endl;

			PieceActivity[i] += BishopPawnSameColor[PawnCountColor[i][SquareColor[k]]]; //bishop pawn same color adjustment
			PieceActivity[i] += BishopOppPawnSameColor[PawnCountColor[getOpponent(i)][SquareColor[k]]]; //bishop opp pawn same color adjustment
			if (Trace)
			{
				cout << "Bishop on " << Int2Sq(k) << " Adj for our pawns on the same color for " << PlayerStrings[i] << ": " << BishopPawnSameColor[PawnCountColor[i][SquareColor[k]]] << endl;
				cout << "Bishop on " << Int2Sq(k) << " Adj for opponent pawns on the same color for " << PlayerStrings[i] << ": " << BishopOppPawnSameColor[PawnCountColor[getOpponent(i)][SquareColor[k]]] << endl;
			}

			if (m&pos.Pieces[getOpponent(i)][PIECE_KNIGHT])
			{
				PieceActivity[i] += BishopAttacksKnightBonus;
				if (Trace)
					cout << "Bonus for bishop on " << Int2Sq(k) << " attacking a knight: " << BishopAttacksKnightBonus << endl;
			}

			PieceActivity[i] += PawnPressureBonus[popcnt(m&WeakPawns[getOpponent(i)])];
			if (Trace)
			{
				cout << "Pawn pressure bonus for bishop on " << Int2Sq(k) << ": " << PawnPressureBonus[popcnt(m&WeakPawns[getOpponent(i)])] << endl;
			}
				
			//eval += ColorFactor[i]*popcnt(KingField[getOpponent(i)]&m)*AttackWeights[PIECE_BISHOP];
			KingAttackUnits[getOpponent(i)] += popcnt(KingField[getOpponent(i)]&m)*AttackWeights[PIECE_BISHOP];

			if (Trace && popcnt(KingField[getOpponent(i)] & m) != 0)
				cout << "King attack units penalty for bishop on " << Int2Sq(k) << " for " << PlayerStrings[getOpponent(i)] << ": " << popcnt(KingField[getOpponent(i)] & m)*AttackWeights[PIECE_BISHOP] << endl;

			if(getRank(getColorMirror(i,k))==0)
			{
				MinorsOnBackRank[i]++;
			}
		}

		//Queens
		b = pos.Pieces[i][PIECE_QUEEN];
		while(b)
		{
			//unsigned long k = 0;
			_BitScanForward64(&k,b);
			b ^= getPos2Bit(k);

			//queen attacks near opposing king
			/*Bitset m = getRookRankMoves(k,(pos.OccupiedSq>>(getRankOffset(k)))&0xff);
			m |= getRookFileMoves(k,(pos.OccupiedSq90>>(getFileOffset(k)))&0xff);
			m |= getBishopA1H8Moves(k,(pos.OccupiedSq135>>getDiag(getturn135(k)))&0xff);
			m |= getBishopA8H1Moves(k,(pos.OccupiedSq45>>getDiag(getturn45(k)))&0xff);*/
			Bitset m = getQueenAttacks(k, pos.OccupiedSq);

			//PieceActivity[i] += popcnt(m&CenterBits)*CenterSquareBonus;
			//PieceActivity[i] += popcnt(m&CenterBorderBits)*CenterBorderSquareBonus;
			//PieceActivity[i] += popcnt(m&EnemyTerritory[i])*EnemyTerritorySquareBonus;

			m &= m^ColorPieces[i];

			PieceActivity[i] += QueenMobility[popcnt(m)]; //mobility factor
			if (Trace)
				cout << "Queen on " << Int2Sq(k) << " mobility bonus for " << PlayerStrings[i] << ": " << QueenMobility[popcnt(m)] << endl;

			PieceActivity[i] += (int)((unsigned)(PawnPressureBonus[popcnt(m&WeakPawns[getOpponent(i)])])>>1); //half bonus for queens
			if (Trace)
			{
				cout << "Pawn pressure bonus for queen on " << Int2Sq(k) << ": " << PawnPressureBonus[popcnt(m&WeakPawns[getOpponent(i)])]/2 << endl;
			}

			//eval += ColorFactor[i]*popcnt(KingField[getOpponent(i)]&m)*AttackWeights[PIECE_QUEEN];
			KingAttackUnits[getOpponent(i)] += long(popcnt(KingField[getOpponent(i)]&m))*AttackWeights[PIECE_QUEEN];

			if (Trace && popcnt(KingField[getOpponent(i)] & m) != 0)
				cout << "King attack units penalty for queen on " << Int2Sq(k) << " for " << PlayerStrings[getOpponent(i)] << ": " << popcnt(KingField[getOpponent(i)] & m)*AttackWeights[PIECE_QUEEN] << endl;

			if (getRank(getColorMirror(i, k))>1)
			{
				PieceActivity[i] -= QueenOutEarlyPenalty*MinorsOnBackRank[i];
				if (Trace)
					cout << "Queen out early penalty for " << PlayerStrings[i] << ": " << QueenOutEarlyPenalty*MinorsOnBackRank[i] << endl;
			}
		}
	}

	if(Trace)
		cout << endl << "King Attack Evaluation:" << endl;
	for(int i = 0;i<2;i++)
	{
		KingSafety[i] -= SafetyTable[min(99,KingAttackUnits[i])];
		if (Trace)
		{
			cout << "King Attack Units for " << PlayerStrings[i] << ": " << KingAttackUnits[i] << endl;
			cout << "Safety table for " << PlayerStrings[i] << ": " << SafetyTable[min(99, KingAttackUnits[i])] << endl;
			cout << endl;
		}
	}

	if (Trace)
	{
		for (int i = 0;i < 2;i++)
		{
			cout << PlayerStrings[i] << " King Safety: " << KingSafety[i] << endl;
			cout << PlayerStrings[i] << " Pawn Structure: " << PawnStructure[i] << endl;
			cout << PlayerStrings[i] << " Piece Activity: " << PieceActivity[i] << endl;
			cout << endl;
		}
	}

	/*for (int i = 0;i < 2;i++)
	{
		PieceActivity[i] = PieceActivity[i] * PieceActivityFactor;
		KingSafety[i] = (KingSafety[i] * KingSafetyFactor * Material[getOpponent(i)]) / TotalMaterial;
		PawnStructure[i] = ((PawnStructure[i] * PawnStructureFactor)/2) + (((PawnStructure[i]*PawnStructureFactor) * (TotalMaterial- Material[getOpponent(i)])) / (TotalMaterial*2));
	}*/
	//if(pawnprobe!=CONS_TTUNKNOWN)
	//	pawnprobe = ((pawnprobe * PawnStructureFactor) / 2) + (((pawnprobe * PawnStructureFactor) * (TotalMaterial - currentMaterial)) / (TotalMaterial * 2));

	/*if (Trace)
	{
		cout << "After scaling:" << endl;
		for (int i = 0;i < 2;i++)
		{
			cout << PlayerStrings[i] << " King Safety: " << KingSafety[i] << endl;
			cout << PlayerStrings[i] << " Pawn Structure: " << PawnStructure[i] << endl;
			cout << PlayerStrings[i] << " Piece Activity: " << PieceActivity[i] << endl;
			cout << endl;
		}
	}*/

	TotalEval += Material[COLOR_WHITE] + KingSafety[COLOR_WHITE] + PawnStructure[COLOR_WHITE] + PieceActivity[COLOR_WHITE];
	TotalEval -= Material[COLOR_BLACK] + KingSafety[COLOR_BLACK] + PawnStructure[COLOR_BLACK] + PieceActivity[COLOR_BLACK];
	//if(pawnprobe!=CONS_TTUNKNOWN)
	//	neteval += pawnprobe;

	int finaleval = TotalEval.eg + (TotalEval.mg - TotalEval.eg)*((float)(currentMaterial) / TotalMaterialBothSides);

	if(pos.turn==COLOR_BLACK)
		return -finaleval;

	//evaltime.Stop();
	return finaleval;
}

bool Engine::isDraw()
{
	Bitset ColorPieces[2];
	long long ColorPiecesCount[2];

	if (popcnt(pos.OccupiedSq) > 3)
	{
		return false;
	}

	for (int i = 0;i < 2;i++)
	{
		ColorPieces[i] = pos.Pieces[i][PIECE_PAWN] | pos.Pieces[i][PIECE_KNIGHT] |
			pos.Pieces[i][PIECE_BISHOP] | pos.Pieces[i][PIECE_ROOK] |
			pos.Pieces[i][PIECE_QUEEN] | pos.Pieces[i][PIECE_KING];
		ColorPiecesCount[i] = popcnt(ColorPieces[i]);
	}
	if (ColorPiecesCount[COLOR_BLACK] == 1 && ColorPiecesCount[COLOR_WHITE] == 2
		&& (popcnt(pos.Pieces[COLOR_WHITE][PIECE_KNIGHT]) == 1 || popcnt(pos.Pieces[COLOR_WHITE][PIECE_BISHOP]) == 1))
	{
		return true;
	}
	if (ColorPiecesCount[COLOR_WHITE] == 1 && ColorPiecesCount[COLOR_BLACK] == 2
		&& (popcnt(pos.Pieces[COLOR_BLACK][PIECE_KNIGHT]) == 1 || popcnt(pos.Pieces[COLOR_BLACK][PIECE_BISHOP]) == 1))
	{
		return true;
	}
	if (ColorPiecesCount[COLOR_WHITE] == 1 && ColorPiecesCount[COLOR_BLACK] == 1)
	{
		return true;
	}
	return false;
}

template <int Color> int Engine::getBoardMaterial()
{
	int eval = 0;
	/*for(int i = 0;i<64;i++)
	{
		if(pos.Squares[i]!=SQUARE_EMPTY)
		{
			int pc = getSquare2Piece(pos.Squares[i]);
			if(pc!=PIECE_KING)
				eval += MaterialValues[pc];
		}
	}*/
	eval += popcnt(pos.Pieces[Color][PIECE_PAWN])*PieceMaterial[PIECE_PAWN];
	eval += popcnt(pos.Pieces[Color][PIECE_KNIGHT])*PieceMaterial[PIECE_KNIGHT];
	eval += popcnt(pos.Pieces[Color][PIECE_BISHOP])*PieceMaterial[PIECE_BISHOP];
	eval += popcnt(pos.Pieces[Color][PIECE_ROOK])*PieceMaterial[PIECE_ROOK];
	eval += popcnt(pos.Pieces[Color][PIECE_QUEEN])*PieceMaterial[PIECE_QUEEN];

	/*eval -= popcnt(pos.Pieces[COLOR_BLACK][PIECE_PAWN])*PieceMaterial[PIECE_PAWN];
	eval -= popcnt(pos.Pieces[COLOR_BLACK][PIECE_KNIGHT])*PieceMaterial[PIECE_KNIGHT];
	eval -= popcnt(pos.Pieces[COLOR_BLACK][PIECE_BISHOP])*PieceMaterial[PIECE_BISHOP];
	eval -= popcnt(pos.Pieces[COLOR_BLACK][PIECE_ROOK])*PieceMaterial[PIECE_ROOK];
	eval -= popcnt(pos.Pieces[COLOR_BLACK][PIECE_QUEEN])*PieceMaterial[PIECE_QUEEN];*/
	return eval;
}

void scaleConstants()
{
	//King Safety
	PawnShield1Bonus *= KingSafetyFactor;
	PawnShield2Bonus *= KingSafetyFactor;
	BishopShieldBonus *= KingSafetyFactor;
	KingOnHalfOpenFilePenalty *= KingSafetyFactor;
	KingOnOpenFilePenalty *= KingSafetyFactor;
	KingAdjHalfOpenFilePenalty *= KingSafetyFactor;
	KingAdjOpenFilePenalty *= KingSafetyFactor;
	KingOnRookFilePenalty *= KingSafetyFactor;
	KingAdjRookFilePenalty *= KingSafetyFactor;
	KingBetweenRooksPenalty *= KingSafetyFactor;

	for (int i = 0;i < 100;i++)
	{
		SafetyTable[i] *= KingSafetyFactor;
	}

	//Pawns
	for (int i = 0;i < 9;i++)
	{
		PawnPressureBonus[i] *= PawnStructureFactor;
	}

	for (int i = 0;i < 8;i++)
	{
		DoubledPawnPenalty[i] *= PawnStructureFactor;
		IsolatedPawnPenalty[i] *= PawnStructureFactor;
		BackwardPawnPenalty[i] *= PawnStructureFactor;
	}
	
	for (int i = 0;i < 64;i++)
	{
		BlockedPawnPenalty[i] *= PawnStructureFactor;
		//Passer
		PassedPawnBonus[i] *= PassedPawnFactor;
	}

	RookBehindPassedPawnBonus *= PassedPawnFactor;
	OppRookBehindPassedPawnPenalty *= PassedPawnFactor;

	//Rooks
	for (int i = 0;i < 8;i++)
	{
		RookHalfOpenBonus[i] *= RookFactor;
		RookOpenBonus[i] *= RookFactor;
	}
	RookConnectedBonus *= RookFactor;
	RookPairBonus *= RookFactor;
	for (int i = 0;i < 9;i++)
	{
		RookPawnAdj[i] *= RookFactor;
		RookOppPawnAdj[i] *= RookFactor;
	}
	
	//Knights
	KnightPairBonus *= KnightFactor;
	for (int i = 0;i < 9;i++)
	{
		KnightPawnAdj[i] *= KnightFactor;
		KnightOppPawnAdj[i] *= KnightFactor;
	}

	//Bishop
	BishopPairBonus *= BishopFactor;
	for (int i = 0;i < 9;i++)
	{
		BishopPawnSameColor[i] *= BishopFactor;
		BishopOppPawnSameColor[i] *= BishopFactor;
	}

	//Mobility
	for (int i = 0;i < 9;i++)
	{
		KnightMobility[i] *= MobilityFactor*KnightFactor;
	}
	for (int i = 0;i < 16;i++)
	{
		BishopMobility[i] *= MobilityFactor*BishopFactor;
		RookMobility[i] *= MobilityFactor*RookFactor;
	}
	for (int i = 0;i < 32;i++)
	{
		QueenMobility[i] *= MobilityFactor*QueenFactor;
	}

	//Outposts
	for (int i = 0;i < 64;i++)
	{
		KnightOutpostBonus[i] *= OutpostFactor*KnightFactor;
		BishopOutpostBonus[i] *= OutpostFactor*BishopFactor;
	}
}

void evalinit()
{
	int KnightAggrValues[8] = { -10,-5,0,5,10,15,10,5 };
	int KnightFileValues[8] = { -20,-10,0,5,5,0,-10,-20 };
	int KnightRankValues[8] = { -20,-10,0,5,5,0,-10,-20 };
	int BishopRankValues[8] = { -6, -2, 0, 2, 2, 0, -2, -6 };
	int BishopFileValues[8] = { -6, -2, 0, 2, 2, 0, -2, -6 };
	int BishopDiagBonus = 4;
	int RookFileValues[8] = {-6,-3,0,3,3,0,-3,-6};
	int Rook7thRankBonus = 5;
	int QueenCentralizationValues[8] = { -12,-4,0,4,4,0,-4,-12 };
	int KingRankValues[8] = { 10,0,-20,-30,-40,-50,-60,-70 };
	int KingFileValues[8] = { 30,40,20,0,0,20,40,30 };
	int KingCentralizationValues[8] = { -36,-12,0,12,12,0,-12,-36 };
	for (int i = 0;i < 64;i++)
	{
		int rank = getRank(i);
		int file = getFile(i);
		PieceSqValues[PIECE_KNIGHT][i] = KnightRankValues[rank] + KnightFileValues[file] + KnightAggrValues[rank];
		PieceSqValuesEG[PIECE_KNIGHT][i] = KnightRankValues[rank] + KnightFileValues[file];
		PieceSqValues[PIECE_BISHOP][i] = BishopRankValues[rank] + BishopFileValues[file];
		PieceSqValuesEG[PIECE_BISHOP][i] = BishopRankValues[rank] + BishopFileValues[file];
		if (rank == file || rank == (7 - file))
		{
			PieceSqValues[PIECE_BISHOP][i] += BishopDiagBonus;
		}
		PieceSqValues[PIECE_ROOK][i] = RookFileValues[file];
		PieceSqValuesEG[PIECE_ROOK][i] = 0;
		if (i / 8 == 6)
		{
			PieceSqValues[PIECE_ROOK][i] += Rook7thRankBonus;
			PieceSqValuesEG[PIECE_ROOK][i] += Rook7thRankBonus;
		}
		PieceSqValues[PIECE_QUEEN][i] = (QueenCentralizationValues[rank] + QueenCentralizationValues[file])/4;
		PieceSqValuesEG[PIECE_QUEEN][i] = QueenCentralizationValues[rank] + QueenCentralizationValues[file];
		PieceSqValues[PIECE_KING][i] = KingRankValues[rank] + KingFileValues[file];
		PieceSqValuesEG[PIECE_KING][i] = KingCentralizationValues[rank] + KingCentralizationValues[file];
	}
	for(int i = 1;i<13;i++)
	{
		//cout << "Piece " << i << endl;
		int avg = 0;
		int avgEG = 0;
		/*for(int k = 0;k<64;k++)
		{
			if(i<7)
			{
				avg += PieceSqValues[i][k];
				avgEG += PieceSqValuesEG[i][k];
			}
			else
			{
				avg += PieceSqValues[i-6][getMirror(k)];
				avgEG += PieceSqValuesEG[i-6][getMirror(k)];
			}
		}
		avg /= 64;
		avgEG /= 64;
		cout << "avg " << avg << " avgeg " << avgEG << endl;*/
		for(int j = 0;j<64;j++)
		{
			if(i<7)
			{
				PieceSq[i][j] = PieceSqValues[i-1][j]-avg;
				PieceSqEG[i][j] = PieceSqValuesEG[i-1][j]-avg;
				//cout << PieceSq[i][j] << " ";
			}
			else
			{
				PieceSq[i][j] = -PieceSqValues[i-7][getMirror(j)]+avg;
				PieceSqEG[i][j] = -PieceSqValuesEG[i-7][getMirror(j)]+avgEG;
				//cout << PieceSq[i][j] << " ";
			}
			/*if((j+1)%8==0)
				cout << endl;*/
		}
	}
	scaleConstants();
}