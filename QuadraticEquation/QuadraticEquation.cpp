#include "QuadraticEquation.h"
#include <cctype>
#include <iostream>
int main() {
    bool math = true;
    std::string line;
    while (true) {
        std::cout << (math ? "[MATH] " : "[SCI] ") << "_x^2+_x+_=0 > " << std::flush;
        if (!std::getline(std::cin, line)) {
            break;
        }
        for (char& ch : line) {
            ch = (char)std::tolower((unsigned char)ch);
        }
        auto b = line.find_first_not_of(" \t\r"), e = line.find_last_not_of(" \t\r");
        line = b == std::string::npos ? "" : line.substr(b, e - b + 1);
        if (line.empty()) {
            continue;
        }
        if (line == "exit") {
            break;
        }
        if (line == "shift") {
            math = !math;
            continue;
        }
        Rat A, B, C;
        parseEquation(line, A, B, C);
        printSolution(solve(A, B, C), math);
    }
    return 0;
}
