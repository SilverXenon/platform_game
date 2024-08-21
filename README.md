# Platform Game

Welcome to the Platform Game project! This repository contains three versions of a platformer where the player can run and jump. Each version introduces new features and complexity.

## Game Versions

1. **mario_v1.c**: A simple terminal-based version using `ncurses`.
2. **mario_v2.c**: A 2D graphical version built with SDL2.
3. **mario_v3.c**: An advanced 2D version with an AI learning through Q-learning (experimental).

## How to Run

After compiling the game (instructions below), you can run the executables directly from the command line:

```bash
./mario_v1   # Runs the terminal-based version
./mario_v2   # Runs the 2D SDL2 version
./mario_v3   # Runs the AI-enhanced version
```

## How to Compile

To compile each version, use the following commands:

### For `mario_v1` (Terminal version using `ncurses`)

```bash
clang -o mario_v1 mario_v1.c -lncurses
```

### For `mario_v2` (2D version using `SDL2`)

```bash
clang -o mario_v2 mario_v2.c -lSDL2
```

### For `mario_v3` (2D version with AI using `SDL2` and Q-learning)

```bash
clang -o mario_v3 mario_v3.c -lSDL2
```

### Installing Dependencies

Make sure you have the necessary libraries installed:

- For `ncurses`:
  - On Debian/Ubuntu:
    ```bash
    sudo apt-get install libncurses-dev
    ```
  - On Arch Linux:
    ```bash
    sudo pacman -S ncurses
    ```

- For `SDL2`:
  - On Debian/Ubuntu:
    ```bash
    sudo apt-get install libsdl2-dev
    ```
  - On Arch Linux:
    ```bash
    sudo pacman -S sdl2
    ```

## Notes

- The AI in `mario_v3` is experimental, and its learning behavior may vary.
- The game is designed to work on Linux systems. Compatibility with other platforms is not guaranteed.
- Feel free to modify and experiment with the code to learn and enhance the game.

## Contributing

If you'd like to contribute or report issues, feel free to open a pull request or issue in this repository.

## License

This project is licensed under the GNU General Public License v3.0 - see the [LICENSE](LICENSE) file for details.
