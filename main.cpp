// main.cpp
#include <chrono>
#include <iostream>
#include <string>
#include <functional>
#include <thread>
#include <atomic>
#include <algorithm>

#include "LoadFile.h"
#include "wordSearch.h"

using namespace std;

// ---------- I/O rápido ----------
static void setupFastIO() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
}

// ---------- ANSI no Windows ----------
#ifdef _WIN32
#include <windows.h>
static void enableVT() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;
    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return;
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}
#else
static void enableVT() {}
#endif

// ---------- Terminal helpers ----------
namespace term {
    constexpr const char* CLR   = "\x1b[2J";
    constexpr const char* HOME  = "\x1b[H";
    constexpr const char* HIDE  = "\x1b[?25l";
    constexpr const char* SHOW  = "\x1b[?25h";
    constexpr const char* BOLD  = "\x1b[1m";
    constexpr const char* DIM   = "\x1b[2m";
    constexpr const char* RESET = "\x1b[0m";
    constexpr const char* GREEN = "\x1b[32m";
}

static inline void clearFast() { cout << term::CLR << term::HOME; }

// ---------- Leitura de UMA tecla VÁLIDA ('0'..'6') ----------
#ifdef _WIN32
#include <conio.h>
static int readMenuKeyBlocking() {
    for (;;) {
        int ch = _getch();
        // teclas especiais do Windows (setas/F-keys) chegam como 0 ou 224 + próximo byte
        if (ch == 0 || ch == 224) { (void)_getch(); continue; }
        if (ch >= '0' && ch <= '6') return ch;
        this_thread::sleep_for(chrono::milliseconds(10));
    }
}
static void readAnyKey() {
    int ch = _getch();
    if (ch == 0 || ch == 224) (void)_getch(); // consome o segundo byte das especiais
}
#else
#include <termios.h>
#include <unistd.h>
static int readMenuKeyBlocking() {
    termios oldt{}, newt{};
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt; newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    int ret = '0';
    for (;;) {
        int ch = getchar();
        if (ch >= '0' && ch <= '6') { ret = ch; break; }
        this_thread::sleep_for(chrono::milliseconds(10));
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ret;
}
static void readAnyKey() {
    termios oldt{}, newt{};
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt; newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    (void)getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}
#endif

// ---------- Toast simples ----------
static void toast(const string& msg, int ms = 900) {
    using namespace std::chrono;
    cout << "\n\n" << term::BOLD << msg << term::RESET << flush;
    this_thread::sleep_for(milliseconds(ms));
}

// ---------- Render do menu ----------
static string renderMenu(double elapsedSeconds) {
    string s;
    s.reserve(1200);
    s += term::HIDE;
    s += term::CLR; s += term::HOME;

    s += "\n=====================================\n";
    s += "   MENU PRINCIPAL - CACA PALAVRAS\n";
    s += "=====================================\n";
    s += "Tempo de busca: " + to_string(elapsedSeconds) + " s\n\n";
    s += "1: Ver diagrama (entrada)\n";
    s += "2: Ver lista de palavras-alvo\n";
    s += "3: Ver resumo do arquivo de entrada\n";
    s += "4: Ver coordenadas destacadas\n";
    s += "5: Ver relatorio da busca (encontradas e ausentes)\n";
    s += "6: Exportar resultado para output.txt\n";
    s += "0: Sair\n";
    s += "-------------------------------------\n";
    s += "Escolha [0-6]: ";
    return s;
}

static void header(const string& titulo) {
    cout << "\n" << term::BOLD
         << "=====================================\n   "
         << titulo
         << "\n=====================================\n"
         << term::RESET;
}

static void pressAnyToMenu() {
    cout << "\n\n" << term::DIM << "[Qualquer tecla volta ao menu]" << term::RESET << flush;
    readAnyKey();
}

int main() {
    setupFastIO();
    enableVT();

    // Carregar o arquivo automaticamente
    LoadFile file("D:/Projetos CLion/enunciado/cacapalavras.txt");
    if (!file.readFile()) {
        cerr << "Erro: nao foi possivel carregar o arquivo." << endl;
        cout << term::SHOW;
        return 1;
    }

    // Construção do buscador + medição
    auto start = chrono::high_resolution_clock::now();
    wordSearch searcher(
        file.getDiagramLines(),
        file.getDiagramColumns(),
        file.getDiagram(),
        file.getWords()
    );
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;
    double elapsed = duration.count();

    bool rodando = true;
    while (rodando) {
        clearFast();
        cout << renderMenu(elapsed) << flush;

        int ch = readMenuKeyBlocking(); // só retorna com '0'..'6'
        switch (ch) {
            case '1': {
                clearFast();
                header("DIAGRAMA (ENTRADA)");
                file.showDiagram();
                pressAnyToMenu();
                break;
            }
            case '2': {
                clearFast();
                header("PALAVRAS-ALVO");
                file.showWords();
                pressAnyToMenu();
                break;
            }
            case '3': {
                clearFast();
                header("RESUMO DO ARQUIVO DE ENTRADA");
                file.showSummary();
                pressAnyToMenu();
                break;
            }
            case '4': {
                clearFast();
                header("COORDENADAS DESTACADAS");
                searcher.showFound();
                pressAnyToMenu();
                break;
            }
            case '5': {
                clearFast();
                header("RELATORIO DA BUSCA");
                searcher.showList();
                pressAnyToMenu();
                break;
            }
            case '6': {
                clearFast();
                header("EXPORTAR RESULTADO");
                searcher.output();
                toast(string(term::GREEN) + "✔ Arquivo gravado: output.txt" + term::RESET);
                break;
            }
            case '0': {
                rodando = false;
                break;
            }
            default:
                // não chega aqui, porque readMenuKeyBlocking só retorna 0..6
                break;
        }
    }

    cout << term::SHOW; // garante cursor visível ao sair
    return 0;
}
