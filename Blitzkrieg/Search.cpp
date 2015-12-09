#include "Engine.h"

#include <setjmp.h>

jmp_buf env;

//int FutilityMargin[4] = {0,100,100,500};
//int ForwardPruningMargin[4] = { 0,1000,1000,1500};
//int SmallPruningMargin[8] = { 0, 120, 120, 310, 310, 400, 400, 500 };

int MAXTIME = 1000;
int MAXDEPTH = 100;
const int CheckupNodeCount = 16384;

inline int getRazorMargin(int depth)
{
	return (512 + 32*depth);
}

inline int getFutilityMargin(int depth)
{
	return (200 * depth);
}

Move Engine::IterativeDeepening(int movetime, bool print)
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

	vector<Move> moves;
	pos.generateMoves(moves);
	if (moves.size() == 1) //only 1 legal move
		return moves.at(0);

	//init
	prepareSearch();

	MAXTIME = movetime;

	vector<Move> line;
	line.reserve(128);
	PrincipalVariation = vector<Move>();
	PrincipalVariation.reserve(128);
	int score = AlphaBeta(1,CONS_NEGINF,CONS_INF,&line,false,false);
	//int score = think(1,CONS_NEGINF,CONS_INF,&line);
	//PrincipalVariation = line;
	int val = 0;
	timer.Start();
	int takeback = 0;
	int initialmovenumber = pos.movelist.size();
	Move bestmove = CONS_NULLMOVE;
	/*for (int i = 0;i < 128;i++)
	{
		PrincipalVariation[i] = CONS_NULLMOVE;
	}*/
	//cout << "takeback set " << takeback << " " << PrincipalVariation.size() << endl;
	takeback = setjmp(env);
	if(takeback!=0)
	{
		while(pos.movelist.size()>initialmovenumber)
		{
			pos.takebackMove();
			takeback--;
		}

		if (print)
		{
			cout << "info string ";
			//cout << "Eval time: " << evaltime.time << ", Sort time: " << sorttime.time << ", Quisc time: " << quisctime.time << ", movegen time: " << movegentime.time << ", Timer: " << timer.ElapsedMilliseconds();
			cout << "Nodes: " << nodes << ", Pruned nodes: " << prunednodes << ": " << ((double)(prunednodes * 100) / nodes) << "%, Futility nodes: " << futilitynodes << ": " << ((double)(futilitynodes * 100) / nodes) << "%";
			cout << ", Avg. beta: " << ((double)betacutoff_sum / betacutoff_counter);
			cout << ", First beta: " << ((double)(firstbetacutoffcount*100) / betacutoff_counter) << "%";
			cout << ", Latemove researches: " << latemoveresearch;
			cout << ", PV researches: " << pvresearch;
			/*cout << ", Avg. alpha first: " << ((double)alphafirst_sum / alpha_counter);
			cout << ", Avg. alpha last: " << ((double)alphalast_sum / alpha_counter);*/
			cout << ", TT hits: " << tthitcount << ": " << ((double)(tthitcount * 100) / nodes) << "%" << endl;
		}
		
		if (PvSize < 0)
		{
			cout << "info string ERROR: pv size is 0\n";
			return CONS_NULLMOVE;
		}
		ply = 0;
		return bestmove;
	}
	incheck[0] = pos.underCheck(pos.turn);
	for (int i = 2;i <= MAXDEPTH;i++)
	{
		//mr = think(i,CONS_NEGINF,CONS_INF);
		//mr = think(i,score - 100,score + 100); //aspiration search
		//if(mr.eval <= score-150 || mr.eval >= score+150)
		//{
		//	cout << "Aspiration fail " << mr.eval << endl;
		//	mr = think(i,CONS_NEGINF,CONS_INF);
		//}
		//int low = 8;
		//int high = 8;

		/*PvSize = -1;
		PvPly = -1;*/

		int delta = 16;
		int alpha = max(score - delta, int(CONS_NEGINF));
		int beta = min(score + delta, int(CONS_INF));

		while (true)
		{
			//line = deque<Move>();
			ply = 0;
			//PvSize = -1;
			val = AlphaBeta(i, alpha, beta, &line, true, true);
			//cout << "asp. " << alpha << " " << beta << endl;
			/*for (int i = 0;i < 2;i++)
			{
				for (int j = 0;j < 100;j++)
				{
					KillerScores[i][j] = CONS_NEGINF;
				}
			}*/
			if (val <= alpha)
			{
				beta = (alpha + beta) / 2;
				alpha = max(score - delta, int(CONS_NEGINF));
				//cout << "val is " << val << endl;
				//low = low << 1;
			}
			else if (val >= beta)
			{
				alpha = (alpha + beta) / 2;
				beta = min(score + delta, int(CONS_INF));
				//cout << "val is " << val << endl;
				//high = high << 1;
			}
			else break;

			delta += delta / 2;
		}
		score = val;
		//firstguess = val;
		timer.Stop();
		if (print)
		{
			cout << "info score cp " << score << " depth " << i << " seldepth " << SelectiveDepth << " nodes " << nodes << " nps " << getNPS(nodes, timer.ElapsedMilliseconds()) << " time " << timer.ElapsedMilliseconds() << " pv ";
			/*for(int j = 0;j<=PvSize;j++)
			{
				cout << PrincipalVariation[j].toString() << " ";
			}*/
			for (int j = line.size() - 1;j >= 0;j--)
			{
				cout << line.at(j).toString() << " ";
			}
			cout << endl;
		}
		PrincipalVariation = line;
		bestmove = PrincipalVariation.at(PrincipalVariation.size()-1);
	}
	if (print)
	{
		cout << "Nodes: " << nodes << endl;
		cout << "info string Eval time: " << evaltime.ElapsedMilliseconds() << ", Sort time: " << sorttime.ElapsedMilliseconds() << ", Quisc time: " << quisctime.ElapsedMilliseconds() << ", movegen time: " << movegentime.ElapsedMilliseconds() << endl;
	}
	//cout << bestmove.toString() << " " << firstguess << endl;
	if (PvSize < 0)
	{
		cout << "info string ERROR: principal variation size is 0\n";
		return CONS_NULLMOVE;
	}
	//Move bestmove = PrincipalVariation[0];
	ply = 0;
	return bestmove;
}

int Engine::AlphaBeta(int depth, int alpha, int beta, vector<Move>* variation, bool cannull, bool dopv)
{
	int tablekey = pos.PawnKey;
	if (alpha > beta || alpha < CONS_NEGINF || beta > CONS_INF)
	{
		cout << "info string ERROR: alpha > beta" << alpha << " " << beta << " " << ply << endl;
	}

	//bool underCheck = pos.underCheck(pos.turn);
	//if (incheck[ply]) //check extension
	//{
	//	depth++;
	//}

	if (depth == 0)
	{
		int ttqs = pos.TTKey;
		int value = QuiescenceSearchStandPat(alpha, beta); //go to quiescence
		if (ttqs != pos.TTKey)
		{
			cout << "info string ERROR: TT key quiescence fail" << endl;
		}
		if (value > alpha && value < beta)
			PvSize = ply - 1;
		//Table.Save(pos.TTKey,0,value,TT_EXACT,CONS_NULLMOVE);
		return value;
	}

	nodes++;
	if (nodes%CheckupNodeCount == 0)
	{
		checkup();
		//nodes = 0;
	}

	if (ply != 0 && pos.isRepetition()) //check for repetition
	{
		//Table.Save(pos.TTKey,depth,0,TT_EXACT,CONS_NULLMOVE);
		return 0;
	}

	int probe = Table.Probe(pos.TTKey, depth, alpha, beta);
	Move ttbestmove = createNullMove(pos.epsquare);
	if (probe != CONS_TTUNKNOWN)
	{
		//cout << probe << " found " << pos.TTKey << endl;
		if (ply != 0)
		{
			ttbestmove = Table.getBestMove(pos.TTKey);
			if (!ttbestmove.isNullMove())
			{
				variation->push_back(ttbestmove);
				//PrincipalVariation[ply] = ttbestmove;
				//PvSize = ply;
				//PvPly = ply;
				/*vector<Move> vec;
				vec.reserve(128);
				pos.generateMoves(vec);
				int flag = 0;
				for (int i = 0;i < vec.size();i++)
				{
					if (vec.at(i) == ttbestmove)
					{
						flag = 1;
						break;
					}
				}
				if (flag == 0)
				{
					cout << "info string ERROR: ILL EAGLE " << ttbestmove.toString() << endl;
				}*/
				tthitcount++;
				return probe;
			}
			else if (ply != 0)
			{
				//PvSize = ply - 1;
				//PvPly = ply;
				tthitcount++;
				return probe;
			}
		}
		/*else if(ply!=0)
		{
			return probe;
		}*/
	}

	int leafeval = 0;
	int leafevalprobe = Table.Probe(pos.TTKey, 0, alpha, beta);
	if (leafevalprobe != CONS_TTUNKNOWN)
	{
		leafeval = probe; //use TT probe as a better leafeval
	}
	else
	{
		leafeval = LeafEval<false>();
	}

	if (!dopv && ply != 0 && depth < 4 && !incheck[ply] &&
		(((leafeval + getRazorMargin(depth)) <= alpha))) //razoring
	{
		prunednodes++;
		if (depth <= 1 && (leafeval + getRazorMargin(3)) <= alpha)
			return QuiescenceSearchStandPat(alpha, beta);

		int ralpha = alpha - getRazorMargin(depth);
		int v = QuiescenceSearchStandPat(ralpha, ralpha + 1);
		if (v <= ralpha)
			return v;
	}

	if (depth < 5 && ply != 0 && !incheck[ply] && ((leafeval - getFutilityMargin(depth)) >= beta)) //futility pruning
	{
		prunednodes++;
		return (leafeval - getFutilityMargin(depth));
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
	if (cannull && !dopv && depth >= 3 && incheck[ply] == false
		&& (popcnt(pos.Pieces[pos.turn][PIECE_QUEEN]) || popcnt(pos.Pieces[pos.turn][PIECE_ROOK])
			|| popcnt(pos.Pieces[pos.turn][PIECE_BISHOP] || popcnt(pos.Pieces[pos.turn][PIECE_KNIGHT]))
			) //side to move does not have only pawns(to avoid zugzwang)
		//&& leafeval >= beta
		)
	{
		//int R = depth > 5 ? 3 : 2; //dynamic depth-based reduction
		int R = ((823 + 67 * depth) / 256 + std::min(max(0, leafeval - beta) / PieceMaterial[PIECE_PAWN], 3));
		m = createNullMove(pos.epsquare);
		ply++;
		int ttkeynull = pos.TTKey;
		pos.forceMove(m);

		/*bool fullnullmovesearch = true;

		if (depth >= 7)
		{
			score = -QuiescenceSearchStandPat(-beta, -beta+1);
			if (score < beta)
				fullnullmovesearch = false;
		}
		if(fullnullmovesearch)*/
		score = -AlphaBeta(max(0, depth - R), -beta, -beta + 1, &line, false, false); //make a null-window search (we don't care by how much it fails high, if it does)
		ply--;
		pos.unmakeMove(m);
		if (line.empty()!=true)
			Threats[ply] = line.at(line.size() - 1);
		if (ttkeynull != pos.TTKey)
		{
			cout << "info string ERROR: Null TT fail" << endl;
			_getch();
		}
		if (score >= beta)
		{
			//cout << "Null move cutoff " << beta << endl;
			return score;
		}
		//if (score < CONS_MATED + 1000) //score is so bad, we are in danger, so increase depth
		//{
		//	depth++;
		//}
	}

	//futility pruning
	//bool futilityprune = false;

	//if (depth < 4 && !underCheck && 
	//	(((leafeval + FutilityMargin[depth]) <= alpha))) //futility pruning
	//{
	//	futilitynodes++;
	//	futilityprune = true;
	//}

	//movesort(vec,depth);
	bool alpharaised = false;
	bool foundlegal = false;
	Move alphamove = CONS_NULLMOVE;
	int finalalpha = -1;
	int firstalpha = -1;
	//vec = pos.generateMoves();
	vector<Move> vec;
	vec.reserve(128);
	//movegentime.Start();
	//if (futilityprune)
	//{
	//	pos.generateCaptures(vec); //search only captures in futility pruning
	//}
	//else
	//{
	pos.generateMoves(vec);
	//}
	//movegentime.Stop();
	/*vector<Move> line;
	line.reserve(128);*/
	/*vector<int> scores;
	scores.reserve(128);
	generateCaptureScores(vec, scores);*/

	//if (probe == CONS_TTUNKNOWN && dopv && depth>=2) //internal iterative deepening
	//{
	//	int score = AlphaBeta(depth-2, alpha, beta, &line, false, dopv);
	//}	

	vector<Move> quietmoves;
	quietmoves.reserve(128);
	int bestscore = CONS_NEGINF;
	//int oldsortphase = SORTPHASE_NONE;
	for (unsigned int i = 0;i < vec.size();i++) //search
	{
		line.clear();
		//dummyline.clear();
		//m = vec.at(i);
		//int tablekey2 = pos.TTKey;
		m = getHighestScoringMove(vec, i);

		if (SortPhase == SORTPHASE_NONE)
			cout << "info string Sort Phase error" << endl;

		int capturedpiece = m.getCapturedPiece();
		int special = m.getSpecial();
		int movingpiece = m.getMovingPiece();
		int moveto = m.getTo();
		int movefrom = m.getFrom();

		int iscapture = isCapture(m);
		int see = 0;
		if (iscapture)
		{
			see = StaticExchangeEvaluation(moveto, movefrom, movingpiece, capturedpiece);
		}

		//if (iscapture && depth <= 1 && see < 0)
		//{ //prune bad captures at low depths
		//	continue;
		//}

		if (!pos.makeMove(m))
		{
			continue;
		}
		//if (vec.size() == 1 || (foundlegal==false && i==vec.size()-1)) //singular extension, only 1 legal move, so extend
		//{
		//	depth++;
		//}
		foundlegal = true;
		ply++;
		score = 0;

		int reductiondepth = 1;

		if (pos.underCheck(pos.turn))
		{
			incheck[ply] = true;
			reductiondepth--;
		}
		else
		{
			incheck[ply] = false;
		}

		

		//if (depth < 8 && ((LeafEval<false>(alpha, beta) + SmallPruningMargin[depth]) <= alpha)) //small forward razoring
		//{
		//	reductiondepth++;
		//}

		if (!alpharaised
			//&& i >= 4
			&& depth >= 4
			//&& special!=PIECE_QUEEN
			//&& (see < 0 || !iscapture)
			&& SortPhase >= SORTPHASE_HISTORY
			//&& noMaterialGain(m)
			//&& (KillerMoves[0][ply].getTo() != moveto || KillerMoves[0][ply].getFrom() != movefrom)
			//&& (KillerMoves[1][ply].getTo() != moveto || KillerMoves[1][ply].getFrom() != movefrom)
			&& !incheck[ply]
			&& !incheck[ply-1]
			) //latemove reduction
		{
			//if (movingpiece != PIECE_PAWN || getRank(getColorMirror(pos.turn,moveto))<6) //dont reduce pawn moves past 6th rank
				//reductiondepth += depth > 4 ? 2 : 1;
			reductiondepth += depth > 4 ? 2 : 1;
			/*if (HistoryScores[movingpiece][moveto] < 0) //history reduction
			{
				reductiondepth++;
			}*/
			//if (reductiondepth >= depth-3) reductiondepth = max(1,depth - 3);
		}

		//if (isCapture(m) && !dopv && see > 400 && depth>=5) //prune really good captures
		//{
		//	reductiondepth += 4;
		//}

		//if (alpha_counter != 0 && (depth-reductiondepth)>=3 && i>((double)alphalast_sum/alpha_counter) && capturedpiece == SQUARE_EMPTY && special == PIECE_NONE
		//	&& !pos.underCheck(pos.turn)
		//	&& (KillerMoves[0][ply].getTo() != m.getTo() || KillerMoves[0][ply].getFrom() != m.getFrom())
		//	&& (KillerMoves[1][ply].getTo() != m.getTo() || KillerMoves[1][ply].getFrom() != m.getFrom())) //latemove reduction
		//{
		//	reductiondepth += 2;
		//	/*reductiondepth ++;
		//	if (i >= 8 && depth >= 6)
		//	{
		//		reductiondepth++;
		//		if (i >= 12 && depth >= 9) 
		//			reductiondepth++;
		//	}*/
		//}
		
		if(dopv && alpharaised && depth>=3) //principal variation search
		{
			score = -AlphaBeta(max(depth - reductiondepth, 0), -alpha - 1, -alpha, &line, true, false);
			if(score>alpha && score < beta) //check for failure
			{
				line.clear();
				score = -AlphaBeta(depth - 1, -beta, -alpha, &line, true, true); //research
				pvresearch++;
				//cout << "pv research" << endl;
			}
		}
		else //latemove reduction
		{
			score = -AlphaBeta(max(depth - reductiondepth,0), -beta, -alpha, &line, true, dopv);
			//cout << "latemove" << endl;
			if(score>alpha && score < beta && reductiondepth>1)
			{
				line.clear();
				score = -AlphaBeta(depth - 1, -beta, -alpha, &line, true, dopv);
				latemoveresearch++;
			}
		}
		incheck[ply] = false;
		ply--;
		pos.unmakeMove(m);
		//if (tablekey2 != pos.TTKey)
		//{
		//	cout << "info string TT Error at " << m.toString() << " " << ply << " " << PvSize << endl;
		//	cout << "info string ";
		//	for (int i = 0;i < PvSize;i++)
		//	{
		//		cout << PrincipalVariation[i].toString() << " ";
		//	}
		//	cout << endl;
		//	//_getch();
		//}

		if(score>=beta)
		{
			if(noMaterialGain(m))
			{
				//if(ttbestmove!=m) //dont store hash move as a killer
				setKiller(m, depth, score);

				int bonus = depth*depth;
				HistoryScores[movingpiece][m.getTo()] += bonus;
				if (HistoryScores[movingpiece][m.getTo()] > 200000) //prevent overflow of history values
				{
					for (int i = 0;i < 6;i++)
					{
						for (int j = 0;j < 64;j++)
						{
							HistoryScores[i][j] /= 2;
						}
					}
				}
				for (int i = 0;i < quietmoves.size();i++)
				{
					if(HistoryScores[quietmoves.at(i).getMovingPiece()][quietmoves.at(i).getTo()] <= bonus)
						HistoryScores[quietmoves.at(i).getMovingPiece()][quietmoves.at(i).getTo()] = 0;
					else
						HistoryScores[quietmoves.at(i).getMovingPiece()][quietmoves.at(i).getTo()] -= bonus;
				}
			}
			Table.Save(pos.TTKey, depth, score, TT_BETA, m);

#ifdef STATS
			betacutoff_counter++;
			betacutoff_sum += i+1;
			if (i == 0)
				firstbetacutoffcount++;
#endif
			return score; //fail soft beta cutoff
		}
		else if(score>bestscore)
		{
			bestscore = score;
			if (score > alpha)
			{
				bound = TT_EXACT;
				alpha = score;
				alpharaised = true;
				alphamove = m;
				*variation = line;

				if (noMaterialGain(m))
					HistoryScores[movingpiece][m.getTo()] += depth;

#ifdef STATS
				if (firstalpha == -1)
				{
					firstalpha = i;
				}
				finalalpha = i;
#endif

			}
		}

		if (noMaterialGain(m))
		{
			quietmoves.push_back(m);
		}
	}
	if(!foundlegal)
	{
		//if(futilityprune)
		//{
		//	//movegentime.Start();
		//	vec.clear();
		//	pos.generateMoves(vec);
		//	//movegentime.Stop();
		//	int flag = 1;
		//	for(int i = 0;i<vec.size();i++)
		//	{
		//		if(pos.makeMove(m))
		//		{
		//			unmakeMove(m);
		//			flag = 0;
		//			break;
		//		}
		//	}
		//	if(flag)
		//	{
		//		if(pos.underCheck(pos.turn))
		//		{
		//			return CONS_MATED-ply;
		//		}
		//		else
		//		{
		//			return CONS_DRAW;
		//		}
		//	}
		//}
		//else
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
	if(!alphamove.isNullMove())
	{
		variation->push_back(alphamove);
		//PrincipalVariation[ply] = alphamove;
		/*if (depth == 1)
		{
			PvSize = ply;
			PvPly = ply;
		}*/
		/*else if (ply == PvPly - 1)
		{
			PrincipalVariation[ply] = alphamove;
			PvPly = ply;
		}*/
		//HistoryScores[alphamove.getFrom()][alphamove.getTo()] += depth+finalalpha;
#ifdef STATS
		alpha_counter++;
		alphalast_sum += (finalalpha + 1);
		alphafirst_sum += (firstalpha + 1);
#endif
		
		//pos.makeMove(alphamove);
		//int le = LeafEval<false>()-PieceSq[getPiece2Square(alphamove.getMovingPiece(), pos.turn)][alphamove.getTo()];
		//pos.unmakeMove(alphamove);

		//if (alpha > le+10)
		//{
		//	if (pos.turn == COLOR_WHITE)
		//	{
		//		PieceSq[getPiece2Square(alphamove.getMovingPiece(), pos.turn)][alphamove.getTo()]++;
		//		PieceSq[getPiece2Square(alphamove.getMovingPiece(), pos.turn)][alphamove.getFrom()]--;
		//	}
		//	else
		//	{
		//		PieceSq[getPiece2Square(alphamove.getMovingPiece(), pos.turn)][alphamove.getTo()]--;
		//		PieceSq[getPiece2Square(alphamove.getMovingPiece(), pos.turn)][alphamove.getFrom()]++;
		//	}
		//	//HistoryScores[alphamove.getFrom()][alphamove.getTo()]++;
		//	
		//}
		//if (alpha < le - 10)
		//{
		//	if (pos.turn == COLOR_WHITE)
		//	{
		//		PieceSq[getPiece2Square(alphamove.getMovingPiece(), pos.turn)][alphamove.getTo()]--;
		//		PieceSq[getPiece2Square(alphamove.getMovingPiece(), pos.turn)][alphamove.getFrom()]++;
		//	}
		//	else
		//	{
		//		PieceSq[getPiece2Square(alphamove.getMovingPiece(), pos.turn)][alphamove.getTo()]++;
		//		PieceSq[getPiece2Square(alphamove.getMovingPiece(), pos.turn)][alphamove.getFrom()]--;
		//	}
		//	/*if(HistoryScores[alphamove.getFrom()][alphamove.getTo()]!=0)
		//		HistoryScores[alphamove.getFrom()][alphamove.getTo()]--;*/
		//}
	}
	Table.Save(pos.TTKey,depth,alpha,bound,alphamove);
	/*if (Table.Probe(pos.TTKey, depth, alpha, beta) != alpha)
	{
		cout << "TT NO MATCH" << endl;
	}*/
	if (pos.PawnKey != tablekey)
	{
		cout << "info string ERROR: Pawn TT key doesnt match" << endl;
	}
	return bestscore;
}

//this function initializes all values before search starts
void Engine::prepareSearch()
{
	nodes = 0;
	prunednodes = 0;
	futilitynodes = 0;

	betacutoff_counter = 0;
	betacutoff_sum = 0;
	firstbetacutoffcount = 0;
	tthitcount = 0;
	alpha_counter = 0;
	alphalast_sum = 0;
	alphafirst_sum = 0;
	latemoveresearch = 0;
	pvresearch = 0;

	ply = 0;
	SelectiveDepth = 0;
	for (int i = 0;i<2;i++)
	{
		for (int j = 0;j<100;j++)
		{
			KillerMoves[i][j] = CONS_NULLMOVE;
			/*if(i!=2)
			KillerScores[i][j] = CONS_NEGINF;*/
		}
	}
	for (int j = 0;j<100;j++)
	{
		Threats[j] = CONS_NULLMOVE;
		incheck[j] = false;
	}
	for (unsigned int i = 0;i<64;i++) //ages the history table
	{
		for (unsigned int j = 0;j<64;j++)
		{
			HistoryScores[i][j] /= 2;
		}
	}
	evaltime.Reset();
	sorttime.Reset();
	movegentime.Reset();
	quisctime.Reset();
	timer.Reset();
}

void Engine::checkup()
{
	timer.Stop();
	int seconds = timer.ElapsedMilliseconds();
	if(seconds >= MAXTIME && MAXTIME!=-1)
	{
		//cout << "milliseconds: " << seconds << endl;
		longjmp(env,seconds);
	}
	//cout << (seconds-MAXTIME) << endl;
}

unsigned long long Engine::perft(int depth)
{
	if (depth == 0) return 1;
	vector<Move> vec;
	vec.reserve(128);
	pos.generateMoves(vec);
	unsigned long long count = 0;
	for (int i = 0;i < vec.size();i++)
	{
		Move m = vec.at(i);
		if (pos.makeMove(m))
		{
			//cout << "counting " << m.toString() << " at depth " << depth << endl;
			//pos.display(0);
			//_getch();
			count += perft(depth - 1);
			pos.unmakeMove(m);
		}
	}
	return count;
}

void searchinit()
{
	/*for(int i = 0;i<64;i++)
	{
		for(int j = 0;j<64;j++)
		{
			HistoryScores[i][j] = 0;
		}
	}*/
}

int getNPS(int nodes,int milliseconds)
{
	if(milliseconds!=0)
	{
		return (nodes/(milliseconds/1000.0));
	}
	return 0;
}