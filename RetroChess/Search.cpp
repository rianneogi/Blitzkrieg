#include "Engine.h"

#include <setjmp.h>

jmp_buf env;

int FutilityMargin[3] = {0,300,600};

unsigned int HistoryScores[64][64];
unsigned int ButterflyScores[64][64];

int MAXTIME = 1000;
int MAXDEPTH = 100;
int CheckupNodeCount = 16384;

Move Engine::IterativeDeepening()
{
	int status = pos.getGameStatus();
	if(status!=STATUS_NOTOVER)
	{
		if(status==STATUS_BLACKMATED || status==STATUS_WHITEMATED)
		{
			/*if(alpha>CONS_MATED)
				return alpha;*/
			return CONS_NULLMOVE;
		}
		if(status==STATUS_STALEMATE)
		{
			/*if(alpha>=CONS_DRAW)
				return alpha;
			if(beta<=CONS_DRAW)
				return beta;*/
			return CONS_NULLMOVE;
		}
	}

	//init
	nodes = 0;
	ply = 0;
	for(int i = 0;i<2;i++)
	{
		for(int j = 0;j<100;j++)
		{
			KillerMoves[i][j]=CONS_NULLMOVE;
		}
	}
	for(unsigned int i = 0;i<64;i++) //ages the history table
	{
		for(unsigned int j = 0;j<64;j++)
		{
			HistoryScores[i][j] /= 8;
			ButterflyScores[i][j] /= 8;
		}
	}
	evaltime.Reset();
	sorttime.Reset();
	movegentime.Reset();
	quisctime.Reset();
	timer.Reset();

	vector<Move> line;
	line.reserve(128);
	PrincipalVariation = vector<Move>();
	PrincipalVariation.reserve(128);
	int score = AlphaBeta(1,CONS_NEGINF,CONS_INF,CONS_NULLMOVE,&line,false,false);
	//int score = think(1,CONS_NEGINF,CONS_INF,&line);
	PrincipalVariation = line;
	int val = 0;
	timer.Start();
	int takeback = 0;
	int initialmovenumber = pos.movelist.size();
	//cout << "takeback set " << takeback << " " << PrincipalVariation.size() << endl;
	takeback = setjmp(env);
	if(takeback!=0)
	{
		while(pos.movelist.size()>initialmovenumber)
		{
			pos.takebackMove();
			takeback--;
		}
		cout << "info string Eval time: " << evaltime.time << ", Sort time: " << sorttime.time << ", Quisc time: " << quisctime.time << ", movegen time: " << movegentime.time << endl;
		return PrincipalVariation.at(PrincipalVariation.size()-1);
	}
	for(int i = 2;i<=MAXDEPTH;i++)
	{
		//mr = think(i,CONS_NEGINF,CONS_INF);
		//mr = think(i,score - 100,score + 100); //aspiration search
		//if(mr.eval <= score-150 || mr.eval >= score+150)
		//{
		//	cout << "Aspiration fail " << mr.eval << endl;
		//	mr = think(i,CONS_NEGINF,CONS_INF);
		//}
		int low = 8;
		int high = 8;
		//if(i==4) DEBUG=true;
		while(true)
		{
			//line = deque<Move>();
			ply = 0;
			//val = think(i,score - low,score + high,&line); //aspiration search
			val = AlphaBeta(i,score - low,score + high,CONS_NULLMOVE,&line,true,true);
			//cout << mr.move.toString() << " " << mr.eval << endl;
			if(val <= score - low)
			{
				low = low << 1;
				//cout << "Value is " << val <<" Aspiration fail low set to " << low << endl;
				//high = 1;
			}
			else if(val >= score + high)
			{
				high = high << 1;
				//cout << "Value is " << val << " Aspiration fail high set to " << high << endl;
				//low = 1;
			}
			else break;
		}
        score = val;
		//firstguess = val;
		timer.Stop();
		cout << "info score cp " << score << " depth " << i << " nodes " << nodes << " nps " << getNPS(nodes,timer.ElapsedMilliseconds()) << " time " << timer.ElapsedMilliseconds() << " pv ";
		for(int j = line.size()-1;j>=0;j--)
		{
			cout << line.at(j).toString() << " ";
		}
		cout << endl;
		PrincipalVariation = line;
	}
	Move bestmove = PrincipalVariation.at(PrincipalVariation.size()-1);
	cout << "Nodes: " << nodes << endl;
	cout << "info string Eval time: " << evaltime.time << ", Sort time: " << sorttime.time << ", Quisc time: " << quisctime.time << ", movegen time: " << movegentime.time << endl;
	//cout << bestmove.toString() << " " << firstguess << endl;
	return bestmove;
}

int Engine::think(int depth,int alpha,int beta,vector<Move>* variation)
{
	Move alphamove=CONS_NULLMOVE;
	int bound=TT_ALPHA;
	/*if(depth==0)
		return LeafEval();*/

	vector<Move> vec;
	vec.reserve(128);
	pos.generateMoves(vec);
	//movesort(vec,depth);

	int score = 0;
	Move m;
	int pvsize = PrincipalVariation.size();
	int rootturn = pos.turn;
	vector<Move>* lineptr;
	//lineptr.reserve(128);
	//if(pvsize!=0 && depth>pvsize) //search pv first
	//{
	//	deque<Move> line;
	//	for(int i = 0;i<pvsize;i++)
	//	{
	//      ply++;
	//		m = PrincipalVariation.at(i);
	//		pos.forceMove(PrincipalVariation.at(i));
	//	}
	//	if(pos.turn==rootturn)
	//	{
	//		score = AlphaBeta(depth-pvsize,-beta,-alpha,m,&line,true,true);
	//	}
	//	else
	//	{
	//		score = -AlphaBeta(depth-pvsize,-beta,-alpha,m,&line,true,true);
	//	}
	//	for(int i = pvsize-1;i>=0;i--)
	//	{
	//      ply--;
	//		pos.unmakeMove(PrincipalVariation.at(i));
	//	}
	//	if(score >= beta)
	//	{
	//		return beta;
	//	}
	//	if(score > alpha)
	//	{
	//		alpha = score;
	//		alphamove = PrincipalVariation.at(0);
	//		lineptr = line;
	//	}
	//}
	//if(lineptr.size()!=0)
	//{
	//	for(int i = pvsize-1;i>=0;i--)
	//	{
	//		lineptr.push_front(PrincipalVariation.at(i));
	//	}
	//}
	vector<Move> line;
	line.reserve(128);
	score = AlphaBeta(depth,alpha,beta,CONS_NULLMOVE,&line,true,true);
	if(score >= beta)
	{
		return beta;
	}
	if(score > alpha)
	{
		alpha = score;
		lineptr = &line;
		alphamove = lineptr->at(lineptr->size()-1);
	}
	//deque<Move> dummyline;
	//bool foundpv = false;
	//bool latemove = true;
	//for(unsigned int i = 0;i<vec.size();i++) //search
	//{
	//	if(!(vec.at(i)==historymove))
	//	{
	//		m = vec.at(i);
	//		pos.forceMove(m);
	//		int score = 0;
	//		if(foundpv) //principal variation search
	//		{
	//			score = -AlphaBeta(depth-1,-alpha-1,-alpha,m,&dummyline);
	//			if(score>alpha && score<beta) //check for failure
	//			{
	//				score = -AlphaBeta(depth-1,-beta,-alpha,m,&line); //research
	//				//cout << "pv research" << endl;
	//			}
	//		}
	//		else if(i>=4 && depth>=3 && m.getCapturedPiece()==0 && m.getSpecial()==PIECE_NONE && latemove) //latemove reduction
	//		{
	//			score = -AlphaBeta(depth-3,-beta,-alpha,m,&dummyline);
	//			//cout << "latemove" << endl;
	//			if(score>alpha)
	//			{
	//				score = -AlphaBeta(depth-1,-beta,-alpha,m,&line);
	//			//cout << "latemove research" << endl;
	//			}
	//		}
	//		else
	//		{
	//			score = -AlphaBeta(depth-1,-beta,-alpha,m,&line);
	//		}
	//		pos.unmakeMove(m);
	//		if(score >= beta)
	//		{
	//			return beta;
	//		}
	//		if(score>alpha)
	//		{
	//			bound = TT_EXACT;
	//			alpha = score;
	//			alphamove = m;
	//			lineptr = &line;
	//			foundpv = true;
	//			latemove = false;
	//		}
	//	}
	//}
	//Table.Save(pos.TTKey,depth,score,bound,alphamove);
	if(!(alphamove==CONS_NULLMOVE))
	{
		variation = lineptr;
	}
	//variation->push_front(alphamove);
	return alpha;
}

int Engine::AlphaBeta(int depth,int alpha,int beta,Move lastmove,vector<Move>* variation,bool cannull,bool dopv)
{
	//int status = pos.getGameStatus();
	//if(status!=STATUS_NOTOVER)
	//{
	//	if(status==STATUS_BLACKMATED || status==STATUS_WHITEMATED)
	//	{
	//		if(alpha>CONS_MATED)
	//			return alpha;
	//		return CONS_MATED; //mate
	//	}
	//	if(status==STATUS_STALEMATE)
	//	{
	//		if(alpha>=CONS_DRAW)
	//			return alpha;
	//		if(beta<=CONS_DRAW)
	//			return beta;
	//		return CONS_DRAW; //stalemate
	//	}
	//}

	/*vector<Move> vec = pos.generateMoves();
	if(vec.size()==0)
	{
		if(pos.underCheck(pos.turn))
		{
			return CONS_MATED;
		}
		else
		{
			return CONS_DRAW;
		}
	}*/

	if(pos.underCheck(pos.turn)) //check extension
	{
		depth++;
	}

	if(depth==0)
	{
		int value = QuiescenceSearchStandPat(alpha,beta,lastmove); //go to quiescence
		//Table.Save(pos.TTKey,0,value,TT_EXACT,CONS_NULLMOVE);
		return value;
	}

	nodes++;
	if(nodes%CheckupNodeCount==0)
	{
		checkup();
		//nodes = 0;
	}

	if(ply!=0 && pos.isRepetition()) //check for repetition
	{
		//Table.Save(pos.TTKey,depth,0,TT_EXACT,CONS_NULLMOVE);
		return 0;
	}

	int probe = Table.Probe(pos.TTKey,depth,alpha,beta);
	if (probe != CONS_TTUNKNOWN)
	{
		//cout << probe << " found " << pos.TTKey << endl;
		Move ttbestmove = Table.getBestMove(pos.TTKey);
		if (!(ttbestmove == CONS_NULLMOVE) && (!dopv || (probe>alpha && probe<beta)))
		{
			variation->push_back(ttbestmove);
			return probe;
		}
		/*else if(ply!=0)
		{
			return probe;
		}*/
	}

	int bound = TT_ALPHA;
	//vector<Move> dummyline;
	//dummyline.reserve(128);
	//vector<Move>* lineptr;
	//lineptr.reserve(128);
	Move m;
	int score = 0;

	//razoring
	//int fscore = LeafEval() + 150;
	//if((depth == 3) && (fscore <= alpha) && (pos.getColorPieces(getOpponent(pos.turn)) > 3)) 
	//{
	//	depth = 2;
	//}
	//
	//more razoring
	/*if(depth<=3 && depth>=2)
	{
		int fscore = LeafEval() + 300;
		if (fscore < beta) 
		{
			deque<Move> line;
			int newfscore = AlphaBeta(1,alpha,beta,CONS_NULLMOVE,&line);
			if (newfscore < beta)
			{
				*variation = line;
				return max(newfscore,fscore);
			}
		}
	}*/

	//adaptive null move pruning
	Bitset Pieces = pos.OccupiedSq ^ pos.Pieces[COLOR_WHITE][PIECE_PAWN] ^ pos.Pieces[COLOR_BLACK][PIECE_PAWN];
	int pieceCount = popcnt(Pieces);
    if(cannull && depth>=3 && pos.underCheck(pos.turn)==false && pieceCount>6) //not endgame
    {
		int R = depth > 5 ? 3 : 2; //dynamic depth-based reduction
		m = CONS_NULLMOVE;
		ply++;
		pos.forceMove(m);
        score = -AlphaBeta(depth-R-1,-beta,-beta+1,m,variation,false,false); //make a null-window search (we don't care by how much it fails high, if it does)
		ply--;
        pos.unmakeMove(m);
		if(score >= beta)
		{
			//cout << "Null move cutoff " << beta << endl;
			return beta;
		}
    }

	//futility pruning
	bool futilityprune = false; 
	if(depth<3 && !pos.underCheck(pos.turn) && (LeafEval(alpha,beta) + FutilityMargin[depth] <= alpha))
	{
		futilityprune = true;
	}

	//movesort(vec,depth);
	bool alpharaised = false;
	bool foundlegal = false;
	Move alphamove = CONS_NULLMOVE;
	//vec = pos.generateMoves();
	vector<Move> vec;
	vec.reserve(128);
	movegentime.Start();
	if(futilityprune)
	{
    	pos.generateCaptures(vec); //search only captures in futility pruning
	}
	else
	{
		pos.generateMoves(vec);
    }
	movegentime.Stop();
	//vector<Move> line;
	//line.reserve(128);
	for(unsigned int i = 0;i<vec.size();i++) //search
	{
		//line.clear();
		//dummyline.clear();
		//m = vec.at(i);
		m = getHighestScoringMove(vec,i);
		//Position x = pos; //debug
		/*if(ply==0)
		{
			cout << "info currmove " << m.toString() << endl;
		}*/
		/*if(ply==0)
		{
			cout << m.toString() << "\n";
			_getch();
		}*/
		if(!pos.makeMove(m))
		{
			continue;
		}
		foundlegal = true;
		ply++;
		score = 0;
		//if(popcnt(pos.Pieces[COLOR_WHITE][PIECE_QUEEN])+popcnt(pos.Pieces[COLOR_WHITE][PIECE_PAWN])>9 || popcnt(pos.Pieces[COLOR_BLACK][PIECE_QUEEN])+popcnt(pos.Pieces[COLOR_BLACK][PIECE_PAWN])>9)
		//{
		//	x.display(0);
		//	pos.display(0);
		//	printBitset(x.Pieces[COLOR_BLACK][PIECE_PAWN]);
		//	cout << "\n";
		//	printBitset(pos.Pieces[COLOR_BLACK][PIECE_PAWN]);
		//	cout << m.toString() << endl;
		//	cout << lastmove.toString() << endl;
		//	for(int i = 0;i<pos.movelist.size();i++)
		//	{
		//		cout << pos.movelist.at(i).toString() << " " << endl;
		//	}
		//	cout << "";
		//	//pos.unmakeMove(m);
		//	//return beta;
		//}

		int capturedpiece = m.getCapturedPiece();
		int special = m.getSpecial();

		//futility pruning
		//if(futilityprune && capturedpiece==SQUARE_EMPTY && (special==PIECE_NONE || special==PIECE_KING)
		//	&& !pos.underCheck(pos.turn))
		//{
		//	ply--;
		//	pos.unmakeMove(m);
		//	continue; //prune away non-capture, non-promotion moves in futility pruning
		//}

		if(dopv && alpharaised && depth>=3) //principal variation search
		{
			score = -AlphaBeta(depth-1,-alpha-1,-alpha,m,variation,cannull,false);
			if(score>alpha && score<beta) //check for failure
			{
				score = -AlphaBeta(depth - 1, -beta, -alpha, m, variation, cannull, true); //research
				//cout << "pv research" << endl;
			}
		}
		else if(i>=4 && depth>=3 && capturedpiece==SQUARE_EMPTY && special==PIECE_NONE && !alpharaised) //latemove reduction
		{
			int reductiondepth = 2;
			if (i >= 8) reductiondepth++;
			if (i >= 12) reductiondepth++;
			if (depth >= 6) reductiondepth++;
			if (depth >= 9) reductiondepth++;
			score = -AlphaBeta(max(depth - reductiondepth,0), -beta, -alpha, m, variation, cannull, dopv);
			//cout << "latemove" << endl;
			if(score>alpha)
			{
				score = -AlphaBeta(depth - 1, -beta, -alpha, m, variation, cannull, dopv);
				//cout << "latemove research" << endl;
			}
		}
		else
		{
			score = -AlphaBeta(depth - 1, -beta, -alpha, m, variation, cannull, dopv);
		}
		ply--;
		pos.unmakeMove(m);
		//if(popcnt(pos.Pieces[COLOR_WHITE][PIECE_QUEEN])+popcnt(pos.Pieces[COLOR_WHITE][PIECE_PAWN])>9 || popcnt(pos.Pieces[COLOR_BLACK][PIECE_QUEEN])+popcnt(pos.Pieces[COLOR_BLACK][PIECE_PAWN])>9)
		//{
		//	x.display(0);
		//	pos.display(0);
		//	printBitset(x.Pieces[COLOR_BLACK][PIECE_PAWN]);
		//	cout << "\n";
		//	printBitset(pos.Pieces[COLOR_BLACK][PIECE_PAWN]);
		//	cout << m.toString() << endl;
		//	cout << lastmove.toString() << endl;
		//	for(int i = 0;i<pos.movelist.size();i++)
		//	{
		//		cout << pos.movelist.at(i).toString() << " " << endl;
		//	}
		//	for(int i = 0;i<line.size();i++)
		//	{
		//		cout << line.at(i).toString() << " " << endl;
		//	}
		//	cout << "";
		//	//return beta;
		//}
		if(score>=beta)
		{
			Table.Save(pos.TTKey,depth,score,TT_BETA,m);
			if(capturedpiece==SQUARE_EMPTY && special!=PIECE_PAWN)
			{
				HistoryScores[m.getFrom()][m.getTo()]+=depth*depth;
				setKiller(m,depth);
			}
			return beta; //fail hard beta cutoff
		}
		else if(score>alpha)
		{
			//if(capturedpiece==SQUARE_EMPTY && special!=PIECE_PAWN)
				//ButterflyScores[m.getFrom()][m.getTo()]++;
			bound = TT_EXACT;
			alpha = score;
			alpharaised = true;
			alphamove = m;
			//*variation = line;
		}
		//else
		//{
			//if(capturedpiece==SQUARE_EMPTY && special!=PIECE_PAWN)
				//ButterflyScores[m.getFrom()][m.getTo()]++;
		//}
	}
	if(!foundlegal)
	{
		if(futilityprune)
		{
			movegentime.Start();
			pos.generateMoves(vec);
			movegentime.Stop();
			int flag = 1;
			for(int i = 0;i<vec.size();i++)
			{
				if(pos.makeMove(m))
				{
					unmakeMove(m);
					flag = 0;
					break;
				}
			}
			if(flag)
			{
				if(pos.underCheck(pos.turn))
				{
					return CONS_MATED;
				}
				else
				{
					return CONS_DRAW;
				}
			}
		}
		else
		{
			if(pos.underCheck(pos.turn))
			{
				return CONS_MATED+ply;
			}
			else
			{
				return CONS_DRAW;
			}
		}
	}
	if(!(alphamove==CONS_NULLMOVE))
	{
		//*variation = lineptr;
		variation->push_back(alphamove);
	}
	Table.Save(pos.TTKey,depth,alpha,bound,alphamove);
	return alpha;
}

unsigned long long Engine::getMoveScore(const Move& m)
{
	int from = m.getFrom();
	int to = m.getTo();
	int capturedpiece = m.getCapturedPiece();
	int special = m.getSpecial();
	unsigned long long score = 100000;
	/*if(ply < PrincipalVariation.size())
	{
		if(m==PrincipalVariation.at(ply))
		{
			score += 600000;
		}
	}*/
	if(m==Table.getBestMove(pos.TTKey)) //history best move is always first, give it a big advantage of 400000
	{
		score += 400000;
		return score;
	}
	if(capturedpiece!=SQUARE_EMPTY) //a capture
	{
		int x = StaticExchangeEvaluation(to,from,m.getMovingPiece(),capturedpiece);
		if(x>=0) //if it is a good capture
		{
			score += 300000;
		}
		else //bad capture
		{
			score -= 100000;
		}
	}
	else if(special==PIECE_PAWN) //enpassant are also captures
	{
		int x = StaticExchangeEvaluation(to,from,m.getMovingPiece(),SQUARE_WHITEPAWN);
		if(x>=0)
		{
			score += 350000;
		}
		else
		{
			score -= 100000;
		}
	}
	else
	{
		if(((from==KillerMoves[0][ply].getFrom() && to==KillerMoves[0][ply].getTo())  //if it is a killer move
			|| (from==KillerMoves[1][ply].getFrom() && to==KillerMoves[1][ply].getTo())))
		{
			score += 200000;
		}
		score += HistoryScores[from][to]; //sort the rest by history
	}
	return score;
}

Move Engine::getHighestScoringMove(vector<Move>& moves,int currentmove)
{
	sorttime.Start();
	int bigmove = currentmove;
	unsigned long long bigscore = getMoveScore(moves.at(currentmove));
	unsigned long long x;
	for(int i = currentmove+1;i<moves.size();i++)
	{
		x = getMoveScore(moves.at(i));
		if(x>bigscore)
		{
			bigscore = x;
			bigmove = i;
		}
	}
	Move m = moves.at(bigmove);
	moves.at(bigmove) = moves.at(currentmove);
	moves.at(currentmove) = m;
	/*if(m.getSpecial()==PIECE_PAWN)
	{
		cout << "info string ep at " << currentmove << endl;
	}*/
	return m;
	sorttime.Stop();
}

void Engine::movesort(vector<Move>& moves,int depth)
{
	vector<double> score;
	for(unsigned int i = 0;i<moves.size();i++)
	{
		Move m = moves.at(i);
		score.push_back((HistoryScores[m.getFrom()][m.getTo()]*1.0));
		if(ply < PrincipalVariation.size())
		{
			if(m==PrincipalVariation.at(ply))
			{
				score.at(i) += 400000;
			}
		}
		int from = m.getFrom();
		int to = m.getTo();
		if((from==KillerMoves[0][ply].getFrom() && to==KillerMoves[0][ply].getTo()) 
			|| (from==KillerMoves[1][ply].getFrom() && to==KillerMoves[1][ply].getTo()))
		{
			score.at(i) += 200000;
		}
	}
	for(unsigned int i = 0;i<moves.size();i++)
	{
		for(unsigned int j = 0;j<moves.size()-1-i;j++)
		{
			if(score.at(j)<score.at(j+1))
			{
				int tmp = score.at(j);
				score.at(j) = score.at(j+1);
				score.at(j+1) = tmp;
				Move mov = moves.at(j);
				moves.at(j) = moves.at(j+1);
				moves.at(j+1) = mov;
			}
		}
	}
}

void Engine::checkup()
{
	timer.Stop();
	int seconds = timer.ElapsedMilliseconds();
	if(seconds >= MAXTIME)
	{
		//cout << "milliseconds: " << seconds << endl;
		longjmp(env,seconds);
	}
	//cout << (seconds-MAXTIME) << endl;
}

void Engine::setKiller(Move m,int depth)
{
	if(m!=KillerMoves[0][ply])
	{
		KillerMoves[1][ply] = KillerMoves[0][ply];
		KillerMoves[0][ply] = m;
		//cout << "Killer set: " << m.toString() << endl;
	}
}

void searchinit()
{
	for(int i = 0;i<64;i++)
	{
		for(int j = 0;j<64;j++)
		{
			ButterflyScores[i][j] = 0;
			HistoryScores[i][j] = 0;
		}
	}
}

int getNPS(int nodes,int milliseconds)
{
	if(milliseconds!=0)
	{
		return (nodes/(milliseconds/1000.0));
	}
	return 0;
}