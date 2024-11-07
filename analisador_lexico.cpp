#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cctype>

using namespace std;

struct Token
{
    int tipo;
    string lexema;
    int linha;
    int coluna;

    Token(int t, string lex, int ln, int col) : tipo(t), lexema(lex), linha(ln), coluna(col) {}
};

unordered_map<string, int> criarTabelaDeTokens()
{
    return {
        {"+", 1},
        {"-", 2},
        {"*", 3},
        {"/", 4},
        {"%", 5},
        {"==", 11},
        {"!=", 12},
        {">", 13},
        {">=", 14},
        {"<", 15},
        {"<=", 16},
        {"||", 21},
        {"&&", 22},
        {"!", 23},
        {"=", 24},
        {"+=", 25},
        {"-=", 26},
        {"*=", 27},
        {"/=", 28},
        {"%=", 29},
        {"int", 31},
        {"float", 32},
        {"string", 33},
        {"if", 41},
        {"else", 42},
        {"for", 43},
        {"while", 44},
        {"break", 45},
        {"continue", 46},
        {"return", 47},
        {"system", 51},
        {"out", 52},
        {"print", 53},
        {"in", 55},
        {"scan", 56},
        {"main", 57},
        {"(", 61},
        {")", 62},
        {"{", 63},
        {"}", 64},
        {";", 65},
        {",", 66},
        {".", 67},
        {"\"", 71},
        {"\'", 72},
    };
}

void imprimirTokens(const vector<Token> &tokens)
{
    for (const auto &token : tokens)
    {
        cout << "{token: " << token.tipo
             << ", lexema: " << token.lexema
             << ", linha: " << token.linha
             << ", coluna: " << token.coluna << "}" << endl;
    }
}

bool isInteger(const string &str)
{
    size_t start = str[0];
    for (size_t i = start; i < str.size(); ++i)
    {
        if (!isdigit(str[i]))
            return false;
    }
    return true;
}

bool isFloat(const string &str)
{
    size_t start = str[0];
    bool pointFound = false;
    for (size_t i = start; i < str.size(); ++i)
    {
        if (str[i] == '.')
        {
            if (pointFound)
                return false;
            pointFound = true;
        }
        else if (!isdigit(str[i]))
        {
            return false;
        }
    }
    return pointFound;
}

bool isHexadecimal(const string &str)
{
    if (str.size() < 3 || str[0] != '0' || (str[1] != 'x'))
        return false;
    for (size_t i = 2; i < str.size(); ++i)
    {
        if (!isxdigit(str[i]))
            return false;
    }
    return true;
}

bool isOctal(const string &str)
{
    if (str.size() < 2 || str[0] != '0')
        return false;
    for (size_t i = 1; i < str.size(); ++i)
    {
        if (str[i] < '0' || str[i] > '7')
            return false;
    }
    return true;
}

string lerArquivo(const string &nome_arquivo)
{
    ifstream arquivo(nome_arquivo);
    if (!arquivo.is_open())
    {
        cerr << "Erro ao abrir o arquivo: " << nome_arquivo << endl;
        return "";
    }
    return string((istreambuf_iterator<char>(arquivo)), istreambuf_iterator<char>());
}

vector<Token> analiseLexica(const string &codigo, const unordered_map<string, int> &tabelaDeTokens)
{
    vector<Token> tokens;
    int linha = 1, coluna = 1;
    size_t i = 0;

    while (i < codigo.size())
    {
        char ch = codigo[i];

        // Ignorar espaços
        if (ch == ' ' || ch == '\t')
        {
            coluna++;
            i++;
            continue;
        }

        // Ignorar quebras de linha fora de strings
        if (ch == '\n')
        {
            linha++;
            coluna = 1;
            i++;
            continue;
        }

        // Ignorar comentários de linha e bloco
        if (ch == '/' && i + 1 < codigo.size())
        {
            if (codigo[i + 1] == '/')
            {
                // Ignorar o comentário de linha
                while (i < codigo.size() && codigo[i] != '\n')
                {
                    i++;
                    coluna++;
                }
                continue;
            }
            else if (codigo[i + 1] == '*')
            { // comentário de bloco
                size_t start_linha = linha;
                size_t start_coluna = coluna;
                i += 2;
                coluna += 2;
                bool fechado = false;

                // Avança até encontrar "*/" ou o final do código
                while (i < codigo.size() - 1)
                {
                    if (codigo[i] == '*' && codigo[i + 1] == '/')
                    {
                        i += 2;
                        coluna += 2;
                        fechado = true;
                        break;
                    }

                    if (codigo[i] == '\n')
                    {
                        linha++;
                        coluna = 1;
                    }
                    else
                    {
                        coluna++;
                    }
                    i++;
                }

                // Se não foi fechado, lança um erro indicando a posição inicial do comentário
                if (!fechado)
                {
                    throw runtime_error("Erro: comentario de bloco iniciado na linha " +
                                        to_string(start_linha) + ", coluna " +
                                        to_string(start_coluna) + " nao foi fechado.");
                }
                continue;
            }
        }

        // Verificacao de String junto com \n dentro delas
        if (ch == '"')
        {
            size_t start_coluna = coluna;
            string lexema = "\"";
            i++;
            coluna++;

            while (i < codigo.size() && codigo[i] != '"')
            {
                if (codigo.substr(i, 2) == "\\n")
                {
                    lexema += '\n';
                    i += 2;
                    coluna += 2;
                }
                else if (codigo.substr(i, 2) == "\\t")
                {
                    lexema += '\t';
                    i += 2;
                    coluna += 2;
                }
                else if (codigo[i] == '\n')
                {
                    throw runtime_error("Erro: string nao fechada na linha " + to_string(linha) + ", coluna " + to_string(start_coluna));
                }
                else
                {
                    lexema += codigo[i];
                    i++;
                    coluna++;
                }
            }

            if (i < codigo.size() && codigo[i] == '"')
            {
                lexema += "\"";
                tokens.emplace_back(6, lexema, linha, start_coluna);
                i++;
                coluna++;
            }
            else
            {
                throw runtime_error("Erro: string nao fechada na linha " + to_string(linha) + ", coluna " + to_string(start_coluna));
            }
            continue;
        }

        // Mesma verificacao de aspas, mas com aspas simples
        if (ch == '\'')
        {
            size_t start_coluna = coluna;
            string lexema = "'";
            i++;
            coluna++;

            while (i < codigo.size() && codigo[i] != '\'')
            {
                if (codigo.substr(i, 2) == "\\n")
                {
                    lexema += '\n';
                    i += 2;
                    coluna += 2;
                }
                else if (codigo.substr(i, 2) == "\\t")
                {
                    lexema += '\t';
                    i += 2;
                    coluna += 2;
                }
                else if (codigo[i] == '\n')
                {
                    throw runtime_error("Erro: string nao fechada na linha " + to_string(linha) + ", coluna " + to_string(start_coluna));
                }
                else
                {
                    lexema += codigo[i];
                    i++;
                    coluna++;
                }
            }

            if (i < codigo.size() && codigo[i] == '\'')
            {
                lexema += '\'';
                tokens.emplace_back(6, lexema, linha, start_coluna);
                i++;
                coluna++;
            }
            else
            {
                throw runtime_error("Erro: string nao fechada na linha " + to_string(linha) + ", coluna " + to_string(start_coluna));
            }
            continue;
        }

        // Verifica caracter em busca de palavras-chaves e variáveis
        if (isalpha(ch))
        {
            size_t start_coluna = coluna;
            string lexema;

            while (i < codigo.size() && (isalnum(codigo[i])))
            {
                lexema += codigo[i];
                i++;
                coluna++;
            }
            // Se nao for uma palavra chave, entao e uma variavel
            if (tabelaDeTokens.count(lexema) > 0)
            {
                tokens.emplace_back(tabelaDeTokens.at(lexema), lexema, linha, start_coluna);
            }
            else
            {
                tokens.emplace_back(99, lexema, linha, start_coluna);
            }
            continue;
        }

        if (isdigit(ch) || (ch == '-' && isdigit(codigo[i + 1])) || (ch == '0' && i + 1 < codigo.size() && codigo[i + 1] == 'x'))
        {
            string numero;
            size_t start_coluna = coluna;
            bool pontoEncontrado = false;
            bool xEncontrado = false;
            bool VAR_isHexadecimal = false;

            if (ch == '-')
            {
                tokens.emplace_back(tabelaDeTokens.at("-"), "-", linha, coluna);
                i++;
                coluna++;
            }
            // Identificar se é hexadecimal
            if (ch == '0' && i + 1 < codigo.size() && codigo[i + 1] == 'x')
            {
                VAR_isHexadecimal = true;
                numero += "0x";
                i += 2;
                coluna += 2;
            }

            // Loop de leitura do número
            while (i < codigo.size() && (isdigit(codigo[i]) || codigo[i] == '.' || (VAR_isHexadecimal && isxdigit(codigo[i]))))
            {
                if (!VAR_isHexadecimal)
                {
                    // Caso geral para números inteiros ou flutuantes
                    if (codigo[i] == '.')
                    {
                        if (pontoEncontrado)
                        {
                            throw runtime_error("Erro: Numero invalido na linha " + to_string(linha) + ", coluna " + to_string(start_coluna));
                        }
                        pontoEncontrado = true;
                    }
                    else if (!isdigit(codigo[i]))
                    {
                        break;
                    }
                }
                numero += codigo[i];
                i++;
                coluna++;
            }

            // Verificação: após a leitura do número, checa se o próximo caractere é válido
            if (i < codigo.size() && (isalpha(codigo[i])))
            {
                throw runtime_error("Erro: Numero invalido na linha " + to_string(linha) + ", coluna " + to_string(start_coluna));
            }

            if (VAR_isHexadecimal && isHexadecimal(numero))
                tokens.emplace_back(101, numero, linha, start_coluna); // 101: hexadecimal
            else if (isOctal(numero))
                tokens.emplace_back(102, numero, linha, start_coluna); // 102: octal
            else if (isInteger(numero))
                tokens.emplace_back(104, numero, linha, start_coluna); // 104 : int
            else if (isFloat(numero))
            {
                numero += '0';
                tokens.emplace_back(103, numero, linha, start_coluna); // 103: float
            }
            else
                throw runtime_error("Erro: Numero invalido na linha " + to_string(linha) + ", coluna " + to_string(start_coluna));
            continue;
        }

        // Verificar operadores e símbolos
        string op(1, ch);
        if (i + 1 < codigo.size())
        {
            string op2 = op + codigo[i + 1];
            if (tabelaDeTokens.count(op2) > 0)
            {
                tokens.emplace_back(tabelaDeTokens.at(op2), op2, linha, coluna);
                i += 2;
                coluna += 2;
                continue;
            }
        }

        if (tabelaDeTokens.count(op) > 0)
        {
            tokens.emplace_back(tabelaDeTokens.at(op), op, linha, coluna);
            i++;
            coluna++;
            continue;
        }

        throw runtime_error("Caractere desconhecido: " + string(1, ch) + " na linha " + to_string(linha) + ", coluna " + to_string(coluna));
    }

    return tokens;
}

int main()
{
    string nome_arquivo;
    cout << "Informe o nome do arquivo a ser analisado: ";
    cin >> nome_arquivo;

    try
    {
        string codigo = lerArquivo(nome_arquivo);
        if (!codigo.empty())
        {
            unordered_map<string, int> tabelaDeTokens = criarTabelaDeTokens();
            vector<Token> tokens = analiseLexica(codigo, tabelaDeTokens);
            imprimirTokens(tokens);
        }
    }
    catch (const runtime_error &e)
    {
        cerr << e.what() << endl;
    }

    return 0;
}
