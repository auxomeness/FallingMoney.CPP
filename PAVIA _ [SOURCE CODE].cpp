// Falling Money game: catch '$' and avoid '.' with a moving basket at the bottom.

#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <termios.h>
#include <sys/select.h>
#include <random>

using namespace std;

// game settings and constants
constexpr int WIDTH = 40;
constexpr int HEIGHT = 20;
constexpr int INITIAL_SCORE = 50;
constexpr int WIN_SCORE = 100;
constexpr int LOSE_SCORE = 0;
constexpr int TICK_MS = 200; // how fast the game updates (ms)

// linked list node holding one line of the sky
struct Node {
    string data;
    Node* next;
};
typedef Node* NodePtr;

// sets terminal to raw mode so we can read keys instantly and restore on exit
class TermiosGuard {
    struct termios oldt;
    bool active = false;
public:
    TermiosGuard() {
        if (tcgetattr(STDIN_FILENO, &oldt) == 0) {
            struct termios raw = oldt;
            raw.c_lflag &= ~(ECHO | ICANON); // disable echo and line buffering
            raw.c_cc[VMIN] = 0;
            raw.c_cc[VTIME] = 0;
            if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == 0) active = true;
        }
    }
    ~TermiosGuard() {
        if (active) tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldt);
    }
};

// check if a key was pressed without blocking
bool kbhit() {
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);
    timeval tv{0, 0};
    return select(STDIN_FILENO + 1, &readfds, nullptr, nullptr, &tv) == 1;
}

// get one char from stdin without waiting
int getCharNonBlock() {
    unsigned char inputChar;
    ssize_t n = read(STDIN_FILENO, &inputChar, 1);
    if (n <= 0) return -1;
    return inputChar;
}

// make an empty line filled with spaces
string makeEmptyLine() {
    return string(WIDTH, ' ');
}

// add a new line at the top of the linked list
void pushTop(NodePtr &headNode, const string &lineString) {
    NodePtr newNode = new Node{lineString, headNode};
    headNode = newNode;
}

// remove the bottom line and return its content
string popBottom(NodePtr &headNode) {
    if (!headNode) return "";
    if (!headNode->next) {
        string data = headNode->data;
        delete headNode;
        headNode = nullptr;
        return data;
    }
    NodePtr currentNode = headNode;
    while (currentNode->next->next) currentNode = currentNode->next;
    // currentNode->next is bottom
    string data = currentNode->next->data;
    delete currentNode->next;
    currentNode->next = nullptr;
    return data;
}

// start with a sky full of empty lines
NodePtr initSky() {
    NodePtr headNode = nullptr;
    for (int i = 0; i < HEIGHT; ++i) {
        pushTop(headNode, makeEmptyLine());
    }
    return headNode;
}

// count how many lines are in the list (debugging)
int countNodes(NodePtr headNode) {
    int c = 0;
    while (headNode) { ++c; headNode = headNode->next; }
    return c;
}

// draw everything on the screen including the basket and score
void render(NodePtr headNode, int basket_pos, int score) {
    // clear the screen and move cursor home
    system("clear");

    cout << "------------- Falling Money --------------\n";

    // gather lines from linked list to print top to bottom
    string lines[HEIGHT];
    int index = 0;
    for (NodePtr currentNode = headNode; currentNode; currentNode = currentNode->next) {
        if (index < HEIGHT) {
            lines[index++] = currentNode->data;
        }
    }
    for (int i = 0; i < HEIGHT; ++i) {
        string lineString = lines[i];
        // put basket 'U' on bottom line at basket_pos
        if (i == HEIGHT - 1) {
            if (basket_pos >= 0 && basket_pos < WIDTH) lineString[basket_pos] = 'U';
        }
        cout << "|" << lineString << "|\n";
    }

    cout << "Your Score: " << score << "\n";
    cout.flush();
}

// generate two random objects ('$' or '.') on a new sky line
string generateRandomLine() {
    string lineString = makeEmptyLine();
    int position1 = rand() % WIDTH;
    int position2;
    do {
        position2 = rand() % WIDTH;
    } while (position2 == position1);

    lineString[position1] = (rand() % 2 ? '$' : '.');
    lineString[position2] = (rand() % 2 ? '$' : '.');
    return lineString;
}

// check what's at basket position on bottom line and update score accordingly
int resolveBottomCollision(const string &bottomLine, int basket_pos) {
    if (basket_pos < 0 || basket_pos >= (int)bottomLine.size()) return 0;
    char c = bottomLine[basket_pos];
    if (c == '$') return +10;
    if (c == '.') return -15;
    return 0;
}

// free all the nodes in the sky linked list
void cleanUp(NodePtr &headNode) {
    while (headNode) {
        NodePtr tmp = headNode;
        headNode = headNode->next;
        delete tmp;
    }
}

int main() {
    srand((unsigned)time(nullptr));
    srand(time(0));  // seed random number generator with current time

    TermiosGuard termiosGuard; // turn on raw mode for instant key reading

    NodePtr skyHead = initSky();
    int basket_pos = WIDTH / 2; // start basket centered at bottom
    int score = INITIAL_SCORE;

    render(skyHead, basket_pos, score);

    bool isRunning = true;
    // main game loop
    while (isRunning) {

        // read all keys pressed and move basket left/right
        while (kbhit()) {
            int inputChar = getCharNonBlock();
            if (inputChar == -1) break;
            if (inputChar == 27) { // ESC sequence for arrow keys
                int inputChar1 = -1, inputChar2 = -1;
                usleep(10000); // wait a bit for next bytes
                if (kbhit()) inputChar1 = getCharNonBlock();
                if (kbhit()) inputChar2 = getCharNonBlock();
                if (inputChar1 == '[') {
                    if (inputChar2 == 'D') { // left arrow
                        if (basket_pos > 0) --basket_pos;
                    } else if (inputChar2 == 'C') { // right arrow
                        if (basket_pos < WIDTH - 1) ++basket_pos;
                    }
                }
            } else if (inputChar == 127 || inputChar == '\b') {
                // ignore backspace or delete keys
            }
        }

        // add a new line with falling objects at the top
        string new_top = generateRandomLine();
        pushTop(skyHead, new_top);

        // find bottom line to check for collisions
        NodePtr currentNode = skyHead;
        if (!currentNode) {
            // safety: if list empty, reset sky
            skyHead = initSky();
            currentNode = skyHead;
        }
        while (currentNode->next) currentNode = currentNode->next;
        string bottomLine = currentNode->data;

        // update score based on what basket caught or missed
        int scoreDelta = resolveBottomCollision(bottomLine, basket_pos);
        score += scoreDelta;

        // remove bottom line to keep sky height steady
        popBottom(skyHead);

        // redraw everything with updated state
        render(skyHead, basket_pos, score);

        // check if player won or lost
        if (score >= WIN_SCORE) {
            cout << "\nYour Score: " << score << "\nYOU WIN!\n";
            isRunning = false;
            break;
        } else if (score <= LOSE_SCORE) {
            cout << "\nYour Score: " << score << "\nYOU LOSE!\n";
            isRunning = false;
            break;
        }

        // wait a bit before next frame
        usleep(TICK_MS * 1000);
    }

    cleanUp(skyHead);
    return 0;
}