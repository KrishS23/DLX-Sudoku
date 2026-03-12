#include "DLXSudokuSolver.hpp"
#include <iostream>
#include <cmath>
#include <string>
#include <ctime>
#include <sstream>
#include <vector>

#define MAX_K 1000
#define SIZE 9

struct Node {
    Node* left;
    Node* right;
    Node* up;
    Node* down;
    Node* head;
    int size;
    int rowID[3];
};

const int SIZE_SQUARED = SIZE * SIZE;
const int SIZE_SQRT = static_cast<int>(sqrt(static_cast<double>(SIZE)));
const int ROW_NB = SIZE * SIZE * SIZE;
const int COL_NB = 4 * SIZE * SIZE;

static Node Head;
static Node* HeadNode = &Head;
static Node* solution[MAX_K];
static Node* orig_values[MAX_K];
static bool matrix[ROW_NB][COL_NB] = {{0}};
static bool isSolved = false;
static int finalGrid[SIZE][SIZE];

void ResetGlobals() {
    for (int i = 0; i < MAX_K; i++) {
        solution[i] = nullptr;
        orig_values[i] = nullptr;
    }

    for (int i = 0; i < ROW_NB; i++) {
        for (int j = 0; j < COL_NB; j++) {
            matrix[i][j] = 0;
        }
    }

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            finalGrid[i][j] = 0;
        }
    }

    isSolved = false;
    HeadNode = &Head;
}

void coverColumn(Node* col) {
    col->left->right = col->right;
    col->right->left = col->left;

    for (Node* row = col->down; row != col; row = row->down) {
        for (Node* node = row->right; node != row; node = node->right) {
            node->down->up = node->up;
            node->up->down = node->down;
            node->head->size--;
        }
    }
}

void uncoverColumn(Node* col) {
    for (Node* row = col->up; row != col; row = row->up) {
        for (Node* node = row->left; node != row; node = node->left) {
            node->head->size++;
            node->down->up = node;
            node->up->down = node;
        }
    }

    col->left->right = col;
    col->right->left = col;
}

void MapSolutionToGrid(int Sudoku[][SIZE]) {
    for (int i = 0; i < MAX_K && solution[i] != nullptr; i++) {
        Sudoku[solution[i]->rowID[1] - 1][solution[i]->rowID[2] - 1] = solution[i]->rowID[0];
    }

    for (int i = 0; i < MAX_K && orig_values[i] != nullptr; i++) {
        Sudoku[orig_values[i]->rowID[1] - 1][orig_values[i]->rowID[2] - 1] = orig_values[i]->rowID[0];
    }
}

void search(int k) {
    if (HeadNode->right == HeadNode) {
        MapSolutionToGrid(finalGrid);
        isSolved = true;
        return;
    }

    Node* col = HeadNode->right;
    for (Node* temp = col->right; temp != HeadNode; temp = temp->right) {
        if (temp->size < col->size) {
            col = temp;
        }
    }

    coverColumn(col);

    for (Node* row = col->down; row != col && !isSolved; row = row->down) {
        solution[k] = row;

        for (Node* node = row->right; node != row; node = node->right) {
            coverColumn(node->head);
        }

        search(k + 1);

        Node* chosenRow = solution[k];
        solution[k] = nullptr;

        for (Node* node = chosenRow->left; node != chosenRow; node = node->left) {
            uncoverColumn(node->head);
        }
    }

    uncoverColumn(col);
}

void BuildSparseMatrix(bool matrix[ROW_NB][COL_NB]) {
    int col = 0, counter = 0;
    for (int row = 0; row < ROW_NB; row++) {
        matrix[row][col] = 1;
        counter++;
        if (counter >= SIZE) {
            col++;
            counter = 0;
        }
    }

    int startRow = 0;
    counter = 1;
    for (col = SIZE_SQUARED; col < 2 * SIZE_SQUARED; col++) {
        for (int row = startRow; row < counter * SIZE_SQUARED; row += SIZE) {
            matrix[row][col] = 1;
        }

        if ((col + 1) % SIZE == 0) {
            startRow = counter * SIZE_SQUARED;
            counter++;
        } else {
            startRow++;
        }
    }

    col = 2 * SIZE_SQUARED;
    for (int row = 0; row < ROW_NB; row++) {
        matrix[row][col] = 1;
        col++;
        if (col >= 3 * SIZE_SQUARED) {
            col = 2 * SIZE_SQUARED;
        }
    }

    int x = 0;
    for (col = 3 * SIZE_SQUARED; col < COL_NB; col++) {
        for (int boxRow = 0; boxRow < SIZE_SQRT; boxRow++) {
            for (int boxCol = 0; boxCol < SIZE_SQRT; boxCol++) {
                matrix[x + boxRow * SIZE + boxCol * SIZE_SQUARED][col] = 1;
            }
        }

        int temp = col + 1 - 3 * SIZE_SQUARED;

        if (temp % (SIZE_SQRT * SIZE) == 0) {
            x += (SIZE_SQRT - 1) * SIZE_SQUARED + (SIZE_SQRT - 1) * SIZE + 1;
        } else if (temp % SIZE == 0) {
            x += SIZE * (SIZE_SQRT - 1) + 1;
        } else {
            x++;
        }
    }
}

void BuildLinkedList(bool matrix[ROW_NB][COL_NB]) {
    Node* header = new Node;
    header->left = header;
    header->right = header;
    header->up = header;
    header->down = header;
    header->head = header;
    header->size = -1;

    Node* temp = header;

    for (int i = 0; i < COL_NB; i++) {
        Node* colNode = new Node;
        colNode->size = 0;
        colNode->up = colNode;
        colNode->down = colNode;
        colNode->head = colNode;

        colNode->left = temp;
        colNode->right = header;
        temp->right = colNode;
        header->left = colNode;

        temp = colNode;
    }

    int ID[3] = {0, 1, 1};

    for (int i = 0; i < ROW_NB; i++) {
        Node* top = header->right;
        Node* prev = nullptr;

        if (i != 0 && i % SIZE_SQUARED == 0) {
            ID[0] -= SIZE - 1;
            ID[1]++;
            ID[2] -= SIZE - 1;
        } else if (i != 0 && i % SIZE == 0) {
            ID[0] -= SIZE - 1;
            ID[2]++;
        } else {
            ID[0]++;
        }

        for (int j = 0; j < COL_NB; j++, top = top->right) {
            if (!matrix[i][j]) continue;

            Node* newNode = new Node;
            newNode->rowID[0] = ID[0];
            newNode->rowID[1] = ID[1];
            newNode->rowID[2] = ID[2];

            if (prev == nullptr) {
                prev = newNode;
                prev->right = newNode;
            }

            newNode->left = prev;
            newNode->right = prev->right;
            newNode->right->left = newNode;
            prev->right = newNode;

            newNode->head = top;
            newNode->down = top;
            newNode->up = top->up;
            top->up->down = newNode;
            top->up = newNode;
            top->size++;

            if (top->down == top) {
                top->down = newNode;
            }

            prev = newNode;
        }
    }

    HeadNode = header;
}

Node* FindRowForCandidate(int value, int row, int col) {
    for (Node* column = HeadNode->right; column != HeadNode; column = column->right) {
        for (Node* node = column->down; node != column; node = node->down) {
            if (node->rowID[0] == value &&
                node->rowID[1] - 1 == row &&
                node->rowID[2] - 1 == col) {
                return node;
            }
        }
    }
    return nullptr;
}

void TransformListToCurrentGrid(int Puzzle[][SIZE]) {
    int index = 0;

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (Puzzle[i][j] <= 0) continue;

            Node* rowNode = FindRowForCandidate(Puzzle[i][j], i, j);
            if (rowNode == nullptr) continue;

            coverColumn(rowNode->head);
            orig_values[index++] = rowNode;

            for (Node* node = rowNode->right; node != rowNode; node = node->right) {
                coverColumn(node->head);
            }
        }
    }
}

bool SolveSudoku(int Sudoku[][SIZE]) {
    ResetGlobals();
    BuildSparseMatrix(matrix);
    BuildLinkedList(matrix);
    TransformListToCurrentGrid(Sudoku);
    search(0);
    return isSolved;
}

static bool parsePuzzleText(const std::string& puzzleText, int grid[SIZE][SIZE]) {
    std::istringstream iss(puzzleText);
    std::string line;
    std::vector<std::string> lines;

    while (std::getline(iss, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (!line.empty()) lines.push_back(line);
    }

    if (lines.size() != SIZE) return false;

    for (int i = 0; i < SIZE; i++) {
        if (lines[i].size() != SIZE) return false;
        for (int j = 0; j < SIZE; j++) {
            if (lines[i][j] < '0' || lines[i][j] > '9') return false;
            grid[i][j] = lines[i][j] - '0';
        }
    }

    return true;
}

static std::string gridToText(int grid[SIZE][SIZE]) {
    std::ostringstream out;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            out << grid[i][j];
        }
        if (i + 1 < SIZE) out << '\n';
    }
    return out.str();
}

std::string solveSudokuText(const std::string& puzzleText) {
    int grid[SIZE][SIZE];

    if (!parsePuzzleText(puzzleText, grid)) {
        return "ERROR: Invalid input";
    }

    if (!SolveSudoku(grid)) {
        return "ERROR: No solution";
    }

    return gridToText(finalGrid);
}