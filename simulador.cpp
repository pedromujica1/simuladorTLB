#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdint>
using namespace std;


int main(){
    string pdgTrace = "pdg.trace";
    ifstream arquivo1(pdgTrace);
    vector<string> enderecos;
    vector<string> paginasUnicas;

    if (!arquivo1.is_open()){
        cerr << "Error ao abrir arquivo!" << endl;
        return 1;
    }

    string linha;

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
            enderecos.push_back(ss.str());
        }
        else {
            //acesso atravessou duas páginas
            stringstream ss1;
            stringstream ss2;
            ss1 << hex << numPag;
            ss2 << hex << numPagFinal;
          
            //adiciona no vetor de endereços
            enderecos.push_back(ss1.str());
            enderecos.push_back(ss2.str());
        }

        
    }

    arquivo1.close();
    //percorre array enderecos para retornar paginasUnicas
    for (string pagina : enderecos) {
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
        min(LIMITE_SAIDA, (int)enderecos.size());

    for (int i = 0; i < limiteReferencia; i++) {
        cout << enderecos[i] << " ";
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
    



    

}
