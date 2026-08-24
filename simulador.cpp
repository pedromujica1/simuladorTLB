#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdint>
#include <queue>
#include <list>
using namespace std;

struct EntradaTLB {
    string pagina;
    bool valida;
};


int main(){
    string pdgTrace = "pdg.trace";
    ifstream arquivo1(pdgTrace);
    vector<string> paginas;
    vector<string> paginasUnicas;

    if (!arquivo1.is_open()){
        cerr << "Error ao abrir arquivo!" << endl;
        return 1;
    }

    string linha;

    // variáveis usadas no cálculo da taxa de compressão
    int totalAcessos = 0;
    int totalComprimido = 0;
    string ultimaPagina = "";

    while (getline(arquivo1, linha)) {
        //pula pra proxima interacao se existem linhas vazias ou cabeçalho
        if (linha[0] == '=') continue;

        size_t pos_virgula = linha.find(',');
        //se posiçao da virgula não foi encontrado pula pra proxima iteracao
        if (pos_virgula == string::npos) continue;

        size_t pos_inicio = 0;

        //pula espaços e letras de instrução
        while (pos_inicio < linha.length() && (linha[pos_inicio] == ' ' || linha[pos_inicio] == 'I' || linha[pos_inicio] == 'S' || linha[pos_inicio] == 'L' || linha[pos_inicio] == 'M')) {
            pos_inicio++;
        }
        //tamanho do end de memoria
        string tamanho_str = linha.substr(pos_virgula + 1);
        //transforma pra int
        int tamanho = stoi(tamanho_str);

        //extrai substring com endereço
        string hex_str = linha.substr(pos_inicio, pos_virgula - pos_inicio);

        //converte hexa pra inteiro
        uint64_t endDecimal = stoull(hex_str,nullptr,16);

        //calcula endereço final do acesso
        uint64_t pos_final = endDecimal + tamanho - 1;


        //descobre numero da pagina removendo os 12 bits de desolcamento
        uint64_t numPag = endDecimal  >> 12;
        uint64_t numPagFinal = pos_final >> 12;

        
        if (numPag == numPagFinal) {
            //acesso aconteceu em uma única página
            stringstream ss;
            ss << hex << numPag;
            
            //adiciona no vetor de endereços
            paginas.push_back(ss.str());
            if (ss.str() != ultimaPagina) {
                totalComprimido++;
                ultimaPagina = ss.str();
            }
            totalAcessos++;
        }
        else {
            //acesso atravessou duas páginas
            stringstream ss1;
            stringstream ss2;
            ss1 << hex << numPag;
            ss2 << hex << numPagFinal;
          
            // página 1
            paginas.push_back(ss1.str());
            if (ss1.str() != ultimaPagina) {
                totalComprimido++;
                ultimaPagina = ss1.str();
            }
            
            // página 2
            paginas.push_back(ss2.str());
            if (ss2.str() != ultimaPagina) {
                totalComprimido++;
                ultimaPagina = ss2.str();
            }
            totalAcessos+=2;
        }

        
    }

    arquivo1.close();
    //percorre array enderecos para retornar paginasUnicas
    for (string pagina : paginas) {
        bool existe = false;

        for (string paginaExistente : paginasUnicas) {
            if (pagina == paginaExistente) {
                existe = true;
                break;
            }
        }

        if (!existe) {
            paginasUnicas.push_back(pagina);
        }
    }

    const int LIMITE_SAIDA = 50;

    cout << "Reference String (primeiras "
         << LIMITE_SAIDA << "):" << endl;

    int limiteReferencia =
        min(LIMITE_SAIDA, (int)paginas.size());

    for (int i = 0; i < limiteReferencia; i++) {
        cout << paginas[i] << " ";
    }

    cout << endl;

    cout << "\nPaginas Unicas (primeiras "
         << LIMITE_SAIDA << "):" << endl;

    int limitePaginas =
        min(LIMITE_SAIDA, (int)paginasUnicas.size());

    for (int i = 0; i < limitePaginas; i++) {
        cout << paginasUnicas[i] << " ";
    }

    cout << endl;

    //paginas -> histórico de acesso à cada página, 
    //paginasUnicas -> conjunto de páginas distintas encontradas

    //tamanhos de TLB(P. exemplo 4, 6, 8, 10 entradas).
    int entradaTLB = 4;
    int TLB_misses =0;
    int TLB_hits = 0;

    vector<EntradaTLB> tlb(entradaTLB);
    for (auto& t : tlb) {
        t.valida = false;
        t.pagina = "";
    }

    list<int> lruQueue;

    //para cada pagina procurar se ela já esta na TLB
    for (string pagina : paginas) {
        bool encontrada = false;

        for (int i = 0; i < entradaTLB; i++){

            // se deu hit, coloca a página no final da fila
            if (tlb[i].valida && tlb[i].pagina == pagina) {
                TLB_hits++;
                encontrada = true;

                lruQueue.remove(i);
                lruQueue.push_back(i);
                break; 
            }

        }

        if (!encontrada) {
            TLB_misses++;

            // se a fila estiver cheia, sacrifica o último elemento (o que está a mais tempo sem ser usado)
            if (lruQueue.size() == entradaTLB) {
                int idx = lruQueue.front();
                lruQueue.pop_front();

                tlb[idx].pagina = pagina;
                tlb[idx].valida = true;
                lruQueue.push_back(idx);
            }
            else { // senão, encontra uma posição vazia
                for (int i = 0; i < entradaTLB; i++) {
                    if (tlb[i].valida == false) {
                        tlb[i].valida = true;
                        tlb[i].pagina = pagina;
                        lruQueue.push_back(i);
                        break;
                    }
                }
            }
        }    
    }


    
    cout << endl << "Quantidade de acessos original: " << totalAcessos << endl;
    cout << "Quantidade comprimida: " << totalComprimido << endl;

    double taxa = (double)totalComprimido / totalAcessos;

    cout << "Taxa de compressao: " << taxa*100 << "%" << endl << endl;

    cout << "Hits: " << TLB_hits << endl;
    cout << "Misses: " << TLB_misses << endl;


    

}
