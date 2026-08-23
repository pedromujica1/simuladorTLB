#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <algorithm>
#include <limits>
#include <fstream>
#include <sstream>
#include <chrono>

struct Componente {
    double x, y;
    std::vector<int> vizinho;
};

struct Best {   // custo total e conexões
    double E_best;
    std::vector<std::pair<int, int> > conexao_best;
};


class SimulatedAnnealing {
private:
    std::mt19937 gen;       // gerar numeros aleatorios via seed com tempo
public:    
    double E;       // solução atual
    double E_next;  // possível nova solução
    Best best;      // melhor solução
    double T;       // temperatura
    double T_min;   // temperatura mínima (0,0001)
    double alfa;    // taxa de resfriamento
    int itSemMelhora;   // trata convergência prematura; se não houver melhora em x iterações, encerra a execução

    int q;          // número de componentes
    int m;          // total de conexões
    int n;          // máximo de conexões por componente

    std::vector<Componente> componentes;
    std::vector<std::pair<int, int> > conexao;

    SimulatedAnnealing(int q, int m, int n, const std::vector<double>& xs, const std::vector<double>& ys); // construtor

    void calcT0();        // cálculo da temperatura inicial através da média do custo de x (metade de q ou algo do tipo) vizinhos

    void resfriamento();  // função de resfriamento exponencial

    double funcCusto(double xi, double yi, double xj, double yj);   // distância euclidiana entre pontos

    void genConnection();           // gera as conexões pra solução inicial

    double calcCusto();             // calcula o custo das conexões atuais

    void vizinhanca();              // remove uma conexão e gera uma aleatória, respeitando as restrições

    void vizinhancaTemp();          // novas soluções temporárias para calcular temperatura inicial

    bool existeConexao(int a, int b);   // verifica se um componente é vizinho de outro

    bool isValida(int a, int b);    // testa conexões conforme restrições

    bool isAceita();                // testa aceitação de nova solução -> P_E == exp(-(E_next - E) / T)

    void removeVizinho(int a, int b);

    void execute();                 // execução do sa;

    void writeFile();

};