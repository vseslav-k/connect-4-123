My implementation uses the 69-win combos for eval board state and win detection function. I chose to use this method because I noticed that it provides a much more accurate eval function and allows for trap detection.

Speaking of trap detection, my AI detects these two types of traps:
1. parity traps, if the enemy will eventually be forced to play a stone into a spot right below your 3 combo, allowing you to complete it to 4. The program does this through even-odd parity calculations
2. The program also detects stack traps. This is when two 3-combos have their missing piece in the same column. This means that eventually the enemy will be forced to play into that column to prevent you from completing the first pattern, allowing you to immediately complete the second.

By including these types of traps into its eval mechanism, the program can both  set up and avoid these traps

The program also uses a primitive version of iterative deepening. This is implemented through an array of 42 integers, each represeting search depth at that turn.

The program uses transposition tables with hashing to avoid recalculating known moves.
