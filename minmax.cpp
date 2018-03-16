#include <cstdio>
#include <cstdlib>
#include <vector>
#include <stack>
#include <functional>
#include <algorithm>

enum Case {Empty, X, O, Draw};

struct Coord
{
  int big;
  int little;
};

struct MinmaxRep
{
  Coord coup;
  int score;
  std::stack<Coord> suivants;
};

MinmaxRep mini(MinmaxRep a, MinmaxRep b, int p) {
  //Avec proba 1/p, si les scores sont égaux, b est renvoyé
  if (a.score != b.score)
    {
      return (a.score < b.score) ? a : b;
    }
  else
    {
      return (std::rand() % p ? a : b);
    }
}

MinmaxRep maxi(MinmaxRep a, MinmaxRep b, int p) {
  //Avec proba 1/p, si les scores sont égaux, b est renvoyé
   if (a.score != b.score)
    {
      return (a.score > b.score) ? a : b;
    }
  else
    {
      return (std::rand() % p ? a : b);
    }
}

MinmaxRep emptyplay (int score) {
  return MinmaxRep {Coord{9,9}, score, std::stack<Coord>()};
}

struct Grid
{
  Case grid[9][9];
  Case metagrid[9];
  int playgrid;

  // CONSTRUCTEUR
    
  Grid()
  {
    playgrid = 9;
    for (int i = 0 ; i < 9 ; i++)
      {
	for (int j = 0; j < 9; j++)
	  {
	    grid[i][j] = Empty;
	  }
	metagrid[i] = Empty;
      }
  }

  //AFFICHAGE
    
  void print()
  {
    printf("+---+---+---+\n");
    for (int il = 0 ; il < 3 ; il++)
      {
	for (int jl = 0 ; jl < 3; jl++)
	  {
	    printf("|");
	    for (int ic = 0 ; ic < 3; ic++)
	      {
		for (int jc = 0 ; jc < 3; jc++)
		  {
		    switch (grid[il*3+ic][jl*3+jc])
		      {
		      case Empty:
			printf(" ");
			break;
		      case X:
			printf("X");
			break;
		      case O:
			printf("O");
			break;
		      }		    
		  }
		printf("|");
	      }
	    if (jl<2) printf("\n");
	  }
	printf("\n+---+---+---+\n");
      }
  }

  //FONCTIONS DE BASE DU JEU

  bool isFull(Case* g)
  {
    bool res = true;
    for (int j = 0 ; j < 9 ; j++) {
      if (g[j] == Empty) res = false;
    }
    return res;
  }
    
  void undo_move(int old_playgrid, Coord play)
  {
    playgrid = old_playgrid;
    grid[play.big][play.little] = Empty;
    //TODO : update les conditions de victoire
  }

  void do_move(Coord play, bool player){
    Case p = player ? X : O
    grid[play.big][play.little] = p;
    if (metagrid[play.big] == Empty)
      {
	switch (won_grid (grid[play.big])) {
	case (player ? 1 : -1) :
	  metagrid[play.big] = p;
	  break;
	case 0:
	  metagrid[play.big] = Draw;
	}
      }
    playgrid = play.little;
  }

  //FONCTIONS AVANCÉES DU JEU

  std::vector<Coord> playableMoves()
  {
    std::vector<Coord> res;
    if (playgrid == 9 or isFull(grid[playgrid]) ){
      for (int i = 0 ; i < 9 ; i++){
	for (int j = 0 ; j < 9 ; j++)
	  {
	    if(grid[i][j] == Empty)
	      res.push_back(Coord{i,j});
	  }
      }
    }
    else {
      for (int j = 0 ; j < 9 ; j++)
	{
	  if(grid[playgrid][j] == Empty)
	    res.push_back(Coord{playgrid,j});
	}
    }
    return res;
  }

  int won_grid (Case* g) {
    //-1 = perdu, 0 = draw, 1 = gagné, 42 = en cours
    if (player_won(g, X)) return 1;
    if (player_won(g, O)) return -1;
    if (is_complete(g))   return 0;
    return 42;
  }

  bool is_complete(Case* g) {
    bool res = false;
    for (int j=0 ; j <9 ; j++)
      {
	if (g[j] == Empty) res = false;
      }
    return res;
  }

  bool player_won (Case* g, Case p){
    bool res = false;
    //Test des lignes
    for (int i = 0; i<3; i++) {
      res = res or (g[3*i] and g[3*i+1] and g[3*i+1]);
    }
    //Test des colonnes
    for (int i = 0; i<3; i++){
      res = res or (g[i] and g[i+3] and g[i+6]);
    }
    //Test des diagonales
    res = res or (g[0] and g[4] and g[8]) or (g[2] and g[4] and g[6]);

    return res;
  }

  //ALGORITHME PSEUDO-EXHAUSTIF
  MinmaxRep pseudo_complete_search(bool player, int range, std::function<int()> eval)
  {
    return emptyplay(0);
  }


  //ALGORITHME MINMAX DETERMINISTE
  //Envoyer 9 comme secteur au départ, range est la valeur absolue du score max = victoire/défaite assurée

  //TODO : renvoyer aussi les coups joués
  
  MinmaxRep min_max(int depth, bool player, int alpha, int beta, int range, std::function<int()> eval)
  {
    if (depth == 0) return emptyplay(eval());

    int end_game = won_grid(metagrid);
    if (end_game != 42) return emptyplay(end_game*range);
        
    std::vector<Coord> playable = playableMoves();
    MinmaxRep score = emptyplay(player ? alpha : beta);
    int nb_eq_scores = 1; //on veut un coup random parmi ceux du même score
    
    for(Coord play : playable)
      {
	int old_playgrid = playgrid;
	do_move(play,player);
	
	MinmaxRep score_under;
	if (player) {
	  score_under = min_max (depth -1, !player, score.score, beta, range, eval);
	  score_under.suivants.push (score_under.coup);
	  score_under.coup = play;
	  if (score_under.score == score.score)
	    {
	      nb_eq_scores ++;
	      score = std::rand() % nb_eq_scores ? score : score_under;
	      // tous les mêmes scores sont equiprobables
	    }
	 
	  if (score_under.score > score.score)
	    {
	      nb_eq_scores = 1;
	      score = score_under;
	    }
	    	  
	  }
	else
	  {
	    score_under = min_max (depth -1, !player, alpha, score.score, range, eval);
	    score_under.suivants.push (score_under.coup);
	    score_under.coup = play;
	    if (score_under.score == score.score)
	      {
		nb_eq_scores ++;
		score = std::rand() % nb_eq_scores ? score : score_under;
		// tous les mêmes scores sont equiprobables
	      }	    
	    if (score_under.score < score.score)
	      {
		nb_eq_scores = 1;
		score = score_under;
	      }
	        
	  }              
            
            
	if ((score.score <= alpha and !player) or (score.score >= beta and player)) {
	  undo_move(old_playgrid, play);
	  return score;
	  }

	undo_move(old_playgrid, play);
           
      }
        
    return score;
  }

  //ALGORITHME MINMAX RANDOMISE

  int random_min_max(bool player, std::function<int()> choice, int range) {
    
    int end_game = won_grid(metagrid);
    if (end_game != 42) return end_game*range;

    std::vector<Coord> playable = playableMoves();
    
    int score = 0;
    if (!playable.empty())
      {
	std::vector<int> scores;
	for(Coord play : playable)
	  {
	    int old_playgrid = playgrid;
	    do_move(play,player);
	    
	    scores.push_back(choice());
	      
	    undo_move(old_playgrid, play);
           
	  }
	
	int index_chosen = 0;
	
	for(int i=1; i<scores.size() ; i++){
	  if (player) {
	    if (scores[i]>scores[index_chosen]) index_chosen = i;
	  }
	  else {
	    if (scores[i]<scores[index_chosen]) index_chosen = i;
	  }
	}
	
	Coord kept = playable[index_chosen];

	int old_playgrid = playgrid;
	do_move(kept, player);
		
	int score_under;
	if (player) score = random_min_max (!player, choice, range);
	else score = random_min_max (!player, choice,  range);
	
	undo_move(old_playgrid, kept);
      }
        
    return score;
  }

  //FONCTIONS D'EVALUATION
  
  int evaluate()
  {
    int score = 5 + (std::rand() % 2 ? -1 : 1);
    return score;
  }
};

int main()
{
  std::srand(42);

  Grid g;
  MinmaxRep res = g.min_max(8, true, -10, 10, 10, [&g](){return g.evaluate();});
    
  std::printf("%d\n", res.score);
    
  return 0;
}
