//
// Created by gabri on 21/09/2025.
//

#ifndef PROJETOS_CLION_WORDSEARCH_H
#define PROJETOS_CLION_WORDSEARCH_H
#include <string>
#include <vector>
using namespace std;


class wordSearch {
   private:
    vector<string> grid;
    vector<string> words;
    int rows;
    int cols;
    vector<string> found;




public:
    explicit wordSearch(int, int, const vector<string> &diagram, const vector<string> &words);
    void search(int x, int y, string word, int dx, int dy);

    void showFound() const;
};



#endif //PROJETOS_CLION_WORDSEARCH_H