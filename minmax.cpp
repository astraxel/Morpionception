#include <cstdio>
#include <cstdlib>
#include <vector>
#include <stack>
#include <functional>
#include <algorithm>

enum Case {Empty, X, O};

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

  bool isFull()
  {
    bool res = true;
    for (int j = 0 ; j < 9 ; j++) {
      if (grid[playgrid][j] == Empty) res = false;
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
    grid[play.big][play.little] = player ? X : O;
    playgrid = play.little;
    //TODO : update les conditions de victoire
  }

  //FONCTIONS AVANCÉES DU JEU

  std::vector<Coord> playableMoves()
  {
    std::vector<Coord> res;
    if (playgrid == 9 or isFull() ){
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

  int won_grid () {
    return 42; //-1 = perdu, 0 = draw, 1 = gagné, 42 = en cours
    //TODO
  }

  //ALGORITHME MINMAX DETERMINISTE
  //Envoyer 9 comme secteur au départ, range est la valeur absolue du score max = victoire/défaite assurée

  //TODO : renvoyer aussi les coups joués
  
  MinmaxRep min_max(int depth, bool player, int alpha, int beta, int range)
  {
    if (depth == 0) return emptyplay(eval());

    int end_game = won_grid();
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
	  score_under = min_max (depth -1, !player, score.score, beta, range);
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
	    score_under = min_max (depth -1, !player, alpha, score.score, range);
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
    
    int end_game = won_grid();
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
  
  int eval()
  {
    int score = 5 + (std::rand() % 2 ? -1 : 1);
    return score;
  }
};

int main()
{
  std::srand(42);

  Grid g;
  MinmaxRep res = g.min_max(8, true, -10, 10, 10);
    
  std::printf("%d\n", res.score);
    
  return 0;
}
