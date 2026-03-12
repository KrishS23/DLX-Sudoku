# DLX Sudoku Solver

A Sudoku solver implemented in **C++ using Knuth's Algorithm X with Dancing Links (DLX)**. The solver models Sudoku as an **exact cover problem** and efficiently finds a solution using a toroidal doubly linked list. A simple web interface is included to interact with the C++ backend.

## Running the Project

Compile the backend:

g++ server.cpp DLXSudokuSolver.cpp -O2 -std=c++17 -o sudoku_server

Run the backend server:

./sudoku_server

Serve the frontend:

python3 -m http.server 5500

Open in browser:

http://localhost:5500

Enter a Sudoku puzzle and press **Solve** to compute the solution using the C++ DLX solver.