#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <limits>
#include "LoadFile.h"
#include "wordSearch.h"

using namespace std;

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void header(const string &titulo) {
    cout << "\n=====================================\n";
    cout << "   " << titulo << "\n";
    cout << "=====================================\n";
}

int main() {
    const string HIGHLIGHT_ON = "\033[1;32m";
    const string HIGHLIGHT_OFF = "\033[0m";

    // Carregar o arquivo automaticamente
    LoadFile file("D:/Projetos CLion/enunciado/exemplo.txt");
    if (!file.readFile()) {
        cerr << "Erro: nao foi possivel carregar o arquivo." << endl;
        return 1;
    }

    wordSearch searcher(file.getDiagramLines(), file.getDiagramColumns(), file.getDiagram(),file.getWords());
    // Loop principal do menu
    bool rodando = true;
    while (rodando) {
        clearScreen();
        header("MENU PRINCIPAL - CAÇA PALAVRAS");

        cout << "1: Ver diagrama original\n";
        cout << "2: Ver palavras a encontrar\n";
        cout << "3: Ver resumo\n";
        cout << "0: Sair\n";
        cout << "-------------------------------------\n";

        string entrada;
        int opcao = -1;

        cout << "Escolha: ";
        getline(cin, entrada);

        if (!entrada.empty()) {
            try {
                opcao = stoi(entrada); // converte entrada para número
            } catch (...) {
                opcao = -1; // se não for número, força inválido
            }
        }

        switch (opcao) {
            case 1:
                file.showDiagram();
                break;
            case 2:
                file.showWords();
                break;
            case 3:
                file.showSummary();
                break;
            case 4:
                searcher.showFound();
                break;
            case 0:
                cout << "Encerrando o programa..." << endl;
                rodando = false;
                break;
            default:
                cout << "Opcao invalida!" << endl;
        }

        if (rodando && opcao != 0) {
            cout << "\n-------------------------------------\n";
            cout << "Pressione ENTER para voltar ao menu...";
            string dummy;
            getline(cin, dummy);
            clearScreen();
        }
    }

    return 0;
}
