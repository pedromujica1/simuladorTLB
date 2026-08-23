#include "SimulatedAnnealing.h"

using namespace std;

// q (numero de componentes)
// m (quantidade total de conexões)
// v (máximo de conexões por componente)

SimulatedAnnealing::SimulatedAnnealing(int q, int m, int n, const vector<double>& xs, const vector<double>& ys)
    : q(q), m(m), n(n), T_min(1e-4), gen(std::chrono::steady_clock::now().time_since_epoch().count()) {

    componentes.resize(q);
    conexao.resize(m);

    for(int i = 0; i < q; i++) {
        componentes[i].x = xs[i];
        componentes[i].y = ys[i];
    }
}


// calcular temperatura inicial
void SimulatedAnnealing::calcT0() { 
    double acc = 0.0;
    int samples = 10;

    for(int i = 0; i < samples; i++) { // geração de vizinhos
        vizinhancaTemp();
        acc += abs(E_next - E);
    }

    double deltaMedio = acc / samples;
    T = deltaMedio / (-log(0.9)); // 80% de aceitação

    E_next = 0;
}

// função de resfriamento exponencial com alfa == 0.96
void SimulatedAnnealing::resfriamento() {
    T *= 0.96; // não ironicamente essa foi a função que mais demorei pra pensar em como fazer
    if(T < T_min) T = T_min;
}

// distância euclidiana entre dois pontos
double SimulatedAnnealing::funcCusto(double xi, double yi, double xj, double yj) {
    return sqrt(pow((xj - xi),2) + pow((yj - yi),2));
}

// gera solução inicial
void SimulatedAnnealing::genConnection() {
    uniform_int_distribution<int> dist(0, q - 1); // gera número de 0 a q-1

    // limpa por garantia
    conexao.clear();
    for(auto& c : componentes) c.vizinho.clear();

    for(int i = 0; i < q; i++) { // garante que cada componente tem pelo menos uma conexão

        int random;

        do {
            random = dist(gen);
        } while(random == i || existeConexao(i, random) || componentes[random].vizinho.size() >= n);
        
        componentes[i].vizinho.push_back(random);
        componentes[random].vizinho.push_back(i);
        conexao.push_back({i, random});
    }

    int conexoesAtuais = q;
    while(conexoesAtuais < m) {
        int i, j;
        do {
            i = dist(gen);
            j = dist(gen);
        } while (!isValida(i, j));

        componentes[i].vizinho.push_back(j);
        componentes[j].vizinho.push_back(i);
        conexao.push_back({i, j});

        conexoesAtuais++;
    }

    best.E_best = E = calcCusto();
    best.conexao_best = conexao;
    itSemMelhora = 0;
}

// faz o cálculo do custo das conexões atuais
double SimulatedAnnealing::calcCusto() {
    double cusTotal = 0.0;

    for(size_t i = 0; i < conexao.size(); i++) {
        int a = conexao[i].first;
        int b = conexao[i].second;
        
        cusTotal += funcCusto(componentes[a].x, componentes[a].y, componentes[b].x, componentes[b].y);
    }

    return cusTotal;
}

// gera nova possível solução
void SimulatedAnnealing::vizinhanca() {
    uniform_int_distribution<int> dist(0, conexao.size() - 1); // sorteia uma conexão dentre as m formadas
    int idx;
    int i; 
    int j; 

    do {
        idx = dist(gen);
        i = conexao[idx].first;
        j = conexao[idx].second;
    } while(componentes[i].vizinho.size() <= 1 || componentes[j].vizinho.size() <= 1);

    conexao.erase(conexao.begin() + idx);
    removeVizinho(i, j);
    removeVizinho(j, i);

    uniform_int_distribution<int> distComp(0, q - 1); // sorteia componentes para formar uma nova conexão
    int a, b;

    do {
        a = distComp(gen);
        b = distComp(gen);
    } while (!isValida(a, b) || (a == i && b == j) || (a == j && b == i)); // garante que siga os parâmetros e seja uma conexão diferente


    componentes[a].vizinho.push_back(b);
    componentes[b].vizinho.push_back(a);
    conexao.push_back({a, b});

    E_next = calcCusto(); // faz o recálculo do custo total com a nova conexão
    if(isAceita()) {
        E = E_next;
        if(E < best.E_best) {
            best.E_best = E;
            best.conexao_best = conexao;
            itSemMelhora = 0;
        } else itSemMelhora++;
    }
    else { // se não aceita a nova solução, volta para como estava antes
        removeVizinho(a, b);
        removeVizinho(b, a);
        conexao.pop_back();

        componentes[i].vizinho.push_back(j);
        componentes[j].vizinho.push_back(i);
        conexao.push_back({i, j});

        itSemMelhora++;
    }
    
}

// gerar solução temporária para calcular Temperatura inicial
void SimulatedAnnealing::vizinhancaTemp() {
    uniform_int_distribution<int> dist(0, conexao.size() - 1); // sorteia uma conexão dentre as m formadas
    int idx;
    int i; 
    int j; 

    do {
        idx = dist(gen);
        i = conexao[idx].first;
        j = conexao[idx].second;
    } while(componentes[i].vizinho.size() <= 1 || componentes[j].vizinho.size() <= 1);

    conexao.erase(conexao.begin() + idx);
    removeVizinho(i, j);
    removeVizinho(j, i);

    uniform_int_distribution<int> distComp(0, q - 1); // sorteia componentes para formar uma nova conexão
    int a, b;

    do {
        a = distComp(gen);
        b = distComp(gen);
    } while (!isValida(a, b) || (a == i && b == j) || (a == j && b == i)); // garante que siga os parâmetros e seja uma conexão diferente


    componentes[a].vizinho.push_back(b);
    componentes[b].vizinho.push_back(a);
    conexao.push_back({a, b});

    E_next = calcCusto(); // faz o recálculo do custo total com a nova conexão

    // Volta pra solução inicial a fim de gerar novos vizinhos
    removeVizinho(a, b);
    removeVizinho(b, a);
    conexao.pop_back();

    componentes[i].vizinho.push_back(j);
    componentes[j].vizinho.push_back(i);
    conexao.push_back({i, j});
}

// verifica se a é vizinho de b 
bool SimulatedAnnealing::existeConexao(int a, int b) {
    const auto& vizinhos = componentes[a].vizinho;
    return find(vizinhos.begin(), vizinhos.end(), b) != vizinhos.end(); // verifica se b é um dos vizinhos
}

// verifica se uma conexão pode ser feita entre componente a e componente b
bool SimulatedAnnealing::isValida(int a, int b) {
    return a != b && !existeConexao(a, b) && componentes[a].vizinho.size() < n && componentes[b].vizinho.size() < n;
}

// verifica se a nova solução é aceita
bool SimulatedAnnealing::isAceita() {
    uniform_real_distribution<double> dist(0.0, 1.0);

    if(E_next < E) return true;
    else {
        double p = dist(gen);
        
        if(exp(-((E_next - E) / T)) >= p) return true;
    }
    return false;
}

// remove o vizinho b de um componente a
void SimulatedAnnealing::removeVizinho(int a, int b) {
    auto& v = componentes[a].vizinho;
    auto it = find(v.begin(), v.end(), b); // confere se é vizinho mesmo
    if(it != v.end()) v.erase(it);         // se for, remove
}

// execução do algoritmo propriamente dito
void SimulatedAnnealing::execute() {
    int iteracoes = 3; // quantidade de iterações que vai fazer por tempo
    int debugIt = 1;
    int max;
    if(q < T){
        max = 30; // número grande pra garantir que não fique preso em mínimo local
    } 
    else max = 31; // No último caso há grande chance de max ser maior que o maximo de iterações possiveis


    genConnection();// gera solução inicial
    calcT0();       // calcula temperatura inicial conforme a solução gerada
    cout << "Temperatura gerada: " << T << endl;


    while(T > T_min) {
        for(int it = 0; it < iteracoes; it++) {
            vizinhanca();   // explora possíveis soluções

            if(itSemMelhora >= max) break;
            debugIt++;
        }
        resfriamento();

        if(itSemMelhora >= max) {
            cerr << "Execucao encerrada por convergencia prematura na iteracao de numero " << debugIt << endl;
            break;
        }
    }
}


