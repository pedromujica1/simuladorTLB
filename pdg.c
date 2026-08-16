//verifica TAMANHO DA LISTA TABU COM O SIMON

//Atividade Avaliativa 02 - Problema da Designação Generalizada
//Alunos: Pedro Miotto Mujica e Ithony Elvis de Lima Avila
//Arquivo: pdg.c
//gcc pdg.c -o pdg; ./pdg PDG1.txt
//Objetivo: Implementar um algoritmo utilizando métodos de otimização para solucionar o problema de Designação Generalizada
//Técnica de otimização utilizada: Busca Tabu

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

typedef struct designacao
{
    int programador;
    int modulo;
    int custo;
    int ch_desenvolvimento;
    
} designacao;

typedef struct programadorCLT
{
    int programador;
    int ch;
} programadorCLT;

static int comparacaodeValores(const void *ponto1,const void *ponto2){
    
    const programadorCLT *a = ((const programadorCLT *)ponto1);
    const programadorCLT *b = ((const programadorCLT *)ponto2);

    return a->ch - b->ch;
}


void imprimir_matriz_int(int linhas, int colunas, int mat[linhas][colunas], const char *nome) {
    printf("\n%s:\n", nome);
    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            printf("%4d ", mat[i][j]);
        }
        printf("\n");
    }
}

void imprimir_transposta(int n_modulos,int n_programadores,programadorCLT transposta[n_modulos][n_programadores]) {
    printf("\nMatriz Transposta Ordenada (CH por módulo):\n");
    for (int m = 0; m < n_modulos; m++) {
        printf("Modulo %d: ", m);
        for (int p = 0; p < n_programadores; p++) {
            printf("[P %02d | CH = %02d] ",
                transposta[m][p].programador,
                transposta[m][p].ch
            );
        }
        printf("\n");
    }
}

void imprimir_designacao(int n_modulos, designacao solucao[]) {
    int custoTotal=0;
    
    for (int m = 0; m < n_modulos; m++) {
        custoTotal += solucao[m].custo;
        printf("Modulo %02d -> Programador %02d | CH = %02d | Custo = %02d\n",
            solucao[m].modulo,
            solucao[m].programador,
            solucao[m].ch_desenvolvimento,
            solucao[m].custo
        );
    }
    printf(" \nCusto total: %d\n",custoTotal);
}

int calcular_custo(int n_modulos, designacao solucao[]){
    int custoTotal=0;
    for (int m = 0; m < n_modulos; m++) {
        custoTotal += solucao[m].custo;
    }
    return custoTotal;
}

void transposta_ordenada(int n_programadores, int n_modulos, int ch_original[n_programadores][n_modulos], programadorCLT transposta[n_modulos][n_programadores]  ){

    //salva no struct transposta
    for (int p = 0; p< n_programadores; p++){
        for (int m = 0; m < n_modulos; m++)
        {
            transposta[m][p].programador = p;
            //inversao dos indices da matriz
            transposta[m][p].ch = ch_original[p][m];
        }
    }
    //ordena com base na carga horaria
    for (int m = 0; m <n_modulos; m++){
        qsort(transposta[m],n_programadores,sizeof(programadorCLT),comparacaodeValores);
    }
   
}

void solucao_inicial(int n_programadores,int n_modulos,int ch_temp[n_programadores],designacao nova_desig[n_modulos],int custo_modulos[n_programadores][n_modulos],int carga_horaria[n_programadores],designacao designacao_inicial[], programadorCLT ch_transposta[n_modulos][n_programadores]){

    for (int i = 0; i < n_programadores; i++)
    {
        ch_temp[i] = carga_horaria[i];
    }
    
    //designando o primeiro caso
    for (int m = 0; m < n_modulos; m++){
        for (int p = 0; p < n_programadores; p++) {
            int prog_atual = ch_transposta[m][p].programador;
            int ch_atual = ch_transposta[m][p].ch;

            if (ch_atual <= ch_temp[prog_atual]){
                designacao_inicial[m].modulo = m;
                designacao_inicial[m].programador = prog_atual;
                designacao_inicial[m].custo = custo_modulos[prog_atual][m];
                designacao_inicial[m].ch_desenvolvimento = ch_atual;

                ch_temp[prog_atual] -= ch_atual;
                break;
            }
        }  
    }
    //faz uma copia de designacao inicial
    for (int i = 0; i < n_modulos; i++){
        nova_desig[i]= designacao_inicial[i];
    }
}

void calcular_uso(int n_programadores,int n_modulos,designacao nova_desig[n_modulos], int ch_usada[n_programadores]){
    for (int i = 0; i < n_programadores; i++){
        ch_usada[i] = 0;
    }
    
    for (int m = 0; m < n_modulos; m++) {
        int p = nova_desig[m].programador;
        ch_usada[p] += nova_desig[m].ch_desenvolvimento;
    }
}

void inicializa_tabu(designacao listaTabu[], int tam_tabu){
    for (int i = 0; i < tam_tabu; i++) {
        listaTabu[i].modulo = -1;
        listaTabu[i].programador = -1;
    }
}


void gerar_vizinhos(int n_programadores,int n_modulos, int *modulo_alterado,designacao solucao[], designacao solucao_vizinha[],int ch_usada[],int carga_horaria[n_programadores],int ch_modulos[n_programadores][n_modulos],int custo_modulos[n_programadores][n_modulos]){
    
    int num_vizinhos = n_modulos*2;
    bool procurando_vizinhos = true;
    int tentativas = 0;

    for (int i = 0; i < n_modulos; i++){
        solucao_vizinha[i] = solucao[i];
    }

    while(procurando_vizinhos && tentativas < num_vizinhos){
        int modulo_sorteado = rand() %n_modulos;
        int programador_sorteado = solucao[modulo_sorteado].programador;
        int novo_progamador = -1;
        *modulo_alterado = -1;

        for (int p = 0; p < n_programadores; p++)
        {
            
            
            int ch_modulo_novo = ch_modulos[p][modulo_sorteado];
            int nova_ch = ch_usada[p] + ch_modulo_novo;

            
            if (p != programador_sorteado && (nova_ch <= carga_horaria[p])){
                *modulo_alterado = modulo_sorteado;
                novo_progamador = p;
                break;
            }
        }
        
           
        if (novo_progamador != -1){
            designacao vizinho;
            vizinho.programador = novo_progamador;
            vizinho.modulo = modulo_sorteado;
            vizinho.ch_desenvolvimento = ch_modulos[novo_progamador][modulo_sorteado];
            vizinho.custo = custo_modulos[novo_progamador][modulo_sorteado];

            solucao_vizinha[modulo_sorteado] = vizinho;
            procurando_vizinhos = false;
                    
        }
        tentativas++;
    }

}

bool vizinho_tabu(int indice_modulo,designacao solucao_vizinha[], designacao listaTabu[], int tabu_size){

    if (indice_modulo < 0)
        return false;
    
    designacao candidato = solucao_vizinha[indice_modulo];
    bool na_tabu = false;
    for (int t = 0; t < tabu_size; t++){
        if (candidato.modulo == listaTabu[t].modulo && candidato.programador == listaTabu[t].programador){
            na_tabu = true;
            break;
        }
    }
    return na_tabu;
}

void busca_tabu(int n_programadores,int n_modulos,designacao nova_desig[],int ch_usada[],int carga_horaria[n_programadores],int ch_modulos[n_programadores][n_modulos],int custo_modulos[n_programadores][n_modulos],int *custo_atual, int max_iteracoes, int num_vizinhos, designacao listaTabu[], int tam_tabu, int *indice_tabu, bool *retornou_vizinho){
    *retornou_vizinho = false;

    for (int i = 0; i < max_iteracoes; i++)
    {
        bool encontrou_vizinho = false;
        int menor_custo = *custo_atual;
        designacao solucao_vizinha[n_modulos];
        designacao melhor_vizinho[n_modulos];
        int melhor_modulo = -1;
        
        for (int v = 0; v < num_vizinhos; v++){

            gerar_vizinhos(n_programadores,n_modulos,&melhor_modulo,nova_desig,solucao_vizinha,ch_usada,carga_horaria,ch_modulos,custo_modulos);

            if (melhor_modulo < 0 || melhor_modulo >= n_modulos){
                continue;
            }

            bool movimento_tabu = vizinho_tabu(melhor_modulo,solucao_vizinha,listaTabu,tam_tabu);
            
            
            int custo_vizinho = calcular_custo(n_modulos,solucao_vizinha);

            if (movimento_tabu && (custo_vizinho >= menor_custo)){
                continue; //não aceita vizinho tabu dado custo_vizinho > menor_custo
            }

            if (custo_vizinho < menor_custo)
            {
                menor_custo = custo_vizinho;
                for (int m = 0; m < n_modulos; m++) {
                    melhor_vizinho[m] = solucao_vizinha[m];
                }
                encontrou_vizinho = true;
                *retornou_vizinho = true;
            }
        }
        if (!encontrou_vizinho)
        {
            continue;
        }

        if (melhor_modulo < 0){
            continue;
        }
        
        
        listaTabu[*indice_tabu].programador = nova_desig[melhor_modulo].programador;
        listaTabu[*indice_tabu].modulo = melhor_modulo;

        *indice_tabu = (*indice_tabu + 1) % tam_tabu;


        int p_antigo = nova_desig[0].programador;
        int p_novo   = melhor_vizinho[0].programador;

        ch_usada[p_antigo] -= nova_desig[0].ch_desenvolvimento;
        ch_usada[p_novo]   += melhor_vizinho[0].ch_desenvolvimento;
        //atualiza nova_desig e custo atual
        *custo_atual = menor_custo;
        for (int m = 0; m < n_modulos; m++) {
            nova_desig[m] = melhor_vizinho[m];
        } 
    }
}


int main(){

    //parte1: leitura do arquivo e verificação da entrada
    //printf("\n---------- HELLO DESIGNAÇÕES!! ----------\n");
    
    int n_programadores = 4; 
    int n_modulos = 8;
    
    //matriz de custos (4 programadores x 8 módulos)
    int custo_modulos[4][8] = {
        {7,  7,  10,  8, 16, 16,  0, 17},
        {10, 5,   9,  9, 14,  4, 16, 11},
        {11, 8,   7,  5,  1, 11, 20, 12},
        {5,  7,   6,  8, 16,  7, 15, 17}
    };
    
    //matriz de carga horária dos módulos (4 programadores x 8 módulos)
    int ch_modulos[4][8] = {
        {10, 14, 16, 12,  8, 20, 10, 16},
        {13, 12, 15, 13,  9, 18,  9, 18},
        { 8,  8, 13,  8,  8, 17, 16, 14},
        {15, 17, 18, 15, 12, 15, 16, 18}
    };
    
    //carga horária máxima disponível para cada um dos 4 programadores
    int carga_horaria[4] = {30, 25, 20, 40};
    
    //parte2: geração da solução inicial
    designacao designacao_inicial[n_modulos];
    programadorCLT ch_transposta[n_modulos][n_programadores];

    //ordena a matriz de carga horaria
    transposta_ordenada(n_programadores,n_modulos,ch_modulos,ch_transposta);

    //salva carga horaria temporaria que está sobrando para cada programador
    int ch_temp[n_programadores];
    designacao nova_desig[n_modulos];
    solucao_inicial(n_programadores,n_modulos,ch_temp,nova_desig,custo_modulos,carga_horaria,designacao_inicial,ch_transposta);
    //imprimir_designacao(n_modulos, designacao_inicial);
 
    //parte3: calcula custo inicial da solução e faz vetor para verifcar quanto cada programador já usou de carga horaria
    int custo_inical = calcular_custo(n_modulos,nova_desig);
    int *custo_atual = &custo_inical;
    int ch_usada[n_programadores];
    
    calcular_uso(n_programadores,n_modulos,nova_desig,ch_usada);
    //parte4: inicializar lista tabu e parâmetros da busca tabu
    srand(time(NULL));
    int max_iteracoes = 50;
    int num_vizinhos = n_modulos*2;
    int tam_tabu = n_modulos*5;
    int indice_tabu = 0;
    designacao listaTabu[tam_tabu];
    inicializa_tabu(listaTabu, tam_tabu);
    bool retornou_vizinho = false;
    //parte 5: execução da busca tabu com geração de vizinhos
    busca_tabu(n_programadores,n_modulos,nova_desig,ch_usada,carga_horaria,ch_modulos,custo_modulos,custo_atual,max_iteracoes,num_vizinhos,listaTabu,tam_tabu,&indice_tabu,&retornou_vizinho);
    
    //imprime designção final
    //printf("\n---- Solução Final Após Busca Tabu ----\n");
    //imprimir_designacao(n_modulos, nova_desig);
    return 0;
}