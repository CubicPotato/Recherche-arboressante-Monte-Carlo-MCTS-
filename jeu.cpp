#include "jeu.h"

int Jeu::nb_coups() {
  switch(_etat._val) {
  case RACINE : return 2; // e
  case 1 : // a
  case 11 : // aa
  case 111 : // aaa
  case 112 : // aab
  case 113 : // aac
  case 12 : // ab
  case 121 : // aba
  case 13 : // ac
  case 132 : // acb
  case 2 :// b
  case 21 : // ba
  case 211 : // baa
  case 212 : // bab
  case 22 : // bb
  case 23 : // bc
    return 3;
  default :
    return -1;
  }
};


bool Jeu::victoire() {
  return (
	  (_etat._val == 1122) ||
	  (_etat._val == 1123) ||
	  (_etat._val == 1132) ||
	  (_etat._val == 1213) ||
	  (_etat._val == 123) ||
	  (_etat._val == 2113) ||
	  (_etat._val == 231) ||
	  (_etat._val == 232) ||
	  (_etat._val == 233) ||
	  (_etat._val == 1113) );
};

bool Jeu::terminal() {
  return (
	  (_etat._val == 1121) || 
	  (_etat._val == 1122) || 
	  (_etat._val == 1123) ||
	  (_etat._val == 1131) ||
	  (_etat._val == 1132) ||
	  (_etat._val == 1133) ||
	  (_etat._val == 1211) ||
	  (_etat._val == 1212) ||
	  (_etat._val == 1213) ||
	  (_etat._val == 122) ||
	  (_etat._val == 123) ||
	  (_etat._val == 131) ||
	  (_etat._val == 1321) ||
	  (_etat._val == 1322) ||
	  (_etat._val == 1323)  ||
	  (_etat._val == 133)  ||
	  (_etat._val == 2111)  ||
	  (_etat._val == 2112)  ||
	  (_etat._val == 2113)  ||
	  (_etat._val == 2121)  ||
	  (_etat._val == 2122)  ||
	  (_etat._val == 2123)  || 
	  (_etat._val == 213)  ||
	  (_etat._val == 221)  ||
	  (_etat._val == 222)  ||
	  (_etat._val == 223)  ||
	  (_etat._val == 231)  ||
	  (_etat._val == 232)  ||
	  (_etat._val == 233)  ||
	  (_etat._val == 1111)  ||
	  (_etat._val == 1112)  ||
	  (_etat._val == 1113) );
};

Jeu::Jeu() {
  _etat._val = RACINE;
}

void Jeu::reset() {

  _etat._val = RACINE;
}

bool Jeu::coup_licite(int coup) {
  return (coup <= nb_coups()); 
}
  
void Jeu::joue(int coup) {
    _etat._val = (_etat._val * 10) + coup;
}

bool Jeu::pat() { 
  return (
	  (_etat._val == 1133) ||
	  (_etat._val == 1212) ||
	  (_etat._val == 131) ||
	  (_etat._val == 1321) ||
	  (_etat._val == 1323) ||
	  (_etat._val == 133) ||
	  (_etat._val == 2122) ||
	  (_etat._val == 221) ||
	  (_etat._val == 222) ||
	  (_etat._val == 223) ||
	  (_etat._val == 1111) ||
	  (_etat._val == 1112) );
}
