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
        {"+", "OP_ADD"}, {"-", "OP_SUB"}, {"*", "OP_MULT"}, {"/", "OP_DIV"}, {"%", "OP_MOD"},
        {"==", "OP_IGUAL"}, {"!=", "OP_DIF"}, {">", "OP_MAIOR"}, {">=", "OP_MAIOR_QUE"}, {"<", "OP_MENOR"}, {"<=", "OP_MENOR_QUE"},
        {"||", "OP_OU_LOGICO"}, {"&&", "OP_&_LOGICO"}, {"!", "OP_NAO_LOGICO"}, {"=", "ATRIBUICAO"}, 
        {"+=", "ATRIBUICAO_ADD"}, {"-=", "ATRIBUICAO_SUB"}, {"*=", "ATRIBUICAO_MULT"}, {"/=", "ATRIBUICAO_DIV"}, 
        {"%=", "ATRIBUICAO_MOD"}, {"int", "TIPO_INT"}, {"float", "TIPO_FLOAT"}, {"string", "TIPO_STRING"}, {"if", "CONDICIONAL_IF"}, 
        {"else", "CONDICIONAL_ELSE"}, {"for", "LACO_FOR"},{"while","LACO_WHILE"}, {"break", "CONTROLE_BREAK"}, {"continue", "CONTROLE_CONTINUE"},
        {"return", "CONTROLE_RETURN"}, {"system", "FUNCAO_SYSTEM"}, {"out", "FUNCAO_OUT"}, {"print", "FUNCAO_PRINT"}, {"println", "FUNCAO_PRINTLN"},
        {"in", "FUNCAO_IN"}, {"scan", "FUNCAO_SCAN"}, {"main", "FUNCAO_MAIN"}, {"(", "PARENTESES_ABRE"}, {")", "PARENTESES_FECHA"},
        {"{", "CHAVE_ABRE"}, {"}", "CHAVE_FECHA"}, {";", "PONTO_VIRGULA"}, {",", "VIRGULA"}, {".", "PONTO"}, {"\"", "ASPAS_DUPLAS"}, {"\'", "ASPAS_SIMPLES"},
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
        // Verificar strings e \n dentro delas
        if (ch == '"') {
            size_t start_coluna = coluna;
            i++;
            coluna++;

            // Identificar conteúdo da string, separando \n
            while (i < codigo.size() && codigo[i] != '"') {
                if (codigo.substr(i, 2) == "\\n") {
                    tokens.emplace_back("QUEBRA_LINHA", linha, coluna);
                }
                if (codigo[i] == '\n') {
                    throw runtime_error("Erro: string nao fechada na linha " + to_string(linha));
                } 
                i++;
                coluna++;
            }

            if (i < codigo.size() && codigo[i] == '"') {
                tokens.emplace_back("STRING", linha, start_coluna);
                i++;
                coluna++;
            } else {
                throw runtime_error("Erro: string nao fechada na linha " + to_string(linha) + ", coluna " + to_string(start_coluna));
            }
            continue;
        }
        
        if (ch == '\'') {
            size_t start_coluna = coluna;
            i++;
            coluna++;

            // Identificar conteúdo da string, separando \n
            while (i < codigo.size() && codigo[i] != '\'') {
                if (codigo.substr(i, 2) == "\\n") {
                    tokens.emplace_back("QUEBRA_LINHA", linha, coluna);
                }
                if (codigo[i] == '\n') {
                    throw runtime_error("Erro: string nao fechada na linha " + to_string(linha));
                } 
                i++;
                coluna++;
            }



            if (i < codigo.size() && codigo[i] == '\'') {
                tokens.emplace_back("STRING", linha, start_coluna);
                i++;
                coluna++;
            } else {
                throw runtime_error("Erro: string nao fechada na linha " + to_string(linha) + ", coluna " + to_string(start_coluna));
            }
            continue;
        }        

        // Verificar números
        if (isdigit(ch) || (ch == '-' && isdigit(codigo[i + 1])) || (ch == '0' && i + 1 < codigo.size() && codigo[i + 1] == 'x')) {
            string numero;
            size_t start_coluna = coluna;
            bool pontoEncontrado = false;
            bool xEncontrado = false;
            bool VAR_isHexadecimal = false;

            // Identificar se é hexadecimal
            if (ch == '0' && i + 1 < codigo.size() && codigo[i + 1] == 'x') {
                VAR_isHexadecimal = true;
                numero += "0x";
                i += 2;
                coluna += 2;
            }

            // Loop de leitura do número
            while (i < codigo.size() && (isdigit(codigo[i]) || codigo[i] == '.' || (VAR_isHexadecimal && isxdigit(codigo[i])))) {
                if (!VAR_isHexadecimal) {
                    // Caso geral para números inteiros ou flutuantes
                    if (codigo[i] == '.') {
                        if (pontoEncontrado) {
                            throw runtime_error("Erro: Numero invalido na linha " + to_string(linha) + ", coluna " + to_string(start_coluna));
                        }
                        pontoEncontrado = true;
                    } else if (!isdigit(codigo[i])) {
                        break;
                    }
                }
                numero += codigo[i];
                i++;
                coluna++;
            }

            // Verificação: após a leitura do número, checa se o próximo caractere é válido
            if (i < codigo.size() && (isalpha(codigo[i]) || codigo[i] == '_')) {
                throw runtime_error("Erro: Numero invalido na linha " + to_string(linha) + ", coluna " + to_string(start_coluna));
            }

            // Classifica o tipo do número
            if (VAR_isHexadecimal && isHexadecimal(numero)) tokens.emplace_back("NUMERO_HEXADECIMAL", linha, start_coluna);
            else if (isOctal(numero)) tokens.emplace_back("NUMERO_OCTAL", linha, start_coluna);
            else if (isInteger(numero)) tokens.emplace_back("NUMERO_INT", linha, start_coluna);
            else if (isFloat(numero)) tokens.emplace_back("NUMERO_FLOAT", linha, start_coluna);
            else throw runtime_error("Erro: Numero invalido na linha " + to_string(linha) + ", coluna " + to_string(start_coluna));

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
