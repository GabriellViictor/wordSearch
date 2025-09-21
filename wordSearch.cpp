//
// Created by gabri on 21/09/2025.
//

#include "wordSearch.h"

#include <iostream>
#include <ostream>

#include "LoadFile.h"
using namespace std;

wordSearch::wordSearch(int row, int col, const vector<string> &diagram, const vector<string> &words) {
    this->words = words;
    this->grid = diagram;
    this->rows = row;
    this->cols = col;

    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            for (string word : this->words) {
                search(i,j,word,1,1);
                search(i,j,word,1,0);
                search(i,j,word,1,-1);
                search(i,j,word,0,-1);
                search(i,j,word,-1,-1);
                search(i,j,word,-1,0);
                search(i,j,word,-1,1);
                search(i,j,word,0,1);
            }
        }
    }
}

void wordSearch::search(int x, int y, string word, int dx, int dy) {
    vector<string> foundwords;
    for (char i : word) {
        if (x >= cols || x < 0 || y >= rows || y < 0) {
            foundwords.clear();
            return;
        }
        if (grid[x][y] == i) {
            foundwords.emplace_back(to_string(x) + "," + to_string(y));
            x += dx;
            y += dy;
            continue;
        }
        foundwords.clear();
        return;
    }
    for (string i : foundwords) {
        found.push_back(i);
    }
}

void wordSearch::showFound() const {
    cout << "\n===== Palavras xdd =====\n";
    for (const auto &linha : found) {
        cout << linha << endl;
    }
    cout << "====================\n";
}

