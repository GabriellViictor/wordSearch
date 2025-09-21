#ifndef LOADFILE_H
#define LOADFILE_H

#include <string>
#include <vector>

class LoadFile {
private:
    std::string fileName;
    int diagramLines{};
    int diagramColumns{};
    std::vector<std::string> diagram;
    std::vector<std::string> words;

public:
    explicit LoadFile(const std::string &fileName);

    bool readFile();

    // Métodos de acesso
    const std::vector<std::string>& getDiagram() const;
    const std::vector<std::string>& getWords() const;
    int getDiagramLines() const;
    int getDiagramColumns() const;

    // Métodos de exibição
    void showSummary() const;
    void showDiagram() const;
    void showWords() const;
};

#endif // LOADFILE_H
