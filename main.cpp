// Matheus Seghatti - Gabriel Costa
// compilar: g++ main.cpp SimulatedAnnealing.cpp -o output/main.exe
// executar: output\main.exe

// Critério de parada = T <= T_min (0,0001) ou convergência prematura (x iterações sem melhora)
/*  a temperatura inicial é ligeiramente aleatória pq a fórmula usada para gerá-la usa como base
    a geração (aleatória) de vizinhos de uma solução inicial (também gerada aleatoriamente)
    a fórmula utilizada foi T0 = ΔE / (-ln(X0)), tal que: 
    ΔE = estimativa da diferença média de custo para transições ruins
    X0 = taxa de aceitação desejada (nesse caso, 0.9)*/


#include "SimulatedAnnealing.h"

using namespace std;

int main() {
    /*==========================================Ler do arquivo==========================================*/
    ifstream f("Circuito1.txt");

    if (!f.is_open()) {
        cout << "Erro ao abrir o arquivo\n";
        return 1;
    }

    string line;
    getline(f, line); int q = stoi(line);   // número de componentes
    getline(f, line); int m = stoi(line);   // total de conexões
    getline(f, line); int n = stoi(line);   // máximo de conexões por componente

    // posição x
    getline(f, line);
    for(char &c: line) { // troca virgula por ponto
        if(c == ',') c = '.';
    }

    istringstream ssx(line); // "separa" os valores
    vector<double> xs;
    double value;
    while(ssx >> value) xs.push_back(value); // adiciona o valor ao vetor de xizes enquanto houver

    // posição y
    getline(f, line);
    for(char &c: line) { // troca virgula por ponto
        if(c == ',') c = '.';
    }

    istringstream ssy(line); // "separa" os valores
    vector<double> ys;
    while(ssy >> value) ys.push_back(value); // adiciona o valor ao vetor de yzes enquanto houver

    f.close();

    cerr << "q=" << q << " m= " << m << " n=" << n << endl;
    /*cout << "xs = ";
    for(auto i : xs) cout << i << ", ";
    cout << endl << "ys = ";
    for(auto i : ys) cout << i << ", ";*/
    /*==========================================Ler do arquivo==========================================*/

    SimulatedAnnealing SA(q, m, n, xs, ys);
    SA.execute();
    cout << endl << "Execucao encerrada!" << endl;
    cout << "Menor custo encontrado: " << SA.best.E_best << endl;

    return 0;
}