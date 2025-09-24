# Pesca Palavras — README

## 1) Objetivo do projeto
Implementar um caça-palavras que:
- lê um arquivo de entrada (dimensões, diagrama e lista de palavras),
- busca cada palavra em **8 direções** (horizontal, vertical e diagonais, inclusive reverso),
- gera um **relatório** das palavras encontradas (posição + direção) e
- exporta um diagrama resolvido com as letras das palavras em **maiúsculo**.

> Decisão: mantivemos a compatibilidade com o formato de exemplo do enunciado.

---

## 2) Como executar

### Compilar (g++)
```bash
g++ -std=c++17 -O3 -pthread -o pesca_palavras \
  main.cpp LoadFile.cpp wordSearch.cpp
```

### Rodar
```bash
# caminho por argumento (recomendado)
./pesca_palavras ./cacapalavras.txt
# se não passar, o main usa "cacapalavras.txt" no diretório atual
```

A opção do menu **“Exportar”** grava `./output.txt`.

---

## 3) Formato do arquivo de entrada

```
<Linhas> <Colunas>
<linha 1 do diagrama>
...
<linha N do diagrama>
<palavra 1>
<palavra 2>
...
```

> Decisão: o `LoadFile` valida que **cada linha do diagrama** tem exatamente `<Colunas>` caracteres. Isso evita desalinhamento em tempo de execução.

---

## 4) Arquitetura (porquê de cada parte)

### `LoadFile`
- Lê dimensões, diagrama e palavras; remove `\r` (CRLF) e ignora linhas vazias.
- **Validação por linha**: se o comprimento não bate com `<Colunas>`, falha cedo com mensagem clara.  
  **Motivo**: robustez e previsibilidade do acesso `grid[i][j]`.

### `wordSearch`
- Implementa a busca nas 8 direções:
  - `searchAllDir(i,j,word)` chama `search` com os 8 deslocamentos `(dx,dy)`.
  - `search` percorre a palavra acumulando as coordenadas; se casar, registra resultado.
- **Porquê assim**: a função fica pequena, fácil de testar e de proteger com *mutex* no trecho crítico.

### `ThreadPool` (concorrência)
- Usamos **pool de threads** (em vez de criar uma thread por célula/palavra).
- **Porquê**: criação de milhares de threads gera overhead e pode ser mais lento que a versão sequencial. O pool **reutiliza** N threads fixas (≈ núcleos da CPU).
- **Destrutor sincronizante**: o destrutor do pool dá `join()` em todos os workers, garantindo que todas as tarefas finalizem antes de o programa sair.  
  **Motivo**: simplicidade de uso — basta deixar o pool sair de escopo.

### Sincronização (*Seção Crítica*)
- `checklist`, `wordlist` e `found` são **estruturas globais** atualizadas por várias tarefas.
- Protegemos **apenas** o “merge” com `std::mutex` (via `std::lock_guard`).
- **Porquê**: evitar *data race* e manter a **seção crítica mínima**, liberando o grosso do trabalho (varredura do grid) para rodar em paralelo.

### Redução de tarefas (bucket por primeira letra)
- As palavras são agrupadas por **primeiro caractere**. Para uma célula `(i,j)`, enfileiramos tarefas **somente** para as palavras que começam com `grid[i][j]`.
- **Porquê**: diminui significativamente o número de tarefas e comparações inúteis → **menos overhead** no pool e melhor uso de cache.

---

## 5) Estratégia de paralelização (critério de SO)

- **Modelo**: Produtor (main) → enfileira **tarefas** de busca; **N workers** do pool consomem e executam.
- **Divisão do trabalho**: tarefas pequenas, mas filtradas por primeira letra; opcionalmente, pode-se usar tarefas por **blocos de linhas** para ainda menos overhead (ver “Melhorias”).
- **Sincronização**: *mutex* apenas no **merge** dos resultados (seção crítica curta).
- **Efeito prático**: o tempo é gasto majoritariamente em leitura do grid (paralela), e não brigando no lock.

---

## 6) Saída e relatórios

- **Console**
  - “Coordenadas destacadas”: lista de `(linha,coluna)` das letras encontradas.
  - “Relatório da busca”: para cada palavra encontrada → texto “palavra – (linha,coluna): direção”; lista também as **não encontradas**.
- **Arquivo** `output.txt`
  - Primeira linha: `<Linhas> <Colunas>`
  - Diagrama com letras de palavras **em maiúsculo**
  - Relatório no final (palavras e status)

> Decisão: caminho **relativo** (`./output.txt`) para funcionar em qualquer máquina/ambiente.

---

## 7) Interface (UX do console)

- **Sem flicker**: limpeza via **ANSI** (ESC `[2J` + `[H]`) em vez de `system("cls")`.
- **Entrada de 1 tecla**: leitura bloqueante apenas para `'0'..'6'` (ignora setas/F-keys), evitando *busy loop*.
- **Rótulos claros**: itens do menu nomeados pelo que **realmente mostram** (entrada x resultado).

> Porquê: melhora a experiência de uso e a demonstração na apresentação.

---

## 8) Decisões de projeto e alternativas

| Decisão | Porquê | Alternativa |
|---|---|---|
| **Thread pool fixo** | Reuso de threads; menos overhead; mais estável | Thread por célula/palavra (ruim); `std::async` (menos controle) |
| **Mutex só no merge** | Minimiza contenção e mantém paralelismo | Vários mutex (mais complexo, pouco ganho) |
| **Bucket por 1ª letra** | Menos tarefas/comparações → mais rápido | Filtrar depois (custa CPU desnecessária) |
| **Validação de largura no `LoadFile`** | Evita falhas silenciosas em runtime | “Tentar ler mesmo assim” (frágil) |
| **`output.txt` relativo** | Portabilidade na correção/apresentação | Caminho absoluto (quebra fora do seu PC) |
| **ANSI em vez de `system("cls")`** | Sem flicker, multiplataforma | `system("cls")/system("clear")` (lento, piscando) |

---

## 9) Limitações conhecidas

- A distribuição por tarefas pequenas ainda pode ser **subótima** em cenários extremos (tabuleiros gigantes com poucas palavras).  
  → Ver “Melhorias” para particionar por **linhas/blocos**.
- Não há **eliminação de duplicatas** no relatório (se a mesma palavra puder ocorrer várias vezes, todas as ocorrências aparecem — isso pode ser desejado).

---

## 10) Mapeamento para os critérios de avaliação

- **Funcionalidade**  
  Entrada/saída conforme enunciado; 8 direções; não encontradas listadas; validação e mensagens claras.
- **SO (concorrência)**  
  Pool fixo, tarefas filtradas, mutex no merge (seção crítica curta), *join* garantido no destrutor.
- **Qualidade do código**  
  Módulos (`LoadFile`, `wordSearch`, `ThreadPool`), nomes descritivos, mensagens úteis, caminho de saída relativo.
- **Apresentação**  
  Interface fluida (ANSI, tecla única), medição de tempo, rótulos claros; README explica **o porquê** de cada decisão.
- **Extras**  
  Bucketização por primeira letra; validação de entrada; toques de UX no console.

---

## 11) Estrutura dos arquivos

```
.
├── main.cpp            # UI do console, timer, orquestra a execução
├── LoadFile.h/.cpp     # Leitura e validação do arquivo de entrada
├── wordSearch.h/.cpp   # Busca nas 8 direções + relatório e exportação
├── ThreadPool.h        # Pool de threads (header-only)
└── README.md           # Este documento
```

---

## 12) Créditos / Ambiente

- C++17, compilado com `-O3 -pthread`.
- Testado em Windows 10/11 (ANSI habilitado) e Linux (xterm/VT100).
