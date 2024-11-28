// main.cpp
#include <iostream>
#include "analiseLexica.h"    // Cabeçalho da análise léxica
#include "analiseSintatica.h" // Cabeçalho da análise sintática
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cctype>
using namespace std;

int main()
{
  string nome_arquivo;
  cout << "Informe o nome do arquivo a ser analisado: ";
  cin >> nome_arquivo;

  // Lê o código do arquivo
  string codigo = lerArquivo(nome_arquivo);
  if (codigo.empty())
  {
    cerr << "Erro ao ler o arquivo." << endl;
    return 1;
  }
  unordered_map<string, int> tabelaDeTokens = criarTabelaDeTokens();
  try
  {
    // Realiza a análise léxica (tokenização)
    vector<Token> tokens = analiseLexica(codigo, tabelaDeTokens);

    cout << "Tokens gerados:" << endl;
    for (const auto &token : tokens)
    {
      cout << "Token tipo: " << token.tipo
           << ", lexema: '" << token.lexema
           << "', linha: " << token.linha
           << ", coluna: " << token.coluna << endl;
    }
    program(tokens); // Chama a função principal da análise sintática
    cout << "Analise sintatica concluida com sucesso!" << endl;
  }
  catch (const exception &e)
  {
    cerr << "Erro: " << e.what() << endl;
    return 1;
  }
  return 0;
}
