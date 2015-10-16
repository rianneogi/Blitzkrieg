#include "Engine.h"

int Engine::QuiescenceSearch(int alpha,int beta,Move lastmove)
{
	nodes++;
	if(nodes%CheckupNodeCount==0)
	{
		checkup();
		//nodes = 0;
	}
	if(lastmove.getCapturedPiece()!=0)
	{
		vector<Move> vec; //generate moves
		vec.reserve(128);
		pos.generateMoves(vec);

		vector<int> scores; //generate move scores
		scores.reserve(128);
		generateCaptureScores(vec, scores);

		//movesort(vec,0);
		int to = lastmove.getTo();
		//int max = CONS_NEGINF;
		bool change = false;
		Move m;
		int score = 0;
		for(int i = 0;i<vec.size();i++)
		{
			m = getHighestScoringMove(vec,scores,i);
			if(m.getTo() == to) //capturing a piece that just captured a piece in the last move
			{
				if(!pos.makeMove(m))
				{
					continue;
				}
				ply++;
				score = -QuiescenceSearch(-beta,-alpha,m);
				pos.unmakeMove(m);
				ply--;
				if(score>=beta)
					return score;
				if(score>alpha)
				{
					alpha = score;
					change = true;
				}
			}
		}
		if(change)
			return alpha;
		return LeafEval(alpha,beta);
	}
	return LeafEval(alpha,beta);
}

int Engine::QuiescenceSearchStandPat(int alpha,int beta,Move lastmove)
{
	quisctime.Start();
	nodes++;
	if(nodes%1024==0)
	{
		checkup();
		//nodes = 0;
	}
	/*if(pos.getGameStatus()!=STATUS_NOTOVER)
	{
		int val = LeafEval(alpha,beta);
		if(val >= beta)
			return beta;
		else if(val < alpha)
			return alpha;
		return val;
	}*/
	int stand_pat = LeafEval(alpha,beta);
	if(stand_pat >= beta) //standpat
	{
		return beta;
	}
	if(stand_pat < alpha-PieceMaterial[PIECE_QUEEN]) //big delta pruning
	{
		return alpha;
	}
	if(alpha < stand_pat)
	{
		alpha = stand_pat;
	}
	Move m;
	int score = 0;

    vector<Move> vec; //generate moves
	vec.reserve(128);
	movegentime.Start();
	pos.generateCaptures(vec);
	movegentime.Stop();

	vector<int> scores; //generate move scores
	scores.reserve(128);
	generateCaptureScores(vec, scores);

	int material = getBoardMaterial();
	for(int i = 0;i<vec.size();i++)
	{
		m = getHighestScoringMove(vec,scores,i);
		int special = m.getSpecial();
		int captured = m.getCapturedPiece();
		if((stand_pat + PieceMaterial[getSquare2Piece(captured)] + 200 < alpha) && //delta pruning
		   (special!=PIECE_QUEEN && special!=PIECE_KNIGHT  && special!=PIECE_ROOK && special!=PIECE_BISHOP) && //not a promotion
           (material > EndgameMaterial))
		{
            continue;
		}
		if(StaticExchangeEvaluation(m.getTo(),m.getFrom(),m.getMovingPiece(),captured)<0)
			continue;
		if(!pos.makeMove(m))
		{
			continue;
		}
		ply++;
		score = -QuiescenceSearchStandPat(-beta,-alpha,m);
		pos.unmakeMove(m);
		ply--;
		if(score >= beta)
			return beta;
		if(alpha < score)
			alpha = score;
	}
	quisctime.Stop();
	return alpha;
}