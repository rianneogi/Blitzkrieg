#include "Engine.h"

#include <setjmp.h>

jmp_buf env;

int FutilityMargin[4] = {0,100,100,500};
int ForwardPruningMargin[4] = { 0,1000,1000,1500};
int SmallPruningMargin[8] = { 0, 120, 120, 310, 310, 400, 400, 500 };

unsigned int HistoryScores[64][64];
unsigned int ButterflyScores[64][64];

int MAXTIME = 1000;
int MAXDEPTH = 100;
int CheckupNodeCount = 16384;

Move Engine::IterativeDeepening(int movetime)
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
	prunednodes = 0;
	futilitynodes = 0;
	betacutoff_counter = 0;
	betacutoff_sum = 0;
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

	MAXTIME = movetime;

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
		cout << "info string Eval time: " << evaltime.time << ", Sort time: " << sorttime.time << ", Quisc time: " << quisctime.time << ", movegen time: " << movegentime.time << ", Timer: " << timer.ElapsedMilliseconds();
		cout << ", Nodes: " << nodes << ", Pruned nodes: " << prunednodes << ": " << (((double)prunednodes / nodes)*100) << "%, Futility nodes: " << futilitynodes << ": " << (((double)futilitynodes / nodes)*100) << "%";
		cout << ", Avg. beta cutoff location: " << ((double)betacutoff_sum / betacutoff_counter) << endl;
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
	cout << "info string Eval time: " << evaltime.ElapsedMilliseconds() << ", Sort time: " << sorttime.ElapsedMilliseconds() << ", Quisc time: " << quisctime.ElapsedMilliseconds() << ", movegen time: " << movegentime.ElapsedMilliseconds() << endl;
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
	if(!(alphamove==CONS_NULLMOVE))
	{
		variation = lineptr;
	}
	//variation->push_front(alphamove);
	return alpha;
}

int Engine::AlphaBeta(int depth,int alpha,int beta,Move lastmove,vector<Move>* variation,bool cannull,bool dopv)
{
	bool underCheck = pos.underCheck(pos.turn);
	if(underCheck) //check extension
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
	/*vector<Move> dummyline;
	dummyline.reserve(128);*/
	/*vector<Move> lineptr;
	lineptr.reserve(128);*/
	vector<Move> line;
	line.reserve(128);
	Move m;
	int score = 0;

	//adaptive null move pruning
	Bitset Pieces = pos.OccupiedSq ^ pos.Pieces[COLOR_WHITE][PIECE_PAWN] ^ pos.Pieces[COLOR_BLACK][PIECE_PAWN];
	int pieceCount = popcnt(Pieces);
    if(cannull && depth>=3 && underCheck==false && pieceCount>6) //not endgame
    {
		int R = depth > 5 ? 3 : 2; //dynamic depth-based reduction
		m = CONS_NULLMOVE;
		ply++;
		pos.forceMove(m);
        score = -AlphaBeta(depth-R-1,-beta,-beta+1,m,&line,false,false); //make a null-window search (we don't care by how much it fails high, if it does)
		ply--;
        pos.unmakeMove(m);
		if(score >= beta)
		{
			//cout << "Null move cutoff " << beta << endl;
			return beta;
		}
    }

	int leafeval = LeafEval(alpha, beta);
	if (depth < 4 && !underCheck && 
		(((leafeval + ForwardPruningMargin[depth]) <= alpha))) //large forward pruning
	{
		prunednodes++;
		return alpha;
	}

	//futility pruning
	bool futilityprune = false; 
	
	if (depth < 4 && !underCheck && 
		(((leafeval + FutilityMargin[depth]) <= alpha))) //futility pruning
	{
		futilitynodes++;
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
	//cout << "info string " << movegentime.time << endl;
	/*vector<Move> line;
	line.reserve(128);*/
	/*vector<int> scores;
	scores.reserve(128);
	generateCaptureScores(vec, scores);*/
	for(unsigned int i = 0;i<vec.size();i++) //search
	{
		line.clear();
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

		int capturedpiece = m.getCapturedPiece();
		int special = m.getSpecial();

		int reductiondepth = 1;

		if (depth < 8 && ((LeafEval(alpha, beta) + SmallPruningMargin[depth]) < alpha)) //small forward razoring
		{
			reductiondepth++;
		}

		if (!alpharaised && depth>=3 && i>=4 && capturedpiece == SQUARE_EMPTY && special == PIECE_NONE
			&& !pos.underCheck(pos.turn)
			&& (KillerMoves[0][ply].getTo() != m.getTo() || KillerMoves[0][ply].getFrom() != m.getFrom())
			&& (KillerMoves[1][ply].getTo() != m.getTo() || KillerMoves[1][ply].getFrom() != m.getFrom())) //latemove reduction
		{
			reductiondepth ++;
			if (i >= 8 && depth >= 6)
			{
				reductiondepth++;
				if (i >= 12 && depth >= 9) 
					reductiondepth++;
			}
		}
		
		if(dopv && alpharaised && depth>=3) //principal variation search
		{
			score = -AlphaBeta(max(depth - reductiondepth, 0), -alpha - 1, -alpha, m, &line, true, false);
			if(score>alpha && score < beta) //check for failure
			{
				line.clear();
				score = -AlphaBeta(depth - 1, -beta, -alpha, m, &line, true, true); //research
				//cout << "pv research" << endl;
			}
		}
		else //latemove reduction
		{
			score = -AlphaBeta(max(depth - reductiondepth,0), -beta, -alpha, m, &line, true, dopv);
			//cout << "latemove" << endl;
			if(score>alpha && score < beta && reductiondepth>1)
			{
				line.clear();
				score = -AlphaBeta(depth - 1, -beta, -alpha, m, &line, true, dopv);
				//cout << "latemove research" << endl;
			}
		}
		ply--;
		pos.unmakeMove(m);

		if(score>=beta)
		{
			Table.Save(pos.TTKey,depth,score,TT_BETA,m);
			if(capturedpiece==SQUARE_EMPTY && special!=PIECE_PAWN)
			{
				HistoryScores[m.getFrom()][m.getTo()]+=depth*depth;
				setKiller(m,depth);
			}
			betacutoff_counter++;
			betacutoff_sum += i+1;
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
			*variation = line;
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

unsigned long long Engine::getMoveScore(const Move& m)
{
	int from = m.getFrom();
	int to = m.getTo();
	int capturedpiece = m.getCapturedPiece();
	int special = m.getSpecial();
	unsigned long long score = 100000;
	if(ply < PrincipalVariation.size())
	{
		if(m==PrincipalVariation.at(ply))
		{
			score += 600000;
			return score;
		}
	}
	if(m==Table.getBestMove(pos.TTKey)) //history best move is always first, give it a big advantage of 400000
	{
		score += 400000;
		return score;
	}
	if(capturedpiece!=SQUARE_EMPTY) //a capture
	{
		int x = StaticExchangeEvaluation(to,from,m.getMovingPiece(),capturedpiece);
		//int x = movescore;
		if(x>=0) //if it is a good capture
		{
			score += 300000 + x;
		}
		else //bad capture
		{
			score += -100000 + x;
		}
	}
	else if(special==PIECE_PAWN) //enpassant are also captures
	{
		int x = StaticExchangeEvaluation(to, from, m.getMovingPiece(), capturedpiece);
		if(x>=0)
		{
			score += 350000 + x;
		}
		else
		{
			score += -100000 + x;
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

Move Engine::getHighestScoringMove(vector<Move>& moves, int currentmove)
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
	Move m = moves.at(bigmove); //swap move
	moves.at(bigmove) = moves.at(currentmove);
	moves.at(currentmove) = m;
	//int sc = scores.at(bigmove); //swamp score
	//scores.at(bigmove) = scores.at(currentmove);
	//scores.at(currentmove) = sc;
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
		if(from==KillerMoves[0][ply].getFrom() && to==KillerMoves[0][ply].getTo())
		{
			score.at(i) += 250000;
		}
		else if (from == KillerMoves[1][ply].getFrom() && to == KillerMoves[1][ply].getTo())
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