# simuladorTLB

Este projeto contém um simulador em C++ para análise de desempenho de uma TLB (Translation Lookaside Buffer) utilizando os algoritmos de substituição de páginas **LRU (Least Recently Used)** e **Random**.


### 1. Compilação dos Programas de Teste

Compile os códigos que serão monitorados para gerar os traces de memória:

```bash
gcc pdg.c -o pdg
g++ main.cpp SimulatedAnnealing.cpp -o circuito

```

### 2. Geração dos Traces com Valgrind (Lackey)

Utilize a ferramenta `lackey` do Valgrind para extrair o rastreamento de acessos à memória de cada aplicação:

```bash
valgrind --log-file=circuito.trace --tool=lackey --trace-mem=yes ./circuito
valgrind --log-file=pdg.trace --tool=lackey --trace-mem=yes ./pdg

```

### 3. Compilação do Simulador

Compile o código do simulador de TLB:

```bash
g++ simulador.cpp -o simulador

```

### 4. Execução da Simulação

Execute o simulador passando como argumento o arquivo de trace gerado anteriormente (por exemplo, `circuito.trace` ou `pdg.trace`):

```bash
./simulador circuito.trace

```
*(O programa solicitará interativamente o número de entradas desejado para a TLB durante a execução).*
