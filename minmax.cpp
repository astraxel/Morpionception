#include <cstdio>
#include <cstdlib>
#include <vector>

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
    
  int eval()
  {
    int score = 5 + (std::rand() % 2 ? -1 : 1);
    return score;
  }

  bool isFull()
  {
    bool res = true;
    for (int j = 0 ; j < 9 ; j++) {
      if (grid[playgrid][j] == Empty) res = false;
    }
    return res;
  }
    
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

  void undo_move(int old_playgrid, Coord play)
  {
    playgrid = old_playgrid;
    grid[play.big][play.little] = Empty;
  }
    
  int min_max(int depth, bool player, int alpha, int beta)
  {
    if (depth == 0) return eval();
        
    std::vector<Coord> playable = playableMoves();
    int score = player ? alpha : beta;
    
    for(Coord play : playable)
      {
	int old_playgrid = playgrid;
	grid[play.big][play.little] = player ? X : O;
	playgrid = play.little;

	int score_under;
	if (player) {
	    score_under = min_max (depth -1, !player, score, beta);
	    score = std::max(score, score_under);
	  }
	else
	  {
	    score_under = min_max (depth -1, !player, alpha, score);
	    score = std::min(score, score_under);
	  }              
            
            
	if (score < alpha or score > beta) {
	  undo_move(old_playgrid, play);
	  return score;
	  }

	undo_move(old_playgrid, play);
           
      }
        
    return score;
  }
};

int main()
{
  std::srand(42);

  Grid g;
  int res = g.min_max(8, true, -10, 10);
    
  std::printf("%d\n", res);
    
  return 0;
}
