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
        {"else", "CONDICIONAL"}, {"for", "LAÇO"}, {"while", "LAÇO"}, {"break", "CONTROLE_FLUXO"}, 
        {"continue", "CONTROLE_FLUXO"}, {"return", "CONTROLE_FLUXO"}, {"system", "FUNCAO"}, {"print", "FUNCAO"}, {"scan", "FUNCAO"}, 
        {"in", "FUNCAO"}, {"out", "FUNCAO"}, {"main", "FUNCAO"}, {"(", "PARENTESES"}, {")", "PARENTESES"}, 
        {"{", "CHAVE"}, {"}", "CHAVE"}, {";", "PONTO_VIRGULA"}, {",", "VIRGULA"}
    };
}

bool isInteger(const string& str) {
    size_t start = (str[0] == '-' ? 1 : 0);
    for (size_t i = start; i < str.size(); ++i) {
        if (!isdigit(str[i])) return false;
    }
    return true;
}

bool isFloat(const string& str) {
    size_t start = (str[0] == '-' ? 1 : 0);
    bool pointFound = false;
    for (size_t i = start; i < str.size(); ++i) {
        if (str[i] == '.') {
            if (pointFound) return false;
            pointFound = true;
        } else if (!isdigit(str[i])) {
            return false;
        }
    }
    return pointFound;
}

bool isHexadecimal(const string& str) {
    if (str.size() < 3 || str[0] != '0' || (str[1] != 'x')) return false;
    for (size_t i = 2; i < str.size(); ++i) {
        if (!isxdigit(str[i])) return false;
    }
    return true;
}

bool isOctal(const string& str) {
    if (str.size() < 2 || str[0] != '0') return false;
    for (size_t i = 1; i < str.size(); ++i) {
        if (str[i] < '0' || str[i] > '7') return false;
    }
    return true;
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

        if (ch == ' ' || ch == '\t') {
            coluna++;
            i++;
            continue;
        }

        if (ch == '\n') {
            linha++;
            coluna = 1;
            i++;
            continue;
        }
        // Ignorar comentários de linha e bloco
        if (ch == '/' && i + 1 < codigo.size()) {
            if (codigo[i + 1] == '/') {  // Comentário de linha
                while (i < codigo.size() && codigo[i] != '\n') {
                    i++;
                    coluna++;
                }
                continue;
            } else if (codigo[i + 1] == '*') {  // Comentário de bloco
                i += 2;
                coluna += 2;
                while (i < codigo.size() - 1 && !(codigo[i] == '*' && codigo[i + 1] == '/')) {
                    if (codigo[i] == '\n') {
                        linha++;
                        coluna = 1;
                    } else {
                        coluna++;
                    }
                    i++;
                }
                i += 2;  // Pular "*/"
                coluna += 2;
                continue;
            }
        }
        if (ch == '"') {
            size_t start_coluna = coluna;
            i++;
            coluna++;

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

        if (isdigit(ch) || (ch == '-' && isdigit(codigo[i + 1]))) {
            string numero;
            size_t start_coluna = coluna;
            while (i < codigo.size() && (isdigit(codigo[i]) || codigo[i] == '.' || (codigo[i] == 'x'))) {
                numero += codigo[i];
                i++;
                coluna++;
            }

            if (isHexadecimal(numero)) tokens.emplace_back("NUMERO_HEXADECIMAL", linha, start_coluna);
            else if (isOctal(numero)) tokens.emplace_back("NUMERO_OCTAL", linha, start_coluna);
            else if (isInteger(numero)) tokens.emplace_back("NUMERO_INTEIRO", linha, start_coluna);
            else if (isFloat(numero)) tokens.emplace_back("NUMERO_FLUTUANTE", linha, start_coluna);
            else throw runtime_error("Erro: Número desconhecido na linha " + to_string(linha) + ", coluna " + to_string(start_coluna));

            continue;
        }

        if (isalpha(ch) || ch == '_') {
            string lexema;
            size_t start_coluna = coluna;
            bool pontoPresente = false;

            while (i < codigo.size() && (isalnum(codigo[i]) || codigo[i] == '_' || (codigo[i] == '.' && isalpha(codigo[i + 1])))) {
                if (codigo[i] == '.') pontoPresente = true;
                lexema += codigo[i];
                i++;
                coluna++;
            }

            if (tabelaDeTokens.count(lexema) > 0) {
                tokens.emplace_back(tabelaDeTokens.at(lexema), linha, start_coluna);
            } else if (pontoPresente) {
                for (size_t j = 0; j < lexema.size(); j++) {
                    if (lexema[j] == '.') {
                        tokens.emplace_back("PONTO", linha, start_coluna + j);
                    } else {
                        string subLexema;
                        while (j < lexema.size() && lexema[j] != '.') {
                            subLexema += lexema[j];
                            j++;
                        }
                        tokens.emplace_back("VARIAVEL", linha, start_coluna);
                        start_coluna += subLexema.size() + 1;
                    }
                }
            } else {
                tokens.emplace_back("VARIAVEL", linha, start_coluna);
            }
            continue;
        }

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
