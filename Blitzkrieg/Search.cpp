#include "Engine.h"

#include <setjmp.h>

jmp_buf env;

//int FutilityMargin[4] = {0,100,100,500};
//int ForwardPruningMargin[4] = { 0,1000,1000,1500};
//int SmallPruningMargin[8] = { 0, 120, 120, 310, 310, 400, 400, 500 };

unsigned long long AllocatedTime = 1000;
int MAXDEPTH = 100;
const unsigned long long CheckupNodeCount = 2048;

inline int getRazorMargin(int depth)
{
	return (512 + 32*depth);
}

inline int getSmallRazorMargin(int depth)
{
	return (128 + 32 * depth);
}

inline int getFutilityMargin(int depth)
{
	return (150 * depth);
}

Move Engine::IterativeDeepening(int mode, uint64_t wtime, uint64_t btime, uint64_t winc, uint64_t binc, bool print)
{
	int status = pos.getGameStatus();
	if(status==STATUS_WHITEMATED || status == STATUS_BLACKMATED || status == STATUS_STALEMATE)
	{
		return CONS_NULLMOVE;
	}

	vector<Move> moves;
	pos.generateMoves(moves);
	if (moves.size() == 1) //only 1 legal move
		return moves.at(0);

	//init
	prepareSearch();

	timeMode = mode;
	AllocatedTime = 1;
	uint64_t mytime = 1;
	uint64_t opptime = 1;
	if (mode == MODE_MOVETIME)
	{
		AllocatedTime = wtime;
	}
	else
	{
		if (pos.turn == COLOR_WHITE)
		{
			mytime = wtime;
			opptime = btime;
		}
		else
		{
			mytime = btime;
			opptime = wtime;
		}
		AllocatedTime = max((uint64_t)1, mytime / 15);
	}
	if(mode==MODE_DEPTH)
	{
		MAXDEPTH = wtime;
	}
	else
	{
		MAXDEPTH = 100;
	}
	if (opptime < mytime / 4 && timeMode == MODE_DEFAULT)
	{
		AllocatedTime = max((uint64_t)1, mytime / 4); //if opponent is in time trouble lets take our sweet time
	}

#ifdef BLITZKRIEG_DEBUG
	if(print)
		cout << "Allocated Time: " << AllocatedTime << endl;
#endif

	vector<Move> line;
	line.reserve(128);
	
	int score = AlphaBeta(1,CONS_NEGINF,CONS_INF,&line,false,false);
	//int score = think(1,CONS_NEGINF,CONS_INF,&line);
	//PrincipalVariation = line;
	int val = 0;
	int lastscore = 0;
	timer.Start();
	int takeback = 0;
	int initialmovenumber = pos.movelist.size();
	Move bestmove = line.at(line.size()-1);

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
			//cout << ", Bad null: " << badavoidnull;
			cout << ", Aspiration Resets: " << aspirationresets;
			cout << ", Nullcutoffs: " << nullcutoffs;
			/*cout << ", Avg. alpha first: " << ((double)alphafirst_sum / alpha_counter);
			cout << ", Avg. alpha last: " << ((double)alphalast_sum / alpha_counter);*/
			cout << ", Fake Hits: " << ((double)(Table.hits * 100) / nodes);
			cout << ", TT hits: " << tthitcount << ": " << ((double)(tthitcount * 100) / nodes) << "%" << endl;
		}
		
		/*if (PvSize < 0)
		{
			cout << "info string ERROR: pv size is 0\n";
			return CONS_NULLMOVE;
		}*/
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

		int delta = 32;
		int alpha = max(score - delta, int(CONS_NEGINF));
		int beta = min(score + delta, int(CONS_INF));

		while (true)
		{
			//line = deque<Move>();
			ply = 0;
			//PvSize = -1;
			line.clear();
			val = AlphaBeta(i, alpha, beta, &line, true, true);
			checkup();
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
			aspirationresets++;
		}
		lastscore = score;
		score = val;
		assert(score > CONS_NEGINF && score < CONS_INF);
		timer.Stop();
		if (print)
		{
			cout << "info score cp " << score << " depth " << i << " seldepth " << SelectiveDepth << " nodes " << nodes << " nps " << getNPS(nodes, timer.ElapsedMilliseconds()) << " time " << timer.ElapsedMilliseconds() << " pv ";
			for (int j = line.size() - 1;j >= 0;j--)
			{
				cout << line.at(j).toString() << " ";
			}
			cout << endl;
			/*for (int j = 0;j < 128;j++)
			{
				if (PrincipalVariation[j].isNullMove())
					break;
				cout << PrincipalVariation[j].toString() << " ";
			}*/
		}
		PrincipalVariation = line;
		bestmove = line.at(line.size()-1);
		if (timeMode == MODE_DEFAULT)
		{
			if (score > lastscore + 100 || score < lastscore - 100) //score changed? allocate more time
			{
				AllocatedTime = min(AllocatedTime + mytime / 30, max((uint64_t)1, mytime / 4));
			}
		}
	}
	if (print)
	{
		cout << "Nodes: " << nodes << endl;
		cout << "info string Eval time: " << evaltime.ElapsedMilliseconds() << ", Sort time: " << sorttime.ElapsedMilliseconds() << ", Quisc time: " << quisctime.ElapsedMilliseconds() << ", movegen time: " << movegentime.ElapsedMilliseconds() << endl;
	}
	/*if (PvSize < 0)
	{
		cout << "info string ERROR: principal variation size is 0\n";
		return CONS_NULLMOVE;
	}*/
	//Move bestmove = PrincipalVariation[0];
	ply = 0;
	return bestmove;
}

int Engine::AlphaBeta(int depth, int alpha, int beta, vector<Move>* variation, bool cannull, bool dopv)
{
#ifdef BLITZKRIEG_DEBUG
	Bitset tablekey = pos.PawnKey;
	if (alpha > beta || alpha < CONS_NEGINF || beta > CONS_INF)
	{
		cout << "info string ERROR: alpha > beta" << alpha << " " << beta << " " << ply << endl;
	}
#endif

	if (isDraw()) return 0;

	///Quiescence
	if (depth == 0)
	{
		int value = QuiescenceSearch(alpha, beta); //go to quiescence
		/*if (value > alpha && value < beta)
			PvSize = ply - 1;*/
		//Table.Save(pos.TTKey,0,value,TT_EXACT,CONS_NULLMOVE);
		return value;
	}

	nodes++;
	if (nodes%CheckupNodeCount == 0)
	{
		checkup();
		//nodes = 0;
	}

	///Repetition
	if (ply != 0 && pos.isRepetition()) //check for repetition
	{
		//Table.Save(pos.TTKey,depth,0,TT_EXACT,CONS_NULLMOVE);
		return 0;
	}

	///Probe
	ProbeStruct probe = Table.Probe(pos.TTKey, depth, alpha, beta);
	Move ttbestmove = createNullMove(pos.epsquare);
	if (probe.found)
	{
		//cout << probe << " found " << pos.TTKey << endl;
		if (ply != 0)
		{
			tthitcount++;
			return probe.score;
		}
		/*else
		{
			ttbestmove = Table.getBestMove(pos.TTKey);
			if (!ttbestmove.isNullMove())
			{
				variation->push_back(ttbestmove);
				tthitcount++;
				return probe;
			}
		}*/
	}
	if (probe.avoidnull) cannull = false;

	ProbeStruct leafevalprobe = Table.Probe(pos.TTKey, 0, alpha, beta);
	if (leafevalprobe.found && leafevalprobe.entry->bound==TT_EXACT)
	{
		Evaluation[ply] = leafevalprobe.score; //use TT probe as a better leafeval
	}
	else
	{
		if (ply > 0 && currentVariation[ply] == CONS_NULLMOVE) //if last move was a nullmove, just invert score
		{
			Evaluation[ply] = -Evaluation[ply-1];
			assert(!cannull);
		}
		else
		{
			Evaluation[ply] = LeafEval<false>();
		}	
	}

	///Razoring
	if (!dopv && ply != 0 && depth < 4 && !incheck[ply] &&
		(((Evaluation[ply] + getRazorMargin(depth)) <= alpha)))
	{
		prunednodes++;
		if (depth <= 1 && (Evaluation[ply] + getRazorMargin(3)) <= alpha)
			return QuiescenceSearch(alpha, beta);

		int ralpha = alpha - getRazorMargin(depth);
		int v = QuiescenceSearch(ralpha, ralpha + 1);
		if (v <= ralpha)
			return v;
	}

	///Futility
	if (depth < 5 && ply != 0 && !incheck[ply] && ((Evaluation[ply] - getFutilityMargin(depth)) >= beta))
	{
		futilitynodes++;
		return (Evaluation[ply] - getFutilityMargin(depth));
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

	///Null Move
	Bitset Pieces = pos.OccupiedSq ^ pos.Pieces[COLOR_WHITE][PIECE_PAWN] ^ pos.Pieces[COLOR_BLACK][PIECE_PAWN];
	int pieceCount = popcnt(Pieces);
	if (cannull && !dopv && depth >= 3 && incheck[ply] == false
		&& (pieceCount>2) //side to move does not have only pawns(to avoid zugzwang)
		//&& Evaluation[ply] >= beta
		)
	{
		//int R = depth > 5 ? 3 : 2; //dynamic depth-based reduction
		int R = ((823 + 67 * depth) / 256 + std::min(max(0, Evaluation[ply] - beta) / PieceMaterial[PIECE_PAWN], 3));
		m = createNullMove(pos.epsquare);
		ply++;

#ifdef BLITZKRIEG_DEBUG
		Bitset ttkeynull = pos.TTKey;
#endif

		pos.forceMove(m);

		/*bool fullnullmovesearch = true;

		if (depth >= 7)
		{
			score = -QuiescenceSearchStandPat(-beta, -beta+1);
			if (score < beta)
				fullnullmovesearch = false;
		}
		if(fullnullmovesearch)*/
		score = -AlphaBeta(max(0, depth - R), -beta, -beta+1, &line, false, false); //make a null-window search (we don't care by how much it fails high, if it does)
		ply--;
		pos.unmakeMove(m);
		if (line.empty()!=true)
			Threats[ply] = line.at(line.size() - 1);

#ifdef BLITZKRIEG_DEBUG
		if (ttkeynull != pos.TTKey)
		{
			cout << "info string ERROR: Null TT fail" << endl;
			_getch();
		}
#endif

		if (score >= beta)
		{
			//cout << "Null move cutoff " << beta << endl;
			/*if (probe.avoidnull)
				badavoidnull++;*/
			nullcutoffs++;
			return score;
		}
		//if (score < alpha - 100) //score is so bad, we are in danger, so increase depth
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

	if (!(probe.found) && (dopv || Evaluation[ply] + 256 >= beta) && depth >= 2) //internal iterative deepening
	{
		int score = AlphaBeta(depth-2, alpha, beta, &line, false, dopv);
	}	

	int evaldiff = ply >= 2 ? Evaluation[ply] - Evaluation[ply - 2] : 0;
	
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

#ifdef BLITZKRIEG_DEBUG
		if (SortPhase == SORTPHASE_NONE)
			cout << "info string Sort Phase error" << endl;
#endif

		int capturedpiece = m.getCapturedPiece();
		int special = m.getSpecial();
		int movingpiece = m.getMovingPiece();
		int moveto = m.getTo();
		int movefrom = m.getFrom();

		int iscapture = isCapture(m);
		/*int see = 0;
		int evade_see = 0;
		Move smallestattckr = pos.getSmallestAttacker(getOpponent(pos.turn), movefrom);
		if (iscapture)
		{
			see = StaticExchangeEvaluation(moveto, movefrom, movingpiece, capturedpiece);
		}
		if (!smallestattckr.isNullMove())
		{
			evade_see = StaticExchangeEvaluation(movefrom, smallestattckr.getTo(), smallestattckr.getMovingPiece(), movingpiece);
		}*/

		//if (iscapture && depth <= 1 && see < 0)
		//{ //prune bad captures at low depths
		//	continue;
		//}

		//if (depth < 8
		//	&& !alpharaised
		//	&& noMaterialGain(m)
		//	//&& !incheck[ply]
		//	&& !incheck[ply - 1]
		//	&& movingpiece!=PIECE_PAWN
		//	)
		//{
		//	if (i >= 2+depth*depth)
		//	{
		//		continue;
		//	}
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
		currentVariation[ply] = m;
		
		score = 0;

		int reductiondepth = 1;

		///Check Extension
		if (pos.underCheck(pos.turn))
		{
			incheck[ply] = true;
			reductiondepth--;
		}
		else
		{
			incheck[ply] = false;
		}

		///Recapture Extension
		/*if (ply > 1 && m.getTo() == currentVariation[ply - 1].getTo() && iscapture && isCapture(currentVariation[ply - 1]))
		{
			reductiondepth--;
		}*/

		//extend when capturing the last piece
		/*if (isCapture(m) && getSquare2Piece(capturedpiece) != PIECE_PAWN
			&& popcnt(pos.Pieces[COLOR_WHITE][PIECE_PAWN]) + popcnt(pos.Pieces[COLOR_BLACK][PIECE_PAWN]) + 2 == popcnt(pos.OccupiedSq))
		{
			reductiondepth -= 3;
		}*/

		/*if (depth < 16)
		{
			if (!incheck[ply - 1]
				&& !incheck[ply]
				&& movingpiece != PIECE_PAWN
				&& noMaterialGain(m))
			{
				if(i > 3 + (1 << (depth-1)))
				{
					pos.unmakeMove(m);
					incheck[ply] = false;
					ply--;
					continue;
				}
			}
		}*/

		//if (!dopv && depth < 4 && ((Evaluation[ply] + getSmallRazorMargin(depth)) <= alpha)) //small forward razoring
		//{
		//	reductiondepth++;
		//}

		///Latemove Reduction
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
			//&& (movingpiece!=PIECE_PAWN || getRank(getColorMirror(getOpponent(pos.turn), moveto))<6) //dont reduce pawn moves past 6th rank
			//&& m!=Threats[ply]
			)
		{
			if (evaldiff > 0)
				reductiondepth += min(depth - 4, min((int)i,4));
			else
				reductiondepth += min(depth - 4, min((int)i+1,5));
			assert((depth - reductiondepth) >= 3);
			if (!dopv && HistoryScores[movingpiece][moveto] < 0) //history reduction
			{
				reductiondepth++;
			}
			if (m == Threats[ply] && reductiondepth > 0) //decrease reduction if move is a threat
			{
				reductiondepth = max(reductiondepth - 1, 0);
			}
			//if (noMaterialGain(m) && !smallestattckr.isNullMove() && evade_see < 0) //decrease reduction if move evades a capture
			//{
			//	reductiondepth = max(reductiondepth - 1, 0);
			//}
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
		
		///Search
		if(dopv && i>0 && depth>=3) //principal variation search
		{
			score = -AlphaBeta(max(depth - reductiondepth, 0), -alpha - 1, -alpha, &line, true, false);
			if(score > alpha && score < beta) //check for failure
			{
				line.clear();
				score = -AlphaBeta(depth - 1, -beta, -alpha, &line, true, true); //research
				alpharaised = false;
				pvresearch++;
				//cout << "pv research" << endl;
			}
		}
		else
		{
			score = -AlphaBeta(max(depth - reductiondepth,0), -beta, -alpha, &line, true, dopv);
			//cout << "latemove" << endl;
			if(score > alpha && score < beta && reductiondepth>1)
			{
				line.clear();
				score = -AlphaBeta(depth - 1, -beta, -alpha, &line, true, dopv);
				latemoveresearch++;
			}
		}
		currentVariation[ply] = CONS_NULLMOVE;
		incheck[ply] = false;
		ply--;
		pos.unmakeMove(m);

		assert(score > CONS_NEGINF && score < CONS_INF);
		assert(score >= CONS_MATED && score <= -CONS_MATED);

		if(score>=beta)
		{
			if(noMaterialGain(m))
			{
				//if(Table.getBestMove(pos.TTKey)!=m) //dont store hash move as a killer
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
					/*if(HistoryScores[quietmoves.at(i).getMovingPiece()][quietmoves.at(i).getTo()] <= bonus)
						HistoryScores[quietmoves.at(i).getMovingPiece()][quietmoves.at(i).getTo()] = 0;
					else*/
						HistoryScores[quietmoves.at(i).getMovingPiece()][quietmoves.at(i).getTo()] -= bonus;
				}
			}
			Table.Save(pos.TTKey, depth, score, TT_BETA, m);

#ifdef BLITZKRIEG_STATS
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

#ifdef BLITZKRIEG_STATS
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
		//if(dopv)
		//	PrincipalVariation[ply] = alphamove;
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
#ifdef BLITZKRIEG_STATS
		alpha_counter++;
		alphalast_sum += (finalalpha + 1);
		alphafirst_sum += (firstalpha + 1);
#endif
	}
	Table.Save(pos.TTKey,depth,bestscore,bound,alphamove);
	/*if (ply == 0)
	{
		cout << "info string Stored: " << alphamove.toString() << " " << bound << " " << depth << endl;
	}*/

#ifdef BLITZKRIEG_DEBUG
	if (pos.PawnKey != tablekey)
	{
		cout << "info string ERROR: Pawn TT key doesnt match" << endl;
	}
#endif
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
	aspirationresets = 0;
	nullcutoffs = 0;
	//badavoidnull = 0;

	ply = 0;
	SelectiveDepth = 0;
	PrincipalVariation = vector<Move>();
	PrincipalVariation.reserve(128);
	for (int i = 0;i<2;i++)
	{
		for (int j = 0;j<100;j++)
		{
			KillerMoves[i][j] = CONS_NULLMOVE;
			/*if(i!=2)
			KillerScores[i][j] = CONS_NEGINF;*/
		}
	}
	for (int i = 0;i < 6;i++)
	{
		for (int j = 0;j < 64;j++)
		{
			for (int k = 0;k < 2;k++)
			{
				CounterMoves[i][j][k] = CONS_NULLMOVE;
				FollowupMoves[i][j][k] = CONS_NULLMOVE;
			}
		}
	}
	for (int i = 0;i<100;i++)
	{
		Threats[i] = CONS_NULLMOVE;
		incheck[i] = false;
		Evaluation[i] = 0;
		currentVariation[i] = CONS_NULLMOVE;
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
	if(timer.ElapsedMilliseconds() >= AllocatedTime && AllocatedTime!=-1)
	{
		//cout << "milliseconds: " << timer.time << endl;
		longjmp(env, timer.time);
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