#include "minmax.h"
#include <ctime>

const int IN_PROGRESS = 42;

MinmaxRep mini(MinmaxRep a, MinmaxRep b, int p)
{
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

MinmaxRep maxi(MinmaxRep a, MinmaxRep b, int p)
{
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

MinmaxRep empty_play(int score)
{
    return MinmaxRep {Coord{9,9}, score, std::stack<Coord>()};
}

Grid::Grid()
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

void Grid::print()
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
                    case Draw:
                        break;
                    }
                }
                printf("|");
            }
            if (jl<2) printf("\n");
        }
        printf("\n+---+---+---+\n");
    }

    printf("+-+-+-+\n");
    for (int c=0; c<3; c++)
    {
        printf("|");
        for (int l=0; l<3; l++)
        {
            switch (metagrid[3*c+l])
            {
            case Empty:
                printf(" |");
                break;
            case X:
                printf("X|");
                break;
            case O:
                printf("O|");
                break;
            case Draw:
                printf("#|");
                break;
            }
        }
        printf("\n+-+-+-+\n");
    }
}

bool Grid::is_full(Case* g)
{
    bool res = true;
    for (int j = 0 ; j < 9 ; j++)
    {
        if (g[j] == Empty) res = false;
    }
    return res;
}

void Grid::undo_move(int old_playgrid, Coord play)
{
    playgrid = old_playgrid;
    grid[play.big][play.little] = Empty;
    if (metagrid[play.big] != Empty)
    {
        if (won_grid(grid[play.big]) == IN_PROGRESS) metagrid[play.big] = Empty;
    }
}

void Grid::do_move(Coord play, bool player)
{
    Case p = player ? X : O;
    grid[play.big][play.little] = p;
    if (metagrid[play.big] == Empty)
    {
        int k = won_grid(grid[play.big]);

        if (k == 0) metagrid[play.big] = Draw;
        else if (k != IN_PROGRESS) metagrid[play.big] = p;

    }
    playgrid = play.little;
}

std::vector<Coord> Grid::playable_moves()
{
    std::vector<Coord> res;
    if (playgrid == 9 || is_full(grid[playgrid]))
    {
        for (int i = 0 ; i < 9 ; i++)
        {
            for (int j = 0 ; j < 9 ; j++)
            {
                if(grid[i][j] == Empty)
                    res.push_back(Coord{i,j});
            }
        }
    }
    else
    {
        for (int j = 0 ; j < 9 ; j++)
        {
            if(grid[playgrid][j] == Empty)
                res.push_back(Coord{playgrid,j});
        }
    }
    return res;
}

int Grid::won_grid(Case* g)
{
    //-1 = perdu, 0 = draw, 1 = gagné, IN_PROGRESS = en cours
    if (player_won(g, X)) return 1;
    if (player_won(g, O)) return -1;
    if (is_complete(g))   return 0;
    return IN_PROGRESS;
}

bool Grid::is_complete(Case* g)
{
    for (int j=0 ; j <9 ; j++)
    {
        if (g[j] == Empty) return false;
    }
    return true;
}

bool Grid::player_won(Case* g, Case p)
{
    bool res = false;

    //Test des lignes
    for (int i = 0; i<3; i++)
    {
        bool line = true;
        for (int j = 0; j < 3; j++) line &= g[3*i+j] == p;

        res |= line;
    }

    //Test des colonnes
    for (int i = 0; i<3; i++)
    {
        bool col = true;
        for (int j = 0; j < 3; j++) col &= g[i+3*j] == p;

        res |= col;
    }

    //Test des diagonales
    res |= (g[0] == p && g[4] == p && g[8] == p);
    res |= (g[2] == p && g[4] == p && g[6] == p);

    return res;
}

MinmaxRep Grid::pseudo_complete_search(int range, std::function<int(bool)> eval, int depth, bool player, bool quiet)
{
    MinmaxRep play;
    MinmaxRep res = empty_play(IN_PROGRESS);
    int play_number = 0;
    while (won_grid(metagrid) == IN_PROGRESS)
    {
        play_number++;
        if(!quiet) std::cout << play_number << std::endl;
        play = min_max(depth, player, -range -1, range + 1, range, eval);
        res.suivants.push(res.coup);
        res.score = play.score;
        res.coup = play.coup;
        do_move(play.coup,player);
        player = !player;
        if(!quiet) print();
    }
    return res;
}

MinmaxRep Grid::min_max(int depth, bool player, int alpha, int beta, int range,
                        std::function<int (bool)> eval)
{
    if (depth == 0) return empty_play(eval(player));

    int end_game = won_grid(metagrid);
    if (end_game != IN_PROGRESS) return empty_play(end_game*range);

    std::vector<Coord> playable = playable_moves();
    MinmaxRep score = empty_play(player ? alpha : beta);
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


        if ((score.score <= alpha && !player) || (score.score >= beta && player))
        {
            undo_move(old_playgrid, play);
            return score;
        }

        undo_move(old_playgrid, play);

    }

    return score;
}

int Grid::random_min_max(bool player, std::function<int ()> choice, int range)
{

    int end_game = won_grid(metagrid);
    if (end_game != IN_PROGRESS) return end_game*range;

    std::vector<Coord> playable = playable_moves();

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

        for(unsigned int i=1; i<scores.size() ; i++)
        {
            if (player)
            {
                if (scores[i]>scores[index_chosen]) index_chosen = i;
            }
            else
            {
                if (scores[i]<scores[index_chosen]) index_chosen = i;
            }
        }

        Coord kept = playable[index_chosen];

        int old_playgrid = playgrid;
        do_move(kept, player);

        score = random_min_max (!player, choice,  range);

        undo_move(old_playgrid, kept);
    }

    return score;
}

int Grid::evaluate()
{
    int score = 5 + (std::rand() % 2 ? -1 : 1);
    return score;
}

int frandom()
{
    return (99 - (std::rand() % 199));
}

int main(int argc, char** argv)
{
    unsigned seed = std::time(nullptr);
    bool quiet = false;

    for(int a = 1 ; a < argc ; a++)
    {
        std::string arg = argv[a];

        if(arg == "-q") quiet = true;
        else if(arg == "-r" && a < argc-1)
        {
            a++;
            seed = std::atoi(argv[a]);
        }
    }

    std::printf("Seed : %d\n", seed);
    std::srand(seed);

    Grid g;
    std::function<int(bool)> eval = [&g](bool player){return g.random_min_max(player, frandom, 1000);};
    MinmaxRep res = g.pseudo_complete_search(1000, eval, 6, true, quiet);
    //g.min_max(8, true, -10, 10, 10, [&g](){return g.evaluate();});

    std::printf("Score : %d\n", res.score);

    return 0;
}
