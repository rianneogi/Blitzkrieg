#include "Engine.h"

#define S(x,y) Score(x,y)

string PlayerStrings[2] = { "White","Black" };

int ColorFactor[2] = {1,-1};

int PieceMaterial[7] = {100,325,335,500,975,-CONS_MATED,0};
int TotalMaterial = (8*PieceMaterial[0] + 2*PieceMaterial[1] + 2*PieceMaterial[2] + 2*PieceMaterial[3] + PieceMaterial[4]);
int TotalMaterialBothSides = TotalMaterial * 2;

const int EndgameMaterial = 3800;

int LazyEval1 = 400;

//int CenterSquareBonus = 4;
//int CenterBorderSquareBonus = 2;
//int EnemyTerritorySquareBonus = 3;

const Scale KingSafetyFactor(1, 0);
const Scale PawnStructureFactor(0.5, 0.5);
const Scale PassedPawnFactor(0.5, 1);
const Scale MobilityFactor(1, 1);
const Scale OutpostFactor(1, 0.5);

const Scale BishopFactor(1, 1);
const Scale KnightFactor(1, 1);
const Scale RookFactor(1, 1);
const Scale QueenFactor(1, 1);

//Mobility
Score BishopMobility[16] = { -24, -8,  0,  4,  6,  8,  10,  12,  14,  16, 18, 20, 24, 28, 32 };
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
Score QueenOutEarlyPenalty = S(2,0); //penalty for queens not on back rank for every minor on back rank

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
Score PawnStormPenalty[8] = { 0,0,5,10,20,40,60,80 };

const int QueenContactCheckPenalty = 12;
const int RookContactCheckPenalty = 5;
const int CheckPenalty[6] = { 1,3,2,4,5,0 }; //penalty when a piece can check your king safely

const int AttackWeights[6] = { 1,3,2,4,5,0 };
Score KingSafetyScore[512];

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
								 0,  0,  0,  0,  0,  0,  0,  0,
								 0,  0,  0,  0,  0,  0,  0,  0,
								 0,  0,  0,  0,  0,  0,  0,  0,
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

int scaleScore(Score s, int mat)
{
	return (s.eg + (s.mg - s.eg)*((float)(mat) / TotalMaterialBothSides));
}

struct EvalStruct
{
	Bitset attackedByPiece[2][6]; //squares attacked by certain piece
	Bitset attackedByColor[2]; //squares attacked by all pieces except king of a given color

	int KingSquare[2];
	Bitset KingField[2];
	Bitset KingAdj[2];
	int KingAttackUnits[2];

	Bitset ColorPieces[2]; //all pieces of a given color
	long long ColorPiecesCount[2]; //popcnt of the above

	int PawnCount[2]; //number of pawns per side
	int PawnCountColor[2][2]; //number of pawns on a square of a certain color per side, PawnCountColor[side][squarecolor]
};

template<bool Trace> int Engine::LeafEval()
{
	nodes++;
	if (nodes%CheckupNodeCount == 0)
	{
		checkup();
		//nodes = 0;
	}
	//evaltime.Start();

	EvalStruct e;
	memset(&e, 0, sizeof(e));

	for (int i = 0;i < 2;i++)
	{
		e.ColorPieces[i] = pos.Pieces[i][PIECE_PAWN] | pos.Pieces[i][PIECE_KNIGHT] |
			pos.Pieces[i][PIECE_BISHOP] | pos.Pieces[i][PIECE_ROOK] |
			pos.Pieces[i][PIECE_QUEEN] | pos.Pieces[i][PIECE_KING];
		e.ColorPiecesCount[i] = popcnt(e.ColorPieces[i]);
	}

	if (e.ColorPiecesCount[COLOR_WHITE] == 2 && e.ColorPiecesCount[COLOR_BLACK] == 2) //2 piece endgame
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
	if (e.ColorPiecesCount[COLOR_WHITE] == 1 && e.ColorPiecesCount[COLOR_BLACK] == 3 && popcnt(pos.Pieces[COLOR_BLACK][PIECE_KNIGHT]) == 2)
	{
		if (Trace)
			cout << "draw, 2 knights cant force checkmate" << endl;
		return 0; //2 knights cant force checkmate
	}
	if (e.ColorPiecesCount[COLOR_BLACK] == 1 && e.ColorPiecesCount[COLOR_WHITE] == 3 && popcnt(pos.Pieces[COLOR_WHITE][PIECE_KNIGHT]) == 2)
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
	Score Passers[2] = { S(0,0),S(0,0) };

	e.KingAttackUnits[COLOR_WHITE] = 30;
	e.KingAttackUnits[COLOR_BLACK] = 30;

	int currentMaterial = Material[COLOR_WHITE] + Material[COLOR_BLACK];
	if (Trace)
	{
		cout << "White material: " << Material[0] << endl;
		cout << "Black material: " << Material[1] << endl;
	}

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
	Bitset b = 0x0;
	int MinorsOnBackRank[2] = { 0,0 };
	
	///King Safety
	if (Trace)
		cout << endl << "King Safety:" << endl;
	for (int i = 0;i < 2;i++) //king safety
	{
		b = pos.Pieces[i][PIECE_KING];
		//unsigned long k = 0;
		_BitScanForward64(&k, b);
		e.KingSquare[i] = k;
		e.KingAdj[i] = getKingMoves(k);
		e.KingField[i] = getKingField(i, k);

		//Pawn Shields
		KingSafety[i] += PawnShield1Bonus*(int)popcnt(KingShield1[i][k] & pos.Pieces[i][PIECE_PAWN]);
		if (Trace)
		{
			cout << "Bonus for pawns in front of king for " << PlayerStrings[i] << ": " << string(PawnShield1Bonus*(int)popcnt(KingShield1[i][k] & pos.Pieces[i][PIECE_PAWN])) << endl;
		}
		KingSafety[i] += PawnShield2Bonus*(int)popcnt(KingShield2[i][k] & pos.Pieces[i][PIECE_PAWN]);
		if (Trace)
		{
			cout << "Bonus for pawns 2 steps in front of king for " << PlayerStrings[i] << ": " << string(PawnShield2Bonus*(int)popcnt(KingShield2[i][k] & pos.Pieces[i][PIECE_PAWN])) << endl;
		}
		if (getPawnMoves(i, k) & pos.Pieces[i][PIECE_BISHOP])
		{
			KingSafety[i] += BishopShieldBonus;
			if (Trace)
			{
				cout << "Bonus for Bishop in front of king for " << PlayerStrings[i] << ": " << string(BishopShieldBonus) << endl;
			}
		}

		//Pawn Storms
		int kingrank = getRank(getColorMirror(i, k));
		Bitset storm = getAboveAndAboveSideBits(i, k)&pos.Pieces[getOpponent(i)][PIECE_PAWN];
		unsigned long enemypawn = 0;
		while (storm)
		{
			_BitScanForward64(&enemypawn, storm);
			storm ^= getPos2Bit(enemypawn);
			KingSafety[i] -= PawnStormPenalty[getRank(getColorMirror(getOpponent(i), enemypawn)) + kingrank];
			if (Trace)
			{
				cout << "Pawn Storm penalty for pawn on " << Int2Sq(enemypawn) << " for " << PlayerStrings[i] << ": " << PawnStormPenalty[getRank(getColorMirror(getOpponent(i), enemypawn)) + kingrank] << endl;
			}
		}

		//King File weaknesses
		if ((getAboveBits(i, k)&pos.Pieces[i][PIECE_PAWN]) == 0) //checks if there arent friendly pawns in the same file
		{
			if (getAboveBits(i, k)&pos.Pieces[getOpponent(i)][PIECE_PAWN]) //checks if there are opponent pawns in the same file
			{
				KingSafety[i] -= KingOnHalfOpenFilePenalty;
				if (Trace)
					cout << "Penalty for King on half open file for " << PlayerStrings[i] << ": " << string(KingOnHalfOpenFilePenalty) << endl;
				//KingAttackUnits[i] += 4; 
			}
			else
			{
				KingSafety[i] -= KingOnOpenFilePenalty;
				if (Trace)
					cout << "Penalty for King on open file for " << PlayerStrings[i] << ": " << string(KingOnOpenFilePenalty) << endl;
				//KingAttackUnits[i] += 8;
			}
		}
		if ((getAboveBits(i, k)&pos.Pieces[getOpponent(i)][PIECE_PAWN]) == 0 && (getAboveBits(i, k)&pos.Pieces[getOpponent(i)][PIECE_ROOK]) != 0)
		{ //checks if there are no opponent pawns on the file and there are rooks on the file
			KingSafety[i] -= KingOnRookFilePenalty;
			if (Trace)
				cout << "Penalty for King on rook file for " << PlayerStrings[i] << ": " << string(KingOnRookFilePenalty) << endl;
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
						cout << "Penalty for King adj to half open file for " << PlayerStrings[i] << ": " << string(KingAdjHalfOpenFilePenalty) << endl;
					//KingAttackUnits[i] += 3;
				}
				else
				{
					KingSafety[i] -= KingAdjOpenFilePenalty;
					if (Trace)
						cout << "Penalty for King adj to open file for " << PlayerStrings[i] << ": " << string(KingAdjOpenFilePenalty) << endl;
					//KingAttackUnits[i] += 6;
				}
			}
			if ((getAboveBits(i, k + 1)&pos.Pieces[getOpponent(i)][PIECE_PAWN]) == 0 && (getAboveBits(i, k + 1)&pos.Pieces[getOpponent(i)][PIECE_ROOK]) != 0)
			{ //checks if there are no opponent pawns on the file and there are rooks on the file
				KingSafety[i] -= KingAdjRookFilePenalty;
				if (Trace)
					cout << "Penalty for King adj to rook file for " << PlayerStrings[i] << ": " << string(KingAdjRookFilePenalty) << endl;
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
						cout << "Penalty for King adj to half open file for " << PlayerStrings[i] << ": " << string(KingAdjHalfOpenFilePenalty) << endl;
					//KingAttackUnits[i] += 3;
				}
				else
				{
					KingSafety[i] -= KingAdjOpenFilePenalty;
					if (Trace)
						cout << "Penalty for King adj to open file for " << PlayerStrings[i] << ": " << string(KingAdjOpenFilePenalty) << endl;
					//KingAttackUnits[i] += 6;
				}
			}
			if ((getAboveBits(i, k - 1)&pos.Pieces[getOpponent(i)][PIECE_PAWN]) == 0 && (getAboveBits(i, k - 1)&pos.Pieces[getOpponent(i)][PIECE_ROOK]) != 0)
			{ //checks if there are no opponent pawns on the file and there are rooks on the file
				KingSafety[i] -= KingAdjRookFilePenalty;
				if (Trace)
					cout << "Penalty for King adj to rook file for " << PlayerStrings[i] << ": " << string(KingAdjRookFilePenalty) << endl;
				//KingAttackUnits[i] += 1;
			}
		}
	}

	///Pawns
	if (Trace)
		cout << endl << "Pawn Structure:" << endl;

	Bitset WeakPawns[2] = { pos.Pieces[COLOR_WHITE][PIECE_PAWN],pos.Pieces[COLOR_BLACK][PIECE_PAWN] };

	for (int i = 0;i < 2;i++)
	{
		b = pos.Pieces[i][PIECE_PAWN];
		e.PawnCount[i] = popcnt(b);
		e.PawnCountColor[i][COLOR_WHITE] = popcnt(b&ColoredSquares[COLOR_WHITE]);
		e.PawnCountColor[i][COLOR_BLACK] = popcnt(b&ColoredSquares[COLOR_BLACK]);

		//No Pawns Penalty
		if (!b)
		{
			PawnStructure[i] -= NoPawnsPenalty;
			if (Trace)
				cout << "Penalty for having no pawns for " << PlayerStrings[i] << ": " << string(NoPawnsPenalty) << endl;
		}

		while (b)
		{
			_BitScanForward64(&k, b);
			b ^= getPos2Bit(k);

			//Weak
			if (getPawnAttacks(getOpponent(i), k)&pos.Pieces[i][PIECE_PAWN]) //checks if this pawn is defended by a friendly pawn
			{
				WeakPawns[i] ^= getPos2Bit(k);
			}

			//Doubled
			if (getAboveBits(i, k)&pos.Pieces[i][PIECE_PAWN]) //checks if there are friendly pawns in the same file
			{
				PawnStructure[i] -= DoubledPawnPenalty[getFile(getColorMirror(i, k))];
				if (Trace)
					cout << "Penalty for doubled pawn on " << Int2Sq(k) << " for " << PlayerStrings[i] << ": " << string(DoubledPawnPenalty[getFile(getColorMirror(i, k))]) << endl;
			}

			//Isolated
			if ((getSideBits(k)&pos.Pieces[i][PIECE_PAWN]) == 0) //checks if there are no friendly pawns on the adjacent files
			{
				PawnStructure[i] -= IsolatedPawnPenalty[getFile(getColorMirror(i, k))];
				if (Trace)
					cout << "Penalty for isolated pawn on " << Int2Sq(k) << " for " << PlayerStrings[i] << ": " << string(IsolatedPawnPenalty[getFile(getColorMirror(i, k))]) << endl;
			}

			//Backward
			if ((getAboveSideBits(getOpponent(i), k)&pos.Pieces[i][PIECE_PAWN]) == 0) //checks if pawn is backward
			{
				if (getRank(getColorMirror(i, k)) < 5)
				{
					if (getPawnAttacks(i, getPlus8(i,k))&pos.Pieces[getOpponent(i)][PIECE_PAWN]
						//&& !(getPawnAttacks(i,k)&pos.Pieces[getOpponent(i)][PIECE_PAWN])
						//&& pos.Squares[getColorMirror(i, getPlus8(getColorMirror(i, k)))] == SQUARE_EMPTY
						)
					{
						PawnStructure[i] -= BackwardPawnPenalty[getFile(k)];
						if (Trace)
							cout << "Penalty for backward pawn on " << Int2Sq(k) << " for " << PlayerStrings[i] << ": " << string(BackwardPawnPenalty[getFile(k)]) << endl;
					}
				}
			}

			//Blocked
			if (pos.OccupiedSq&getPawnMoves(i, k))
			{
				PawnStructure[i] -= BlockedPawnPenalty[getColorMirror(i, k)];
				if (Trace)
					cout << "Penalty for blocked pawn on " << Int2Sq(k) << " for " << PlayerStrings[i] << ": " << string(BlockedPawnPenalty[getColorMirror(i, k)]) << endl;
			}

			//Attack
			Bitset m = getPawnAttacks(i, k);
			e.attackedByColor[i] |= m;
			e.attackedByPiece[i][PIECE_PAWN] |= m;
			e.KingAttackUnits[getOpponent(i)] += popcnt(e.KingField[getOpponent(i)] & m)*AttackWeights[PIECE_PAWN];
			e.KingAttackUnits[getOpponent(i)] += popcnt(e.KingAdj[getOpponent(i)] & m)*AttackWeights[PIECE_PAWN];
			if (Trace && popcnt(e.KingField[getOpponent(i)] & m) != 0)
				cout << "King attack units penalty for pawn on " << Int2Sq(k) << " for " << PlayerStrings[getOpponent(i)] << ": " << popcnt(e.KingField[getOpponent(i)] & m)*AttackWeights[PIECE_PAWN] + popcnt(e.KingAdj[getOpponent(i)] & getPawnAttacks(i, k))*AttackWeights[PIECE_PAWN] << endl;
		}
	}

	///Other pieces
	if (Trace)
		cout << endl << "Piece Activity:" << endl;
	for (int i = 0;i < 2;i++)
	{
		///Rooks
		b = pos.Pieces[i][PIECE_ROOK];
		int rookcount = popcnt(b);

		//Pawn Adj
		PieceActivity[i] += RookPawnAdj[e.PawnCount[i]]*rookcount;
		PieceActivity[i] += RookOppPawnAdj[e.PawnCount[getOpponent(i)]] * rookcount;
		if (Trace)
		{
			cout << "Adj for Rooks based on number of own pawns for " << PlayerStrings[i] << ": " << string(RookPawnAdj[e.PawnCount[i]] * rookcount) << endl;
			cout << "Adj for Rooks based on number of opponent pawns for " << PlayerStrings[i] << ": " << string(RookOppPawnAdj[e.PawnCount[getOpponent(i)]] * rookcount) << endl;
		}

		//Pair
		if(rookcount >= 2)
		{
			PieceActivity[i] += RookPairBonus;
			if (Trace)
				
				cout << "Bonus for Rook Pair for " << PlayerStrings[i] << ": " << string(RookPairBonus) << endl;
		}
		while(b)
		{
			//unsigned long k = 0;
			_BitScanForward64(&k,b);
			b ^= getPos2Bit(k);

			//Open Files
			if((getAboveBits(i,k)&pos.Pieces[i][PIECE_PAWN])==0) //checks if there arent friendly pawns in the same file
			{
				if(getAboveBits(i,k)&pos.Pieces[getOpponent(i)][PIECE_PAWN]) //checks if there are opponent pawns in the same file
				{
					PieceActivity[i] += RookHalfOpenBonus[getFile(k)];
					if(Trace)
						cout << "Bonus for Rook on " << Int2Sq(k) << " on half open for " << PlayerStrings[i] << ": " << string(RookHalfOpenBonus[getFile(k)]) << endl;
				}
				else
				{
					PieceActivity[i] += RookOpenBonus[getFile(k)];
					if (Trace)
						cout << "Bonus for Rook on " << Int2Sq(k) << " on open for " << PlayerStrings[i] << ": " << string(RookOpenBonus[getFile(k)]) << endl;
				}
			}

			Bitset m = getRookAttacks(k, pos.OccupiedSq);
			e.attackedByColor[i] |= m;
			e.attackedByPiece[i][PIECE_ROOK] |= m;

			//Connected Bonus
			if (m&b)
			{
				PieceActivity[i] += RookConnectedBonus;
				if (Trace)
					cout << "Bonus for Rooks being connected for " << PlayerStrings[i] << ": " << string(RookConnectedBonus) << endl;
			}

			//Mobility
			m &= m^e.ColorPieces[i];
			PieceActivity[i] += RookMobility[popcnt(m)];
			if (Trace)
				cout << "Rook on " << Int2Sq(k) << " mobility bonus for " << PlayerStrings[i] << ": " << string(RookMobility[popcnt(m)]) << endl;

			//Pawn Pressure
			PieceActivity[i] += PawnPressureBonus[popcnt(m&WeakPawns[getOpponent(i)])];
			if (Trace)
			{
				cout << "Pawn pressure bonus for rook on " << Int2Sq(k) << ": " << string(PawnPressureBonus[popcnt(m&WeakPawns[getOpponent(i)])]) << endl;
			}
			
			//Attack
			e.KingAttackUnits[getOpponent(i)] += popcnt(e.KingField[getOpponent(i)]&m)*AttackWeights[PIECE_ROOK];
			e.KingAttackUnits[getOpponent(i)] += popcnt(e.KingAdj[getOpponent(i)] & m)*AttackWeights[PIECE_ROOK];
			if (Trace && popcnt(e.KingField[getOpponent(i)] & m) != 0)
				cout << "King attack units penalty for rook on " << Int2Sq(k) << " for " << PlayerStrings[getOpponent(i)] << ": " << popcnt(e.KingField[getOpponent(i)] & m)*AttackWeights[PIECE_ROOK]+ popcnt(e.KingAdj[getOpponent(i)] & m)*AttackWeights[PIECE_ROOK] << endl;
		}

		///Knights
		b = pos.Pieces[i][PIECE_KNIGHT];
		int knightcount = popcnt(b);

		//Pawn Adj
		PieceActivity[i] += KnightPawnAdj[e.PawnCount[i]]*knightcount;
		PieceActivity[i] += KnightOppPawnAdj[e.PawnCount[getOpponent(i)]] * knightcount;
		if (Trace)
		{
			cout << "Adj for Knights based on number of own pawns for " << PlayerStrings[i] << ": " << string(KnightPawnAdj[e.PawnCount[i]] * knightcount) << endl;
			cout << "Adj for Knights based on number of opponent pawns for " << PlayerStrings[i] << ": " << string(KnightOppPawnAdj[e.PawnCount[getOpponent(i)]] * knightcount) << endl;
		}

		//Pair
		if(popcnt(b) >= 2)
		{
			PieceActivity[i] += KnightPairBonus;
			if (Trace)
				cout << "Bonus for Knight Pair for " << PlayerStrings[i] << ": " << string(KnightPairBonus) << endl;
		}

		while(b)
		{
			//unsigned long k = 0;
			_BitScanForward64(&k,b);
			b ^= getPos2Bit(k);

			//Outpost
			if((getAboveSideBits(i,k)&pos.Pieces[getOpponent(i)][PIECE_PAWN])==0) //checks if there are no enemy pawns on the adjacent files
			{
				Score outpostbonus = KnightOutpostBonus[getColorMirror(i, k)];
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
				PieceActivity[i] += outpostbonus;
				if (Trace)
					cout << "Outpost bonus for Knight on " << Int2Sq(k) << " for " << PlayerStrings[i] << ": " << string(outpostbonus) << endl;
			}

			//Mobility
			Bitset m = getKnightMoves(k);
			e.attackedByColor[i] |= m;
			e.attackedByPiece[i][PIECE_KNIGHT] |= m;
			m &= m^e.ColorPieces[i];

			PieceActivity[i] += KnightMobility[popcnt(m)];
			if (Trace)
				cout << "Knight on " << Int2Sq(k) << " mobility bonus for " << PlayerStrings[i] << ": " << string(KnightMobility[popcnt(m)]) << endl;

			//Tension
			if (m&pos.Pieces[getOpponent(i)][PIECE_BISHOP])
			{
				PieceActivity[i] += KnightAttacksBishopBonus;
				if (Trace)
					cout << "Bonus for knight on " << Int2Sq(k) << " attacking a bishop: " << string(KnightAttacksBishopBonus) << endl;
 			}

			//Pawn Pressure
			PieceActivity[i] += PawnPressureBonus[popcnt(m&WeakPawns[getOpponent(i)])];
			if (Trace)
			{
				cout << "Pawn pressure bonus for knight on " << Int2Sq(k) << ": " << string(PawnPressureBonus[popcnt(m&WeakPawns[getOpponent(i)])]) << endl;
			}

			//Attack
			e.KingAttackUnits[getOpponent(i)] += popcnt(e.KingField[getOpponent(i)]&m)*AttackWeights[PIECE_KNIGHT];
			e.KingAttackUnits[getOpponent(i)] += popcnt(e.KingAdj[getOpponent(i)] & m)*AttackWeights[PIECE_KNIGHT];
			if (Trace && popcnt(e.KingField[getOpponent(i)] & m) != 0)
				cout << "King attack units penalty for knight on " << Int2Sq(k) << " for " << PlayerStrings[getOpponent(i)] << ": " << popcnt(e.KingField[getOpponent(i)] & m)*AttackWeights[PIECE_KNIGHT]+ popcnt(e.KingAdj[getOpponent(i)] & m)*AttackWeights[PIECE_KNIGHT] << endl;

			//Minors on back rank
			if(getRank(getColorMirror(i,k))==0)
			{
				MinorsOnBackRank[i]++;
			}
		}

		///Bishops
		b = pos.Pieces[i][PIECE_BISHOP];
		int bishopcount = popcnt(b);

		//Pawn Adj
		PieceActivity[i] += BishopPawnAdj[e.PawnCount[i]]*bishopcount; //adjustment for pawns
		PieceActivity[i] += BishopOppPawnAdj[e.PawnCount[getOpponent(i)]] * bishopcount;
		if (Trace)
		{
			cout << "Adj for Bishops based on number of own pawns for " << PlayerStrings[i] << ": " << string(BishopPawnAdj[e.PawnCount[i]] * bishopcount) << endl;
			cout << "Adj for Bishops based on number of opponent pawns for " << PlayerStrings[i] << ": " << string(BishopOppPawnAdj[e.PawnCount[getOpponent(i)]] * bishopcount) << endl;
		}

		//Pair
		if(popcnt(b) >= 2)
		{
			PieceActivity[i] += BishopPairBonus;
			if (Trace)
				cout << "Bonus for Bishop Pair for " << PlayerStrings[i] << ": " << string(BishopPairBonus) << endl;
		}
		while(b)
		{
			//unsigned long k = 0;
			_BitScanForward64(&k,b);
			b ^= getPos2Bit(k);

			//Outpost
			if((getAboveSideBits(i,k)&pos.Pieces[getOpponent(i)][PIECE_PAWN])==0) //checks if there are no enemy pawns on the adjacent files
			{
				Score outpostbonus = BishopOutpostBonus[getColorMirror(i, k)];
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
				PieceActivity[i] += outpostbonus;
				if (Trace)
					cout << "Outpost bonus for Bishop on " << Int2Sq(k) << " for " << PlayerStrings[i] << ": " << string(outpostbonus) << endl;
			}

			Bitset m = getBishopAttacks(k, pos.OccupiedSq);
			e.attackedByColor[i] |= m;
			e.attackedByPiece[i][PIECE_BISHOP] |= m;
			m &= m^e.ColorPieces[i];

			//Mobility
			PieceActivity[i] += BishopMobility[popcnt(m)];
		
			if (Trace)
				cout << "Bishop on " << Int2Sq(k) << " mobility bonus for " << PlayerStrings[i] << ": " << string(BishopMobility[popcnt(m)]) << endl;

			//Pawn Same Color Adj
			PieceActivity[i] += BishopPawnSameColor[e.PawnCountColor[i][SquareColor[k]]]; //bishop pawn same color adjustment
			PieceActivity[i] += BishopOppPawnSameColor[e.PawnCountColor[getOpponent(i)][SquareColor[k]]]; //bishop opp pawn same color adjustment
			if (Trace)
			{
				cout << "Bishop on " << Int2Sq(k) << " Adj for our pawns on the same color for " << PlayerStrings[i] << ": " << string(BishopPawnSameColor[e.PawnCountColor[i][SquareColor[k]]]) << endl;
				cout << "Bishop on " << Int2Sq(k) << " Adj for opponent pawns on the same color for " << PlayerStrings[i] << ": " << string(BishopOppPawnSameColor[e.PawnCountColor[getOpponent(i)][SquareColor[k]]]) << endl;
			}

			//Tension
			if (m&pos.Pieces[getOpponent(i)][PIECE_KNIGHT])
			{
				PieceActivity[i] += BishopAttacksKnightBonus;
				if (Trace)
					cout << "Bonus for bishop on " << Int2Sq(k) << " attacking a knight: " << string(BishopAttacksKnightBonus) << endl;
			}

			//Pawn Pressure
			PieceActivity[i] += PawnPressureBonus[popcnt(m&WeakPawns[getOpponent(i)])];
			if (Trace)
			{
				cout << "Pawn pressure bonus for bishop on " << Int2Sq(k) << ": " << string(PawnPressureBonus[popcnt(m&WeakPawns[getOpponent(i)])]) << endl;
			}
			
			//Attack
			e.KingAttackUnits[getOpponent(i)] += popcnt(e.KingField[getOpponent(i)]&m)*AttackWeights[PIECE_BISHOP];
			e.KingAttackUnits[getOpponent(i)] += popcnt(e.KingAdj[getOpponent(i)] & m)*AttackWeights[PIECE_BISHOP];
			if (Trace && popcnt(e.KingField[getOpponent(i)] & m) != 0)
				cout << "King attack units penalty for bishop on " << Int2Sq(k) << " for " << PlayerStrings[getOpponent(i)] << ": " << popcnt(e.KingField[getOpponent(i)] & m)*AttackWeights[PIECE_BISHOP]+ popcnt(e.KingAdj[getOpponent(i)] & m)*AttackWeights[PIECE_BISHOP] << endl;

			//Minors on back rank
			if(getRank(getColorMirror(i,k))==0)
			{
				MinorsOnBackRank[i]++;
			}
		}

		///Queens
		b = pos.Pieces[i][PIECE_QUEEN];
		while(b)
		{
			//unsigned long k = 0;
			_BitScanForward64(&k,b);
			b ^= getPos2Bit(k);

			Bitset m = getQueenAttacks(k, pos.OccupiedSq);
			e.attackedByColor[i] |= m;
			e.attackedByPiece[i][PIECE_QUEEN] |= m;
			m &= m^e.ColorPieces[i];

			//Mobility
			PieceActivity[i] += QueenMobility[popcnt(m)]; //mobility factor
			if (Trace)
				cout << "Queen on " << Int2Sq(k) << " mobility bonus for " << PlayerStrings[i] << ": " << string(QueenMobility[popcnt(m)]) << endl;

			//Pawn Pressure
			PieceActivity[i] += (PawnPressureBonus[popcnt(m&WeakPawns[getOpponent(i)])])/2; //half bonus for queens
			if (Trace)
			{
				cout << "Pawn pressure bonus for queen on " << Int2Sq(k) << ": " << string(PawnPressureBonus[popcnt(m&WeakPawns[getOpponent(i)])]/2) << endl;
			}

			//Attack
			e.KingAttackUnits[getOpponent(i)] += popcnt(e.KingField[getOpponent(i)]&m)*AttackWeights[PIECE_QUEEN];
			e.KingAttackUnits[getOpponent(i)] += popcnt(e.KingAdj[getOpponent(i)] & m)*AttackWeights[PIECE_QUEEN];
			if (Trace && popcnt(e.KingField[getOpponent(i)] & m) != 0)
				cout << "King attack units penalty for queen on " << Int2Sq(k) << " for " << PlayerStrings[getOpponent(i)] << ": " << popcnt(e.KingField[getOpponent(i)] & m)*AttackWeights[PIECE_QUEEN]+ popcnt(e.KingAdj[getOpponent(i)] & m)*AttackWeights[PIECE_QUEEN] << endl;

			//Out Early Penalty
			if (getRank(getColorMirror(i, k))>1)
			{
				PieceActivity[i] -= QueenOutEarlyPenalty*MinorsOnBackRank[i];
				if (Trace)
					cout << "Queen out early penalty for " << PlayerStrings[i] << ": " << string(QueenOutEarlyPenalty*MinorsOnBackRank[i]) << endl;
			}
		}
	}

	///Space Bonus
	for (int i = 0;i < 2;i++)
	{
		PieceActivity[i] += S(popcnt(e.attackedByColor[i] & EnemyTerritory[i])*2,0);
		if (Trace)
			cout << "Space bonus for " << PlayerStrings[i] << ": " << string(S(popcnt(e.attackedByColor[i] & EnemyTerritory[i]), 0)*2) << endl;
	}

	///King Attack
	if(Trace)
		cout << endl << "King Attack Evaluation:" << endl;
	for(int i = 0;i<2;i++)
	{
		e.KingAttackUnits[i] -= 5 * popcnt(e.attackedByColor[i] & e.KingAdj[i]);
		if (Trace)
			cout << "Safety Bonus for squares defended adj to king " << PlayerStrings[i] << ": " << 5 * popcnt(e.attackedByColor[i] & e.KingAdj[i]) << endl;

		int whiteDef = popcnt(e.attackedByColor[i] & e.KingField[i] & ColoredSquares[COLOR_WHITE]);
		int blackDef = popcnt(e.attackedByColor[i] & e.KingField[i] & ColoredSquares[COLOR_BLACK]);

		e.KingAttackUnits[i] -= whiteDef*whiteDef + blackDef*blackDef;
		if (Trace)
			cout << "Safety Bonus for colored squares defended near king for " << PlayerStrings[i] << ": " << whiteDef*whiteDef + blackDef*blackDef << endl;

		//e.KingAttackUnits[i] += (unsigned)popcnt(e.KingAdj[i] & e.attackedByColor[getOpponent(i)] & (~e.attackedByColor[i]))*5;

		//Safe Contact Checks
		Bitset undefended = e.KingAdj[i] & (~e.attackedByColor[i]);
		b = undefended & e.attackedByPiece[getOpponent(i)][PIECE_QUEEN];
		if (b)
		{
			b &= e.attackedByPiece[getOpponent(i)][PIECE_PAWN] | e.attackedByPiece[getOpponent(i)][PIECE_BISHOP]
				| e.attackedByPiece[getOpponent(i)][PIECE_KNIGHT] | e.attackedByPiece[getOpponent(i)][PIECE_ROOK];
			e.KingAttackUnits[i] += QueenContactCheckPenalty*(int)popcnt(b);
			if (Trace)
				cout << "Safety Penalty for " << PlayerStrings[i] << " for Queen safe Contact Checks: " << (QueenContactCheckPenalty*(int)popcnt(b)) << endl;
		}

		b = undefended & e.attackedByPiece[getOpponent(i)][PIECE_ROOK] & getRookAttacks(e.KingSquare[i],pos.OccupiedSq);
		if (b)
		{
			b &= e.attackedByPiece[getOpponent(i)][PIECE_PAWN] | e.attackedByPiece[getOpponent(i)][PIECE_BISHOP]
				| e.attackedByPiece[getOpponent(i)][PIECE_KNIGHT] | e.attackedByPiece[getOpponent(i)][PIECE_QUEEN];
			e.KingAttackUnits[i] += RookContactCheckPenalty*(int)popcnt(b);
			if (Trace)
				cout << "Safety Penalty for " << PlayerStrings[i] << " for Rook safe Contact Checks: " << (RookContactCheckPenalty*(int)popcnt(b)) << endl;
		}

		//Safe Checks
		Bitset ratt = getRookAttacks(e.KingSquare[i], pos.OccupiedSq);
		Bitset batt = getBishopAttacks(e.KingSquare[i], pos.OccupiedSq);
		e.KingAttackUnits[i] += CheckPenalty[PIECE_ROOK] * (int)popcnt(~e.attackedByColor[i] & ratt & e.attackedByPiece[getOpponent(i)][PIECE_ROOK]);
		e.KingAttackUnits[i] += CheckPenalty[PIECE_BISHOP] * (int)popcnt(~e.attackedByColor[i] & batt & e.attackedByPiece[getOpponent(i)][PIECE_BISHOP]);
		e.KingAttackUnits[i] += CheckPenalty[PIECE_QUEEN] * (int)popcnt(~e.attackedByColor[i] & (ratt | batt) & e.attackedByPiece[getOpponent(i)][PIECE_QUEEN]);
		e.KingAttackUnits[i] += CheckPenalty[PIECE_KNIGHT] * (int)popcnt(~e.attackedByColor[i] & getKnightMoves(e.KingSquare[i]) & e.attackedByPiece[getOpponent(i)][PIECE_KNIGHT]);

		if (e.KingAttackUnits[i] < 0)
			e.KingAttackUnits[i] = 0;

		if (pos.Pieces[getOpponent(i)][PIECE_QUEEN] == 0)
			e.KingAttackUnits[i] = (3 * e.KingAttackUnits[i]) / 4; //reduce attack units if opponent has no queen

		KingSafety[i] -= KingSafetyScore[min(511,(int)e.KingAttackUnits[i])];
		if (Trace)
		{
			cout << "King Attack Units for " << PlayerStrings[i] << ": " << e.KingAttackUnits[i] << endl;
			cout << "Safety penalty for " << PlayerStrings[i] << ": " << string(KingSafetyScore[min(511, (int)e.KingAttackUnits[i])]) << endl;
			cout << endl;
		}
	}

	///Passers
	for (int i = 0;i < 2;i++)
	{
		b = pos.Pieces[i][PIECE_PAWN];
		while (b)
		{
			_BitScanForward64(&k, b);
			b ^= getPos2Bit(k);

			if ((getAboveAndAboveSideBits(i, k)&pos.Pieces[getOpponent(i)][PIECE_PAWN]) == 0) //checks if the pawn is a passer
			{
				Score passerbonus = PassedPawnBonus[getColorMirror(i, k)];

				Bitset forward = getAboveBits(i, k);
				Bitset advance = getPawnMoves(i, k);
				float factor = 1;

				//Blocked path
				if (forward&e.ColorPieces[getOpponent(i)])
				{
					factor -= 0.15;
				}

				//Unsafe path
				if (forward&e.attackedByColor[getOpponent(i)])
				{
					factor -= 0.15;
				}

				//Blocked advancement
				if (advance&e.ColorPieces[getOpponent(i)])
				{
					factor -= 0.1;
				}

				//Unsafe advancement
				if (advance&e.attackedByColor[getOpponent(i)])
				{
					factor -= 0.1;
				}

				//Protected by pawn
				if (getPawnAttacks(getOpponent(i), k)&pos.Pieces[i][PIECE_PAWN]) 
				{
					factor += 0.5;
				}

				//Attacked by enemy
				else if (getPos2Bit(k)&e.attackedByColor[getOpponent(i)])
				{
					factor -= 0.25;
				}

				//Rook behind Passer
				if (getAboveBits(getOpponent(i), k)&pos.Pieces[i][PIECE_ROOK])
				{
					factor += 0.25;
				}

				//Opponent Rook behind Passer
				if (getAboveBits(getOpponent(i), k)&pos.Pieces[getOpponent(i)][PIECE_ROOK])
				{
					factor -= 0.25;
				}

				//King Proximity
				int sq = (k & 0x7);
				if (i == COLOR_WHITE)
					sq = 63 - getFile(k);
				passerbonus -= S(0, 2 * getSquareDistance(e.KingSquare[i], k));
				passerbonus += S(0, 5 * getSquareDistance(e.KingSquare[getOpponent(i)], sq));

				if (factor < 0.25)
					factor = 0.25;

				passerbonus *= factor;
				Passers[i] += passerbonus;
				if (Trace)
					cout << "Bonus for Passed pawn on " << Int2Sq(k) << " for " << PlayerStrings[i] << ": " << string(passerbonus) << endl;
			}
		}
	}
	

	if (Trace)
	{
		for (int i = 0;i < 2;i++)
		{
			cout << PlayerStrings[i] << " King Safety: " << string(KingSafety[i]) << endl;
			cout << PlayerStrings[i] << " Pawn Structure: " << string(PawnStructure[i]) << endl;
			cout << PlayerStrings[i] << " Piece Activity: " << string(PieceActivity[i]) << endl;
			cout << PlayerStrings[i] << " Passed Pawns: " << string(Passers[i]) << endl;
			cout << endl;
		}

		cout << "Scale Factor: " << ((float)currentMaterial / TotalMaterialBothSides) << endl;

		cout << "After scaling:" << endl;
		for (int i = 0;i < 2;i++)
		{
			cout << PlayerStrings[i] << " King Safety: " << scaleScore(KingSafety[i], currentMaterial) << endl;
			cout << PlayerStrings[i] << " Pawn Structure: " << scaleScore(PawnStructure[i], currentMaterial) << endl;
			cout << PlayerStrings[i] << " Piece Activity: " << scaleScore(PieceActivity[i], currentMaterial) << endl;
			cout << PlayerStrings[i] << " Passed Pawns: " << scaleScore(Passers[i], currentMaterial) << endl;
			cout << endl;
		}
	}

	TotalEval += Score(Material[COLOR_WHITE]) + KingSafety[COLOR_WHITE] + PawnStructure[COLOR_WHITE] + PieceActivity[COLOR_WHITE]
		+ Passers[COLOR_WHITE];
	TotalEval -= Score(Material[COLOR_BLACK]) + KingSafety[COLOR_BLACK] + PawnStructure[COLOR_BLACK] + PieceActivity[COLOR_BLACK]
		+ Passers[COLOR_BLACK];

	int finaleval = scaleScore(TotalEval,currentMaterial);

	if(pos.turn==COLOR_BLACK)
		return -finaleval;

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
	/*RookContactCheckPenalty *= KingSafetyFactor;
	QueenContactCheckPenalty *= KingSafetyFactor;
	for (int i = 0;i < 6;i++)
	{
		CheckPenalty[i] *= KingSafetyFactor;
	}*/

	for (int i = 0;i < 100;i++)
	{
		KingSafetyScore[i] *= KingSafetyFactor;
	}

	for (int i = 0;i < 8;i++)
	{
		PawnStormPenalty[8] *= KingSafetyFactor;
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
	int TrappedMinorPenalty = 150;
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

		if (i == 63 || i == (63 - 7))
			PieceSqValues[PIECE_KNIGHT][i] = TrappedMinorPenalty;
		if (i == (63 - 8) || i == (63 - 8 - 7))
			PieceSqValues[PIECE_BISHOP][i] = TrappedMinorPenalty;
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
	int t = 0;
	for (int i = 0;i < 512;i++)
	{
		t = min(512, (int)min((0.025*i*i), t + 100.0));
		KingSafetyScore[i] = S(t, 0);
	}
	scaleConstants();
}