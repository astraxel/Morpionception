#ifndef MINMAX_H
#define MINMAX_H

#include <cstdio>
#include <cstdlib>
#include <vector>
#include <stack>
#include <functional>
#include <algorithm>
#include <iostream>

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

struct Grid
{
    Case grid[9][9];
    Case metagrid[9];
    int playgrid;

    // CONSTRUCTEUR
    Grid();

    //AFFICHAGE
    void print();

    //FONCTIONS DE BASE DU JEU
    bool is_full(Case* g);
    void undo_move(int old_playgrid, Coord play);
    void do_move(Coord play, bool player);

    //FONCTIONS AVANCÉES DU JEU
    std::vector<Coord> playable_moves();
    int won_grid(Case* g);
    bool is_complete(Case* g);
    bool player_won(Case* g, Case p);

    //ALGORITHME PSEUDO-EXHAUSTIF
    MinmaxRep pseudo_complete_search(int range, std::function<int(bool)> eval,
                                      int depth, bool player);

    /*
     * ALGORITHME MINMAX DETERMINISTE
     * Envoyer 9 comme secteur au départ, range est la valeur absolue du
     * score max = victoire/défaite assurée
     */
    MinmaxRep min_max(int depth, bool player, int alpha, int beta, int range,
                      std::function<int(bool)> eval);

    //ALGORITHME MINMAX RANDOMISE
    int random_min_max(bool player, std::function<int()> choice, int range);

    //FONCTIONS D'EVALUATION
    int evaluate();
};

#endif // MINMAX_H
