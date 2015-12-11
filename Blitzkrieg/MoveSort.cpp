#include "Engine.h"

void Engine::generateCaptureScores(vector<Move>& moves, vector<int>& scores)
{
	for (int i = 0;i < moves.size();i++)
	{
		Move m = moves.at(i);
		int cap = m.getCapturedPiece();
		if (cap != SQUARE_EMPTY)
		{
			scores.push_back(StaticExchangeEvaluation(m.getTo(), m.getFrom(), m.getMovingPiece(), cap));
		}
		else if (m.getSpecial() == PIECE_PAWN)
		{
			scores.push_back(StaticExchangeEvaluation(m.getTo(), m.getFrom(), m.getMovingPiece(), SQUARE_WHITEPAWN));
		}
		else
		{
			scores.push_back(0);
		}
	}
}

long long Engine::getMoveScore(const Move& m)
{
	int from = m.getFrom();
	int to = m.getTo();
	int capturedpiece = m.getCapturedPiece();
	int special = m.getSpecial();
	int movingpiece = m.getMovingPiece();
	long long score = 1000000;
	if (ply < PrincipalVariation.size())
	{
		if (m == PrincipalVariation.at(PrincipalVariation.size() - 1 - ply))
		{
			score += 6000000;
			SortPhase = SORTPHASE_PV;
			//cout << "info string pv hit " << ply << " " << m.toString() << " " << (PrincipalVariation.size() - 1 - ply) << endl;
			return score;
		}
	}
	if (m == Table.getBestMove(pos.TTKey)) //history best move is always first, give it a big advantage of 400000
	{
		score += 4000000;
		SortPhase = SORTPHASE_HASH;
		//tthitcount++;
		return score;
	}
	/*if (!pos.makeMove(m))
		return -10000;
	bool incheck = pos.underCheck(pos.turn);
	pos.unmakeMove(m);
	if (incheck)
	{
		score += 3500000;
		return score;
	}*/
	/*if (incheck[ply] && movingpiece == PIECE_KING)
	{
		SortPhase = SORTPHASE_GOODCAP;
		score += 3200000;
		return score;
	}*/
	if (m.getSpecial() == PIECE_QUEEN) //queen promotion
	{
		SortPhase = SORTPHASE_GOODCAP;
		score += 3100000;
		return score;
	}
	if (capturedpiece != SQUARE_EMPTY) //a capture
	{
		//if (Threats[ply].getFrom() == to && Threats[ply]!=CONS_NULLMOVE) //capturing a threatening piece
		//{
		//	score += 200000; 
		//}
		int x = StaticExchangeEvaluation(to, from, movingpiece, capturedpiece);
		//int x = movescore;
		if (x >= 0) //if it is a good capture
		{
			SortPhase = SORTPHASE_GOODCAP;
			score += 3000000 + x;
			return score;
		}
		else //bad capture
		{
			SortPhase = SORTPHASE_BADCAP;
			score += -500000 + x;
			return score;
		}
	}
	else if (special == PIECE_PAWN) //enpassant are also captures
	{
		int x = StaticExchangeEvaluation(to, from, movingpiece, capturedpiece);
		if (x >= 0)
		{
			SortPhase = SORTPHASE_GOODCAP;
			score += 3000000 + x;
			return score;
		}
		else
		{
			SortPhase = SORTPHASE_BADCAP;
			score += -500000 + x;
			return score;
		}
	}
	else
	{
		SortPhase = SORTPHASE_KILLER;
		if (from == KillerMoves[0][ply].getFrom() && to == KillerMoves[0][ply].getTo()) //if its a killer move
		{
			score += 2500000;
			return score;
		}
		else if (from == KillerMoves[1][ply].getFrom() && to == KillerMoves[1][ply].getTo())
		{
			score += 2000000;
			return score;
		}
		/*else if (from == KillerMoves[2][ply].getFrom() && to == KillerMoves[2][ply].getTo())
		{
			score += 1500000;
			return score;
		}*/
		else
		{
			SortPhase = SORTPHASE_HISTORY;
			//if (pos.underCheck(pos.turn) == false) //move a threatened piece
			//{
			//	Move null = createNullMove(pos.epsquare);
			//	pos.makeMove(null);
			//	Move m2 = pos.getSmallestAttacker(getOpponent(pos.turn), m.getFrom());
			//	pos.unmakeMove(null);

			//	int x = StaticExchangeEvaluation(m.getFrom(), m2.getFrom(), m2.getMovingPiece(), m2.getCapturedPiece());

			//	if (x > 0)
			//	{
			//		score += 10000;
			//	}
			//}

			//if (Threats[ply].getTo() == from && Threats[ply]!=CONS_NULLMOVE) //moving a threatened piece
			//{
			//	score += 1000000;
			//}

			score += HistoryScores[movingpiece][to]; //sort the rest by history
													 /*int p2sq = getPiece2Square(movingpiece, pos.turn);
													 if (pos.turn == COLOR_BLACK)
													 p2sq = -p2sq;
													 score += PieceSq[p2sq][to] - PieceSq[p2sq][from];*/
			return max(score,(long long)0);
		}
	}
	cout << "info string Move sort error" << endl;
	return score;
}

Move Engine::getHighestScoringMove(vector<Move>& moves, int currentmove)
{
	//sorttime.Start();
	int bigmoveid = currentmove;
	Move bigmove = moves.at(currentmove);
	long long bigscore = getMoveScore(bigmove);
	long long x;
	int bigphase = SortPhase;
	for (int i = currentmove + 1;i<moves.size();i++)
	{
		x = getMoveScore(moves.at(i));
		if (x>bigscore)
		{
			bigscore = x;
			bigmoveid = i;
			bigmove = moves.at(i);
			bigphase = SortPhase;
		}
	}
	Move m = bigmove; //swap move
	moves.at(bigmoveid) = moves.at(currentmove);
	moves.at(currentmove) = m;
	SortPhase = bigphase;
	//int sc = scores.at(bigmove); //swap score
	//scores.at(bigmove) = scores.at(currentmove);
	//scores.at(currentmove) = sc;
	//sorttime.Stop();
	return m;
}

void Engine::setKiller(Move m, int depth, int score)
{
	if (m != KillerMoves[0][ply])
	{
		//KillerMoves[2][ply] = KillerMoves[1][ply];
		KillerMoves[1][ply] = KillerMoves[0][ply];
		//KillerScores[1][ply] = KillerScores[0][ply];
		KillerMoves[0][ply] = m;
		//KillerScores[0][ply] = score;
		//cout << "Killer set: " << m.toString() << endl;
	}
	//else if (m != KillerMoves[1][ply])
	//{
	//	//KillerMoves[2][ply] = KillerMoves[1][ply];
	//	KillerMoves[1][ply] = m;
	//	//KillerScores[1][ply] = score;
	//}
}

//void Engine::movesort(vector<Move>& moves,int depth)
//{
//	vector<double> score;
//	for(unsigned int i = 0;i<moves.size();i++)
//	{
//		Move m = moves.at(i);
//		score.push_back((HistoryScores[m.getFrom()][m.getTo()]*1.0));
//		if(ply < PrincipalVariation.size())
//		{
//			if(m==PrincipalVariation.at(ply))
//			{
//				score.at(i) += 400000;
//			}
//		}
//		int from = m.getFrom();
//		int to = m.getTo();
//		if(from==KillerMoves[0][ply].getFrom() && to==KillerMoves[0][ply].getTo())
//		{
//			score.at(i) += 250000;
//		}
//		else if (from == KillerMoves[1][ply].getFrom() && to == KillerMoves[1][ply].getTo())
//		{
//			score.at(i) += 200000;
//		}
//	}
//	for(unsigned int i = 0;i<moves.size();i++)
//	{
//		for(unsigned int j = 0;j<moves.size()-1-i;j++)
//		{
//			if(score.at(j)<score.at(j+1))
//			{
//				int tmp = score.at(j);
//				score.at(j) = score.at(j+1);
//				score.at(j+1) = tmp;
//				Move mov = moves.at(j);
//				moves.at(j) = moves.at(j+1);
//				moves.at(j+1) = mov;
//			}
//		}
//	}
//}