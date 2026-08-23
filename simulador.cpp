#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdint>
using namespace std;

struct EntradaTLB {
    string pagina;
    bool valida;
};


int main(){
    string pdgTrace = "pdg.trace";
    ifstream arquivo1(pdgTrace);
    vector<string> refString;
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

        totalAcessos++;
        if (numPag == numPagFinal) {
            //acesso aconteceu em uma única página
            stringstream ss;
            ss << hex << numPag;
            //adiciona no vetor de endereços
            refString.push_back(ss.str());

            if (ss.str() != ultimaPagina) {
                totalComprimido++;
                ultimaPagina = ss.str();
            }
        }
        else {
            //acesso atravessou duas páginas
            stringstream ss1;
            stringstream ss2;
            ss1 << hex << numPag;
            ss2 << hex << numPagFinal;
          
            // página 1
            refString.push_back(ss1.str());
            if (ss1.str() != ultimaPagina) {
                totalComprimido++;
                ultimaPagina = ss1.str();
            }
            
            // página 2
            refString.push_back(ss2.str());
            if (ss2.str() != ultimaPagina) {
                totalComprimido++;
                ultimaPagina = ss2.str();
            }
        }

        
    }

    arquivo1.close();
    //percorre array enderecos para retornar paginasUnicas
    for (string pagina : refString) {
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
        min(LIMITE_SAIDA, (int)refString.size());

    for (int i = 0; i < limiteReferencia; i++) {
        cout << refString[i] << " ";
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

    //refString -> reference string, 
    //paginasUnicas -> conjunto de páginas distintas encontradas

    //tamanhos de TLB(P. exemplo 4, 6, 8, 10 entradas).
    int entradaTLB = 4;
    int TLB_misses =0;
    int TLB_hits = 0;

    vector<EntradaTLB> tlb(entradaTLB);

    //para cada pagina procurar se ela já esta na TLB
    for (string pagina : refString) {
        bool encontrada = false;

        for (EntradaTLB& entrada : tlb){

            if (entrada.valida && entrada.pagina == pagina) {

                encontrada = true;
                break; 
            }

        }

        if (encontrada) {
            TLB_hits++;
            //lógica de HIT precisa ser colocada abaixo
        } else {
            TLB_misses++;
            //lógica de MISS 
        }
        

        
        
    
    // procurar pagina na TLB
    
    }


    
    cout << endl << "Quantidade de acessos original: " << totalAcessos << endl;
    cout << "Quantidade comprimida: " << totalComprimido << endl;

    double taxa = (double)totalAcessos / totalComprimido;

    cout << "Taxa de compressao: " << taxa << ":1" << endl;


    

}
