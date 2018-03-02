#include <cstdio>
#include <cstdlib>
#include <vector>
#include <functional>

enum Case {Empty, X, O};

struct Coord
{
  int big;
  int little;
};

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
    if (playgrid == 9 or isFull() ) {
      for (int i = 0 ; i < 9 ; i++)
	for (int j = 0 ; j < 9 ; j++)
	  {
	    if(grid[i][j] == Empty)
	      res.push_back(Coord{i,j});
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
  
  int min_max(int depth, bool player, int alpha, int beta, int range)
  {
    if (depth == 0) return eval();

    int end_game = won_grid();
    if (end_game != 42) return end_game*range;
        
    std::vector<Coord> playable = playableMoves();
    int score = player ? alpha : beta;
    
    for(Coord play : playable)
      {
	int old_playgrid = playgrid;
	do_move(play,player);
	
	int score_under;
	if (player) {
	  score_under = min_max (depth -1, !player, score, beta, range);
	    score = std::max(score, score_under);
	  }
	else
	  {
	    score_under = min_max (depth -1, !player, alpha, score, range);
	    score = std::min(score, score_under);
	  }              
            
            
	if ((score <= alpha and !player) or (score >= beta and player)) {
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
  int res = g.min_max(8, true, -10, 10, 10);
    
  std::printf("%d\n", res);
    
  return 0;
}
