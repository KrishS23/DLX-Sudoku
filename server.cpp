#include "httplib.h"
#include "DLXSudokuSolver.hpp"
#include <iostream>

int main() {
    httplib::Server server;

    server.set_default_headers({
        {"Access-Control-Allow-Origin", "*"},
        {"Access-Control-Allow-Headers", "Content-Type"},
        {"Access-Control-Allow-Methods", "POST, OPTIONS"}
    });

    server.Options("/solve", [](const httplib::Request&, httplib::Response& res) {
        res.status = 204;
    });

    server.Post("/solve", [](const httplib::Request& req, httplib::Response& res) {
        std::string result = solveSudokuText(req.body);

        if (result.rfind("ERROR:", 0) == 0) {
            res.status = 400;
            res.set_content(result, "text/plain");
            return;
        }

        res.set_content(result, "text/plain");
    });

    std::cout << "Sudoku backend running on http://localhost:8080\n";
    server.listen("0.0.0.0", 8080);
}