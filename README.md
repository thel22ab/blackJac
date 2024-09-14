# Blackjack Game in C

Welcome to the **Blackjack Game in C**! This project is a command-line implementation of the classic Blackjack card game. Play against the dealer, make strategic decisions, and receive feedback on your moves based on basic Blackjack strategy.

## Table of Contents

- [Features](#features)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Compilation](#compilation)
  - [Running the Game](#running-the-game)
- [How to Play](#how-to-play)
- [Game Options](#game-options)
- [Basic Strategy Feedback](#basic-strategy-feedback)
- [Code Structure](#code-structure)
- [Contributing](#contributing)
- [License](#license)
- [Acknowledgments](#acknowledgments)

## Features

- **Standard Blackjack Rules**: Play with the standard rules of Blackjack, including hitting, standing, doubling down, and splitting.
- **Betting System**: Start with an initial balance and place bets on each hand.
- **Basic Strategy Guidance**: Receive real-time feedback on your moves according to basic Blackjack strategy.
- **Splitting and Doubling Down**: Options to split pairs and double down when the situation allows.
- **Randomized Deck**: The deck is shuffled before each game to ensure randomness.

## Getting Started

### Prerequisites

- A C compiler (e.g., GCC)
- Standard C libraries (`stdio.h`, `stdlib.h`, `string.h`, etc.)

### Compilation

Open your terminal and compile the program using:

```bash
gcc -o blackjack blackjack.c
```

### Running the Game

After compiling, start the game by running:

```bash
./blackjack
```

## How to Play

1. **Starting Balance**: You begin the game with a balance of 500 units.
2. **Placing Bets**: Enter your desired bet amount at the beginning of each hand.
3. **Dealing Cards**: Both you and the dealer receive two cards. One of the dealer's cards is revealed.
4. **Making Decisions**: Choose to hit, stand, double down, or split based on your hand and the dealer's upcard.
5. **Basic Strategy Feedback**: After each decision, you'll receive feedback indicating whether your move aligns with basic Blackjack strategy.
6. **Outcome**: The game determines the winner based on Blackjack rules and updates your balance.
7. **Continuing Play**: Choose to play another hand or exit the game.

## Game Options

- **Hit (`h`)**: Take another card.
- **Stand (`s`)**: Keep your current hand.
- **Double Down (`d`)**: Double your bet and receive one final card.
- **Split (`sp`)**: If you have a pair, split them into two hands (requires additional bet equal to your original bet).

## Basic Strategy Feedback

The game provides feedback on your decisions based on basic Blackjack strategy matrices for:

- **Hard Hands**: Hands without an Ace counted as 11.
- **Soft Hands**: Hands where an Ace counts as 11.
- **Pairs**: Two cards of the same rank.

After each move, you'll see a message indicating whether your choice was optimal according to the strategy.

## Code Structure

- **Data Structures**:
  - `Card`: Represents a playing card with rank, suit, and value.
  - `BasicStrategyRow`: Stores strategy actions based on hand values and dealer's upcard.
- **Functions**:
  - `main()`: Entry point that initializes the game loop.
  - `play_blackjack()`: Manages the main game flow, including betting and handling splits.
  - `play_hand()`: Handles player decisions within a hand.
  - `basic_strategy_calculation()`: Provides feedback on the player's decisions based on strategy matrices.
  - Helper Functions:
    - `shuffle_deck()`: Shuffles the deck of cards.
    - `calculate_hand_value()`: Calculates the total value of a hand.
    - `print_hand()`: Displays the player's or dealer's hand.
- **Strategy Matrices**:
  - `hard_strategy[]`: Actions for hard hands.
  - `soft_strategy[]`: Actions for soft hands.
  - `pair_strategy[]`: Actions for pairs.

## License

This project is open-source and available under the [MIT License](https://opensource.org/licenses/MIT).


Enjoy the game, and may the odds be in your favor!
