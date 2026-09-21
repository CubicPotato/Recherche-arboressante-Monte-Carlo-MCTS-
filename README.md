# Recherche arborescente Monte Carlo (MCTS)

## Project overview
This project applies **Monte Carlo Tree Search (MCTS)** to solve a **black-box competitive game** setting:
- the internal game logic is unknown,
- feedback is limited to win/loss outcomes,
- multiple opponents are available,
- the objective is to design an agent that consistently wins.

## Why this project matters (for recruiters)
This work highlights practical software engineering and AI fundamentals:
- **Algorithmic reasoning** in uncertain environments,
- **Decision-making under sparse feedback** (binary outcome only),
- **Search strategy design** (exploration vs exploitation trade-off),
- **Experiment-driven improvement** against different opponents,
- **Performance-focused thinking** for iterative simulations.

## Core approach
The agent follows the standard MCTS loop:
1. **Selection**: traverse the current search tree to choose a promising node,
2. **Expansion**: add a new state/action to the tree,
3. **Simulation (rollout)**: estimate outcome by simulating from that state,
4. **Backpropagation**: update node statistics with simulation results.

Over repeated iterations, the tree converges toward stronger decisions and improves match outcomes.

## Skills demonstrated
- Artificial Intelligence / Search algorithms  
- Applied probability and statistics  
- Strategy optimization in adversarial contexts  
- Problem-solving with incomplete information  
- Clean, explainable technical communication

## Summary
This repository demonstrates a hands-on implementation mindset for **AI agent strategy**, with a focus on building robust decision systems when only minimal feedback is available.
