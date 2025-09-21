#include "LoadFile.h"
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

LoadFile::LoadFile(const std::string &fileName) : fileName(fileName) {}

bool LoadFile::readFile() {
    ifstream file(fileName);

    if (!file.is_open()) {
        cerr << "Erro: não foi possível abrir o arquivo " << fileName << endl;
        return false;
    }

    diagram.clear();
    words.clear();

    string firstLine;
    if (getline(file, firstLine)) {
        stringstream ss(firstLine);
        if (!(ss >> diagramLines >> diagramColumns)) {
            cerr << "Erro: cabeçalho inválido no arquivo " << fileName << endl;
            return false;
        }

        string line;
        int lineCounter = 0;
        while (getline(file, line)) {
            // Remove '\r' em caso de arquivos Windows
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }

            if (!line.empty()) {
                if (lineCounter < diagramLines) {
                    diagram.push_back(line);
                } else {
                    words.push_back(line);
                }
                lineCounter++;
            }
        }

    } else {
        cerr << "Erro: não foi possível ler a primeira linha (dimensões)." << endl;
        return false;
    }

    file.close();
    return true;
}

const vector<string>& LoadFile::getDiagram() const {
    return diagram;
}

const vector<string>& LoadFile::getWords() const {
    return words;
}

int LoadFile::getDiagramLines() const {
    return diagramLines;
}

int LoadFile::getDiagramColumns() const {
    return diagramColumns;
}

// 🔹 Métodos de exibição
void LoadFile::showSummary() const {
    cout << "\n===== RESUMO =====\n";
    cout << "Linhas do diagrama: " << diagramLines << endl;
    cout << "Colunas do diagrama: " << diagramColumns << endl;
    cout << "Quantidade de palavras: " << words.size() << endl;
    cout << "==================\n";
}

void LoadFile::showDiagram() const {
    cout << "\n===== DIAGRAMA =====\n";
    for (const auto &linha : diagram) {
        cout << linha << endl;
    }
    cout << "====================\n";
}

void LoadFile::showWords() const {
    cout << "\n===== PALAVRAS A ENCONTRAR =====\n";
    for (const auto &palavra : words) {
        cout << "- " << palavra << endl;
    }
    cout << "================================\n";
}
