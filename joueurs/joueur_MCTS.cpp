#include "joueur_MCTS.h"
#include <cmath>
#include <random>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>


Joueur_MCTS::Joueur_MCTS(std::string nom, bool joueur)
  : Joueur(nom, joueur), _etat(0), _savepath("mcts_tree.dat")
{
  load_tree();
}

Joueur_MCTS::~Joueur_MCTS() {
  save_tree();
}

void Joueur_MCTS::initialisation()
{
}

void Joueur_MCTS::init_partie()
{
}

char Joueur_MCTS::nom_abbrege() const
{
  return 'M';
}


static Node* best_child(Node* node, double c, bool our_turn)
{
  Node* best = nullptr;
  double best_val = -1e300;
  double parent_vis = std::max(1, node->visits);
  for (auto &chptr : node->children) {
    Node* ch = chptr.get();
    double wr = (ch->visits == 0) ? 0.5 : (ch->wins / (double)ch->visits);
    double exploit = our_turn ? wr : (1.0 - wr);
    double explore = (ch->visits == 0) ? 1e9 : c * std::sqrt(2.0 * std::log((double)parent_vis) / (double)ch->visits);
    double val = exploit + explore;
    if (val > best_val) { best_val = val; best = ch; }
  }
  return best;
}

// Serialization: write node in pre-order: move visits wins untried_count [untried...] child_count\n then children
void Joueur_MCTS::save_node(std::ostream &os, const Node* n) const {
  os << n->move << ' ' << n->visits << ' ' << n->wins << ' ' << n->untried.size();
  for (int m : n->untried) os << ' ' << m;
  os << ' ' << n->children.size() << '\n';
  for (const auto &ch : n->children) save_node(os, ch.get());
}

Node* Joueur_MCTS::load_node(std::istream &is, Node* parent) {
  int move;
  int visits;
  double wins;
  size_t untried_sz;
  if (!(is >> move >> visits >> wins >> untried_sz)) return nullptr;
  std::vector<int> untried;
  for (size_t i = 0; i < untried_sz; ++i) { int m; is >> m; untried.push_back(m); }
  size_t children_sz;
  is >> children_sz;
  // consume end of line if any
  std::string rest;
  std::getline(is, rest);
  Node* node = new Node(parent, move, untried);
  node->visits = visits;
  node->wins = wins;
  for (size_t i = 0; i < children_sz; ++i) {
    Node* ch = load_node(is, node);
    if (ch) node->children.emplace_back(std::unique_ptr<Node>(ch));
  }
  return node;
}

void Joueur_MCTS::save_tree() {
  std::unique_lock<std::mutex> lk(_tree_mutex, std::try_to_lock);
  if (!lk.owns_lock() || !_root) return;
  std::ofstream ofs(_savepath, std::ios::out);
  if (!ofs) return;
  save_node(ofs, _root.get());
}

bool Joueur_MCTS::load_tree() {
  std::ifstream ifs(_savepath, std::ios::in);
  if (!ifs) return false;
  Node* n = load_node(ifs, nullptr);
  if (!n) return false;
  std::lock_guard<std::mutex> lk(_tree_mutex);
  _root.reset(n);
  return true;
}
void Joueur_MCTS::start_worker() {
  // no background worker in synchronous mode
}

void Joueur_MCTS::stop_worker() {
  // no background worker in synchronous mode
}

void Joueur_MCTS::worker_loop() {
  // no background worker in synchronous mode
}

void Joueur_MCTS::set_game_state(Jeu jeu) {
  // create new root matching current game legal moves, but try to reuse matching first-level children
  Jeu jeu_local = jeu;
  int nb = jeu_local.nb_coups();
  std::vector<int> moves;
  for (int i = 1; i <= nb; ++i) moves.push_back(i);

  {
    std::unique_lock<std::mutex> lk(_tree_mutex, std::try_to_lock);
    if (!lk.owns_lock()) return;
    // create new root
    auto new_root = std::make_unique<Node>(nullptr, -1, moves);

    if (_root) {
      // try to transfer children whose move exists in new root moves
      for (auto &chptr : _root->children) {
        if (!chptr) continue;
        int mv = chptr->move;
        auto it = std::find(new_root->untried.begin(), new_root->untried.end(), mv);
        if (it != new_root->untried.end()) {
          // transfer
          chptr->parent = new_root.get();
          new_root->children.emplace_back(std::move(chptr));
          new_root->untried.erase(it);
        }
      }
    }

    _root.swap(new_root);
    _best_move = 1;
  }
}

void Joueur_MCTS::recherche_coup(Jeu jeu, int &coup)
{
  int nb = jeu.nb_coups();
  if (nb <= 1) {
    coup = 1;
    _best_move = 1;
    return;
  }

  static thread_local std::mt19937 rng((unsigned)std::chrono::high_resolution_clock::now().time_since_epoch().count());
  std::uniform_int_distribution<int> fallback_dist(1, nb);
  int fallback = fallback_dist(rng);
  coup = fallback;

  if (_search_in_progress.exchange(true)) {
    int cached = _best_move.load();
    coup = (cached >= 1 && cached <= nb) ? cached : fallback;
    return;
  }
  struct SearchGuard {
    std::atomic<bool>& flag;
    ~SearchGuard() { flag = false; }
  } guard{_search_in_progress};

  set_game_state(jeu);

  const double C = 1.35;
  const int ITERATIONS = 8;
  const auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(1);

  for (int iter = 0; iter < ITERATIONS; ++iter) {
    if (std::chrono::steady_clock::now() >= deadline) break;
    std::unique_lock<std::mutex> lk(_tree_mutex, std::try_to_lock);
    if (!lk.owns_lock()) break;

    if (!_root) {
      std::vector<int> moves;
      for (int i = 1; i <= nb; ++i) moves.push_back(i);
      _root = std::make_unique<Node>(nullptr, -1, moves);
    }

    Node* node = _root.get();
    Jeu state = jeu;
    bool our_turn = true;
    std::vector<Node*> path;
    path.push_back(node);

    while (node->untried.empty() && !node->children.empty()) {
      Node* next = best_child(node, C, our_turn);
      if (!next) break;
      node = next;
      state.joue(node->move);
      our_turn = !our_turn;
      path.push_back(node);
    }

    if (!node->untried.empty()) {
      std::uniform_int_distribution<int> pick(0, (int)node->untried.size() - 1);
      int idx = pick(rng);
      int mv = node->untried[idx];
      node->untried[idx] = node->untried.back();
      node->untried.pop_back();

      state.joue(mv);
      our_turn = !our_turn;

      std::vector<int> moves2;
      if (!state.terminal()) {
        int nbc = state.nb_coups();
        for (int i = 1; i <= nbc; ++i) moves2.push_back(i);
      }

      node->children.push_back(std::make_unique<Node>(node, mv, moves2));
      node = node->children.back().get();
      path.push_back(node);
    }

    while (!state.terminal()) {
      if (std::chrono::steady_clock::now() >= deadline) break;
      int nbc = std::max(1, state.nb_coups());
      std::uniform_int_distribution<int> dist(1, nbc);
      int mv = dist(rng);
      state.joue(mv);
      our_turn = !our_turn;
    }

    double result = 0.5;
    if (!state.pat()) {
      result = (state.victoire() == this->joueur()) ? 1.0 : 0.0;
    }

    for (Node* p : path) {
      p->visits += 1;
      p->wins += result;
    }
  }

  std::unique_lock<std::mutex> lk(_tree_mutex, std::try_to_lock);
  if (!lk.owns_lock() || !_root) {
    int cached = _best_move.load();
    coup = (cached >= 1 && cached <= nb) ? cached : fallback;
    return;
  }

  int best_move = 1;
  int best_visits = -1;
  double best_wr = -1.0;
  for (const auto &chptr : _root->children) {
    const Node* ch = chptr.get();
    double wr = (ch->visits == 0) ? 0.0 : (ch->wins / (double)ch->visits);
    if (ch->visits > best_visits || (ch->visits == best_visits && wr > best_wr)) {
      best_visits = ch->visits;
      best_wr = wr;
      best_move = ch->move;
    }
  }

  if (best_move < 1 || best_move > nb) best_move = fallback;
  _best_move = best_move;
  coup = best_move;
}
