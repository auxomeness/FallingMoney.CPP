# Falling Money (C++ Terminal Game)

A terminal-based game written in C++ where the player controls a basket to catch falling money (`$`) and avoid harmful objects (`.`).

---

## Gameplay

* Move the basket (`U`) left and right using arrow keys.
* Catch `$` to gain **+10 points**.
* Avoid `.` to lose **-15 points**.
* The game starts at **50 points**.

### Win Condition

* Reach **100 points**

### Lose Condition

* Score drops to **0 points**

---

## Features

* Real-time keyboard input (no Enter required)
* Terminal raw mode handling
* Linked list used to simulate falling objects
* Continuous frame updates
* Random object generation

---

## Requirements

* C++ compiler (g++ recommended)
* Unix-based system (Linux or macOS)

This project uses the following headers:

* `<unistd.h>`
* `<termios.h>`
* `<sys/select.h>`

These are not natively supported on Windows.

---

## Compilation

Run the following command in the project directory:

```bash
g++ -std=c++17 -o falling_money main.cpp
```

---

## Running the Game

```bash
./falling_money
```

---

## Controls

| Key         | Action     |
| ----------- | ---------- |
| Left Arrow  | Move left  |
| Right Arrow | Move right |
| Ctrl + C    | Exit game  |

---

## How It Works

* The game uses a linked list where each node represents a row of the screen.
* New rows are added at the top, and bottom rows are removed each frame.
* Objects fall downward over time.
* Collision is detected at the bottom row where the basket is positioned.

---

## Configuration

You can modify these constants in the source code:

```cpp
constexpr int WIDTH = 40;
constexpr int HEIGHT = 20;
constexpr int INITIAL_SCORE = 50;
constexpr int WIN_SCORE = 100;
constexpr int LOSE_SCORE = 0;
constexpr int TICK_MS = 200;
```

---

## Memory Management

The program:

* Frees all dynamically allocated linked list nodes
* Restores terminal settings on exit

---

## Limitations

* Not compatible with Windows CMD or PowerShell
* Uses `system("clear")`, which may cause screen flickering
* No difficulty scaling

---

## Possible Improvements

* Add levels or dynamic difficulty
* Replace rendering with ncurses for smoother output
* Add sound effects
* Improve cross-platform compatibility

---

## License

This project is open-source and available for modification and distribution.
