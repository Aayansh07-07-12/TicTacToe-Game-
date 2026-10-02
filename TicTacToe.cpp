// Tic-Tac-Toe: a complete console game.
// Features: 2-player mode, vs Computer (Easy / Medium / Unbeatable),
// custom names and markers, scoreboard across rounds, input validation,
// winning-line highlight, and alternating first move.
//
// Build:  g++ -o tictactoe tictactoe.cpp
// Run:    ./tictactoe   (Windows: tictactoe.exe)

#include <iostream>
#include <string>
#include <cstdlib>
#include <cctype>
#include <ctime>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
using namespace std;

// ---------- Types & constants ----------

struct Player {
    string name;
    char marker;
    bool isCpu;
    int wins;
};

enum Difficulty { EASY = 1, MEDIUM = 2, HARD = 3 };

const int LINES[8][3] = {
    {0, 1, 2}, {3, 4, 5}, {6, 7, 8},   // rows
    {0, 3, 6}, {1, 4, 7}, {2, 5, 8},   // columns
    {0, 4, 8}, {2, 4, 6}               // diagonals
};

// ---------- Small helpers ----------

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

string trim(const string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

string readLine(const string& prompt) {
    cout << prompt;
    string s;
    if (!getline(cin, s)) {          // input closed (Ctrl+D / Ctrl+Z): exit cleanly
        cout << "\nGoodbye!\n";
        exit(0);
    }
    return s;
}

int readInt(const string& prompt, int lo, int hi) {
    while (true) {
        string s = trim(readLine(prompt));
        bool ok = !s.empty() && s.size() <= 3;
        for (size_t i = 0; i < s.size(); i++)
            if (!isdigit((unsigned char)s[i])) ok = false;
        if (ok) {
            int v = atoi(s.c_str());
            if (v >= lo && v <= hi) return v;
        }
        cout << "  Please enter a number from " << lo << " to " << hi << ".\n";
    }
}

bool askYesNo(const string& prompt) {
    while (true) {
        string s = trim(readLine(prompt));
        if (!s.empty()) {
            char c = (char)tolower((unsigned char)s[0]);
            if (c == 'y') return true;
            if (c == 'n') return false;
        }
        cout << "  Please answer y or n.\n";
    }
}

void pause(int ms) {
#ifdef _WIN32
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
}

Player makePlayer(const string& name, char marker, bool isCpu) {
    Player p;
    p.name = name;
    p.marker = marker;
    p.isCpu = isCpu;
    p.wins = 0;
    return p;
}

// ---------- Board logic ----------

bool isMark(char c) { return c == 'X' || c == 'O'; }

void resetBoard(char b[9]) {
    for (int i = 0; i < 9; i++) b[i] = (char)('1' + i);
}

bool isFree(const char b[9], int i) { return !isMark(b[i]); }

bool boardFull(const char b[9]) {
    for (int i = 0; i < 9; i++)
        if (isFree(b, i)) return false;
    return true;
}

// Returns 'X' or 'O' if someone has three in a row, else 0.
// If lineOut is given, it receives the index of the winning line.
char winnerOf(const char b[9], int* lineOut = 0) {
    for (int i = 0; i < 8; i++) {
        char a = b[LINES[i][0]];
        if (isMark(a) && a == b[LINES[i][1]] && a == b[LINES[i][2]]) {
            if (lineOut) *lineOut = i;
            return a;
        }
    }
    return 0;
}

void drawBoard(const char b[9]) {
    bool hl[9] = {false};
    int line = -1;
    if (winnerOf(b, &line))
        for (int k = 0; k < 3; k++) hl[LINES[line][k]] = true;

    cout << "\n";
    for (int r = 0; r < 3; r++) {
        cout << "   ";
        for (int c = 0; c < 3; c++) {
            int i = r * 3 + c;
            if (hl[i]) cout << "(" << b[i] << ")";
            else       cout << " " << b[i] << " ";
            if (c < 2) cout << "|";
        }
        cout << "\n";
        if (r < 2) cout << "   ---+---+---\n";
    }
    cout << "\n";
}

// ---------- Computer player ----------

// Minimax with depth scoring so the AI wins fast and loses slow.
int minimax(char b[9], char cpu, char human, bool cpuTurn, int depth) {
    char w = winnerOf(b);
    if (w == cpu) return 10 - depth;
    if (w == human) return depth - 10;
    if (boardFull(b)) return 0;

    int best = cpuTurn ? -100 : 100;
    for (int i = 0; i < 9; i++) {
        if (!isFree(b, i)) continue;
        char saved = b[i];
        b[i] = cpuTurn ? cpu : human;
        int score = minimax(b, cpu, human, !cpuTurn, depth + 1);
        b[i] = saved;
        if (cpuTurn) { if (score > best) best = score; }
        else         { if (score < best) best = score; }
    }
    return best;
}

int randomFreeCell(const char b[9]) {
    int cells[9], n = 0;
    for (int i = 0; i < 9; i++)
        if (isFree(b, i)) cells[n++] = i;
    return cells[rand() % n];
}

// Returns a cell that completes three-in-a-row for 'mark', or -1.
int findCompletingMove(char b[9], char mark) {
    for (int i = 0; i < 9; i++) {
        if (!isFree(b, i)) continue;
        char saved = b[i];
        b[i] = mark;
        bool wins = (winnerOf(b) == mark);
        b[i] = saved;
        if (wins) return i;
    }
    return -1;
}

int bestMinimaxMove(char b[9], char cpu, char human) {
    int bestScore = -1000, candidates[9], n = 0;
    for (int i = 0; i < 9; i++) {
        if (!isFree(b, i)) continue;
        char saved = b[i];
        b[i] = cpu;
        int score = minimax(b, cpu, human, false, 1);
        b[i] = saved;
        if (score > bestScore) { bestScore = score; n = 0; }
        if (score == bestScore) candidates[n++] = i;
    }
    return candidates[rand() % n];     // vary play among equally good moves
}

int cpuMove(char b[9], char cpu, char human, Difficulty d) {
    if (d == EASY) return randomFreeCell(b);
    if (d == MEDIUM) {
        int m = findCompletingMove(b, cpu);        // win if possible
        if (m == -1) m = findCompletingMove(b, human); // otherwise block
        if (m == -1) m = randomFreeCell(b);
        return m;
    }
    return bestMinimaxMove(b, cpu, human);         // unbeatable
}

// ---------- Game flow ----------

void showScore(const Player p[2], int draws) {
    cout << "  Score:  " << p[0].name << " (" << p[0].marker << ") " << p[0].wins
         << "   |   " << p[1].name << " (" << p[1].marker << ") " << p[1].wins
         << "   |   Draws " << draws << "\n";
}

// Plays one round. Returns winner index (0/1), -1 for a draw, -2 if quit.
int playRound(Player p[2], int starter, Difficulty diff, int draws) {
    char b[9];
    resetBoard(b);
    int cur = starter;

    while (true) {
        clearScreen();
        cout << "=== TIC-TAC-TOE ===\n";
        showScore(p, draws);
        drawBoard(b);

        int slot;
        if (p[cur].isCpu) {
            cout << "  " << p[cur].name << " is thinking...\n";
            pause(700);
            slot = cpuMove(b, p[cur].marker, p[1 - cur].marker, diff);
        } else {
            while (true) {
                string prompt = "  " + p[cur].name + " (" + string(1, p[cur].marker) +
                                "), pick a slot 1-9 (0 = quit to menu): ";
                int v = readInt(prompt, 0, 9);
                if (v == 0) {
                    if (askYesNo("  Quit this match? (y/n): ")) return -2;
                    continue;
                }
                if (!isFree(b, v - 1)) {
                    cout << "  That slot is taken. Try another.\n";
                    continue;
                }
                slot = v - 1;
                break;
            }
        }

        b[slot] = p[cur].marker;

        if (winnerOf(b)) {
            clearScreen();
            cout << "=== TIC-TAC-TOE ===\n";
            drawBoard(b);
            cout << "  *** " << p[cur].name << " (" << p[cur].marker << ") wins this round! ***\n\n";
            return cur;
        }
        if (boardFull(b)) {
            clearScreen();
            cout << "=== TIC-TAC-TOE ===\n";
            drawBoard(b);
            cout << "  It's a tie!\n\n";
            return -1;
        }
        cur = 1 - cur;
    }
}

void playSession(bool vsCpu) {
    Player p[2];
    Difficulty diff = HARD;

    clearScreen();
    cout << (vsCpu ? "=== VS COMPUTER ===\n\n" : "=== TWO PLAYERS ===\n\n");

    string n1 = trim(readLine("  Player 1 name (Enter for 'Player 1'): "));
    if (n1.empty()) n1 = "Player 1";
    string m;
    while (true) {
        m = trim(readLine("  " + n1 + ", choose your marker (X or O): "));
        if (m.size() == 1 && (toupper((unsigned char)m[0]) == 'X' || toupper((unsigned char)m[0]) == 'O')) break;
        cout << "  Please type X or O.\n";
    }
    char mk1 = (char)toupper((unsigned char)m[0]);
    char mk2 = (mk1 == 'X') ? 'O' : 'X';

    p[0] = makePlayer(n1, mk1, false);
    if (vsCpu) {
        cout << "\n  Difficulty:  1) Easy   2) Medium   3) Unbeatable\n";
        diff = (Difficulty)readInt("  Choose 1-3: ", 1, 3);
        p[1] = makePlayer("Computer", mk2, true);
    } else {
        string n2 = trim(readLine("  Player 2 name (Enter for 'Player 2'): "));
        if (n2.empty()) n2 = "Player 2";
        if (n2 == n1) n2 += " (2)";
        p[1] = makePlayer(n2, mk2, false);
    }

    int draws = 0;
    int starter = (p[0].marker == 'X') ? 0 : 1;   // X moves first in round 1, then alternate

    while (true) {
        int result = playRound(p, starter, diff, draws);
        if (result == -2) break;
        if (result == -1) draws++;
        else p[result].wins++;

        showScore(p, draws);
        cout << "\n";
        if (!askYesNo("  Play another round? (y/n): ")) break;
        starter = 1 - starter;
    }

    clearScreen();
    cout << "=== FINAL RESULT ===\n\n";
    showScore(p, draws);
    cout << "\n";
    if (p[0].wins > p[1].wins)      cout << "  " << p[0].name << " wins the match!\n";
    else if (p[1].wins > p[0].wins) cout << "  " << p[1].name << " wins the match!\n";
    else                            cout << "  The match ends level.\n";
    readLine("\n  Press Enter to return to the menu...");
}

void showHelp() {
    clearScreen();
    cout << "=== HOW TO PLAY ===\n\n"
         << "  Players take turns placing their marker (X or O) on a 3x3 grid.\n"
         << "  Get three in a row - across, down or diagonally - to win.\n"
         << "  If all nine slots fill up with no winner, the round is a tie.\n\n"
         << "  The slots are numbered like this:\n";
    char b[9];
    resetBoard(b);
    drawBoard(b);
    cout << "  Type a slot number to place your marker. Type 0 to quit a match.\n"
         << "  The player who moves first alternates each round.\n";
    readLine("\n  Press Enter to return to the menu...");
}

int main() {
    srand((unsigned)time(0));

    while (true) {
        clearScreen();
        cout << "=============================\n"
             << "        TIC-TAC-TOE\n"
             << "=============================\n\n"
             << "  1) Two players\n"
             << "  2) Play vs Computer\n"
             << "  3) How to play\n"
             << "  4) Quit\n\n";
        int choice = readInt("  Choose 1-4: ", 1, 4);
        if (choice == 1) playSession(false);
        else if (choice == 2) playSession(true);
        else if (choice == 3) showHelp();
        else break;
    }
    cout << "\nThanks for playing!\n";
    return 0;
}