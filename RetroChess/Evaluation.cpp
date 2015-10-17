#include "Engine.h"

int ColorFactor[2] = {1,-1};

int PieceMaterial[7] = {100,325,335,500,975,-CONS_MATED,0};
int MaterialValues[13] = {0,100,325,335,500,975,-CONS_MATED,-100,-325,-335,-500,-975,CONS_MATED};

int EndgameMaterial = 3800;

int LazyEval1 = 400;

int BishopPairBonus = 45;
int KnightPairBonus = -5;
int RookPairBonus = 10;

int KnightOutpostBonus = 25;
int BishopOutpostBonus = 10;

int QueenOutEarlyPenalty = 20; //penalty for queens not on back rank for every minor on back rank

// mobility bonus based on how many squares the pieces attack
int BishopMobility[16]  = {-12, -4,  0,  2,  4,  4,  6,  6,  8,  8, 10, 10, 12, 14, 16};
int RookMobility[16]  = {-8,-4, 0, 2, 2, 2, 4, 4, 4, 6, 6, 6, 8, 8, 8};
int KnightMobility[9] = {-12, -8,  0,  4,  8, 10, 12, 14, 16};
 
// adjustments to piece values depending on number of pawns
int KnightAdj[9] = {-20,-16,-12, -8, -4,  0,  4,  8, 12};
int BishopAdj[9] = {0,0,0,0,0,0,0,0,0};
int RookAdj[9] =   {15,12, 9, 6, 3, 0,-3,-6,-9};

//adjustments to bishop values depending on number of pawns on same color square as bishop
int BishopPawnSameColor[9] = {15,12,9,6,3,0,-3,-6,-9};

//King Safety
int PawnShield1Bonus = 15;
int PawnShield2Bonus = 7;
int BishopShieldBonus = 9;
int KingOnHalfOpenFilePenalty = 15; //penalty for king being on half open files
int KingOnOpenFilePenalty = 35; //penalty for king being on open files
int KingAdjHalfOpenFilePenalty = 12; //penalty for king being adjacent to half open files
int KingAdjOpenFilePenalty = 26; //penalty for king being adjacent to open files
int KingOnRookFilePenalty = 10; //penalty for king being on an opponent semiopen file with a rook on it
int KingAdjRookFilePenalty = 5; //penalty for king being adjacent an opponent semiopen file with a rook on it
int AttackWeights[6] = {1,3,3,3,4,0};
int KingBetweenRooksPenalty = 30;
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
int DoubledPawnPenalty[8] = {13,17,20,23,23,20,17,13};
int IsolatedPawnPenalty[8] = {7,10,18,30,30,18,10,7};
int PassedPawnBonus[64] = {  0,  0,  0,  0,  0,  0,  0,  0,
						     5, 10, 10, 10, 10, 10, 10,  5,
						    10, 20, 20, 20, 20, 20, 20, 10,
                            20, 40, 40, 40, 40, 40, 40, 20,
                            30, 60, 60, 60, 60, 60, 60, 30,
							40, 80, 80, 80, 80, 80, 80, 40,
						    90,120,120,120,120,120,120, 90,
						   200,200,200,200,200,200,200,200};
int BlockedPawnPenalty[64] = {   0,  0,  0,  0,  0,  0,  0,  0,
	                             1,  1,  2, 15, 15,  5,  1,  1,
								 0,  0,  0,  5,  5,  0,  0,  0,
								 0,  0,  0,  0,  0,  0,  0,  0,
								 0,  0,  0,  0,  0,  0,  0,  0,
								 0,  0,  0,  0,  0,  0,  0,  0,
								 0,  0,  0,  0,  0,  0,  0,  0,
								 0,  0,  0,  0,  0,  0,  0,  0};

//Rooks
int RookHalfOpenBonus[8] = {15,15,20,30,30,20,15,15};
int RookOpenBonus[8] = {20,20,30,40,40,30,20,20};
int RookConnectedBonus = 10;

int PieceSq[13][64];
int PieceSqEG[13][64];
int PieceSqValues[7][64] = 
{
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //empty
	{  0,  0,  0,  0,  0,  0,  0,  0, //pawn
	  -4,  0,  0,-10,-10,  0,  0, -4,
	  -6, -2,  2, 14, 14,  5, -2, -6,
	  -8, -4,  4, 40, 40, 15, -4, -8,
	 -10, -6,  4, 15, 15, 10, -6,-10,
	 -12, -8, 10, 12, 12, 10, -8,-12,
	   0,  0,  0,  0,  0,  0,  0,  0,
	   0,  0,  0,  0,  0,  0,  0,  0},

	{-30,-25, -8, -8, -8, -8,-25,-30, //knight
	  -6,  0,  4,  6,  6,  4,  0, -6,
	  -5,  6, 10, 12, 12, 10,  6, -5,
	  -5,  6, 14, 18, 18, 14,  6, -5,
	  -5,  6, 16, 24, 24, 16,  6, -5,
	  35, 40, 50, 60, 60, 50, 40, 35,
	  10, 15, 20, 25, 25, 20, 15, 10,
	   0,  0,  0,  0,  0,  0,  0,  0},

	{  5,-17,-22, -6, -6,-22,-17,  5, //bishop
	   0, 10,  0,  0,  0,  0, 10,  0,
	   0,  5, 10,  5,  5, 10,  5,  0,
	   0,  0,  8, 12, 12,  8,  0,  0,
	   0,  2,  5, 12, 12,  5,  2,  0,
	   0,  0, 10,  5,  5, 10,  0,  0,
	   0, 10,  0,  0,  0,  0, 10,  0,
	   5,  0,  0,  0,  0,  0,  0,  5},

	{  0,  0, 10, 13, 13, 13,  0,  0, //rook
	  -5,  0,  0, 10, 10, 10,  0, -5,
	  -5,  0,  0, 10, 10, 10,  0, -5,
	  -5,  0,  0, 10, 10, 10,  0, -5,
	  -5,  0,  0, 10, 10, 10,  0, -5,
	  -5,  0,  0, 10, 10, 10,  0, -5,
	  10, 15, 15, 25, 25, 25, 15, 10,
	   5, 10, 10, 20, 20, 20, 10,  5},

	{  0,  0,  0,  0,  0,  0,  0,  0, //queen
	   0,  0,  0,  0,  1,  2,  0,  0,
	   0,  0,  2,  1,  1,  2,  2,  0,
	   0,  0,  1,  4,  4,  1,  0,  0,
	   0,  0,  1,  4,  4,  1,  0,  0,
	   0,  0,  2,  1,  1,  2,  0,  0,
	   0,  0,  0,  0,  0,  0,  0,  0,
	   0,  0,  0,  0,  0,  0,  0,  0},

	{ 35, 30,-10,-20,-20, 15, 30, 35, //king
	   0, 20,  0,-30,-30,  0, 15,  0,
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

	{-10, -7, -5, -5, -5, -5, -7,-10, //knight
	  -6,  0,  4,  6,  6,  4,  0, -6,
	  -5,  0, 10, 12, 12, 10,  0, -5,
	  -5,  3, 14, 18, 18, 14,  3, -5,
	  -5,  6, 16, 24, 24, 16,  6, -5,
	  35, 40, 50, 60, 60, 50, 40, 35,
	  10, 15, 20, 25, 25, 20, 15, 10,
	   0,  0,  0,  0,  0,  0,  0,  0},

	{  5, -5, -7, -5, -5, -7, -5,  5, //bishop
	   0, 10,  0,  0,  0,  0, 10,  0,
	   0,  0, 10,  5,  5, 10,  0,  0,
	   0,  0,  8, 12, 12,  8,  0,  0,
	   0,  2,  5, 12, 12,  5,  2,  0,
	   0,  0, 10,  5,  5, 10,  0,  0,
	   0, 10,  0,  0,  0,  0, 10,  0,
	   5,  0,  0,  0,  0,  0,  0,  5},

	{ -5,  0, 10, 10, 10, 10,  0, -5, //rook
	   0,  0,  0, 10, 10, 10,  0,  0,
	   0,  0,  0, 10, 10, 10,  0,  0,
	   0,  0,  0, 10, 10, 10,  0,  0,
	   0,  0,  0, 10, 10, 10,  0,  0,
	   0,  0,  0, 10, 10, 10,  0,  0,
	  15, 15, 15, 25, 25, 25, 15, 15,
	  10, 10, 10, 20, 20, 20, 10, 10},

	{ -1, -1, -1, -1, -1, -1, -1, -1, //queen
	   0,  0,  0,  0,  1,  2,  0,  0,
	   0,  0,  2,  1,  1,  2,  2,  0,
	   0,  0,  1,  4,  4,  1,  0,  0,
	   0,  0,  1,  4,  4,  1,  0,  0,
	   0,  0,  2,  1,  1,  2,  0,  0,
	   0,  0,  0,  0,  0,  0,  0,  0,
	   0,  0,  0,  0,  0,  0,  0,  0},

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

int Engine::LeafEval(int alpha,int beta)
{
	evaltime.Start();
	nodes++;
	if(nodes%CheckupNodeCount==0)
	{
		checkup();
		//nodes = 0;
	}
	int neteval = 0;
	int sideeval[2]={0,0};
	//Bitset Pieces = pos.OccupiedSq ^ pos.Pieces[COLOR_WHITE][PIECE_PAWN] ^ pos.Pieces[COLOR_BLACK][PIECE_PAWN];
	//int pieceCount = popcnt(Pieces);
	bool isEG = false;
	if(getBoardMaterial() <= EndgameMaterial)
	{
		isEG = true;
	}

	//Material eval
	for(int i = 0;i<64;i++)
	{
		neteval += MaterialValues[pos.Squares[i]];
		if(isEG)
		    neteval += PieceSqEG[pos.Squares[i]][i];
		else
			neteval += PieceSq[pos.Squares[i]][i];
	}

	Bitset b = 0x0;
	Bitset KingField[2];
	Bitset ColorPieces[2];
	int KingAttackUnits[2]={0,0};
	int MinorsOnBackRank[2]={0,0};
	int MinorPieceCount[2] = { 0,0 };

	for(int i = 0;i<2;i++) //king safety
	{
		b = pos.Pieces[i][PIECE_KING];
		unsigned long k = 0;
		_BitScanForward64(&k,b);
		KingField[i] = getKingField(i,k);
		ColorPieces[i] = pos.Pieces[i][PIECE_PAWN] | pos.Pieces[i][PIECE_KNIGHT] |
                         pos.Pieces[i][PIECE_BISHOP] | pos.Pieces[i][PIECE_ROOK] |
                         pos.Pieces[i][PIECE_QUEEN] | pos.Pieces[i][PIECE_KING];
		MinorPieceCount[i] = popcnt(pos.Pieces[i][PIECE_KNIGHT] | pos.Pieces[i][PIECE_BISHOP]);

		if(!isEG)
		{
			if(getRank(getColorMirror(i,k))<2)
			{
				if(pos.Squares[getColorMirror(i,getPlus8(getColorMirror(i,k)))]==getPiece2Square(PIECE_PAWN,i))
				{
					sideeval[i] += PawnShield1Bonus;
				}
				else if(pos.Squares[getColorMirror(i,getPlus8(getColorMirror(i,k)))]==getPiece2Square(PIECE_BISHOP,i))
				{
					sideeval[i] += BishopShieldBonus;
				}
				if(pos.Squares[getColorMirror(i,getPlus8(getPlus8(getColorMirror(i,k))))]==getPiece2Square(PIECE_PAWN,i))
				{
					sideeval[i] += PawnShield2Bonus;
				}
				if(getFile(k)!=7)
				{
					if(pos.Squares[k+7*ColorFactor[i]]==getPiece2Square(PIECE_PAWN,i))
					{
						sideeval[i] += PawnShield1Bonus;
					}
					if(pos.Squares[k+15*ColorFactor[i]]==getPiece2Square(PIECE_PAWN,i))
					{
						sideeval[i] += PawnShield2Bonus;
					}
				}
				if(getFile(k)!=0)
				{
					if(pos.Squares[k+9*ColorFactor[i]]==getPiece2Square(PIECE_PAWN,i))
					{
						sideeval[i] += PawnShield1Bonus;
					}
					if(pos.Squares[k+17*ColorFactor[i]]==getPiece2Square(PIECE_PAWN,i))
					{
						sideeval[i] += PawnShield2Bonus;
					}
				}
			}
		}

		if((getAboveBits(i,k)&pos.Pieces[i][PIECE_PAWN])==0) //checks if there arent friendly pawns in the same file
		{
			if(getAboveBits(i,k)&pos.Pieces[getOpponent(i)][PIECE_PAWN]) //checks if there are opponent pawns in the same file
			{
				sideeval[i] -= KingOnHalfOpenFilePenalty;
				//KingAttackUnits[i] += 4; 
			}
			else
			{
				sideeval[i] -= KingOnOpenFilePenalty;
				//KingAttackUnits[i] += 8;
			}
		}
		if((getAboveBits(i,k)&pos.Pieces[getOpponent(i)][PIECE_PAWN])==0 && (getAboveBits(i,k)&pos.Pieces[getOpponent(i)][PIECE_ROOK])!=0)
		{ //checks if there are no opponent pawns on the file and there are rooks on the file
			sideeval[i] -= KingOnRookFilePenalty;
			//KingAttackUnits[i] += 2;
		}
		if(getFile(k)!=0)
		{
			if((getAboveBits(i,k+1)&pos.Pieces[i][PIECE_PAWN])==0) //checks if there arent friendly pawns in the same file
			{
				if(getAboveBits(i,k+1)&pos.Pieces[getOpponent(i)][PIECE_PAWN]) //checks if there are opponent pawns in the same file
				{
					sideeval[i] -= KingAdjHalfOpenFilePenalty;
					//KingAttackUnits[i] += 3;
				}
				else
				{
					sideeval[i] -= KingAdjOpenFilePenalty;
					//KingAttackUnits[i] += 6;
				}
			}
			if((getAboveBits(i,k+1)&pos.Pieces[getOpponent(i)][PIECE_PAWN])==0 && (getAboveBits(i,k+1)&pos.Pieces[getOpponent(i)][PIECE_ROOK])!=0)
			{ //checks if there are no opponent pawns on the file and there are rooks on the file
				sideeval[i] -= KingAdjRookFilePenalty;
				//KingAttackUnits[i] += 1;
			}
		}
		if(getFile(k)!=7)
		{
			if((getAboveBits(i,k-1)&pos.Pieces[i][PIECE_PAWN])==0) //checks if there arent friendly pawns in the same file
			{
				if(getAboveBits(i,k-1)&pos.Pieces[getOpponent(i)][PIECE_PAWN]) //checks if there are opponent pawns in the same file
				{
					sideeval[i] -= KingAdjHalfOpenFilePenalty;
					//KingAttackUnits[i] += 3;
				}
				else
				{
					sideeval[i] -= KingAdjOpenFilePenalty;
					//KingAttackUnits[i] += 6;
				}
			}
			if((getAboveBits(i,k-1)&pos.Pieces[getOpponent(i)][PIECE_PAWN])==0 && (getAboveBits(i,k-1)&pos.Pieces[getOpponent(i)][PIECE_ROOK])!=0)
			{ //checks if there are no opponent pawns on the file and there are rooks on the file
			    sideeval[i] -= KingAdjRookFilePenalty;
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
	for(int i = 0;i<2;i++)
	{
		//Pawns
		b = pos.Pieces[i][PIECE_PAWN];
		int pawncount = popcnt(b);
		int pawncountcolor[2];
		pawncountcolor[COLOR_WHITE] = popcnt(b&0x55AA55AA55AA55AA);
		pawncountcolor[COLOR_BLACK] = popcnt(b&0xAA55AA55AA55AA55);
		if(!b) //penalty for having no pawns
		{
			sideeval[i] -= NoPawnsPenalty;
		}
		while(b)
		{
			//int k = firstOf(b);
			unsigned long k = 0;
			_BitScanForward64(&k,b);
			b ^= getPos2Bit(k);
			if(getAboveBits(i,k)&pos.Pieces[i][PIECE_PAWN]) //checks if there are friendly pawns in the same file
			{
				sideeval[i] -= DoubledPawnPenalty[getFile(k)];
			}
			if((getAboveAndAboveSideBits(i,k)&pos.Pieces[getOpponent(i)][PIECE_PAWN])==0) //checks if the pawn is a passer
			{
				sideeval[i] += PassedPawnBonus[getColorMirror(i,k)];
			}
			if((getSideBits(k)&pos.Pieces[i][PIECE_PAWN])==0) //checks if there are no friendly pawns on the adjacent files
			{
				sideeval[i] -= IsolatedPawnPenalty[getFile(k)];
			}
			if(pos.Squares[getColorMirror(i,getPlus8(getColorMirror(i,k)))]!=SQUARE_EMPTY) //checks if pawn is blocked
			{
				sideeval[i] -= BlockedPawnPenalty[getColorMirror(i,k)];
			}

			//pawn attacks near opposing king
			//eval += ColorFactor[i]*popcnt(KingField[getOpponent(i)]&getPawnAttacks(i,k))*AttackWeights[PIECE_PAWN];
			KingAttackUnits[getOpponent(i)] += popcnt(KingField[getOpponent(i)]&getPawnAttacks(i,k))*AttackWeights[PIECE_PAWN];
		}

		//Rooks
		b = pos.Pieces[i][PIECE_ROOK];
		int rookcount = popcnt(b);
		sideeval[i] += RookAdj[pawncount]*rookcount; //adjustment for pawns
		if(rookcount >= 2) //Pair Bonus
		{
			sideeval[i] += RookPairBonus;
		}
		while(b)
		{
			//int k = firstOf(b);
			unsigned long k = 0;
			_BitScanForward64(&k,b);
			b ^= getPos2Bit(k);
			if((getAboveBits(i,k)&pos.Pieces[i][PIECE_PAWN])==0) //checks if there arent friendly pawns in the same file
			{
				if(getAboveBits(i,k)&pos.Pieces[getOpponent(i)][PIECE_PAWN]) //checks if there are opponent pawns in the same file
				{
					sideeval[i] += RookHalfOpenBonus[getFile(k)];
				}
				else
				{
					sideeval[i] += RookOpenBonus[getFile(k)];
				}
			}

			//Rook Attacks
			Bitset m = getRookRankMoves(k,(pos.OccupiedSq>>(getRankOffset(k)))&0xff);
			m |= getRookFileMoves(k,(pos.OccupiedSq90>>(getFileOffset(k)))&0xff);
			m &= m^ColorPieces[i];
			sideeval[i] += RookMobility[popcnt(m)];
			if(m&b)
			{
				sideeval[i] += RookConnectedBonus;
			}
			//eval += ColorFactor[i]*popcnt(KingField[getOpponent(i)]&m)*AttackWeights[PIECE_ROOK];
			KingAttackUnits[getOpponent(i)] += popcnt(KingField[getOpponent(i)]&m)*AttackWeights[PIECE_ROOK];
		}

		//Knights
		b = pos.Pieces[i][PIECE_KNIGHT];
		int knightcount = popcnt(b);
		sideeval[i] += KnightAdj[pawncount]*knightcount; //adjustment for pawns
		if(popcnt(b) >= 2)
		{
			sideeval[i] += KnightPairBonus;
		}
		while(b)
		{
			unsigned long k = 0;
			_BitScanForward64(&k,b);
			b ^= getPos2Bit(k);
			if((getAboveSideBits(i,k)&pos.Pieces[getOpponent(i)][PIECE_PAWN])==0) //checks if there are no enemy pawns on the adjacent files
			{
				sideeval[i] += KnightOutpostBonus;
				if (MinorPieceCount[getOpponent(i)] == 0)
				{
					sideeval[i] += KnightOutpostBonus; //double bonus if there are no opponent minor pieces
				}
			}

			//knight attacks near opposing king
			//eval += ColorFactor[i]*popcnt(KingField[getOpponent(i)]&getKnightMoves(k))*AttackWeights[PIECE_KNIGHT];
			Bitset m = getKnightMoves(k);
			sideeval[i] += KnightMobility[popcnt(m)];
			KingAttackUnits[getOpponent(i)] += popcnt(KingField[getOpponent(i)]&m)*AttackWeights[PIECE_KNIGHT];

			if(getRank(getColorMirror(i,k))==0)
			{
				MinorsOnBackRank[i]++;
			}
		}

		//Bishops
		b = pos.Pieces[i][PIECE_BISHOP];
		int bishopcount = popcnt(b);
		sideeval[i] += BishopAdj[pawncount]*bishopcount; //adjustment for pawns
		if(popcnt(b) >= 2)
		{
			sideeval[i] += BishopPairBonus;
		}
		while(b)
		{
			unsigned long k = 0;
			_BitScanForward64(&k,b);
			b ^= getPos2Bit(k);
			if((getAboveSideBits(i,k)&pos.Pieces[getOpponent(i)][PIECE_PAWN])==0) //checks if there are no enemy pawns on the adjacent files
			{
				sideeval[i] += BishopOutpostBonus;
				if (MinorPieceCount[getOpponent(i)] == 0)
				{
					sideeval[i] += BishopOutpostBonus; //double bonus if there are no opponent minor pieces
				}
			}

			//bishop attacks near opposing king
			Bitset m = getBishopA1H8Moves(k,(pos.OccupiedSq135>>getDiag(getturn135(k)))&0xff);
			m |= getBishopA8H1Moves(k,(pos.OccupiedSq45>>getDiag(getturn45(k)))&0xff);
			m &= m^ColorPieces[i];
			sideeval[i] += BishopMobility[popcnt(m)];
			sideeval[i] += BishopPawnSameColor[pawncountcolor[SquareColor[k]]]; //bishop pawn same color adjustment
			//eval += ColorFactor[i]*popcnt(KingField[getOpponent(i)]&m)*AttackWeights[PIECE_BISHOP];
			KingAttackUnits[getOpponent(i)] += popcnt(KingField[getOpponent(i)]&m)*AttackWeights[PIECE_BISHOP];

			if(getRank(getColorMirror(i,k))==0)
			{
				MinorsOnBackRank[i]++;
			}
		}

		//Queens
		b = pos.Pieces[i][PIECE_QUEEN];
		while(b)
		{
			unsigned long k = 0;
			_BitScanForward64(&k,b);
			b ^= getPos2Bit(k);

			//queen attacks near opposing king
			Bitset m = getRookRankMoves(k,(pos.OccupiedSq>>(getRankOffset(k)))&0xff);
			m |= getRookFileMoves(k,(pos.OccupiedSq90>>(getFileOffset(k)))&0xff);
			m |= getBishopA1H8Moves(k,(pos.OccupiedSq135>>getDiag(getturn135(k)))&0xff);
			m |= getBishopA8H1Moves(k,(pos.OccupiedSq45>>getDiag(getturn45(k)))&0xff);
			m &= m^ColorPieces[i];
			//eval += ColorFactor[i]*popcnt(KingField[getOpponent(i)]&m)*AttackWeights[PIECE_QUEEN];
			KingAttackUnits[getOpponent(i)] += long(popcnt(KingField[getOpponent(i)]&m))*AttackWeights[PIECE_QUEEN];

			if(!isEG)
			{
				if(getRank(getColorMirror(i,k))>1)
				{
					sideeval[i] -= QueenOutEarlyPenalty*MinorsOnBackRank[i];
				}
			}
		}
	}

	for(int i = 0;i<2;i++)
	{
		sideeval[i] -= SafetyTable[KingAttackUnits[i]];
	}

	neteval += sideeval[COLOR_WHITE];
	neteval -= sideeval[COLOR_BLACK];

	if(pos.turn==COLOR_BLACK)
		return -neteval;

	evaltime.Stop();
	return neteval;
}

int Engine::getBoardMaterial()
{
	int eval = 0;
	for(int i = 0;i<64;i++)
	{
		if(pos.Squares[i]!=SQUARE_EMPTY)
		{
			int pc = getSquare2Piece(pos.Squares[i]);
			if(pc!=PIECE_KING)
				eval += MaterialValues[pc];
		}
	}
	return eval;
}

int Engine::StaticExchangeEvaluation(int to, int from,int movpiece,int capt)
{
	capt = getSquare2Piece(capt);
	if (PieceMaterial[capt] >= PieceMaterial[movpiece])
	{
		return PieceMaterial[capt] - PieceMaterial[movpiece];
	}
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
	}while(m!=CONS_NULLMOVE);

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

int Engine::StaticExchangeEvaluation2(Move m)
{
	//cout << "see " << m.toString() << endl;
	int value = 0;
	if(m.getSpecial()!=PIECE_PAWN)
	{
		value = MaterialValues[getSquare2Piece(m.getCapturedPiece())+1];
	}
	else
	{
		value = MaterialValues[SQUARE_WHITEPAWN];
	}
	pos.forceMove(m);
	int piece = pos.getSmallestAttacker2(pos.turn,m.getTo());
	if(piece!=PIECE_NONE)
	{
		Move mov = pos.makeCapture(piece,m.getTo());
		//cout << "move " << mov.toString() << endl;
		//int piece = mov.getMovingPiece();
		if(mov!=CONS_NULLMOVE)
		{
			value -= StaticExchangeEvaluation2(mov);
		}
	}
	pos.unmakeMove(m);
	return value;
}

int Engine::StaticExchangeEvaluation(Move m)
{
	//cout << "see " << m.toString() << endl;
	int value = 0;
	if(m.getSpecial()!=PIECE_PAWN)
	{
		value = MaterialValues[getSquare2Piece(m.getCapturedPiece())+1];
	}
	else
	{
		value = MaterialValues[SQUARE_WHITEPAWN];
	}
	pos.forceMove(m);
	Move mov = pos.getSmallestAttacker(pos.turn,m.getTo());
	if(mov!=CONS_NULLMOVE)
	{
		//cout << "move " << mov.toString() << endl;
		int piece = mov.getMovingPiece();
		if(piece!=PIECE_NONE)
		{
			value -= StaticExchangeEvaluation(mov);
		}
	}
	pos.unmakeMove(m);
	return value;
}

int Engine::StaticExchangeEvaluation(int square,int side)
{
	int value = 0;
	int piece = pos.getSmallestAttacker2(side,square);
	if(piece!=PIECE_NONE)
	{
		Move m = pos.makeCapture(piece,square);
		if(m!=CONS_NULLMOVE)
		{
			pos.forceMove(m);
			value = max(0,MaterialValues[piece] - StaticExchangeEvaluation(square,getOpponent(side)));
			pos.unmakeMove(m);
		}
	}
	return value;
}

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

int Engine::Trace(int alpha,int beta)
{
	//nodes++;
	//if(nodes%1024==0)
	//{
	//	checkup();
	//	//nodes = 0;
	//}
	int eval = 0;
	//Bitset Pieces = pos.OccupiedSq ^ pos.Pieces[COLOR_WHITE][PIECE_PAWN] ^ pos.Pieces[COLOR_BLACK][PIECE_PAWN];
	//int pieceCount = popcnt(Pieces);
	bool isEG = false;
	if(getBoardMaterial() <= EndgameMaterial)
	{
		isEG = true;
		cout << "It is endgame" << endl;
	}

	//Material eval
	cout << "Material:" << endl;
	for(int i = 0;i<64;i++)
	{
		eval += MaterialValues[pos.Squares[i]];
		if(isEG)
		{
			eval += PieceSqEG[pos.Squares[i]][i];
			cout << PieceSqEG[pos.Squares[i]][i] << " for " << Int2Sq(i) << endl;
		}
		else
		{
			eval += PieceSq[pos.Squares[i]][i];
			cout << PieceSq[pos.Squares[i]][i] << " for " << Int2Sq(i) << endl;
		}
	}

	Bitset b = 0x0;
	Bitset KingField[2];
	Bitset ColorPieces[2];
	int MinorsOnBackRank[2] = {0,0};
	cout << "King Safety: " << endl;
	for(int i = 0;i<2;i++) //king safety
	{
		b = pos.Pieces[i][PIECE_KING];
		unsigned long k = 0;
		_BitScanForward64(&k,b);
		KingField[i] = getKingField(i,k);
		ColorPieces[i] = pos.Pieces[i][PIECE_PAWN] | pos.Pieces[i][PIECE_KNIGHT] |
                         pos.Pieces[i][PIECE_BISHOP] | pos.Pieces[i][PIECE_ROOK] |
                         pos.Pieces[i][PIECE_QUEEN] | pos.Pieces[i][PIECE_KING];

		if(!isEG)
		{
			if(getRank(getColorMirror(i,k))<2)
			{
				if(pos.Squares[getColorMirror(i,getPlus8(getColorMirror(i,k)))]==getPiece2Square(PIECE_PAWN,i))
				{
					eval += ColorFactor[i]*PawnShield1Bonus;
					cout << ColorFactor[i]*PawnShield1Bonus << " for pawn shield 1 on " << Int2Sq(k) << endl;
				}
				else if(pos.Squares[getColorMirror(i,getPlus8(getColorMirror(i,k)))]==getPiece2Square(PIECE_BISHOP,i))
				{
					eval += ColorFactor[i]*BishopShieldBonus;
					cout << ColorFactor[i]*BishopShieldBonus << " for bishop shield on " << Int2Sq(k) << endl;
				}
				if(pos.Squares[getColorMirror(i,getPlus8(getPlus8(getColorMirror(i,k))))]==getPiece2Square(PIECE_PAWN,i))
				{
					eval += ColorFactor[i]*PawnShield2Bonus;
					cout << ColorFactor[i]*PawnShield2Bonus << " for pawn shield 2 on " << Int2Sq(k) << endl;
				}
				if(getFile(k)!=7)
				{
					if(pos.Squares[k+7*ColorFactor[i]]==getPiece2Square(PIECE_PAWN,i))
					{
						eval += ColorFactor[i]*PawnShield1Bonus;
						cout << ColorFactor[i]*PawnShield1Bonus << " for pawn shield 1 on " << Int2Sq(k) << endl;
					}
					if(pos.Squares[k+15*ColorFactor[i]]==getPiece2Square(PIECE_PAWN,i))
					{
						eval += ColorFactor[i]*PawnShield2Bonus;
						cout << ColorFactor[i]*PawnShield2Bonus << " for pawn shield 2 on " << Int2Sq(k) << endl;
					}
				}
				if(getFile(k)!=0)
				{
					if(pos.Squares[k+9*ColorFactor[i]]==getPiece2Square(PIECE_PAWN,i))
					{
						eval += ColorFactor[i]*PawnShield1Bonus;
						cout << ColorFactor[i]*PawnShield1Bonus << " for pawn shield 1 on " << Int2Sq(k) << endl;
					}
					if(pos.Squares[k+17*ColorFactor[i]]==getPiece2Square(PIECE_PAWN,i))
					{
						eval += ColorFactor[i]*PawnShield2Bonus;
						cout << ColorFactor[i]*PawnShield2Bonus << " for pawn shield 2 on " << Int2Sq(k) << endl;
					}
				}
			}
		}

		if((getAboveBits(i,k)&pos.Pieces[i][PIECE_PAWN])==0) //checks if there arent friendly pawns in the same file
		{
			if(getAboveBits(i,k)&pos.Pieces[getOpponent(i)][PIECE_PAWN]) //checks if there are opponent pawns in the same file
			{
				eval -= ColorFactor[i]*KingOnHalfOpenFilePenalty;
				cout << ColorFactor[i]*KingOnHalfOpenFilePenalty << " for king on half open on " << Int2Sq(k) << endl;
			}
			else
			{
				eval -= ColorFactor[i]*KingOnOpenFilePenalty;
				cout << ColorFactor[i]*KingOnOpenFilePenalty << " for king on open on " << Int2Sq(k) << endl;
			}
		}
		if((getAboveBits(i,k)&pos.Pieces[getOpponent(i)][PIECE_PAWN]==0) && (getAboveBits(i,k)&pos.Pieces[getOpponent(i)][PIECE_ROOK]!=0))
		{ //checks if there are no opponent pawns on the file and there are rooks on the file
			eval -= ColorFactor[i]*KingOnRookFilePenalty;
			cout << ColorFactor[i]*KingOnRookFilePenalty << " for king on rook file on " << Int2Sq(k) << endl;
		}
		if(getFile(k)!=0)
		{
			if((getAboveBits(i,k+1)&pos.Pieces[i][PIECE_PAWN])==0) //checks if there arent friendly pawns in the same file
			{
				if(getAboveBits(i,k+1)&pos.Pieces[getOpponent(i)][PIECE_PAWN]) //checks if there are opponent pawns in the same file
				{
					eval -= ColorFactor[i]*KingAdjHalfOpenFilePenalty;
					cout << ColorFactor[i]*KingAdjHalfOpenFilePenalty << " for king adj half open file on " << Int2Sq(k) << endl;
				}
				else
				{
					eval -= ColorFactor[i]*KingAdjOpenFilePenalty;
					cout << ColorFactor[i]*KingAdjOpenFilePenalty << " for king adj open file on " << Int2Sq(k) << endl;
				}
			}
			if((getAboveBits(i,k+1)&pos.Pieces[getOpponent(i)][PIECE_PAWN])==0 && (getAboveBits(i,k+1)&pos.Pieces[getOpponent(i)][PIECE_ROOK]!=0))
			{ //checks if there are no opponent pawns on the file and there are rooks on the file
				eval -= ColorFactor[i]*KingAdjRookFilePenalty;
				cout << ColorFactor[i]*KingAdjRookFilePenalty << " for king adj rook file on " << Int2Sq(k) << endl;
			}
		}
		if(getFile(k)!=7)
		{
			if((getAboveBits(i,k-1)&pos.Pieces[i][PIECE_PAWN])==0) //checks if there arent friendly pawns in the same file
			{
				if(getAboveBits(i,k-1)&pos.Pieces[getOpponent(i)][PIECE_PAWN]) //checks if there are opponent pawns in the same file
				{
					eval -= ColorFactor[i]*KingAdjHalfOpenFilePenalty;
					cout << ColorFactor[i]*KingAdjHalfOpenFilePenalty << " for king adj half open file on " << Int2Sq(k) << endl;
				}
				else
				{
					eval -= ColorFactor[i]*KingAdjOpenFilePenalty;
					cout << ColorFactor[i]*KingAdjOpenFilePenalty << " for king adj open file on " << Int2Sq(k) << endl;
				}
			}
			if(getAboveBits(i,k-1)&pos.Pieces[getOpponent(i)][PIECE_PAWN]==0 && getAboveBits(i,k-1)&pos.Pieces[getOpponent(i)][PIECE_ROOK]!=0)
			{ //checks if there are no opponent pawns on the file and there are rooks on the file
				eval -= ColorFactor[i]*KingAdjRookFilePenalty;
				cout << ColorFactor[i]*KingAdjRookFilePenalty << " for king adj rook file on " << Int2Sq(k) << endl;
			}
		}
	}

	//Lazy eval
	cout << "Lazy eval checking, eval is " << eval << endl;
	int lazy;
	if(pos.turn==COLOR_WHITE)
	{
		lazy = eval;
	}
	else
	{
		lazy = -eval;
	}
	if((lazy + LazyEval1 < alpha) || (lazy - LazyEval1 > beta))
	{
		cout << "Lazy eval success" << endl;
		return lazy;
	}

	cout << "Positional eval " << endl;
	//Positional evaluation
	for(int i = 0;i<2;i++)
	{
		//Pawns
		b = pos.Pieces[i][PIECE_PAWN];
		int pawncount = popcnt(b);
		if(!b) //penalty for having no pawns
		{
			eval -= ColorFactor[i]*NoPawnsPenalty;
			cout << ColorFactor[i]*NoPawnsPenalty << " for no pawns" << endl;
		}
		while(b)
		{
			//int k = firstOf(b);
			unsigned long k = 0;
			_BitScanForward64(&k,b);
			b ^= getPos2Bit(k);
			if(getAboveBits(i,k)&pos.Pieces[i][PIECE_PAWN]) //checks if there are friendly pawns in the same file
			{
				eval -= ColorFactor[i]*DoubledPawnPenalty[getFile(k)];
				cout << ColorFactor[i]*DoubledPawnPenalty[getFile(k)] << " for doubled pawn on " << Int2Sq(k) << endl;
			}
			if((getAboveAndAboveSideBits(i,k)&pos.Pieces[getOpponent(i)][PIECE_PAWN])==0) //checks if the pawn is a passer
			{
				eval += ColorFactor[i]*PassedPawnBonus[getColorMirror(i,k)];
				cout << ColorFactor[i]*PassedPawnBonus[getColorMirror(i,k)] << " for passed pawn on " << Int2Sq(k) << endl;
			}
			if((getSideBits(k)&pos.Pieces[i][PIECE_PAWN])==0) //checks if there are no friendly pawns on the adjacent files
			{
				eval -= ColorFactor[i]*IsolatedPawnPenalty[getFile(k)];
				cout << ColorFactor[i]*IsolatedPawnPenalty[getFile(k)] << " for isolated pawn on " << Int2Sq(k) << endl;
			}
			if(pos.Squares[getColorMirror(i,getPlus8(getColorMirror(i,k)))]!=SQUARE_EMPTY) //checks if pawn is blocked
			{
				eval -= ColorFactor[i]*BlockedPawnPenalty[getColorMirror(i,k)];
				cout << ColorFactor[i]*BlockedPawnPenalty[getColorMirror(i,k)] << " for blocked pawn on " << Int2Sq(k) << endl;
			}

			//pawn attacks near opposing king
			eval += ColorFactor[i]*long(popcnt(KingField[getOpponent(i)]&getPawnAttacks(i,k))*AttackWeights[PIECE_PAWN]);
			cout << ColorFactor[i]*long(popcnt(KingField[getOpponent(i)]&getPawnAttacks(i,k))*AttackWeights[PIECE_PAWN]) << " attacking pawn on " << Int2Sq(k) << endl;
		}

		//Rooks
		b = pos.Pieces[i][PIECE_ROOK];
		int rookcount = popcnt(b);
		eval += ColorFactor[i]*RookAdj[pawncount]*rookcount; //adjustment for pawns
		if(rookcount >= 2) //Pair Bonus
		{
			eval += ColorFactor[i]*RookPairBonus;
		}
		while(b)
		{
			//int k = firstOf(b);
			unsigned long k = 0;
			_BitScanForward64(&k,b);
			b ^= getPos2Bit(k);
			if((getAboveBits(i,k)&pos.Pieces[i][PIECE_PAWN])==0) //checks if there arent friendly pawns in the same file
			{
				if(getAboveBits(i,k)&pos.Pieces[getOpponent(i)][PIECE_PAWN]) //checks if there are opponent pawns in the same file
				{
					eval += ColorFactor[i]*RookHalfOpenBonus[getFile(k)];
					cout << ColorFactor[i]*RookHalfOpenBonus[getFile(k)] << " for half open file rook on " << Int2Sq(k) << endl;
				}
				else
				{
					eval += ColorFactor[i]*RookOpenBonus[getFile(k)];
					cout << ColorFactor[i]*RookOpenBonus[getFile(k)] << " for open file rook on " << Int2Sq(k) << endl;
				}
			}

			//Rook Attacks
			Bitset m = getRookRankMoves(k,(pos.OccupiedSq>>(getRankOffset(k)))&0xff);
			m |= getRookFileMoves(k,(pos.OccupiedSq90>>(getFileOffset(k)))&0xff);
			m &= m^ColorPieces[i];
			eval += ColorFactor[i]*popcnt(KingField[getOpponent(i)]&m)*AttackWeights[PIECE_ROOK];
			cout << ColorFactor[i]*popcnt(KingField[getOpponent(i)]&m)*AttackWeights[PIECE_ROOK] << " attacking rook on " << Int2Sq(k) << endl;
		}

		//Knights
		b = pos.Pieces[i][PIECE_KNIGHT];
		int knightcount = popcnt(b);
		eval += ColorFactor[i]*KnightAdj[pawncount]*knightcount; //adjustment for pawns
		if(popcnt(b) >= 2)
		{
			eval += ColorFactor[i]*KnightPairBonus;
			cout << ColorFactor[i]*KnightPairBonus << " for knight pair " << endl;
		}
		while(b)
		{
			unsigned long k = 0;
			_BitScanForward64(&k,b);
			b ^= getPos2Bit(k);
			if((getAboveSideBits(i,k)&pos.Pieces[getOpponent(i)][PIECE_PAWN])==0) //checks if there are no enemy pawns on the adjacent files
			{
				eval += ColorFactor[i]*KnightOutpostBonus;
				cout << ColorFactor[i]*KnightOutpostBonus << " for knight outpost " << Int2Sq(k) << endl;
			}

			//knight attacks near opposing king
			eval += ColorFactor[i]*popcnt(KingField[getOpponent(i)]&getKnightMoves(k))*AttackWeights[PIECE_KNIGHT];
			cout << ColorFactor[i]*popcnt(KingField[getOpponent(i)]&getKnightMoves(k))*AttackWeights[PIECE_KNIGHT] << " attacking knight on " << Int2Sq(k) << endl;
			if(getRank(getColorMirror(i,k))==0)
			{
				MinorsOnBackRank[i]++;
			}
		}

		//Bishops
		b = pos.Pieces[i][PIECE_BISHOP];
		int bishopcount = popcnt(b);
		eval += ColorFactor[i]*BishopAdj[pawncount]*bishopcount; //adjustment for pawns
		if(popcnt(b) >= 2)
		{
			eval += ColorFactor[i]*BishopPairBonus;
			cout << ColorFactor[i]*BishopPairBonus << " for bishop pair " << endl;
		}
		while(b)
		{
			unsigned long k = 0;
			_BitScanForward64(&k,b);
			b ^= getPos2Bit(k);
			if((getAboveSideBits(i,k)&pos.Pieces[getOpponent(i)][PIECE_PAWN])==0) //checks if there are no enemy pawns on the adjacent files
			{
				eval += ColorFactor[i]*BishopOutpostBonus;
				cout << ColorFactor[i]*BishopOutpostBonus << " for bishop outpost " << Int2Sq(k) << endl;
			}

			//bishop attacks near opposing king
			Bitset m = getBishopA1H8Moves(k,(pos.OccupiedSq135>>getDiag(getturn135(k)))&0xff);
			m |= getBishopA8H1Moves(k,(pos.OccupiedSq45>>getDiag(getturn45(k)))&0xff);
			m &= m^ColorPieces[i];
			eval += ColorFactor[i]*popcnt(KingField[getOpponent(i)]&m)*AttackWeights[PIECE_BISHOP];
			cout << ColorFactor[i]*popcnt(KingField[getOpponent(i)]&m)*AttackWeights[PIECE_BISHOP] << " attacking bishop on " << Int2Sq(k) << endl;
			if(getRank(getColorMirror(i,k))==0)
			{
				MinorsOnBackRank[i]++;
			}
		}

		//Queens
		b = pos.Pieces[i][PIECE_QUEEN];
		while(b)
		{
			unsigned long k = 0;
			_BitScanForward64(&k,b);
			b ^= getPos2Bit(k);

			//queen attacks near opposing king
			Bitset m = getRookRankMoves(k,(pos.OccupiedSq>>(getRankOffset(k)))&0xff);
			m |= getRookFileMoves(k,(pos.OccupiedSq90>>(getFileOffset(k)))&0xff);
			m |= getBishopA1H8Moves(k,(pos.OccupiedSq135>>getDiag(getturn135(k)))&0xff);
			m |= getBishopA8H1Moves(k,(pos.OccupiedSq45>>getDiag(getturn45(k)))&0xff);
			m &= m^ColorPieces[i];
			eval += ColorFactor[i]*long(popcnt(KingField[getOpponent(i)]&m)*AttackWeights[PIECE_QUEEN]);
			cout << ColorFactor[i]*long(popcnt(KingField[getOpponent(i)]&m)*AttackWeights[PIECE_QUEEN]) << " attacking queen on " << Int2Sq(k) << endl;

			if(!isEG)
			{
				if(getRank(getColorMirror(i,k))!=0)
				{
					eval -= ColorFactor[i]*QueenOutEarlyPenalty*MinorsOnBackRank[i];
					cout << "-" << ColorFactor[i]*QueenOutEarlyPenalty*MinorsOnBackRank[i] << " for queen out early " << MinorsOnBackRank[i] << endl;
				}
			}
		}
	}

	if(pos.turn==COLOR_BLACK)
		return -eval;
	return eval;
}