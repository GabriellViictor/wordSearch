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
    // define a quantidade de threads
    ThreadPool pool(2);

    // o processo principal vasculha cada letra e abre uma thread quando necessario verificar se alguma palavra é formada
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            for (const string& word : this->words) {
                if (grid[i][j] != word[0]) {
                    continue;
                }
                pool.enqueue([this,i,j,word] {
                    searchAllDir(i, j, word);
                    //cout<< this_thread::get_id() << endl;
                });
                //searchAllDir(i, j, word);
            }
        }
    }
}

// função para procurar em uma direção, sendo dx e dy responsaveis por determinar a direção
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
    // salva em checklist que a palavra foi encontrada e no vetor wordlist as coordenadas iniciais e a direção da palavra
    // as coordenadas de letras que compõem palavras sao salvas em found
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
//mostra o vetor que contem todas as coordenadas de letras que compõem uma palavra
void wordSearch::showFound() const {
    cout << "\n===== Palavras xdd =====\n";
    for (const auto &linha : found) {
        cout << "(" + to_string(linha.x + 1) + "," + to_string(linha.y + 1) + ")" << endl;
    }
    cout << "====================\n";
}
 //mostra a lista com a coordenada inicial e a direção em que a palavra foi encontrada, e uma mensagem especial caso a palavra nao tenha sido encontrada
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
// função que gera arquivo de saida
void wordSearch::output() const {
    ofstream fout;
    fout.open("D:/Projetos CLion/enunciado/output.txt");
    if (!fout.is_open()) {
        std::cerr << "Erro ao criar arquivo!" << std::endl;
    }else {
        //coloca numero de linhas e colunas no arquivo
        fout << to_string(this->rows) + " " + to_string(this->cols) << endl;
        // escreve o diagrama de novo, se determinada coordenada estiver no vetor found, escreve em maiúsculo
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
        // escreve as palavras que foram encotradas
        for (const auto &linha : wordlist) {
            fout << linha << endl;
        }
        // escreve as palavras que nao foram encontradas
        for (const auto& word : checklist) {
            if (!word.second) {
                fout << word.first + ": nao encontrado" << endl;
            }
        }
        // finaliza a gravação
        fout.close();
        cout << "Arquivo Gravado com Sucesso." << endl;
    }
}
// função que busca em todas as direções
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