#include <iostream>
#include <vector>
#include <unordered_map>
#include <stdexcept>
using namespace std;

#ifndef ANALISELEXICA_H
#define ANALISELEXICA_H

using namespace std;

struct Token
{
  int tipo;
  string lexema;
  int linha;
  int coluna;

  Token(int t, string lex, int ln, int col) : tipo(t), lexema(lex), linha(ln), coluna(col) {}
};

unordered_map<string, int> criarTabelaDeTokens();
string lerArquivo(const string &nome_arquivo);
vector<Token> analiseLexica(const string &codigo, const unordered_map<string, int> &tabelaDeTokens);

#endif // ANALISELEXICA_H
