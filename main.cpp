
#include <iostream>
#include <mutex>

#include "arbitre.h"

#define NB_PARTIES 150;
using namespace std;

int main()
{
    //initialise la graine du générateur aléatoire
    std::srand(std::time(nullptr));

    // création de l'Arbitre (graine , joueur 1, joueur 2 , nombre de parties)
    Arbitre a (player::RAND, player::MCTS, NB_PARTIES);
    // commence le challenge
    a.challenge();
    return 0;
}
