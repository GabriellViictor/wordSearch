//
// Created by gabri on 21/09/2025.
//

#include "wordSearch.h"

#include <fstream>
#include <functional>
#include <iostream>
#include <ostream>
#include <thread>

#include "Coordinates.h"
#include "LoadFile.h"
#include "ThreadPool.h"
using namespace std;
static std::mutex g_ws_mtx; // protege checklist, wordlist e found


wordSearch::wordSearch(int row, int col, const vector<string> &diagram, const vector<string> &words) {
    this->words = words;
    this->grid = diagram;
    this->rows = row;
    this->cols = col;

    for (string i : words) {
        checklist[i] = false;
    }
    ThreadPool pool(2);
    //vector<thread> threads;
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            for (const string& word : this->words) {
                if (grid[i][j] != word[0]) {
                    continue;
                }
                pool.enqueue([this,i,j,word]{searchAllDir(i, j, word);});
                //threads.emplace_back(&wordSearch::searchAllDir, this,i,j,word);
                //searchAllDir(i, j, word);
            }
        }
    }
    //pool.~ThreadPool();
    //for (auto& thread : threads){
        //thread.join();
    //}
    //cout<<threads.size()<<endl;
}

void wordSearch::search(int x, int y, const string& word, const int dx, const int dy) {
    vector<Coordinates> foundwords;
    for (const char i : word) {
        if (x >= rows || x < 0 || y >= cols || y < 0) {
            foundwords.clear();
            return;
        }
        if (grid[x][y] == i) {
            foundwords.emplace_back(x,y);
            x += dx;
            y += dy;
            continue;
        }
        foundwords.clear();
        return;
    }
    checklist[word] = true;
    string lado;
    string alt;
    if (dy == 1) {
        lado = "direita";
    }else if (dy == -1) {
        lado = "esquerda";
    }
    if (dx == 1) {
        alt = "baixo";
    }else if (dx == -1) {
        alt = "cima";
    }
    if (!lado.empty() && !alt.empty()) {
        alt = "/" + alt;
    }
    {
        std::lock_guard<std::mutex> lk(g_ws_mtx);
        checklist[word] = true;
        wordlist.push_back(
            word + " - (" + to_string(foundwords[0].x + 1) + "," +
            to_string(foundwords[0].y + 1) + "): " + lado + alt
        );
        for (const Coordinates& c : foundwords) found.push_back(c);
    }
}

void wordSearch::showFound() const {
    cout << "\n===== Palavras xdd =====\n";
    for (const auto &linha : found) {
        cout << "(" + to_string(linha.x + 1) + "," + to_string(linha.y + 1) + ")" << endl;
    }
    cout << "====================\n";
}

void wordSearch::showList() const {
    cout << "\n===== Palavras xdd =====\n";
    for (const auto &linha : wordlist) {
        cout << linha << endl;
    }
    for (const auto& word : checklist) {
        if (!word.second) {
            cout << word.first + ": nao encontrado" << endl;
        }
    }
    cout << "====================\n";
}

void wordSearch::output() const {
    ofstream fout;
    fout.open("D:/Projetos CLion/enunciado/output.txt");
    if (!fout.is_open()) {
        std::cerr << "Erro ao criar arquivo!" << std::endl;
    }else {
        fout << to_string(this->rows) + " " + to_string(this->cols) << endl;
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                bool f = false;
                for (const Coordinates ij : found) {
                    if (ij.x == i && ij.y == j) {
                        f = true;
                        fout << static_cast<char>(toupper(this->grid[i][j]));
                        break;
                    }
                }
                if (!f) {
                    fout << this->grid[i][j];
                }
            }
            fout << endl;
        }
        for (const auto &linha : wordlist) {
            fout << linha << endl;
        }
        for (const auto& word : checklist) {
            if (!word.second) {
                fout << word.first + ": nao encontrado" << endl;
            }
        }
        fout.close();
        cout << "Arquivo Gravado com Sucesso." << endl;
    }
}

void wordSearch::searchAllDir(int x, int y, const string& word) {
    search(x,y,word,1,1);
    search(x,y,word,1,0);
    search(x,y,word,1,-1);
    search(x,y,word,0,-1);
    search(x,y,word,-1,-1);
    search(x,y,word,-1,0);
    search(x,y,word,-1,1);
    search(x,y,word,0,1);
}