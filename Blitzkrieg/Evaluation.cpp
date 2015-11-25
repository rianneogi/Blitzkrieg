#include "Engine.h"

string PlayerStrings[2] = { "White","Black" };

int ColorFactor[2] = {1,-1};

const float PieceActivityFactor = 1;
const float PawnStructureFactor = 0.7;
const float KingSafetyFactor = 1;

int PieceMaterial[7] = {100,325,335,500,975,-CONS_MATED,0};
int MaterialValues[13] = {0,100,325,335,500,975,-CONS_MATED,-100,-325,-335,-500,-975,CONS_MATED};
int TotalMaterial = 2 * (PieceMaterial[0] + PieceMaterial[1] + PieceMaterial[2] + PieceMaterial[3] + PieceMaterial[4]);

int EndgameMaterial = 3800;

int LazyEval1 = 400;

int BishopPairBonus = 45;
int KnightPairBonus = -5;
int RookPairBonus = 10;

//int CenterSquareBonus = 4;
//int CenterBorderSquareBonus = 2;
//int EnemyTerritorySquareBonus = 3;

//int KnightOutpostBonus = 25;
//int BishopOutpostBonus = 10;
//int KnightOutpostBonus[8] = { 0,0,5,10,15,25,25,15 }; //non-negative values always(code will bug otherwise)
//int BishopOutpostBonus[8] = {0, 0, 2, 4, 6, 10, 10, 6}; //non-negative values always(code will bug otherwise)

int QueenOutEarlyPenalty = 10; //penalty for queens not on back rank for every minor on back rank

// mobility bonus based on how many squares the pieces attack
int BishopMobility[16]  = {-12, -4,  0,  2,  4,  4,  6,  6,  8,  8, 10, 10, 12, 14, 16};
int RookMobility[16]  = {-8,-4, 0, 2, 2, 2, 4, 4, 4, 6, 6, 6, 8, 8, 8};
int KnightMobility[9] = {-12, -8,  0,  4,  8, 10, 12, 14, 16};
int QueenMobility[32] = { -10,-9,-8,-7,-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21};
 
// adjustments to piece values depending on number of pawns
int KnightPawnAdj[9] = {-20,-16,-12, -8, -4,  0,  4,  8, 12};
int KnightOppPawnAdj[9] = { -20,-16,-12,-8,-4,0,4,8,12 };
int BishopPawnAdj[9] = { 0,0,0,0,0,0,0,0,0 };
int BishopOppPawnAdj[9] = { 0,0,0,0,0,0,0,0,0 };
int RookPawnAdj[9] =   {15,12, 9, 6, 3, 0,-3,-6,-9};
int RookOppPawnAdj[9] = { -8,-4, 0, 4, 8,12,16,20,24 };

//adjustments to bishop values depending on number of pawns on same color square as bishop
int BishopPawnSameColor[9] = {15,12,9,6,3,0,-3,-6,-9};
int BishopOppPawnSameColor[9] = { -9,-6,-3,0,3,6,9,12,15 };

//King Safety
int PawnShield1Bonus = 10;
int PawnShield2Bonus = 5;
int BishopShieldBonus = 7;
int KingOnHalfOpenFilePenalty = 15; //penalty for king being on half open files
int KingOnOpenFilePenalty = 25; //penalty for king being on open files
int KingAdjHalfOpenFilePenalty = 10; //penalty for king being adjacent to half open files
int KingAdjOpenFilePenalty = 15; //penalty for king being adjacent to open files
int KingOnRookFilePenalty = 10; //penalty for king being on an opponent semiopen file with a rook on it
int KingAdjRookFilePenalty = 5; //penalty for king being adjacent an opponent semiopen file with a rook on it
int AttackWeights[6] = {1,2,2,3,4,0};
int KingBetweenRooksPenalty = 10;
//int NoPawnShieldPenalty = 15;

int SafetyTable[100] = {
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
int NoPawnsPenalty = 32;
int DoubledPawnPenalty[8] = { 6,8,10,15,15,10,8,6 };
int IsolatedPawnPenalty[8] = { 9,12,18,30,30,18,12,9 };
int BackwardPawnPenalty[8] = { 6,8,10,12,12,10,8,6 };
int PassedPawnBonus[64] = {  0,  0,  0,  0,  0,  0,  0,  0, //non-negative values always(code will bug otherwise)
						     5, 10, 10, 10, 10, 10, 10,  5,
						    10, 20, 20, 20, 20, 20, 20, 10,
                            20, 40, 40, 40, 40, 40, 40, 20,
                            30, 60, 60, 60, 60, 60, 60, 30,
							40, 80, 80, 80, 80, 80, 80, 40,
						    90,120,120,120,120,120,120, 90,
						   200,200,200,200,200,200,200,200};
int BlockedPawnPenalty[64] = {   0,  0,  0,  0,  0,  0,  0,  0,
	                             1,  1, 14, 16, 16, 14,  1,  1,
								 0,  0,  0,  4,  4,  0,  0,  0,
								 0,  0,  0,  0,  0,  0,  0,  0,
								 2,  2,  2,  2,  2,  2,  2,  2,
								10, 10, 10, 10, 10, 10, 10, 10,
								20, 20, 20, 20, 20, 20, 20, 20,
								 0,  0,  0,  0,  0,  0,  0,  0};

//Rooks
int RookHalfOpenBonus[8] = {15,15,20,30,30,20,15,15};
int RookOpenBonus[8] = {20,20,30,40,40,30,20,20};
int RookConnectedBonus = 10;
int RookBehindPassedPawnBonus = 20;
int OppRookBehindPassedPawnPenalty = 15;

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

int PieceSqValues[7][64] = 
{
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //empty
	{  2,  4,  4,  4,  4,  4,  4,  2, //pawn
	   2,  6,  6,  8,  8,  6,  6,  2,
	   2,  6, 10, 12, 12, 10,  6,  2,
	   4, 12, 20, 24, 24, 20, 12,  4,
	   4, 12, 12, 16, 16, 12, 12,  4,
	   4,  8,  8,  8,  8,  8,  8,  4,
	   4,  8,  8,  8,  8,  8,  8,  4,
	   4,  8,  8,  8,  8,  8,  8,  4},

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

int PieceSqValuesEG[7][64] = 
{
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //empty
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

template<bool Trace>
int Engine::LeafEval()
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

	if (ColorPiecesCount[COLOR_WHITE] == 1 && ColorPiecesCount[COLOR_BLACK] == 1)
	{
		if (Trace)
			cout << "draw, King vs King" << endl;
		return 0; //draw
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
	if(ColorPiecesCount[COLOR_BLACK]==1 && ColorPiecesCount[COLOR_WHITE] == 2
		&& (popcnt(pos.Pieces[COLOR_WHITE][PIECE_KNIGHT]) == 1 || popcnt(pos.Pieces[COLOR_WHITE][PIECE_BISHOP]) == 1))
	{
		if (Trace)
			cout << "draw, king and minor piece vs king" << endl;
		return 0;
	}
	if (ColorPiecesCount[COLOR_WHITE] == 1 && ColorPiecesCount[COLOR_BLACK] == 2
		&& (popcnt(pos.Pieces[COLOR_BLACK][PIECE_KNIGHT]) == 1 || popcnt(pos.Pieces[COLOR_BLACK][PIECE_BISHOP]) == 1))
	{
		if (Trace)
			cout << "draw, king and minor piece vs king" << endl;
		return 0;
	}

	int neteval = 0;
	int Material[2] = { getBoardMaterial<COLOR_WHITE>(),getBoardMaterial<COLOR_BLACK>() };
	int KingSafety[2] = { 0,0 };
	int PawnStructure[2] = { 0,0 };
	int PieceActivity[2] = { 0,0 };

	int currentMaterial = Material[COLOR_WHITE] + Material[COLOR_BLACK];

	//int sideeval[2]={Material[0],Material[1]};
	if (Trace)
	{
		cout << "White material: " << Material[0] << endl;
		cout << "Black material: " << Material[1] << endl;
	}
		
	//Bitset Pieces = pos.OccupiedSq ^ pos.Pieces[COLOR_WHITE][PIECE_PAWN] ^ pos.Pieces[COLOR_BLACK][PIECE_PAWN];
	//int pieceCount = popcnt(Pieces);
	bool isEG = false;
	if(currentMaterial <= EndgameMaterial)
	{
		isEG = true;
		if (Trace)
			cout << "We are in the endgame" << endl;
	}

	//Piece Sq eval
	if (isEG)
	{
		for (int i = 0;i<64;i++)
		{
			neteval += PieceSqEG[pos.Squares[i]][i];
			if (Trace && pos.Squares[i] != SQUARE_EMPTY)
				cout << PieceSqEG[pos.Squares[i]][i] << " for piece on " << Int2Sq(i) << endl;
		}
	}
	else
	{
		for (int i = 0;i<64;i++)
		{
			neteval += PieceSq[pos.Squares[i]][i];
			if (Trace && pos.Squares[i] != SQUARE_EMPTY)
				cout << PieceSq[pos.Squares[i]][i] << " for piece on " << Int2Sq(i) << endl;
		}
	}
	
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
	Bitset KingField[2];
	int KingAttackUnits[2] = { 0,0 };
	int MinorsOnBackRank[2] = { 0,0 };
	int PawnCount[2] = { 0,0 }; //number of pawns per side
	int PawnCountColor[2][2] = { {0,0},{0,0} }; //number of pawns on a square of a certain color per side, PawnCountColor[side][squarecolor]

	if (Trace)
		cout << endl << "King Safety:" << endl;
	for(int i = 0;i<2;i++) //king safety
	{
		b = pos.Pieces[i][PIECE_KING];
		//unsigned long k = 0;
		_BitScanForward64(&k,b);
		KingField[i] = getKingField(i,k);

		if(!isEG)
		{
			if(getRank(getColorMirror(i,k))<2)
			{
				if(pos.Squares[getColorMirror(i,getPlus8(getColorMirror(i,k)))]==getPiece2Square(PIECE_PAWN,i))
				{
					KingSafety[i] += PawnShield1Bonus;
					if (Trace)
						cout << "Bonus for Pawn in front of king for " << PlayerStrings[i] << ": " << PawnShield1Bonus << endl;
				}
				else if(pos.Squares[getColorMirror(i,getPlus8(getColorMirror(i,k)))]==getPiece2Square(PIECE_BISHOP,i))
				{
					KingSafety[i] += BishopShieldBonus;
					if (Trace)
						cout << "Bonus for Bishop in front of king for " << PlayerStrings[i] << ": " << BishopShieldBonus << endl;
				}
				if(pos.Squares[getColorMirror(i,getPlus8(getPlus8(getColorMirror(i,k))))]==getPiece2Square(PIECE_PAWN,i))
				{
					KingSafety[i] += PawnShield2Bonus;
					if (Trace)
						cout << "Bonus for Pawn 2 steps in front of king for " << PlayerStrings[i] << ": " << PawnShield2Bonus << endl;
				}
				if(getFile(k)!=7)
				{
					if(pos.Squares[k+7*ColorFactor[i]]==getPiece2Square(PIECE_PAWN,i))
					{
						KingSafety[i] += PawnShield1Bonus;
						if (Trace)
							cout << "Bonus for Pawn in front of king for " << PlayerStrings[i] << ": " << PawnShield1Bonus << endl;
					}
					if(pos.Squares[k+15*ColorFactor[i]]==getPiece2Square(PIECE_PAWN,i))
					{
						KingSafety[i] += PawnShield2Bonus;
						if (Trace)
							cout << "Bonus for Pawn 2 steps in front of king for " << PlayerStrings[i] << ": " << PawnShield2Bonus << endl;
					}
				}
				if(getFile(k)!=0)
				{
					if(pos.Squares[k+9*ColorFactor[i]]==getPiece2Square(PIECE_PAWN,i))
					{
						KingSafety[i] += PawnShield1Bonus;
						if (Trace)
							cout << "Bonus for Pawn in front of king for " << PlayerStrings[i] << ": " << PawnShield1Bonus << endl;
					}
					if(pos.Squares[k+17*ColorFactor[i]]==getPiece2Square(PIECE_PAWN,i))
					{
						KingSafety[i] += PawnShield2Bonus;
						if (Trace)
							cout << "Bonus for Pawn 2 steps in front of king for " << PlayerStrings[i] << ": " << PawnShield2Bonus << endl;
					}
				}
			}
		}

		if((getAboveBits(i,k)&pos.Pieces[i][PIECE_PAWN])==0) //checks if there arent friendly pawns in the same file
		{
			if(getAboveBits(i,k)&pos.Pieces[getOpponent(i)][PIECE_PAWN]) //checks if there are opponent pawns in the same file
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
		if((getAboveBits(i,k)&pos.Pieces[getOpponent(i)][PIECE_PAWN])==0 && (getAboveBits(i,k)&pos.Pieces[getOpponent(i)][PIECE_ROOK])!=0)
		{ //checks if there are no opponent pawns on the file and there are rooks on the file
			KingSafety[i] -= KingOnRookFilePenalty;
			if (Trace)
				cout << "Penalty for King on rook file for " << PlayerStrings[i] << ": " << KingOnRookFilePenalty << endl;
			//KingAttackUnits[i] += 2;
		}
		if(getFile(k)!=0)
		{
			if((getAboveBits(i,k+1)&pos.Pieces[i][PIECE_PAWN])==0) //checks if there arent friendly pawns in the same file
			{
				if(getAboveBits(i,k+1)&pos.Pieces[getOpponent(i)][PIECE_PAWN]) //checks if there are opponent pawns in the same file
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
			if((getAboveBits(i,k+1)&pos.Pieces[getOpponent(i)][PIECE_PAWN])==0 && (getAboveBits(i,k+1)&pos.Pieces[getOpponent(i)][PIECE_ROOK])!=0)
			{ //checks if there are no opponent pawns on the file and there are rooks on the file
				KingSafety[i] -= KingAdjRookFilePenalty;
				if (Trace)
					cout << "Penalty for King adj to rook file for " << PlayerStrings[i] << ": " << KingAdjRookFilePenalty << endl;
				//KingAttackUnits[i] += 1;
			}
		}
		if(getFile(k)!=7)
		{
			if((getAboveBits(i,k-1)&pos.Pieces[i][PIECE_PAWN])==0) //checks if there arent friendly pawns in the same file
			{
				if(getAboveBits(i,k-1)&pos.Pieces[getOpponent(i)][PIECE_PAWN]) //checks if there are opponent pawns in the same file
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
			if((getAboveBits(i,k-1)&pos.Pieces[getOpponent(i)][PIECE_PAWN])==0 && (getAboveBits(i,k-1)&pos.Pieces[getOpponent(i)][PIECE_ROOK])!=0)
			{ //checks if there are no opponent pawns on the file and there are rooks on the file
				KingSafety[i] -= KingAdjRookFilePenalty;
				if (Trace)
					cout << "Penalty for King adj to rook file for " << PlayerStrings[i] << ": " << KingAdjRookFilePenalty << endl;
				//KingAttackUnits[i] += 1;
			}
		}
	}

	//Lazy eval
	/*int lazy;
	if(pos.turn==COLOR_WHITE)
	{
		lazy = sideeval[COLOR_WHITE]-sideeval[COLOR_BLACK];
	}
	else
	{
		lazy = sideeval[COLOR_BLACK] - sideeval[COLOR_WHITE];
	}
	if((lazy + LazyEval1 < alpha) || (lazy - LazyEval1 > beta))
	{
		return lazy;
	}*/

	//Positional evaluation
	//Pawn Evaluation
	if (Trace)
		cout << endl << "Pawn Structure:" << endl;
	for (int i = 0;i < 2;i++)
	{
		b = pos.Pieces[i][PIECE_PAWN];
		PawnCount[i] = popcnt(b);
		PawnCountColor[i][COLOR_WHITE] = popcnt(b&ColoredSquares[COLOR_WHITE]);
		PawnCountColor[i][COLOR_BLACK] = popcnt(b&ColoredSquares[COLOR_BLACK]);
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
			if (getAboveBits(i, k)&pos.Pieces[i][PIECE_PAWN]) //checks if there are friendly pawns in the same file
			{
				PawnStructure[i] -= DoubledPawnPenalty[getFile(getColorMirror(i, k))];
				if (Trace)
					cout << "Penalty for doubled pawn on " << Int2Sq(k) << " for " << PlayerStrings[i] << ": " << DoubledPawnPenalty[getFile(getColorMirror(i, k))] << endl;
			}

			if ((getAboveAndAboveSideBits(i, k)&pos.Pieces[getOpponent(i)][PIECE_PAWN]) == 0) //checks if the pawn is a passer
			{
				PawnStructure[i] += PassedPawnBonus[getColorMirror(i, k)];
				if (Trace)
					cout << "Bonus for Passed pawn on " << Int2Sq(k) << " for " << PlayerStrings[i] << ": " << PassedPawnBonus[getColorMirror(i, k)] << endl;

				if ((getSideBits(k)&pos.Pieces[i][PIECE_PAWN]) == 0) //checks if there are no friendly pawns on the adjacent files
				{
					PawnStructure[i] -= IsolatedPawnPenalty[getFile(getColorMirror(i, k))];
					if (Trace)
						cout << "Penalty for isolated pawn on " << Int2Sq(k) << " for " << PlayerStrings[i] << ": " << IsolatedPawnPenalty[getFile(getColorMirror(i, k))] << endl;

					if (isEG)
					{
						PawnStructure[i] += (unsigned)PassedPawnBonus[getColorMirror(i, k)] / 2; //extra bonus in endgame
						if (Trace)
							cout << "Extra endgame Bonus for Passed pawn on " << Int2Sq(k) << " for " << PlayerStrings[i] << ": " << PassedPawnBonus[getColorMirror(i, k)]/2 << endl;
					}
				}
				else
				{
					if (isEG)
					{
						PawnStructure[i] += PassedPawnBonus[getColorMirror(i, k)]; //protected passed pawn
						if (Trace)
							cout << "Extra endgame Bonus for protected Passed pawn on " << Int2Sq(k) << " for " << PlayerStrings[i] << ": " << PassedPawnBonus[getColorMirror(i, k)] << endl;
					}
					else
					{
						PawnStructure[i] += (unsigned)PassedPawnBonus[getColorMirror(i, k)] / 2;
						if (Trace)
							cout << "Extra Bonus for protected Passed pawn on " << Int2Sq(k) << " for " << PlayerStrings[i] << ": " << PassedPawnBonus[getColorMirror(i, k)]/2 << endl;
					}
				}

				if (getAboveBits(getOpponent(i), k)&pos.Pieces[i][PIECE_ROOK]) //rook behind passer
				{
					PawnStructure[i] += RookBehindPassedPawnBonus;
					if (Trace)
						cout << "Bonus for Rook behind Passed pawn on " << Int2Sq(k) << " for " << PlayerStrings[i] << ": " << RookBehindPassedPawnBonus << endl;
				}
				if (getAboveBits(getOpponent(i), k)&pos.Pieces[getOpponent(i)][PIECE_ROOK])
				{
					PawnStructure[i] -= OppRookBehindPassedPawnPenalty;
					if (Trace)
						cout << "Penalty for Rook behind Passed pawn on " << Int2Sq(k) << " for " << PlayerStrings[i] << ": " << OppRookBehindPassedPawnPenalty << endl;
				}
			}
			else if ((getSideBits(k)&pos.Pieces[i][PIECE_PAWN]) == 0) //checks if there are no friendly pawns on the adjacent files
			{
				PawnStructure[i] -= IsolatedPawnPenalty[getFile(getColorMirror(i, k))];
				if (Trace)
					cout << "Penalty for isolated pawn on " << Int2Sq(k) << " for " << PlayerStrings[i] << ": " << IsolatedPawnPenalty[getFile(getColorMirror(i, k))] << endl;
			}

			if (pos.Squares[getColorMirror(i, getPlus8(getColorMirror(i, k)))] != SQUARE_EMPTY) //checks if pawn is blocked
			{
				PawnStructure[i] -= BlockedPawnPenalty[getColorMirror(i, k)];
				if (Trace)
					cout << "Penalty for blocked pawn on " << Int2Sq(k) << " for " << PlayerStrings[i] << ": " << BlockedPawnPenalty[getColorMirror(i, k)] << endl;
			}
			//if (pos.OccupiedSq&getPos2Bit(getColorMirror(i,getPlus8(getColorMirror(i, k))))) //checks if pawn is blocked
			//{
			//	PawnStructure[i] -= BlockedPawnPenalty[getColorMirror(i, k)];
			//	if (Trace)
			//		cout << "Penalty for blocked pawn on " << Int2Sq(k) << " for " << PlayerStrings[i] << ": " << BlockedPawnPenalty[getColorMirror(i, k)] << endl;
			//}

			if ((getAboveSideBits(getOpponent(i), k)&pos.Pieces[i][PIECE_PAWN])==0) //checks if pawn is backward
			{
				if (getRank(getColorMirror(i, k)) < 5)
				{
					if (getPawnAttacks(i, getColorMirror(i, getPlus8(getColorMirror(i, k))))&pos.Pieces[getOpponent(i)][PIECE_PAWN]
						&& pos.Squares[getColorMirror(i, getPlus8(getColorMirror(i, k)))]==SQUARE_EMPTY)
					{
						PawnStructure[i] -= BackwardPawnPenalty[getFile(k)];
						if (Trace)
							cout << "Penalty for backward pawn on " << Int2Sq(k) << " for " << PlayerStrings[i] << ": " << BackwardPawnPenalty[getFile(k)] << endl;
					}
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
			Bitset m = getRookRankMoves(k,(pos.OccupiedSq>>(getRankOffset(k)))&0xff);
			m |= getRookFileMoves(k,(pos.OccupiedSq90>>(getFileOffset(k)))&0xff);

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
				//unsigned int outpostbonus = KnightOutpostBonus[getRank(getColorMirror(i, k))];
				unsigned int outpostbonus = PieceSqValues[PIECE_KNIGHT][getColorMirror(i, k)];
				if (Trace)
					cout << "Bonus for Knight outpost on " << Int2Sq(k) << " for " << PlayerStrings[i] << ":" << PieceSqValues[PIECE_KNIGHT][getColorMirror(i, k)] << endl;
				int piececolor = SquareColor[k];
				if (pos.Pieces[getOpponent(i)][PIECE_KNIGHT] == 0 && (pos.Pieces[getOpponent(i)][PIECE_BISHOP] & ColoredSquares[piececolor]) == 0)
				{
					outpostbonus += PieceSqValues[PIECE_KNIGHT][getColorMirror(i, k)]; //double bonus if there are no opponent minor pieces
					if (Trace)
						cout << "	Extra bonus because there are no opponent minor pieces that can capture it: " << PieceSqValues[PIECE_KNIGHT][getColorMirror(i, k)] << endl;
				}
				if (pos.Pieces[i][PIECE_PAWN] & PawnAttacks[getOpponent(i)][k] != 0)
				{
					outpostbonus += ((unsigned)PieceSqValues[PIECE_KNIGHT][getColorMirror(i, k)] / 2); //extra bonus if defended by a pawn
					if (Trace)
						cout << "	Extra bonus because its defended by a pawn: " << PieceSqValues[PIECE_KNIGHT][getColorMirror(i, k)]/2 << endl;
				}
				if (isEG)
				{
					outpostbonus /= 2; //half bonus in endgames
					if (Trace)
						cout << "	Its an endgame so this outpost bonus is halved" << endl;
				}
				PieceActivity[i] += outpostbonus;
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
				//unsigned int outpostbonus = BishopOutpostBonus[getRank(getColorMirror(i, k))];
				unsigned int outpostbonus = PieceSqValues[PIECE_BISHOP][getColorMirror(i, k)];
				if (Trace)
					cout << "Bonus for Bishop outpost on " << Int2Sq(k) << " for " << PlayerStrings[i] << ":" << PieceSqValues[PIECE_BISHOP][getColorMirror(i, k)];
				int piececolor = SquareColor[k];
				if (pos.Pieces[getOpponent(i)][PIECE_KNIGHT] == 0 && (pos.Pieces[getOpponent(i)][PIECE_BISHOP]&ColoredSquares[piececolor])==0)
				{
					outpostbonus += PieceSqValues[PIECE_BISHOP][getColorMirror(i, k)]; //double bonus if there are no opponent minor pieces
					if (Trace)
						cout << "	Extra bonus because there are no opponent minor pieces that can capture it: " << PieceSqValues[PIECE_BISHOP][getColorMirror(i, k)] << endl;
				}
				if (pos.Pieces[i][PIECE_PAWN] & PawnAttacks[getOpponent(i)][k] != 0)
				{
					outpostbonus += (unsigned)(PieceSqValues[PIECE_BISHOP][getColorMirror(i, k)])>>1; //extra bonus if defended by a pawn
					if (Trace)
						cout << "	Extra bonus because its defended by a pawn: " << PieceSqValues[PIECE_BISHOP][getColorMirror(i, k)] / 2 << endl;
				}
				if (isEG)
				{
					outpostbonus /= 2;
					if (Trace)
						cout << "	Its an endgame so this outpost bonus is halved" << endl;
				}
				PieceActivity[i] += outpostbonus;
			}

			//bishop attacks near opposing king
			Bitset m = getBishopA1H8Moves(k,(pos.OccupiedSq135>>getDiag(getturn135(k)))&0xff);
			m |= getBishopA8H1Moves(k,(pos.OccupiedSq45>>getDiag(getturn45(k)))&0xff);

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
			Bitset m = getRookRankMoves(k,(pos.OccupiedSq>>(getRankOffset(k)))&0xff);
			m |= getRookFileMoves(k,(pos.OccupiedSq90>>(getFileOffset(k)))&0xff);
			m |= getBishopA1H8Moves(k,(pos.OccupiedSq135>>getDiag(getturn135(k)))&0xff);
			m |= getBishopA8H1Moves(k,(pos.OccupiedSq45>>getDiag(getturn45(k)))&0xff);

			//PieceActivity[i] += popcnt(m&CenterBits)*CenterSquareBonus;
			//PieceActivity[i] += popcnt(m&CenterBorderBits)*CenterBorderSquareBonus;
			//PieceActivity[i] += popcnt(m&EnemyTerritory[i])*EnemyTerritorySquareBonus;

			m &= m^ColorPieces[i];

			PieceActivity[i] += QueenMobility[popcnt(m)]; //mobility factor
			if (Trace)
				cout << "Queen on " << Int2Sq(k) << " mobility bonus for " << PlayerStrings[i] << ": " << QueenMobility[popcnt(m)] << endl;

			//eval += ColorFactor[i]*popcnt(KingField[getOpponent(i)]&m)*AttackWeights[PIECE_QUEEN];
			KingAttackUnits[getOpponent(i)] += long(popcnt(KingField[getOpponent(i)]&m))*AttackWeights[PIECE_QUEEN];

			if (Trace && popcnt(KingField[getOpponent(i)] & m) != 0)
				cout << "King attack units penalty for queen on " << Int2Sq(k) << " for " << PlayerStrings[getOpponent(i)] << ": " << popcnt(KingField[getOpponent(i)] & m)*AttackWeights[PIECE_QUEEN] << endl;

			if(!isEG)
			{
				if(getRank(getColorMirror(i,k))>1)
				{
					PieceActivity[i] -= QueenOutEarlyPenalty*MinorsOnBackRank[i];
					if (Trace)
						cout << "Queen out early penalty for " << PlayerStrings[i] << ": " << QueenOutEarlyPenalty*MinorsOnBackRank[i] << endl;
				}
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

	for (int i = 0;i < 2;i++)
	{
		PieceActivity[i] = PieceActivity[i] * PieceActivityFactor;
		KingSafety[i] = (KingSafety[i] * KingSafetyFactor * currentMaterial) / TotalMaterial;
		PawnStructure[i] = ((PawnStructure[i] * PawnStructureFactor)/2) + (((PawnStructure[i]*PawnStructureFactor) * (TotalMaterial-currentMaterial)) / (TotalMaterial*2));
	}

	if (Trace)
	{
		cout << "After scaling:" << endl;
		for (int i = 0;i < 2;i++)
		{
			cout << PlayerStrings[i] << " King Safety: " << KingSafety[i] << endl;
			cout << PlayerStrings[i] << " Pawn Structure: " << PawnStructure[i] << endl;
			cout << PlayerStrings[i] << " Piece Activity: " << PieceActivity[i] << endl;
			cout << endl;
		}
	}

	neteval += Material[COLOR_WHITE] + KingSafety[COLOR_WHITE] + PawnStructure[COLOR_WHITE] + PieceActivity[COLOR_WHITE];
	neteval -= Material[COLOR_BLACK] + KingSafety[COLOR_BLACK] + PawnStructure[COLOR_BLACK] + PieceActivity[COLOR_BLACK];

	if(pos.turn==COLOR_BLACK)
		return -neteval;

	//evaltime.Stop();
	return neteval;
}

template <int Color>
int Engine::getBoardMaterial()
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

int Engine::StaticExchangeEvaluation(int to, int from,int movpiece,int capt)
{
	capt = getSquare2Piece(capt);
	/*if (PieceMaterial[capt] >= PieceMaterial[movpiece])
	{
		return PieceMaterial[capt] - PieceMaterial[movpiece];
	}*/
	int gain[100],d=0;
    Bitset occ = pos.OccupiedSq;
    Bitset occ90 = pos.OccupiedSq90;
	Bitset occ45 = pos.OccupiedSq45;
    Bitset occ135 = pos.OccupiedSq135;
	Bitset pieces[2][6];
	for(int i = 0;i<2;i++)
	{
		for(int j = 0;j<6;j++)
		{
			pieces[i][j] = pos.Pieces[i][j];
		}
	}
	int turn = pos.turn;
	gain[d] = PieceMaterial[capt];
	//cout << "gain " << d << " is " << gain[0] << endl;
	Move m = CONS_NULLMOVE;
	do
	{
		//cout << m.toString() << movpiece << endl;
		d++; // next depth and side
        gain[d] = PieceMaterial[movpiece] - gain[d-1]; // speculative store, if defended
		if (max (-gain[d-1], gain[d]) < 0) break; // pruning does not influence the result
		//cout << "gain " << d << " is " << gain[d] << endl;

        pos.OccupiedSq ^= getPos2Bit(from); // reset bit in temporary occupancy (for x-Rays)
		pos.OccupiedSq90 ^= getPos2Bit(getturn90(from));
		pos.OccupiedSq45 ^= getPos2Bit(getturn45(from));
		pos.OccupiedSq135 ^= getPos2Bit(getturn135(from));
		pos.Pieces[turn][movpiece] ^= getPos2Bit(from);
		turn = getOpponent(turn);

		m = pos.getSmallestAttacker(turn,to);
		from = m.getFrom();
		capt = movpiece;
		movpiece = m.getMovingPiece();
	}while(m.isNullMove()==false);

    pos.OccupiedSq = occ;
    pos.OccupiedSq90 = occ90;
	pos.OccupiedSq45 = occ45;
    pos.OccupiedSq135 = occ135;
	for(int i = 0;i<2;i++)
	{
		for(int j = 0;j<6;j++)
		{
			pos.Pieces[i][j] = pieces[i][j];
		}
	}
	//cout << "gain " << d << " is " << gain[d] << endl;
	while(--d)
	{
        gain[d-1]= -max(-gain[d-1], gain[d]);
	    //d--;
		//cout << "gain " << d << " is " << gain[d] << endl;
	}
	return gain[0];
}

//int Engine::StaticExchangeEvaluation2(Move m)
//{
//	//cout << "see " << m.toString() << endl;
//	int value = 0;
//	if(m.getSpecial()!=PIECE_PAWN)
//	{
//		value = MaterialValues[getSquare2Piece(m.getCapturedPiece())+1];
//	}
//	else
//	{
//		value = MaterialValues[SQUARE_WHITEPAWN];
//	}
//	pos.forceMove(m);
//	int piece = pos.getSmallestAttacker2(pos.turn,m.getTo());
//	if(piece!=PIECE_NONE)
//	{
//		Move mov = pos.makeCapture(piece,m.getTo());
//		//cout << "move " << mov.toString() << endl;
//		//int piece = mov.getMovingPiece();
//		if(mov!=CONS_NULLMOVE)
//		{
//			value -= StaticExchangeEvaluation2(mov);
//		}
//	}
//	pos.unmakeMove(m);
//	return value;
//}
//
//int Engine::StaticExchangeEvaluation(Move m)
//{
//	//cout << "see " << m.toString() << endl;
//	int value = 0;
//	if(m.getSpecial()!=PIECE_PAWN)
//	{
//		value = MaterialValues[getSquare2Piece(m.getCapturedPiece())+1];
//	}
//	else
//	{
//		value = MaterialValues[SQUARE_WHITEPAWN];
//	}
//	pos.forceMove(m);
//	Move mov = pos.getSmallestAttacker(pos.turn,m.getTo());
//	if(mov!=CONS_NULLMOVE)
//	{
//		//cout << "move " << mov.toString() << endl;
//		int piece = mov.getMovingPiece();
//		if(piece!=PIECE_NONE)
//		{
//			value -= StaticExchangeEvaluation(mov);
//		}
//	}
//	pos.unmakeMove(m);
//	return value;
//}
//
//int Engine::StaticExchangeEvaluation(int square,int side)
//{
//	int value = 0;
//	int piece = pos.getSmallestAttacker2(side,square);
//	if(piece!=PIECE_NONE)
//	{
//		Move m = pos.makeCapture(piece,square);
//		if(m!=CONS_NULLMOVE)
//		{
//			pos.forceMove(m);
//			value = max(0,MaterialValues[piece] - StaticExchangeEvaluation(square,getOpponent(side)));
//			pos.unmakeMove(m);
//		}
//	}
//	return value;
//}

void evalinit()
{
	for(int i = 0;i<13;i++)
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
				PieceSq[i][j] = PieceSqValues[i][j]-avg;
				PieceSqEG[i][j] = PieceSqValuesEG[i][j]-avg;
				//cout << PieceSq[i][j] << " ";
			}
			else
			{
				PieceSq[i][j] = -PieceSqValues[i-6][getMirror(j)]+avg;
				PieceSqEG[i][j] = -PieceSqValuesEG[i-6][getMirror(j)]+avgEG;
				//cout << PieceSq[i][j] << " ";
			}
			/*if((j+1)%8==0)
				cout << endl;*/
		}
	}
}

//int Engine::Trace(int alpha,int beta)
//{
//	//nodes++;
//	//if(nodes%1024==0)
//	//{
//	//	checkup();
//	//	//nodes = 0;
//	//}
//	int eval = 0;
//	//Bitset Pieces = pos.OccupiedSq ^ pos.Pieces[COLOR_WHITE][PIECE_PAWN] ^ pos.Pieces[COLOR_BLACK][PIECE_PAWN];
//	//int pieceCount = popcnt(Pieces);
//	bool isEG = false;
//	if(getBoardMaterial() <= EndgameMaterial)
//	{
//		isEG = true;
//		cout << "It is endgame" << endl;
//	}
//
//	//Material eval
//	cout << "Material:" << endl;
//	for(int i = 0;i<64;i++)
//	{
//		eval += MaterialValues[pos.Squares[i]];
//		if(isEG)
//		{
//			eval += PieceSqEG[pos.Squares[i]][i];
//			cout << PieceSqEG[pos.Squares[i]][i] << " for " << Int2Sq(i) << endl;
//		}
//		else
//		{
//			eval += PieceSq[pos.Squares[i]][i];
//			cout << PieceSq[pos.Squares[i]][i] << " for " << Int2Sq(i) << endl;
//		}
//	}
//
//	Bitset b = 0x0;
//	Bitset KingField[2];
//	Bitset ColorPieces[2];
//	int MinorsOnBackRank[2] = {0,0};
//	cout << "King Safety: " << endl;
//	for(int i = 0;i<2;i++) //king safety
//	{
//		b = pos.Pieces[i][PIECE_KING];
//		unsigned long k = 0;
//		_BitScanForward64(&k,b);
//		KingField[i] = getKingField(i,k);
//		ColorPieces[i] = pos.Pieces[i][PIECE_PAWN] | pos.Pieces[i][PIECE_KNIGHT] |
//                         pos.Pieces[i][PIECE_BISHOP] | pos.Pieces[i][PIECE_ROOK] |
//                         pos.Pieces[i][PIECE_QUEEN] | pos.Pieces[i][PIECE_KING];
//
//		if(!isEG)
//		{
//			if(getRank(getColorMirror(i,k))<2)
//			{
//				if(pos.Squares[getColorMirror(i,getPlus8(getColorMirror(i,k)))]==getPiece2Square(PIECE_PAWN,i))
//				{
//					eval += ColorFactor[i]*PawnShield1Bonus;
//					cout << ColorFactor[i]*PawnShield1Bonus << " for pawn shield 1 on " << Int2Sq(k) << endl;
//				}
//				else if(pos.Squares[getColorMirror(i,getPlus8(getColorMirror(i,k)))]==getPiece2Square(PIECE_BISHOP,i))
//				{
//					eval += ColorFactor[i]*BishopShieldBonus;
//					cout << ColorFactor[i]*BishopShieldBonus << " for bishop shield on " << Int2Sq(k) << endl;
//				}
//				if(pos.Squares[getColorMirror(i,getPlus8(getPlus8(getColorMirror(i,k))))]==getPiece2Square(PIECE_PAWN,i))
//				{
//					eval += ColorFactor[i]*PawnShield2Bonus;
//					cout << ColorFactor[i]*PawnShield2Bonus << " for pawn shield 2 on " << Int2Sq(k) << endl;
//				}
//				if(getFile(k)!=7)
//				{
//					if(pos.Squares[k+7*ColorFactor[i]]==getPiece2Square(PIECE_PAWN,i))
//					{
//						eval += ColorFactor[i]*PawnShield1Bonus;
//						cout << ColorFactor[i]*PawnShield1Bonus << " for pawn shield 1 on " << Int2Sq(k) << endl;
//					}
//					if(pos.Squares[k+15*ColorFactor[i]]==getPiece2Square(PIECE_PAWN,i))
//					{
//						eval += ColorFactor[i]*PawnShield2Bonus;
//						cout << ColorFactor[i]*PawnShield2Bonus << " for pawn shield 2 on " << Int2Sq(k) << endl;
//					}
//				}
//				if(getFile(k)!=0)
//				{
//					if(pos.Squares[k+9*ColorFactor[i]]==getPiece2Square(PIECE_PAWN,i))
//					{
//						eval += ColorFactor[i]*PawnShield1Bonus;
//						cout << ColorFactor[i]*PawnShield1Bonus << " for pawn shield 1 on " << Int2Sq(k) << endl;
//					}
//					if(pos.Squares[k+17*ColorFactor[i]]==getPiece2Square(PIECE_PAWN,i))
//					{
//						eval += ColorFactor[i]*PawnShield2Bonus;
//						cout << ColorFactor[i]*PawnShield2Bonus << " for pawn shield 2 on " << Int2Sq(k) << endl;
//					}
//				}
//			}
//		}
//
//		if((getAboveBits(i,k)&pos.Pieces[i][PIECE_PAWN])==0) //checks if there arent friendly pawns in the same file
//		{
//			if(getAboveBits(i,k)&pos.Pieces[getOpponent(i)][PIECE_PAWN]) //checks if there are opponent pawns in the same file
//			{
//				eval -= ColorFactor[i]*KingOnHalfOpenFilePenalty;
//				cout << ColorFactor[i]*KingOnHalfOpenFilePenalty << " for king on half open on " << Int2Sq(k) << endl;
//			}
//			else
//			{
//				eval -= ColorFactor[i]*KingOnOpenFilePenalty;
//				cout << ColorFactor[i]*KingOnOpenFilePenalty << " for king on open on " << Int2Sq(k) << endl;
//			}
//		}
//		if((getAboveBits(i,k)&pos.Pieces[getOpponent(i)][PIECE_PAWN]==0) && (getAboveBits(i,k)&pos.Pieces[getOpponent(i)][PIECE_ROOK]!=0))
//		{ //checks if there are no opponent pawns on the file and there are rooks on the file
//			eval -= ColorFactor[i]*KingOnRookFilePenalty;
//			cout << ColorFactor[i]*KingOnRookFilePenalty << " for king on rook file on " << Int2Sq(k) << endl;
//		}
//		if(getFile(k)!=0)
//		{
//			if((getAboveBits(i,k+1)&pos.Pieces[i][PIECE_PAWN])==0) //checks if there arent friendly pawns in the same file
//			{
//				if(getAboveBits(i,k+1)&pos.Pieces[getOpponent(i)][PIECE_PAWN]) //checks if there are opponent pawns in the same file
//				{
//					eval -= ColorFactor[i]*KingAdjHalfOpenFilePenalty;
//					cout << ColorFactor[i]*KingAdjHalfOpenFilePenalty << " for king adj half open file on " << Int2Sq(k) << endl;
//				}
//				else
//				{
//					eval -= ColorFactor[i]*KingAdjOpenFilePenalty;
//					cout << ColorFactor[i]*KingAdjOpenFilePenalty << " for king adj open file on " << Int2Sq(k) << endl;
//				}
//			}
//			if((getAboveBits(i,k+1)&pos.Pieces[getOpponent(i)][PIECE_PAWN])==0 && (getAboveBits(i,k+1)&pos.Pieces[getOpponent(i)][PIECE_ROOK]!=0))
//			{ //checks if there are no opponent pawns on the file and there are rooks on the file
//				eval -= ColorFactor[i]*KingAdjRookFilePenalty;
//				cout << ColorFactor[i]*KingAdjRookFilePenalty << " for king adj rook file on " << Int2Sq(k) << endl;
//			}
//		}
//		if(getFile(k)!=7)
//		{
//			if((getAboveBits(i,k-1)&pos.Pieces[i][PIECE_PAWN])==0) //checks if there arent friendly pawns in the same file
//			{
//				if(getAboveBits(i,k-1)&pos.Pieces[getOpponent(i)][PIECE_PAWN]) //checks if there are opponent pawns in the same file
//				{
//					eval -= ColorFactor[i]*KingAdjHalfOpenFilePenalty;
//					cout << ColorFactor[i]*KingAdjHalfOpenFilePenalty << " for king adj half open file on " << Int2Sq(k) << endl;
//				}
//				else
//				{
//					eval -= ColorFactor[i]*KingAdjOpenFilePenalty;
//					cout << ColorFactor[i]*KingAdjOpenFilePenalty << " for king adj open file on " << Int2Sq(k) << endl;
//				}
//			}
//			if(getAboveBits(i,k-1)&pos.Pieces[getOpponent(i)][PIECE_PAWN]==0 && getAboveBits(i,k-1)&pos.Pieces[getOpponent(i)][PIECE_ROOK]!=0)
//			{ //checks if there are no opponent pawns on the file and there are rooks on the file
//				eval -= ColorFactor[i]*KingAdjRookFilePenalty;
//				cout << ColorFactor[i]*KingAdjRookFilePenalty << " for king adj rook file on " << Int2Sq(k) << endl;
//			}
//		}
//	}
//
//	//Lazy eval
//	cout << "Lazy eval checking, eval is " << eval << endl;
//	int lazy;
//	if(pos.turn==COLOR_WHITE)
//	{
//		lazy = eval;
//	}
//	else
//	{
//		lazy = -eval;
//	}
//	if((lazy + LazyEval1 < alpha) || (lazy - LazyEval1 > beta))
//	{
//		cout << "Lazy eval success" << endl;
//		return lazy;
//	}
//
//	cout << "Positional eval " << endl;
//	//Positional evaluation
//	for(int i = 0;i<2;i++)
//	{
//		//Pawns
//		b = pos.Pieces[i][PIECE_PAWN];
//		int pawncount = popcnt(b);
//		if(!b) //penalty for having no pawns
//		{
//			eval -= ColorFactor[i]*NoPawnsPenalty;
//			cout << ColorFactor[i]*NoPawnsPenalty << " for no pawns" << endl;
//		}
//		while(b)
//		{
//			//int k = firstOf(b);
//			unsigned long k = 0;
//			_BitScanForward64(&k,b);
//			b ^= getPos2Bit(k);
//			if(getAboveBits(i,k)&pos.Pieces[i][PIECE_PAWN]) //checks if there are friendly pawns in the same file
//			{
//				eval -= ColorFactor[i]*DoubledPawnPenalty[getFile(k)];
//				cout << ColorFactor[i]*DoubledPawnPenalty[getFile(k)] << " for doubled pawn on " << Int2Sq(k) << endl;
//			}
//			if((getAboveAndAboveSideBits(i,k)&pos.Pieces[getOpponent(i)][PIECE_PAWN])==0) //checks if the pawn is a passer
//			{
//				eval += ColorFactor[i]*PassedPawnBonus[getColorMirror(i,k)];
//				cout << ColorFactor[i]*PassedPawnBonus[getColorMirror(i,k)] << " for passed pawn on " << Int2Sq(k) << endl;
//			}
//			if((getSideBits(k)&pos.Pieces[i][PIECE_PAWN])==0) //checks if there are no friendly pawns on the adjacent files
//			{
//				eval -= ColorFactor[i]*IsolatedPawnPenalty[getFile(k)];
//				cout << ColorFactor[i]*IsolatedPawnPenalty[getFile(k)] << " for isolated pawn on " << Int2Sq(k) << endl;
//			}
//			if(pos.Squares[getColorMirror(i,getPlus8(getColorMirror(i,k)))]!=SQUARE_EMPTY) //checks if pawn is blocked
//			{
//				eval -= ColorFactor[i]*BlockedPawnPenalty[getColorMirror(i,k)];
//				cout << ColorFactor[i]*BlockedPawnPenalty[getColorMirror(i,k)] << " for blocked pawn on " << Int2Sq(k) << endl;
//			}
//
//			//pawn attacks near opposing king
//			eval += ColorFactor[i]*long(popcnt(KingField[getOpponent(i)]&getPawnAttacks(i,k))*AttackWeights[PIECE_PAWN]);
//			cout << ColorFactor[i]*long(popcnt(KingField[getOpponent(i)]&getPawnAttacks(i,k))*AttackWeights[PIECE_PAWN]) << " attacking pawn on " << Int2Sq(k) << endl;
//		}
//
//		//Rooks
//		b = pos.Pieces[i][PIECE_ROOK];
//		int rookcount = popcnt(b);
//		eval += ColorFactor[i]*RookPawnAdj[pawncount]*rookcount; //adjustment for pawns
//		if(rookcount >= 2) //Pair Bonus
//		{
//			eval += ColorFactor[i]*RookPairBonus;
//		}
//		while(b)
//		{
//			//int k = firstOf(b);
//			unsigned long k = 0;
//			_BitScanForward64(&k,b);
//			b ^= getPos2Bit(k);
//			if((getAboveBits(i,k)&pos.Pieces[i][PIECE_PAWN])==0) //checks if there arent friendly pawns in the same file
//			{
//				if(getAboveBits(i,k)&pos.Pieces[getOpponent(i)][PIECE_PAWN]) //checks if there are opponent pawns in the same file
//				{
//					eval += ColorFactor[i]*RookHalfOpenBonus[getFile(k)];
//					cout << ColorFactor[i]*RookHalfOpenBonus[getFile(k)] << " for half open file rook on " << Int2Sq(k) << endl;
//				}
//				else
//				{
//					eval += ColorFactor[i]*RookOpenBonus[getFile(k)];
//					cout << ColorFactor[i]*RookOpenBonus[getFile(k)] << " for open file rook on " << Int2Sq(k) << endl;
//				}
//			}
//
//			//Rook Attacks
//			Bitset m = getRookRankMoves(k,(pos.OccupiedSq>>(getRankOffset(k)))&0xff);
//			m |= getRookFileMoves(k,(pos.OccupiedSq90>>(getFileOffset(k)))&0xff);
//			m &= m^ColorPieces[i];
//			eval += ColorFactor[i]*popcnt(KingField[getOpponent(i)]&m)*AttackWeights[PIECE_ROOK];
//			cout << ColorFactor[i]*popcnt(KingField[getOpponent(i)]&m)*AttackWeights[PIECE_ROOK] << " attacking rook on " << Int2Sq(k) << endl;
//		}
//
//		//Knights
//		b = pos.Pieces[i][PIECE_KNIGHT];
//		int knightcount = popcnt(b);
//		eval += ColorFactor[i]*KnightPawnAdj[pawncount]*knightcount; //adjustment for pawns
//		if(popcnt(b) >= 2)
//		{
//			eval += ColorFactor[i]*KnightPairBonus;
//			cout << ColorFactor[i]*KnightPairBonus << " for knight pair " << endl;
//		}
//		while(b)
//		{
//			unsigned long k = 0;
//			_BitScanForward64(&k,b);
//			b ^= getPos2Bit(k);
//			if((getAboveSideBits(i,k)&pos.Pieces[getOpponent(i)][PIECE_PAWN])==0) //checks if there are no enemy pawns on the adjacent files
//			{
//				eval += ColorFactor[i]*KnightOutpostBonus[getRank(getColorMirror(i,k))];
//				cout << ColorFactor[i]* KnightOutpostBonus[getRank(getColorMirror(i, k))] << " for knight outpost " << Int2Sq(k) << endl;
//			}
//
//			//knight attacks near opposing king
//			eval += ColorFactor[i]*popcnt(KingField[getOpponent(i)]&getKnightMoves(k))*AttackWeights[PIECE_KNIGHT];
//			cout << ColorFactor[i]*popcnt(KingField[getOpponent(i)]&getKnightMoves(k))*AttackWeights[PIECE_KNIGHT] << " attacking knight on " << Int2Sq(k) << endl;
//			if(getRank(getColorMirror(i,k))==0)
//			{
//				MinorsOnBackRank[i]++;
//			}
//		}
//
//		//Bishops
//		b = pos.Pieces[i][PIECE_BISHOP];
//		int bishopcount = popcnt(b);
//		eval += ColorFactor[i]*BishopPawnAdj[pawncount]*bishopcount; //adjustment for pawns
//		if(popcnt(b) >= 2)
//		{
//			eval += ColorFactor[i]*BishopPairBonus;
//			cout << ColorFactor[i]*BishopPairBonus << " for bishop pair " << endl;
//		}
//		while(b)
//		{
//			unsigned long k = 0;
//			_BitScanForward64(&k,b);
//			b ^= getPos2Bit(k);
//			if((getAboveSideBits(i,k)&pos.Pieces[getOpponent(i)][PIECE_PAWN])==0) //checks if there are no enemy pawns on the adjacent files
//			{
//				eval += ColorFactor[i]* BishopOutpostBonus[getRank(getColorMirror(i, k))];
//				cout << ColorFactor[i]* BishopOutpostBonus[getRank(getColorMirror(i, k))] << " for bishop outpost " << Int2Sq(k) << endl;
//			}
//
//			//bishop attacks near opposing king
//			Bitset m = getBishopA1H8Moves(k,(pos.OccupiedSq135>>getDiag(getturn135(k)))&0xff);
//			m |= getBishopA8H1Moves(k,(pos.OccupiedSq45>>getDiag(getturn45(k)))&0xff);
//			m &= m^ColorPieces[i];
//			eval += ColorFactor[i]*popcnt(KingField[getOpponent(i)]&m)*AttackWeights[PIECE_BISHOP];
//			cout << ColorFactor[i]*popcnt(KingField[getOpponent(i)]&m)*AttackWeights[PIECE_BISHOP] << " attacking bishop on " << Int2Sq(k) << endl;
//			if(getRank(getColorMirror(i,k))==0)
//			{
//				MinorsOnBackRank[i]++;
//			}
//		}
//
//		//Queens
//		b = pos.Pieces[i][PIECE_QUEEN];
//		while(b)
//		{
//			unsigned long k = 0;
//			_BitScanForward64(&k,b);
//			b ^= getPos2Bit(k);
//
//			//queen attacks near opposing king
//			Bitset m = getRookRankMoves(k,(pos.OccupiedSq>>(getRankOffset(k)))&0xff);
//			m |= getRookFileMoves(k,(pos.OccupiedSq90>>(getFileOffset(k)))&0xff);
//			m |= getBishopA1H8Moves(k,(pos.OccupiedSq135>>getDiag(getturn135(k)))&0xff);
//			m |= getBishopA8H1Moves(k,(pos.OccupiedSq45>>getDiag(getturn45(k)))&0xff);
//			m &= m^ColorPieces[i];
//			eval += ColorFactor[i]*long(popcnt(KingField[getOpponent(i)]&m)*AttackWeights[PIECE_QUEEN]);
//			cout << ColorFactor[i]*long(popcnt(KingField[getOpponent(i)]&m)*AttackWeights[PIECE_QUEEN]) << " attacking queen on " << Int2Sq(k) << endl;
//
//			if(!isEG)
//			{
//				if(getRank(getColorMirror(i,k))!=0)
//				{
//					eval -= ColorFactor[i]*QueenOutEarlyPenalty*MinorsOnBackRank[i];
//					cout << "-" << ColorFactor[i]*QueenOutEarlyPenalty*MinorsOnBackRank[i] << " for queen out early " << MinorsOnBackRank[i] << endl;
//				}
//			}
//		}
//	}
//
//	if(pos.turn==COLOR_BLACK)
//		return -eval;
//	return eval;
//}