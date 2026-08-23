// Gabriel Costa de Moraes
//Matheus Seghatti

// executar
// make
// ./myrb < palavras5.txt


#include <iostream>
#include <string>
#include <fstream>

using namespace std;

#define black false
#define red true

struct Node
{
    string word, meaning;
    Node *left, *right, *parent;
    bool color; // true == red; false == black

    Node(string w, string m) : word(w), meaning(m), left(nullptr), right(nullptr), parent(nullptr), color(red) {}
};

class Dictionary
{
public:
    Node *root;
    Dictionary() { root = NULL; }

    void Insert(const string &w, const string &m)
    {
        Node *z = new Node(w, m);
        Node *n = Insertrb(z);

        if (n != z)
        {
            delete z;
            return;
        }

        InsertFixup(n);
        if (root)
            root->color = black;
    }

  Node *buscarPalavra(const string &w)
    {
        Node *cur = root; 
        while (cur)
        {
            if (w == cur->word) return cur;
            else if (w < cur->word) cur = cur->left;
            else cur = cur->right;
        }
        return nullptr; //  nulo se não achar
    }

    void Remove(const string &word)
    {
        Node *z = buscarPalavra(word);
        if(!z) return;

        Node *y = z;
        bool true_color = y->color;

        Node *x = nullptr;        //nó que determina o fixup necessário
        Node *x_parent = nullptr; //pra não ter q ficar testando sempre se x é nulo

        if(!z->left) { //tem 0 ou 1 filhos
            x = z->right;
            x_parent = z->parent;
            replace(z, z->right);
        }
        else if(!z->right) {
            x = z->left;
            x_parent = z->parent;
            replace(z, z->left);
        }
        else { //tem 2 filhos
            y = maxValor(z->left); //máximo à esquerda
            true_color = y->color;
            x = y->left;

            if(y->parent == z) { //é filho direto
                x_parent = y;
                if(x) x->parent = y;
            }
            else {
                x_parent = y->parent;
                replace(y, y->left);
                y->left = z->left;
                if(y->left) y->left->parent = y;
            }
            replace(z, y);
            y->right = z->right;
            if(y->right) y->right->parent = y;
            y->color = z->color;
        }

        delete z;

        if(true_color == black) removeFixUp(x, x_parent);
        
    }

    Node *rLeft(Node *x)
    {
        if (!x || !x->right)
            return x;
        Node *y = x->right;
        Node *w = y->left;

        y->left = x;
        y->parent = x->parent;

        if (y->parent)
        {
            if (y->parent->left == x)
                y->parent->left = y;
            else
                y->parent->right = y;
        }
        else
        { // parent == nullptr
            root = y;
        }

        x->parent = y;
        x->right = w;
        if (w)
            w->parent = x;

        return y;
    }

    Node *rRight(Node *x)
    {
        if (!x || !x->left)
            return x;
        Node *y = x->left;
        Node *w = y->right;

        y->right = x;
        y->parent = x->parent;

        if (y->parent)
        {
            if (y->parent->left == x)
                y->parent->left = y;
            else
                y->parent->right = y;
        }
        else
        { // parent == nullptr
            root = y;
        }

        x->parent = y;
        x->left = w;
        if (w)
            w->parent = x;

        return y;
    }

private:
    Node *Insertrb(Node *z)
    {
        Node *x = root; // não teve jeito, tive que fazer isso
        Node *y = nullptr;

        while (x)
        {
            y = x;
            if (z->word < x->word)
                x = x->left;
            else if (z->word > x->word)
                x = x->right;
            else
            {
                return x;
            }
        }
        z->parent = y;
        if (!y)
            root = z;
        else if (z->word < y->word)
            y->left = z;
        else
            y->right = z;

        return z;
    }

    void InsertFixup(Node *z)
    {
        while (z->parent && z->parent->color == red)
        {
            Node *y = z->parent;
            Node *g = y->parent; // g de godfather
            if (!g)
            { // y == root
                y->color = black;
                break;
            }

            Node *uncle = (y == g->left) ? g->right : g->left;

            // case 1: z's uncle is red
            if (uncle && uncle->color)
            {
                uncle->color = black; // o tio e o pai pegam a negritude do avô
                y->color = black;
                g->color = red;
                z = g;
            }
            else
            {
                if (y == g->left)
                {

                    // case 2.1: z is an inner right child (left-right)
                    if (z == y->right)
                    {
                        z = y;
                        rLeft(z); // single rotation left (do pai do nó atual)
                        y = z->parent;
                        g = y ? y->parent : nullptr;
                    } // segue pro caso 3.1

                    // case 3.1: z is an outer left child (left-left)
                    y = z->parent;
                    g = y ? y->parent : nullptr;

                    // troca as cores do pai e do vô
                    if (y)
                        y->color = black;
                    if (g)
                    {
                        g->color = red;
                        rRight(g); // single rotation right (do vô do nó atual)
                    }
                    z = y ? y : z;
                }
                else
                { // y == g->right

                    // case 2.2: z is an inner left child (right-left)
                    if (z == y->left)
                    {
                        z = y;
                        rRight(z); // single rotation right
                        y = z->parent;
                        g = y ? y->parent : nullptr;
                    } // segue pro caso 3.2

                    // case 3.2: z is an outer left child (left-left) perigoso isso
                    y = z->parent;
                    g = y ? y->parent : nullptr;

                    if (y)
                        y->color = black;
                    if (g)
                    {
                        g->color = red;
                        rLeft(g); // single rotation left
                    }
                    z = y ? y : z;
                }
            }
        }

        if (root)
            root->color = black;
    }

    Node *maxValor(Node *no)
	{
		Node* atual = no;
		while(atual->right != nullptr)
			atual = atual->right;
		return atual;
	}

    void replace(Node *a, Node *b) {
        if(!a->parent) root = b;
        else if(a == a->parent->left) a->parent->left = b;
        else a->parent->right = b;
        if(b) b->parent = a->parent;
    }

    void removeFixUp(Node *x, Node *x_parent) { //vai se ferrar matsu com esses comentarios em english
        while((x != root) && (!x || x->color == black)) {

            if(x_parent && x == x_parent->left) { //x é filho da esquerda
                Node *w = x_parent->right;

                //case 1: x's sibling is red
                if(w && w->color == red) {
                    w->color = black;
                    x_parent->color = red;
                    rLeft(x_parent);
                    w = x_parent->right;
                }
                if(!w || ((!w->left || w->left->color == black) && (!w->right || w->right->color == black)) ) { //os dois filhos de w são pretos
                    //case 2: w and both w's child are black
                    if(w) w->color = red;
                    x = x_parent; //atualiza para continuar os testes
                    x_parent = x? x->parent: nullptr;
                }
                else {
                    //case 3: w's left child is red
                    if(w->left && w->left->color == red) {
                        w->color = red;
                        w->left->color = black; //troca as cores
                        rRight(w); //single rotation right
                        w = x_parent->right;
                    }
                    //case 4: w's right child is red
                    if(w) w->color = x_parent->color;
                    if(x_parent) x_parent->color = black; //pai de x e filho da direita de w ficam pretos
                    if(w->right) w->right->color = black;
                    rLeft(x_parent);
                    x = root;
                    x_parent = nullptr;
                }
            }
            else { //x é filho da direita
                Node *w = x_parent->left;

                //case 1: x's sibling is red
                if(w && w->color == red) {
                    w->color = black;
                    x_parent->color = red;
                    rRight(x_parent);
                    w = x_parent->left;
                }
                if(!w || ((!w->left || w->left->color == black) && (!w->right || w->right->color == black)) ) { //os dois filhos de w são pretos
                    //case 2: w and both w's child are black
                    if(w) w->color = red;
                    x = x_parent; //atualiza para continuar os testes
                    x_parent = x? x->parent: nullptr;
                }
                else {
                    //case 3: w's left child is red
                    if(w->left && w->left->color == red) {
                        w->color = red;
                        w->left->color = black; //troca as cores
                        rRight(w); //single rotation left
                        w = x_parent->left;
                    }
                    //case 4: w's right child is red
                    if(w) w->color = x_parent->color;
                    if(x_parent) x_parent->color = black; //pai de x e filho da direita de w ficam pretos
                    if(w->right) w->right->color = black;
                    rLeft(x_parent);
                    x = root;
                    x_parent = nullptr;
                }
            }
        }
        if(x) x->color = black;
    }
};

// ---- funcoes do menu para teste ----
Dictionary dict;

void inserirPalavra()
{
    string w, m;
    cout << "Palavra: ";
    getline(cin, w);
    if (w.empty())
    {
        cout << "Palavra vazia. Abortando.\n";
        return;
    }
    cout << "Significado (uma linha): ";
    getline(cin, m);
    dict.Insert(w, m);
    cout << "Inserido: " << w << "\n";
}

void inorderPrint(Node *node)
{
    if (!node)
        return;
    inorderPrint(node->left);
    cout << (node->color == red ? "(R) " : "(B) ") << node->word << " : " << node->meaning << "\n";
    inorderPrint(node->right);
}

void listarPalavras()
{
    if (!dict.root)
    {
        cout << "Dicionario vazio.\n";
        return;
    }
    inorderPrint(dict.root);
}

void excluirPalavra()
{
    cout << "Excluir Palavra, entre com a palavra: ";
    string palavraParaExcluir;
    getline(cin, palavraParaExcluir);

    
    if (dict.buscarPalavra(palavraParaExcluir)) //precisa existir para excluir
    {
        dict.Remove(palavraParaExcluir); 
        cout << "Palavra '" << palavraParaExcluir << "' excluida com sucesso.\n";
    }
    else
    {
        cout << "Palavra '" << palavraParaExcluir << "' nao encontrada. Nada foi excluido.\n";
    }
}

void inorderSave(Node *node, ofstream &arquivo)
{
    if (!node)
        return;
    inorderSave(node->left, arquivo);
    // Escreve a palavra, um espaço, e o significado, seguido de uma nova linha.
    arquivo << node->word << " " << node->meaning << endl;
    inorderSave(node->right, arquivo);
}

void salvarEmArquivo()
{
    string nomeDoArquivo = "teste.out";

    ofstream arquivo(nomeDoArquivo);

    if (!arquivo.is_open())
    {
        cout << "ERRO: Nao foi possivel abrir ou criar o arquivo '" << nomeDoArquivo << "'.\n";
        return;
    }

    cout << "\nSalvando dicionario em '" << nomeDoArquivo << "'...\n";

    if (dict.root)
    {
        inorderSave(dict.root, arquivo);
    }

    arquivo.close();

    cout << "Dicionario salvo com sucesso!\n";
}

void carregarDeArquivo(Dictionary &dict)
{
    string linha, palavra, significado;
    int linhasLidas = 0;

    // o loop continua enquanto tem linhas para ler 
    while (getline(cin, linha))
    {
        // indica o início das entradas
        if (linha == "---MENU---")
            break;

        linhasLidas++;

        // encontra a pos do primeiro espaço na linha
        size_t pos = linha.find(' ');

        if (pos == string::npos)
        {
            // Se não tem espaço a palavra é a linha inteira, significado vazio
            palavra = linha;
            significado = "";
        }
        else
        {
            // palavra pos 0 até a pos do espaço
            palavra = linha.substr(0, pos);

            // pos depis do espaço até o final da linha
            significado = linha.substr(pos + 1);
        }

        // sem palavras vazias
        if (palavra.empty())
        {
            continue;
        }

        dict.Insert(palavra, significado);
    }

    // Limpa o estado de falha (EOF) do cin
    cin.clear();

    cout << "Carregamento finalizado. " << linhasLidas << " linhas carregadas para a arvore.\n";
}

void buscarPalavraMenu()
{
    cout << "Buscar Palavra, entre com a palavra: ";
    string palavraBuscada;

    getline(cin, palavraBuscada); 

    Node *resultado = dict.buscarPalavra(palavraBuscada); 

    if (resultado) 
    {
        cout << "\n--- Palavra Encontrada ---\n";
        cout << "Palavra: " << resultado->word << "\n";
        cout << "Significado: " << resultado->meaning << "\n";
        
        cout << "Cor do No: " << (resultado->color == red ? "Vermelho" : "Preto") << "\n";
        cout << "---------------------------\n";
    }
    else 
    {
        cout << "\nPalavra '" << palavraBuscada << "' nao encontrada no dicionario.\n";
    }
}

int main()
{
    int opcao;

    carregarDeArquivo(dict);

    cin.clear();

    do
    {
        cout << "\n========== DICIONÁRIO DIGITAL ==========\n\n";
        cout << "1. Inserir nova palavra\n";
        cout << "2. Buscar palavra\n";
        cout << "3. Listar todas as palavras (ordem alfabética)\n";
        cout << "4. Excluir palavra\n";
        cout << "5. Salvar e sair\n\n";
        cout << "========================================\n";
        cout << "Selecione uma opção: ";

        if (!(cin >> opcao))
            break;
        cin.ignore(); // limpar buffer

        switch (opcao)
        {
        case 1:
            inserirPalavra();
            break;
        case 2:
            buscarPalavraMenu();
            break;
        case 3:
            listarPalavras();
            break;
        case 4:
            excluirPalavra();
            break;
        case 5:
            salvarEmArquivo();
            cout << "\nEncerrando o programa e salvando alterações...\n";
            break;
        default:
            cout << "\nOpção inválida. Tente novamente.\n";
        }
    } while (opcao != 5);

    return 0;
}
