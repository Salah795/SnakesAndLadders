# Markov Chain Text Generator and Game Simulator
This project implements a generic Markov Chain framework in C, which is used in two distinct applications:

Text-based Tweet Generator using real text input.

Snakes and Ladders Game Simulator using board transitions.

📁 Project Structure
markov_chain.h / markov_chain.c: Core Markov Chain data structures and logic.

linked_list.h / linked_list.c: Linked list implementation used by the Markov Chain.

tweets_generator.c: Program that generates sentences from input text.

snakes_and_ladders.c: Simulates paths through a snakes and ladders board using a Markov Chain.

makefile: Compilation instructions for both applications.

README.md: Project documentation.

🧠 How It Works
A Markov Chain is a stochastic model that predicts the next state based only on the current state. In this project, a generic Markov chain is implemented, allowing custom types for states and transitions, defined through function pointers (compare, print, copy, etc.).

🐍 Snakes and Ladders Simulator
Description
Simulates random paths in a Snakes and Ladders game using the Markov Chain to represent board positions and possible moves (via dice rolls and transitions like ladders/snakes).

Usage
bash: ./snakes_and_ladders <SEED> <NUM_OF_WALKS>
Example
bash: ./snakes_and_ladders 17 3
This will simulate 3 random walks starting from cell 1 and ending at 100.

🐦 Tweet Generator
Description
Generates tweet-like sentences based on an input text file using Markov Chains.

Usage
bash: ./tweets_generator <SEED> <NUM_OF_TWEETS> <FILE_PATH> [<WORDS_TO_READ>]
<SEED>: Random seed.

<NUM_OF_TWEETS>: Number of tweets to generate.

<FILE_PATH>: Path to a text file (e.g., tweets).

[<WORDS_TO_READ>]: Optional. Max number of words to read from the file.

Example
bash:
./tweets_generator 7 5 tweets.txt
./tweets_generator 10 3 tweets.txt 100

🔧 Compilation
Use the provided makefile to compile the project.

bash:
make all
This will generate the tweets_generator and snakes_and_ladders executables.

To clean up:

bash:
make clean
🧪 Features
Generic, type-safe Markov chain engine.

Easily extendable to other applications.

Proper memory management with custom alloc/free per type.

Customizable output (e.g., sentence ending for tweets, win cell for the game).
