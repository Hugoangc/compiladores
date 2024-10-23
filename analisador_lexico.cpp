#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>

using namespace std;

struct Token {
    int tipo;
    string lexema;
    int linha;
    int coluna;

    Token(int t, string l, int ln, int col) : tipo(t), lexema(l), linha(ln), coluna(col) {}
};

unordered_map<string, int> criarTabelaDeTokens() {
    return {
        {"+", 1}, {"-", 2}, {"*", 3}, {"/", 4}, {"%", 5},
        {"==", 6}, {"!=", 7}, {">", 8}, {">=", 9}, {"<", 10}, {"<=", 11},
        {"||", 12}, {"&&", 13}, {"!", 14}, {"=", 15}, {"+=", 16}, {"-=", 17}, 
        {"*=", 18}, {"/=", 19}, {"%=", 20}, {"int", 21}, {"float", 22}, {"string", 23}, 
        {"if", 24}, {"else", 25}, {"for", 26}, {"while", 27}, {"print", 28}, 
        {"return", 29}, {"{", 30}, {"}", 31}, {"(", 32}, {")", 33}, 
        {";", 34}, {",", 35}, {"NUMERO_INTEIRO", 36}, {"NUMERO_FLUTUANTE", 37}, 
        {"IDENTIFICADOR", 38}, {"ASPAS_DUPLAS", 39}
    };
}


void imprimirTokens(const vector<Token>& tokens) {
    for (const auto& token : tokens) {
        cout << "Tipo: " << token.tipo
                  << ", Lexema: " << token.lexema
                  << ", Linha: " << token.linha
                  << ", Coluna: " << token.coluna << endl;
    }
}

string lerArquivo(const string& nome_arquivo) {
    ifstream arquivo(nome_arquivo);
    if (!arquivo.is_open()) {
        cerr << "Erro ao abrir o arquivo: " << nome_arquivo << endl;
        return "";
    }

    string codigo((istreambuf_iterator<char>(arquivo)), istreambuf_iterator<char>());
    return codigo;
}

int main() {
    string nome_arquivo;
    cout << "Informe o nome do arquivo a ser analisado: ";
    cin >> nome_arquivo;

    string codigo = lerArquivo(nome_arquivo);
    if (!codigo.empty()) {
        unordered_map<string, int> tabelaDeTokens = criarTabelaDeTokens();
    }

    return 0;
}
