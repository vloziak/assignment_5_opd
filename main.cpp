#include <iostream>
#include <unistd.h>
#include <termios.h>
#include <cstdlib>
#include <ctime>
#include <vector>

using namespace std;

bool gameOver;
const int width = 20;
const int height = 17;
int x, y, score;
int tailX[100], tailY[100];
int nTail;
const int numFruits = 5;
const int numBombs = 3;
struct Object {
    int x;
    int y;
};

Object fruits[numFruits];
Object bombs[numBombs];

enum Direction { STOP = 0, LEFT, RIGHT, UP, DOWN };
Direction dir;

void Setup() {
    gameOver = false;
    dir = STOP;
    x = width / 2;
    y = height / 2;
    score = 0;

    for (int i = 0; i < numFruits; i++) {
        fruits[i].x = rand() % width;
        fruits[i].y = rand() % height;
    }

    for (int i = 0; i < numBombs; i++) {
        bombs[i].x = rand() % width;
        bombs[i].y = rand() % height;
    }
}

void Draw(const string &playerName) {
    system("clear");
    for (int i = 0; i < width + 2; i++)
        cout << "-";
    cout << endl;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (j == 0)
                cout << "|";
            if (i == y && j == x)
                cout << "O";
            else {
                bool printFruit = false;
                for (int k = 0; k < numFruits; k++) {
                    if (i == fruits[k].y && j == fruits[k].x) {
                        cout << "F";
                        printFruit = true;
                        break;
                    }
                }

                bool printBomb = false;
                for (int k = 0; k < numBombs; k++) {
                    if (i == bombs[k].y && j == bombs[k].x) {
                        cout << "B";
                        printBomb = true;
                        break;
                    }
                }

                bool printTail = false;
                for (int k = 0; k < nTail; k++) {
                    if (tailX[k] == j && tailY[k] == i) {
                        cout << "o";
                        printTail = true;
                        break;
                    }
                }

                if (!printFruit && !printBomb && !printTail)
                    cout << " ";
            }

            if (j == width - 1)
                cout << "|";
        }
        cout << endl;
    }

    for (int i = 0; i < width + 2; i++)
        cout << "-";
    cout << endl;
    cout << playerName << "'s Score: " << score << endl;
}

void GameEngine() {
    int prevX = tailX[0];
    int prevY = tailY[0];
    int prev2X, prev2Y;
    tailX[0] = x;
    tailY[0] = y;

    for (int i = 1; i < nTail; i++) {
        prev2X = tailX[i];
        prev2Y = tailY[i];
        tailX[i] = prevX;
        tailY[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }

    switch (dir) {
        case LEFT:  x--; break;
        case RIGHT: x++; break;
        case UP:    y--; break;
        case DOWN:  y++; break;
        default:    break;
    }

    if (x >= width) x = 0; else if (x < 0) x = width - 1;
    if (y >= height) y = 0; else if (y < 0) y = height - 1;

    for (int i = 0; i < nTail; i++) {
        if (tailX[i] == x && tailY[i] == y) {
            gameOver = true;
        }
    }

    for (int i = 0; i < numFruits; i++) {
        if (x == fruits[i].x && y == fruits[i].y) {
            score += 10;
            fruits[i].x = rand() % width;
            fruits[i].y = rand() % height;
            nTail++;
        }
    }

    for (int i = 0; i < numBombs; i++) {
        if (x == bombs[i].x && y == bombs[i].y) {
            gameOver = true;
        }
    }
}

void ChangeDirection(char key) {
    switch (key) {
        case 'a': dir = LEFT; break;
        case 'd': dir = RIGHT; break;
        case 'w': dir = UP; break;
        case 's': dir = DOWN; break;
        case 'i': gameOver = true; break;
    }
}

void Input() {
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    char key;
    if (read(STDIN_FILENO, &key, 1) == 1) {
        ChangeDirection(key);
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

int main() {
    string playerName;
    cout << "Enter your name: ";
    cin >> playerName;
    srand(static_cast<unsigned>(time(0)));
    Setup();

    while (!gameOver) {
        Draw(playerName);
        Input();
        GameEngine();
        usleep(100000);
    }

    cout << "Game Over! Final Score: " << score << endl;
    return 0;
}
