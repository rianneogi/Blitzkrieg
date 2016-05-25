#include "Engine.h"

int Engine::QuiescenceSearch(int alpha,int beta)
{
	//quisctime.Start();

	if (isDraw()) return 0;

	nodes++;
	if(nodes%CheckupNodeCount == 0)
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
	int stand_pat = 0;
	//ProbeStruct probe = Table.Probe(pos.TTKey, -1, alpha, beta);
	//if (probe.found && probe.entry->bound == TT_EXACT)
	//{
	//	return probe.score;
	//	stand_pat = probe.score; //use TT probe as leafeval
	//}
	//else
	//{
		stand_pat = LeafEval<false>();
	//} 
	if(stand_pat >= beta) //standpat
	{
		return stand_pat;
	}
	if(stand_pat <= alpha-PieceMaterial[PIECE_QUEEN]) //big delta pruning
	{
		return stand_pat;
	}
	if(alpha < stand_pat)
	{
		alpha = stand_pat;
	}
	Move m;
	int score = 0;

    vector<Move> vec; //generate moves
	vec.reserve(128);
	//movegentime.Start();
	pos.generateCaptures(vec);
	//pos.generatePawnPushes(vec);
	//movegentime.Stop();

	//vector<int> scores; //generate move scores
	//scores.reserve(128);
	//generateCaptureScores(vec, scores);

	int material = getBoardMaterial<COLOR_WHITE>()+getBoardMaterial<COLOR_BLACK>();
	Move bestmove = CONS_NULLMOVE;
	int bound = TT_ALPHA;
	for(int i = 0;i<vec.size();i++)
	{
		//m = getHighestScoringMove(vec,i);
		m = vec[i];
		int special = m.getSpecial();
		int captured = m.getCapturedPiece();
		if ((stand_pat + PieceMaterial[getSquare2Piece(captured)] + 200 < alpha) //delta pruning
			&& (special != PIECE_QUEEN && special != PIECE_KNIGHT  && special != PIECE_ROOK && special != PIECE_BISHOP) //not a promotion
			//&& (material > EndgameMaterial)
			)
		{
            continue;
		}
		if (StaticExchangeEvaluation(m.getTo(), m.getFrom(), m.getMovingPiece(), captured) < 0)
			continue;
			//break; //since moves are sorted by SEE, we know remaining moves will also have SEE<0
		//if (getSquare2Piece(m.getCapturedPiece()) == PIECE_KING) //captured opponent king
		//	return CONS_INF;
		if(!pos.makeMove(m))
		{
			continue;
		}
		//pos.forceMove(m);
		ply++;
		if (ply > SelectiveDepth)
		{
			SelectiveDepth = ply;
		}
		score = -QuiescenceSearch(-beta,-alpha);
		pos.unmakeMove(m);
		ply--;
		if (score >= beta)
		{
			//Table.Save(pos.TTKey, -1, score, TT_BETA, m);
			return score;
		}
			
		if (alpha < score)
		{
			alpha = score;
			bestmove = m;
			bound = TT_EXACT;
		}
			
	}
	//Table.Save(pos.TTKey, -1, alpha, TT_ALPHA, bestmove);
	//quisctime.Stop();
	return alpha;
}