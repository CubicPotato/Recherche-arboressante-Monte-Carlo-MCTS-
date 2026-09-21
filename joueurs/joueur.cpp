#include "joueur.h"

Joueur::Joueur(std::string nom, bool joueur)
    :_nom(nom),_joueur(joueur)
{}

void Joueur::jouer(Jeu jeu, int & coup, std::mutex &coup_mutex)
{
    std::lock_guard<std::mutex> lk(coup_mutex);
    recherche_coup(jeu,coup);
}

std::string Joueur::nom() const
{
    return _nom;
}

bool Joueur::joueur() const
{
    return _joueur;
}

void Joueur::setJoueur(bool joueur)
{
    _joueur = joueur;
}
