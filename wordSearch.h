//
// Created by gabri on 21/09/2025.
//

#ifndef PROJETOS_CLION_WORDSEARCH_H
#define PROJETOS_CLION_WORDSEARCH_H
#include <map>
#include <string>
#include <vector>

#include "Coordinates.h"
using namespace std;


class wordSearch {
   private:
    vector<string> grid;
    vector<string> words;
    int rows;
    int cols;
    vector<Coordinates> found;
    vector<string> wordlist;
    map<string,bool> checklist;



public:
    explicit wordSearch(int, int, const vector<string> &diagram, const vector<string> &words);
    void search(int x, int y, const string& word, int dx, int dy);
    void showList() const;
    void showFound() const;
    void output() const;

    void searchAllDir(int x, int y, const string &word);
};



#endif //PROJETOS_CLION_WORDSEARCH_H