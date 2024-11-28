#include <iostream>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include "analiseSintatica.h"
using namespace std;
// TODO ESSA É A VERSÃO MAIS ATUALIZADA E FUNCIONAL

// Índice global para controlar o token atual
int tokenIndex = 0;
#define DEBUG_MODE true

void logDebug(const string &mensagem)
{
  if (DEBUG_MODE)
  {
    cerr << "[DEBUG]: " << mensagem << endl;
  }
}

// Função para obter o próximo token
Token nextToken(const vector<Token> &tokens)
{
  if (tokenIndex < tokens.size())
  {
    return tokens[tokenIndex++];
  }
  throw runtime_error("Fim inesperado dos tokens.");
}

// Função para verificar o token atual sem consumir
Token peekToken(const vector<Token> &tokens)
{
  if (tokenIndex < tokens.size())
  {
    return tokens[tokenIndex];
  }
  throw runtime_error("Fim inesperado dos tokens.");
}

// Função para reportar erros de sintaxe
void erroSintaxe(const Token &token, const string &contexto)
{
  string mensagem = "Erro de sintaxe em " + contexto + " no token '" + token.lexema +
                    "' na linha " + to_string(token.linha) +
                    ", coluna " + to_string(token.coluna);
  throw runtime_error(mensagem);
}

// Declaração antecipada das funções de parsing
bool foundMain = false;
// Produção principal <program>
void program(const vector<Token> &tokens)
{
  while (tokenIndex < tokens.size())
  {
    Token token = peekToken(tokens);
    logDebug("Analisando token: " + token.lexema + " (tipo: " + to_string(token.tipo) + ")");

    if (token.tipo == 31 || token.tipo == 32 || token.tipo == 33) // int, float, string
    {
      // Tratar declaração de tipo
      declaration(tokens);
    }
    else if (token.lexema == "main")
    {
      // Caso especial para main
      declaration(tokens);
      foundMain = true;
    }
    else if (token.tipo == 41) // if
    {
      ifStmt(tokens);
    }
    else if (token.tipo == 43) // for
    {
      forStmt(tokens);
    }
    else if (token.tipo == 44) // while
    {
      whileStmt(tokens);
    }
    else if (token.tipo == 99) // Identificadores gerais
    {
      Token prox = peekToken(tokens);
      if (prox.tipo >= 25 && prox.tipo <= 29) // Operadores compostos
      {
        optAtrib(tokens);
      }
      else
      {
        erroSintaxe(prox, "<program> (identificador seguido por token inesperado)");
      }
    }
    else
    {
      erroSintaxe(token, "<program>");
    }
  }
  logDebug("Analise de <program> concluida");

  if (!foundMain)
  {
    throw runtime_error("Erro de sintaxe: Funcao 'main' nao encontrada.");
  }
}

void declaration(const vector<Token> &tokens)
{
  Token tipo = nextToken(tokens);
  logDebug("Analisando token: " + tokens[tokenIndex].lexema + " (tipo: " + to_string(tokens[tokenIndex].tipo) + ")");

  if (tipo.tipo != 31 && tipo.tipo != 32 && tipo.tipo != 33) // Tipos: int, float, string
    erroSintaxe(tipo, "<declaration>");

  Token identificador = nextToken(tokens);

  if (identificador.lexema == "main")
  {
    foundMain = true;

    Token token = nextToken(tokens);
    if (token.tipo != 61) // Espera '('
      erroSintaxe(token, "<declaration>");

    token = nextToken(tokens);
    if (token.tipo != 62) // Espera ')'
      expr(tokens);
    // erroSintaxe(token, "<declaration>");

    block(tokens); // Processa bloco da função main
    return;
  }

  // Continuação para declarações regulares
  while (true)
  {
    if (peekToken(tokens).tipo == 24) // '='
    {
      nextToken(tokens);
      expr(tokens);
    }
    if (peekToken(tokens).tipo == 66) // ','
    {
      nextToken(tokens);
      identificador = nextToken(tokens);
      if (identificador.tipo != 99)
        erroSintaxe(identificador, "<declaration>");
    }
    else
    {
      break;
    }
  }

  Token token = nextToken(tokens);
  if (token.tipo != 65) // ';'
    erroSintaxe(token, "<declaration>");
}

void verificaTipo(const Token &tipoVariavel, const Token &valor)
{
  bool valido = false;

  switch (tipoVariavel.tipo)
  {
  case 31:                                                                  // int
    valido = (valor.tipo == 104 || valor.tipo == 102 || valor.tipo == 101); // Inteiro, octal, hexadecimal
    break;

  case 32:                                                                  // float
    valido = (valor.tipo == 103 || valor.tipo == 104 || valor.tipo == 101); // Float, inteiro, hexadecimal
    break;

  case 33:                      // string
    valido = (valor.tipo == 6); // Literal de string
    break;

  default:
    erroSintaxe(tipoVariavel, "Tipo desconhecido para verificacao");
  }

  if (!valido)
  {
    string mensagemErro = "Tipo incompativel: esperado " + tipoParaString(tipoVariavel.tipo) +
                          ", mas recebeu " + tipoParaString(valor.tipo);
    throw runtime_error(mensagemErro);
  }
}

string tipoParaString(int tipo)
{
  switch (tipo)
  {
  case 31:
    return "int";
  case 32:
    return "float";
  case 33:
    return "string";
  case 101:
    return "hexadecimal";
  case 102:
    return "octal";
  case 103:
    return "float";
  case 104:
    return "inteiro";
  case 6:
    return "string literal";
  default:
    return "tipo desconhecido";
  }
}

void block(const vector<Token> &tokens)
{
  Token token = nextToken(tokens);
  if (token.tipo != 63) // Espera '{'
    erroSintaxe(token, "<block>");

  while (peekToken(tokens).tipo != 64) // Enquanto não encontrar '}'
  {
    stmt(tokens); // Processa declarações ou comandos
  }

  token = nextToken(tokens);
  if (token.tipo != 64) // Espera '}'
    erroSintaxe(token, "<block>");
}

////////////////////////
void stmt(const vector<Token> &tokens)
{
  Token token = peekToken(tokens);

  if (token.lexema == "system") // Verifica início da expressão
  {
    nextToken(tokens); // Consome 'system'
    token = nextToken(tokens);
    if (token.tipo != 67 || token.lexema != ".") // Verifica ponto
      erroSintaxe(token, "<stmt> (esperado '.') apos 'system'");

    token = nextToken(tokens);
    if (token.lexema == "out")
    {
      token = nextToken(tokens);
      if (token.tipo != 67 || token.lexema != ".") // Verifica ponto
        erroSintaxe(token, "<stmt> (esperado '.') apos 'out'");

      token = nextToken(tokens);
      if (token.lexema == "print")
      {
        token = nextToken(tokens);
        if (token.tipo != 61) // Espera '('
          erroSintaxe(token, "<stmt> (esperado '(' apos 'print')");

        // Argumentos da função print
        if (peekToken(tokens).tipo != 62) // Se não for ')'
        {
          expr(tokens); // Analisa a expressão passada para print
        }

        token = nextToken(tokens);
        if (token.tipo != 62) // Espera ')'
          erroSintaxe(token, "<stmt> (esperado ')' apos argumentos)");

        token = nextToken(tokens);
        if (token.tipo != 65) // Espera ';'
          erroSintaxe(token, "<stmt> (esperado ';' apos 'print')");
      }
      else
      {
        erroSintaxe(token, "<stmt> (esperado 'print' apos 'out.')");
      }
    }
    else if (token.lexema == "in")
    {
      token = nextToken(tokens);
      if (token.tipo != 67 || token.lexema != ".") // Verifica ponto
        erroSintaxe(token, "<stmt> (esperado '.') apos 'in'");

      token = nextToken(tokens);
      if (token.lexema == "scan")
      {
        token = nextToken(tokens);
        if (token.tipo != 61) // Espera '('
          erroSintaxe(token, "<stmt> (esperado '(' apos 'scan')");

        // Argumentos da função scan
        if (peekToken(tokens).tipo != 62) // Se não for ')'
        {
          expr(tokens); // Analisa a variável ou expressão para scan
        }

        token = nextToken(tokens);
        if (token.tipo != 62) // Espera ')'
          erroSintaxe(token, "<stmt> (esperado ')' apos argumentos)");

        token = nextToken(tokens);
        if (token.tipo != 65) // Espera ';'
          erroSintaxe(token, "<stmt> (esperado ';' apos 'scan')");
      }
      else
      {
        erroSintaxe(token, "<stmt> (esperado 'scan' apos 'in.')");
      }
    }
    else
    {
      erroSintaxe(token, "<stmt> (esperado 'out' ou 'in' apos 'system.')");
    }
  }
  else if (token.tipo == 31 || token.tipo == 32 || token.tipo == 33) // int, float, string
  {
    declaration(tokens); // Chama a produção de declaração
  }
  else if (token.tipo == 43) // for
  {
    forStmt(tokens);
  }
  else if (token.tipo == 41) // if
  {
    ifStmt(tokens);
  }
  else if (token.tipo == 44) // while
  {
    whileStmt(tokens);
  }
  else if (token.tipo == 45 || token.tipo == 46) // break ou continue
  {
    nextToken(tokens);
    token = nextToken(tokens);
    if (token.tipo != 65) // Ponto e vírgula
      erroSintaxe(token, "<stmt>");
  }
  else if (token.tipo == 99) // Identificador
  {
    optAtrib(tokens);
    token = nextToken(tokens);
    if (token.tipo != 65) // Ponto e vírgula
      erroSintaxe(token, "<stmt>");
  }
  else if (token.tipo == 63) // Bloco '{'
  {
    block(tokens);
  }
  else if (token.tipo == 65) // Ponto e vírgula (stmt vazio)
  {
    nextToken(tokens);
  }
  else
  {
    expr(tokens); // Trata expressão simples
    token = nextToken(tokens);
    if (token.tipo != 65) // Ponto e vírgula
      erroSintaxe(token, "<stmt>");
  }
}

// Produção para <forStmt>
void forStmt(const vector<Token> &tokens)
{
  Token token = nextToken(tokens);
  if (token.tipo != 43) // Espera 'for'
    erroSintaxe(token, "<forStmt>");

  token = nextToken(tokens);
  if (token.tipo != 61) // Espera '('
    erroSintaxe(token, "<forStmt>");

  // Inicialização
  if (peekToken(tokens).tipo == 31 || peekToken(tokens).tipo == 32 || peekToken(tokens).tipo == 33)
  {
    declaration(tokens);
  }
  else if (peekToken(tokens).tipo == 99)
  {
    optAtrib(tokens);
  }

  token = nextToken(tokens);
  if (token.tipo != 65) // Espera ';'
    erroSintaxe(token, "<forStmt>");

  // Condição
  if (peekToken(tokens).tipo != 65) // Se não for ';'
  {
    expr(tokens);
  }
  token = nextToken(tokens);
  if (token.tipo != 65) // Espera ';'
    erroSintaxe(token, "<forStmt>");

  // Incremento
  if (peekToken(tokens).tipo == 99)
  {
    optAtrib(tokens);
  }

  token = nextToken(tokens);
  if (token.tipo != 62) // Espera ')'
    erroSintaxe(token, "<forStmt>");

  // Corpo do for
  stmt(tokens); // Permite expressão, bloco ou comando
}
void whileStmt(const vector<Token> &tokens)
{
  Token token = nextToken(tokens);
  if (token.tipo != 44) // while
    erroSintaxe(token, "<whileStmt>");

  token = nextToken(tokens);
  if (token.tipo != 61) // (
    erroSintaxe(token, "<whileStmt>");

  expr(tokens); // Condição do while

  token = nextToken(tokens);
  if (token.tipo != 62) // )
    erroSintaxe(token, "<whileStmt>");

  stmt(tokens); // Corpo do while
}

// Produção para <ifStmt>
void ifStmt(const vector<Token> &tokens)
{
  Token token = nextToken(tokens);
  if (token.tipo != 41) // if
    erroSintaxe(token, "<ifStmt>");

  token = nextToken(tokens);
  if (token.tipo != 61) // (
    erroSintaxe(token, "<ifStmt>");

  expr(tokens);

  token = nextToken(tokens);
  if (token.tipo != 62) // )
    erroSintaxe(token, "<ifStmt>");

  stmt(tokens); // Analisando o corpo do if

  if (peekToken(tokens).tipo == 42) // else
  {
    nextToken(tokens); // Consome 'else'
    stmt(tokens);      // Corpo do else
  }
}

// Produção para <expr>
void expr(const vector<Token> &tokens)
{
  relExpr(tokens);
  while (peekToken(tokens).tipo == 21 || peekToken(tokens).tipo == 22) // ||
  {
    nextToken(tokens);
    relExpr(tokens);
  }
}

// Produção para <relExpr>
void relExpr(const vector<Token> &tokens)
{
  addExpr(tokens);
  while (peekToken(tokens).tipo == 11 || peekToken(tokens).tipo == 12 ||
         peekToken(tokens).tipo == 13 || peekToken(tokens).tipo == 14 ||
         peekToken(tokens).tipo == 15 || peekToken(tokens).tipo == 16)
  {
    nextToken(tokens);
    addExpr(tokens);
  }
}

// Produção para <addExpr>
void addExpr(const vector<Token> &tokens)
{
  multExpr(tokens);
  while (peekToken(tokens).tipo == 1 || peekToken(tokens).tipo == 2) // +, -
  {
    nextToken(tokens);
    multExpr(tokens);
  }
}

// Produção para <multExpr>
void multExpr(const vector<Token> &tokens)
{
  Token token = peekToken(tokens);

  if (token.tipo == 99 || token.tipo == 104 || token.tipo == 6 || token.tipo == 101 || token.tipo == 102 || token.tipo == 103) // Identificador, número, string
  {
    nextToken(tokens);
  }
  else if (token.tipo == 61) // '('
  {
    nextToken(tokens);
    expr(tokens);
    token = nextToken(tokens);
    if (token.tipo != 62) // Espera ')'
      erroSintaxe(token, "<multExpr>");
  }
  else
  {
    erroSintaxe(token, "<multExpr>");
  }
}

// Produção para <optAtrib>
void optAtrib(const vector<Token> &tokens)
{
  Token id = nextToken(tokens); // Identificador
  Token op = nextToken(tokens); // Operador

  if (op.tipo >= 25 && op.tipo <= 29) // Operadores compostos (+=, -=, etc.)
  {
    Token valor = nextToken(tokens);

    if (valor.tipo == 104 || valor.tipo == 103 || valor.tipo == 101) // Inteiro, float, hexadecimais
    {
      return; // Atribuição composta válida
    }
    else
    {
      erroSintaxe(valor, "<optAtrib> (valor inválido para atribuição composta)");
    }
  }
  else
  {
    erroSintaxe(op, "<optAtrib> (operador inválido)");
  }
}

// void optAtrib(const vector<Token> &tokens)
// {
//   Token identificador = nextToken(tokens);
//   if (identificador.tipo != 99) // Identificador
//     erroSintaxe(identificador, "<optAtrib>");

//   Token operador = nextToken(tokens);
//   if (operador.tipo < 24 || operador.tipo > 29) // Atribuições
//     erroSintaxe(operador, "<optAtrib>");

//   Token valor = peekToken(tokens);
//   expr(tokens); // Processa expressão
//   // Verifica compatibilidade do tipo da variável com o valor atribuído
//   verificaTipo(identificador, valor);
// }

// Função principal
// int main()
// {
//   vector<Token> tokens = {
//       Token(31, "int", 1, 1), // Tipo
//       Token(99, "a", 1, 5),   // Identificador
//       Token(65, ";", 1, 6),   // Ponto e vírgula
//       Token(43, "for", 2, 1), // for
//       Token(61, "(", 2, 4),   // (
//       Token(99, "a", 2, 5),   // Identificador
//       Token(24, "=", 2, 6),   // =
//       Token(104, "10", 2, 8), // Número
//       Token(65, ";", 2, 10),  // ;
//       Token(99, "a", 2, 12),  // Identificador
//       Token(13, ">", 2, 14),  // >
//       Token(104, "0", 2, 15), // Número
//       Token(65, ";", 2, 16),  // ;
//       Token(99, "a", 2, 18),  // Identificador
//       Token(26, "-=", 2, 20), // -=
//       Token(104, "1", 2, 23), // Número
//       Token(62, ")", 2, 24),  // )
//       Token(63, "{", 3, 1),   // {
//       Token(64, "}", 4, 1)    // }
//   };

//   try
//   {
//     program(tokens);
//     cout << "Analise sintatica concluída com sucesso!" << endl;
//   }
//   catch (const runtime_error &e)
//   {
//     cerr << e.what() << endl;
//   }

//   return 0;
// }