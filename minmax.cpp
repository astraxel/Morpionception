#include <cstdio>
#include <cstdlib>
#include <vector>

enum Case {Empty, X, O};

struct Grid
{
    Case grid[9][9];
    Case metagrid[9];
    int playgrid;
    
    Grid()
    {
        playgrid = 0;
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
        for (int il = 0 ; il < 3 ; il++)
        {
            for (int jl = 0 ; jl < 3; jl++)
            {
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
                }
            }
        }
    }
    
    int eval()
    {
        return std::rand() % 2 ? -1 : 1;
    }
    
    std::vector<int> playableMoves()
    {
        std::vector<int> res;
        for (int j = 0 ; j < 9 ; j++)
        {
            if(grid[playgrid][j] == Empty)
                res.push_back(j);
        }
        return res;
    }
    
    int min_max(int depth, bool player, int alpha, int beta)
    {
        if (depth == 0) return eval();
        
        std::vector<int> playable = playableMoves();
    
        for(int play : playable)
        {
            int old_playgrid = playgrid;
            grid[playgrid][play] = player ? X : O;
            playgrid = play;
            
            int score = min_max(depth - 1, !player, alpha, beta);
            
            
            
            if (player && score > alpha)
            {
                alpha = score;
                if(alpha >= beta) break;
            }
            else if (!player && score < beta)
            {
                beta = score;
                if(alpha >= beta) break;
            }
            
            playgrid = old_playgrid;
            grid[playgrid][play] = Empty;
        }
        
        return player ? alpha : beta;
    }
};

int main()
{
    std::srand(42);

    Grid g;
    int res = g.min_max(8, true, -1000, 1000);
    
    std::printf("%d", res);
    
    return 0;
}
