#pragma once

#include "joueur.h"
#include <memory>
#include <vector>
#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

struct Node {
  Node* parent;
  int move; // move that led to this node (from parent)
  int visits;
  double wins;
  std::vector<std::unique_ptr<Node>> children;
  std::vector<int> untried;

  Node(Node* p, int m, const std::vector<int>& moves)
    : parent(p), move(m), visits(0), wins(0), untried(moves) {}
};


class Joueur_MCTS : public Joueur
{
private :
  unsigned int _etat;
  std::unique_ptr<Node> _root;
  std::string _savepath;
  // serialization helpers
  void save_node(std::ostream &os, const Node* n) const;
  Node* load_node(std::istream &is, Node* parent);
  void save_tree();
  bool load_tree();
  // background worker
  std::thread _worker;
  std::atomic<bool> _running{false};
  std::mutex _tree_mutex; // protects _root
  std::condition_variable _cv;
  std::atomic<int> _best_move{1};
  std::atomic<bool> _search_in_progress{false};
  void worker_loop();
  void start_worker();
  void stop_worker();
  // provide current game state to recentre the tree root (reuse matching children when possible)
  void set_game_state(Jeu jeu);
  
public:
  Joueur_MCTS(std::string nom, bool joueur);
  ~Joueur_MCTS();

  void initialisation() override;

  void init_partie() override;

  char nom_abbrege() const override;

  void recherche_coup(Jeu, int & coup) override;
};
