#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cctype>

using namespace std;

struct Token {
    string tipo;
    int linha;
    int coluna;

    Token(string t, int ln, int col) : tipo(t), linha(ln), coluna(col) {}
};

unordered_map<string, string> criarTabelaDeTokens() {
    return {
        {"+", "OPERADOR"}, {"-", "OPERADOR"}, {"*", "OPERADOR"}, {"/", "OPERADOR"}, {"%", "OPERADOR"},
        {"==", "OPERADOR"}, {"!=", "OPERADOR"}, {">", "OPERADOR"}, {">=", "OPERADOR"}, {"<", "OPERADOR"}, {"<=", "OPERADOR"},
        {"||", "OPERADOR_LOGICO"}, {"&&", "OPERADOR_LOGICO"}, {"!", "OPERADOR_LOGICO"}, {"=", "ATRIBUICAO"}, 
        {"+=", "OPERADOR_ATRIBUICAO"}, {"-=", "OPERADOR_ATRIBUICAO"}, {"*=", "OPERADOR_ATRIBUICAO"}, {"/=", "OPERADOR_ATRIBUICAO"}, 
        {"%=", "OPERADOR_ATRIBUICAO"}, {"int", "TIPO"}, {"float", "TIPO"}, {"string", "TIPO"}, {"if", "CONDICIONAL"}, 
        {"else", "CONDICIONAL"}, {"for", "LAÇO"}, {"system", "Funcao"},{"println", "funcao"},{"while", "LAÇO"}, {"break", "CONTROLE_FLUXO"}, 
        {"continue", "CONTROLE_FLUXO"}, {"return", "CONTROLE_FLUXO"}, {"print", "FUNCAO"}, {"scan", "FUNCAO"}, 
        {"in", "FUNCAO"}, {"out", "FUNCAO"}, {"main", "FUNCAO"}, {"(", "PARENTESES"}, {")", "PARENTESES"}, 
        {"{", "CHAVE"}, {"}", "CHAVE"}, {";", "PONTO_VIRGULA"}, {",", "VIRGULA"}, {".", "PONTO"}
    };
}

void imprimirTokens(const vector<Token>& tokens) {
    for (const auto& token : tokens) {
        cout << "{Tipo: " << token.tipo
             << ", Linha: " << token.linha
             << ", Coluna: " << token.coluna << "}" << endl;
    }
}

string lerArquivo(const string& nome_arquivo) {
    ifstream arquivo(nome_arquivo);
    if (!arquivo.is_open()) {
        cerr << "Erro ao abrir o arquivo: " << nome_arquivo << endl;
        return "";
    }
    return string((istreambuf_iterator<char>(arquivo)), istreambuf_iterator<char>());
}

vector<Token> analisarCodigo(const string& codigo, const unordered_map<string, string>& tabelaDeTokens) {
    vector<Token> tokens;
    int linha = 1, coluna = 1;
    size_t i = 0;

    while (i < codigo.size()) {
        char ch = codigo[i];

        // Ignorar espaços e tabulações
        if (ch == ' ' || ch == '\t') {
            coluna++;
            i++;
            continue;
        }

        // Ignorar quebras de linha fora de strings
        if (ch == '\n') {
            linha++;
            coluna = 1;
            i++;
            continue;
        }

        // Verificar strings e \n dentro delas
        if (ch == '"') {
            size_t start_coluna = coluna;
            i++;
            coluna++;

            // Identificar conteúdo da string, separando \n
            while (i < codigo.size() && codigo[i] != '"') {
                if (codigo[i] == '\n') {
                    tokens.emplace_back("QUEBRA_LINHA", linha, coluna);
                } 
                i++;
                coluna++;
            }

            if (i < codigo.size() && codigo[i] == '"') {
                tokens.emplace_back("STRING", linha, start_coluna);
                i++;
                coluna++;
            } else {
                throw runtime_error("Erro: string não fechada na linha " + to_string(linha) + ", coluna " + to_string(start_coluna));
            }
            continue;
        }

        // Verificar números
        if (isdigit(ch)) {
            bool isFloat = false;
            size_t start_coluna = coluna;
            while (i < codigo.size() && (isdigit(codigo[i]) || (codigo[i] == '.' && !isFloat))) {
                if (codigo[i] == '.') {
                    isFloat = true;
                }
                i++;
                coluna++;
            }

            if (i < codigo.size() && codigo[i] == '.') {
                throw runtime_error("Erro: múltiplos pontos em número na linha " + to_string(linha) + ", coluna " + to_string(start_coluna));
            }

            tokens.emplace_back(isFloat ? "NUMERO_FLUTUANTE" : "NUMERO_INTEIRO", linha, start_coluna);
            continue;
        }

        // Verificar identificadores e palavras-chave, tratando o ponto como um delimitador separado
        if (isalpha(ch) || ch == '_') {
            string lexema;
            size_t start_coluna = coluna;

            while (i < codigo.size() && (isalnum(codigo[i]) || codigo[i] == '_')) {
                lexema += codigo[i];
                i++;
                coluna++;
            }

            if (tabelaDeTokens.count(lexema) > 0) {
                tokens.emplace_back(tabelaDeTokens.at(lexema), linha, start_coluna);
            } else {
                tokens.emplace_back("VARIAVEL", linha, start_coluna);
            }
            continue;
        }

        // Tratar ponto como um token separado
        if (ch == '.') {
            tokens.emplace_back("PONTO", linha, coluna);
            i++;
            coluna++;
            continue;
        }

        // Operadores compostos e símbolos únicos
        string op(1, ch);
        if (i + 1 < codigo.size()) {
            string op2 = op + codigo[i + 1];
            if (tabelaDeTokens.count(op2) > 0) {
                tokens.emplace_back(tabelaDeTokens.at(op2), linha, coluna);
                i += 2;
                coluna += 2;
                continue;
            }
        }

        if (tabelaDeTokens.count(op) > 0) {
            tokens.emplace_back(tabelaDeTokens.at(op), linha, coluna);
            i++;
            coluna++;
            continue;
        }

        throw runtime_error("Caractere desconhecido: " + string(1, ch) + " na linha " + to_string(linha) + ", coluna " + to_string(coluna));
    }

    return tokens;
}

int main() {
    string nome_arquivo;
    cout << "Informe o nome do arquivo a ser analisado: ";
    cin >> nome_arquivo;

    try {
        string codigo = lerArquivo(nome_arquivo);
        if (!codigo.empty()) {
            unordered_map<string, string> tabelaDeTokens = criarTabelaDeTokens();
            vector<Token> tokens = analisarCodigo(codigo, tabelaDeTokens);
            imprimirTokens(tokens);
        }
    } catch (const runtime_error& e) {
        cerr << e.what() << endl;
    }

    return 0;
}
