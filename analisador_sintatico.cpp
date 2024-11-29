#include <iostream>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include "analisesintatica.h"
using namespace std;

Token nextToken(const vector<Token> &tokens, int &tokenIndex)
{
  if (tokenIndex >= tokens.size())
    throw runtime_error("Fim inesperado do código.");
  return tokens[tokenIndex++];
}

Token peekToken(const vector<Token> &tokens, int &tokenIndex)
{
  if (tokenIndex >= tokens.size())
    throw runtime_error("Fim inesperado do código.");
  return tokens[tokenIndex];
}
void erroSintaxe(const Token &token, const string &contexto)
{
  throw runtime_error("Erro de sintaxe no contexto " + contexto + token.lexema +
                      " na linha " + to_string(token.linha) +
                      ", coluna " + to_string(token.coluna) + ".");
}

// Função principal
void program(const vector<Token> &tokens, int &tokenIndex)
{
  Token token = nextToken(tokens, tokenIndex);
  // Referente ao <type>
  if (token.tipo != 31 && token.tipo != 32 && token.tipo != 33) // int, float, string
    erroSintaxe(token, "<program>. Esperava um <type> e recebeu um ");

  token = nextToken(tokens, tokenIndex);
  if (token.lexema != "main")
    erroSintaxe(token, "<program>. Esperava main e recebeu um ");

  token = nextToken(tokens, tokenIndex);
  if (token.tipo != 61) // '('
    erroSintaxe(token, "<program>. Esperava '(' e recebeu um ");

  token = nextToken(tokens, tokenIndex);
  if (token.tipo != 62) // ')'
    erroSintaxe(token, "<program>. Esperava ')' e recebeu um ");

  block(tokens, tokenIndex); // Processa o bloco principal
}

void block(const vector<Token> &tokens, int &tokenIndex)
{
  Token token = nextToken(tokens, tokenIndex);
  if (token.tipo != 63) // '{'
    erroSintaxe(token, "<block>. Esperava '{' e recebeu um ");

  stmtList(tokens, tokenIndex);

  token = nextToken(tokens, tokenIndex);
  if (token.tipo != 64) // '}'
    erroSintaxe(token, "<block>. Esperava '}' e recebeu um ");
}

void stmtList(const vector<Token> &tokens, int &tokenIndex)
{
  if (tokenIndex >= tokens.size() || peekToken(tokens, tokenIndex).tipo == 64) // Verifica '}' como marcador de fim
  {
    return; // Caso vazio
  }
  stmt(tokens, tokenIndex);
  stmtList(tokens, tokenIndex); 
}

void stmt(const std::vector<Token> &tokens, int &tokenIndex)
{
  Token token = peekToken(tokens, tokenIndex);
  if (token.tipo == 43)
  { // for
    forStmt(tokens, tokenIndex);
  }
  else if (token.tipo == 51)
  { // system (in/out)
    ioStmt(tokens, tokenIndex);
  }
  else if (token.tipo == 44)
  { // while
    whileStmt(tokens, tokenIndex);
  }
  else if (token.tipo == 99)
  { // Identificador (atribuição)
    atrib(tokens, tokenIndex);
    token = nextToken(tokens, tokenIndex);
    if (token.tipo != 65) // ';'
      erroSintaxe(token, "<stmt>. Esperava ';', recebeu um ");
  }
  else if (token.tipo == 41)
  { // if
    ifStmt(tokens, tokenIndex);
  }
  else if (token.tipo == 63)
  { // '{'
    block(tokens, tokenIndex);
  }
  else if (token.tipo == 45 || token.tipo == 46)
  { // break/continue
    nextToken(tokens, tokenIndex);
  }
  else if (token.tipo == 31 || token.tipo == 32 || token.tipo == 33)
  { // Declaração
    declaration(tokens, tokenIndex);
  }
  else if (token.tipo == 65)
  { // ';' (stmt vazio)
    nextToken(tokens, tokenIndex);
  }
  else
  {
    erroSintaxe(token, "<stmt>");
  }
}

void declaration(const std::vector<Token> &tokens, int &tokenIndex)
{
  Token token = nextToken(tokens, tokenIndex);
  //<type>
  if (token.tipo != 31 && token.tipo != 32 && token.tipo != 33) // int, float, string
    erroSintaxe(token, "<declaration>. Esperava um <type> e recebeu um ");
  identList(tokens, tokenIndex);
  token = nextToken(tokens, tokenIndex);
  if (token.tipo != 65) // ';'
    erroSintaxe(token, "<declaration>. Esperava um ';' e recebeu um ");
}

void identList(const vector<Token> &tokens, int &tokenIndex)
{
  Token token = nextToken(tokens, tokenIndex);
  if (token.tipo != 99) // IDENT
    erroSintaxe(token, "<identList>. Esperava um identificador e recebeu um ");
  while (peekToken(tokens, tokenIndex).tipo == 66)
  { // ','
    nextToken(tokens, tokenIndex);
    token = nextToken(tokens, tokenIndex);
    if (token.tipo != 99) // IDENT
      erroSintaxe(token, "<identList>. Esperava um identificador e recebeu um ");
  }
}

void forStmt(const vector<Token> &tokens, int &tokenIndex)
{
  Token token = nextToken(tokens, tokenIndex);
  if (token.tipo != 43) // for
    erroSintaxe(token, "<forStmt>. Esperava um 'for', recebeu um ");

  token = nextToken(tokens, tokenIndex);
  if (token.tipo != 61) // '('
    erroSintaxe(token, "<forStmt>. Esperava um '(', recebeu um ");

  if (peekToken(tokens, tokenIndex).tipo == 99)
  {
    atrib(tokens, tokenIndex);
  }
  token = nextToken(tokens, tokenIndex);
  if (token.tipo != 65) // ';'
    erroSintaxe(token, "<forStmt>. Esperava um ';', recebeu um ");

  if (peekToken(tokens, tokenIndex).tipo != 65)
  { // Se não for ';', espera expressão
    expr(tokens, tokenIndex);
  }
  token = nextToken(tokens, tokenIndex);
  if (token.tipo != 65) // ';'
    erroSintaxe(token, "<forStmt>. Esperava um ';', recebeu um ");

  if (peekToken(tokens, tokenIndex).tipo == 99)
  { // Incremento (opcional)
    atrib(tokens, tokenIndex);
  }
  token = nextToken(tokens, tokenIndex);
  if (token.tipo != 62) // ')'
    erroSintaxe(token, "<forStmt>. Esperava um ')', recebeu um ");

  stmt(tokens, tokenIndex);
}

void ifStmt(const vector<Token> &tokens, int &tokenIndex)
{
  Token token = nextToken(tokens, tokenIndex);
  if (token.tipo != 41) // 'if'
    erroSintaxe(token, "<ifStmt>. Esperava um 'if', recebeu um ");

  token = nextToken(tokens, tokenIndex);
  if (token.tipo != 61) // '('
    erroSintaxe(token, "<ifStmt>. Esperava um '(', recebeu um ");

  expr(tokens, tokenIndex); // Processa a expressão dentro do 'if'

  token = nextToken(tokens, tokenIndex);
  if (token.tipo != 62) // ')'
    erroSintaxe(token, "<ifStmt>. Esperava um ')', recebeu um ");

  stmt(tokens, tokenIndex); // Processa a instrução do 'if'

  if (peekToken(tokens, tokenIndex).tipo == 42) // 'else'
  {
    nextToken(tokens, tokenIndex);
    stmt(tokens, tokenIndex); // Processa a instrução do 'else'
  }
}

void ioStmt(const vector<Token> &tokens, int &tokenIndex)
{
  Token token = nextToken(tokens, tokenIndex);
  if (token.tipo != 51) // 'system'
    erroSintaxe(token, "<ioStmt>. Esperava um 'system', recebeu um ");

  token = nextToken(tokens, tokenIndex);
  if (token.tipo != 67) // '.'
    erroSintaxe(token, "<ioStmt>. Esperava um '.', recebeu um ");

  token = nextToken(tokens, tokenIndex);
  if (token.tipo == 55) // 'in'
  {
    token = nextToken(tokens, tokenIndex);
    if (token.tipo != 67) // '.'
      erroSintaxe(token, "<ioStmt>. Esperava um '.', recebeu um ");

    token = nextToken(tokens, tokenIndex);
    if (token.tipo != 56) // 'scan'
      erroSintaxe(token, "<ioStmt>. Esperava um 'scan', recebeu um ");

    token = nextToken(tokens, tokenIndex);
    if (token.tipo != 61) // '('
      erroSintaxe(token, "<ioStmt>. Esperava um '(', recebeu um ");

    // Verifica o tipo 
    token = nextToken(tokens, tokenIndex);
    if ((token.tipo != 31 && token.tipo != 32 && token.tipo != 33)) // Tipo, <type>: 'int', 'float' e 'string'
      erroSintaxe(token, "<ioStmt>. Esperava um tipo, recebeu um ");

    token = nextToken(tokens, tokenIndex); // ','
    if (token.tipo != 66)
      erroSintaxe(token, "<ioStmt>. Esperava uma ',', recebeu um ");

    // Verifica o IDENT
    token = nextToken(tokens, tokenIndex);
    if (token.tipo != 99) // IDENT
      erroSintaxe(token, "<ioStmt>. Esperava um 'IDENT', recebeu um ");

    token = nextToken(tokens, tokenIndex);
    if (token.tipo != 62) // ')'
      erroSintaxe(token, "<ioStmt>. Esperava um ')', recebeu um ");

    token = nextToken(tokens, tokenIndex);
    if (token.tipo != 65) // ';'
      erroSintaxe(token, "<ioStmt>. Esperava um ';', recebeu um ");
  }
  else if (token.tipo == 52) // 'out'
  {
    token = nextToken(tokens, tokenIndex);
    if (token.tipo != 67) // '.'
      erroSintaxe(token, "<ioStmt>. Esperava um '.', recebeu um ");

    token = nextToken(tokens, tokenIndex);
    if (token.tipo != 53) // 'print'
      erroSintaxe(token, "<ioStmt>. Esperava um 'print', recebeu um ");

    token = nextToken(tokens, tokenIndex);
    if (token.tipo != 61) // '('
      erroSintaxe(token, "<ioStmt>. Esperava um '(', recebeu um ");

    outList(tokens, tokenIndex);

    token = nextToken(tokens, tokenIndex);
    if (token.tipo != 62) // ')'
      erroSintaxe(token, "<ioStmt>. Esperava um ')', recebeu um ");

    token = nextToken(tokens, tokenIndex);
    if (token.tipo != 65) // ';'
      erroSintaxe(token, "<ioStmt>. Esperava um ';', recebeu um ");
  }
  else
  {
    erroSintaxe(token, "<ioStmt>. Esperava um 'in' ou 'out', recebeu um ");
  }
}

void outList(const vector<Token> &tokens, int &tokenIndex)
{
  Token token = nextToken(tokens, tokenIndex);
  // Verifica 'STR', 'IDENT', 'NUMint', 'NUMdec', 'NUMfloat', 'NUMoct', 'NUMhex'
  if (token.tipo != 6 && token.tipo != 99 && token.tipo != 101 && token.tipo != 102 && token.tipo != 103 && token.tipo != 104)
    erroSintaxe(token, "<outList>. Esperava um 'STR', 'IDENT', 'NUMint', 'NUMdec', 'NUMfloat', 'NUMoct' ou 'NUMhex', recebeu um ");

  token = peekToken(tokens, tokenIndex);
  if (token.tipo == 66) // Se for uma vírgula (',')
  {
    nextToken(tokens, tokenIndex);
    outList(tokens, tokenIndex);
  }
}

void whileStmt(const vector<Token> &tokens, int &tokenIndex)
{
  Token token = nextToken(tokens, tokenIndex);
  if (token.tipo != 44) // while
    erroSintaxe(token, "<whileStmt>. Esperava um 'while', recebeu um ");

  token = nextToken(tokens, tokenIndex);
  if (token.tipo != 61) // '('
    erroSintaxe(token, "<whileStmt>. Esperava um '(', recebeu um ");

  expr(tokens, tokenIndex);

  token = nextToken(tokens, tokenIndex);
  if (token.tipo != 62) // ')'
    erroSintaxe(token, "<whileStmt>. Esperava um ')', recebeu um ");

  stmt(tokens, tokenIndex);
}

void atrib(const vector<Token> &tokens, int &tokenIndex)
{
  Token token = nextToken(tokens, tokenIndex);
  if (token.tipo != 99) // IDENT
    erroSintaxe(token, "<atrib>. Esperava uma variavel, recebeu um ");

  token = nextToken(tokens, tokenIndex);
  if (token.tipo < 24 || token.tipo > 29) // Operadores de atribuição
    erroSintaxe(token, "<atrib>. Esperava um operador, recebeu um ");

  expr(tokens, tokenIndex);
}

void expr(const vector<Token> &tokens, int &tokenIndex)
{
  orExpr(tokens, tokenIndex);
}

void orExpr(const vector<Token> &tokens, int &tokenIndex)
{
  andExpr(tokens, tokenIndex);
  while (peekToken(tokens, tokenIndex).tipo == 21) // '||'
  {
    nextToken(tokens, tokenIndex);
    andExpr(tokens, tokenIndex);
  }
}

void andExpr(const vector<Token> &tokens, int &tokenIndex)
{
  notExpr(tokens, tokenIndex);
  while (peekToken(tokens, tokenIndex).tipo == 22) // '&&'
  {
    nextToken(tokens, tokenIndex);
    notExpr(tokens, tokenIndex);
  }
}

void notExpr(const vector<Token> &tokens, int &tokenIndex)
{
  if (peekToken(tokens, tokenIndex).tipo == 23) // '!'
  {
    nextToken(tokens, tokenIndex);
    notExpr(tokens, tokenIndex);
  }
  else
  {
    relExpr(tokens, tokenIndex);
  }
}

void relExpr(const vector<Token> &tokens, int &tokenIndex)
{
  addExpr(tokens, tokenIndex);
  while (peekToken(tokens, tokenIndex).tipo >= 11 && peekToken(tokens, tokenIndex).tipo <= 16) // Operadores relacionais
  {
    nextToken(tokens, tokenIndex);
    addExpr(tokens, tokenIndex);
  }
}

void addExpr(const vector<Token> &tokens, int &tokenIndex)
{
  multExpr(tokens, tokenIndex);
  while (peekToken(tokens, tokenIndex).tipo == 1 || peekToken(tokens, tokenIndex).tipo == 2) // '+' ou '-'
  {
    nextToken(tokens, tokenIndex);
    multExpr(tokens, tokenIndex);
  }
}

void multExpr(const vector<Token> &tokens, int &tokenIndex)
{
  unaryExpr(tokens, tokenIndex);
  while (peekToken(tokens, tokenIndex).tipo == 3 || peekToken(tokens, tokenIndex).tipo == 4 || peekToken(tokens, tokenIndex).tipo == 5) // '*' ou '/' ou '%'
  {
    nextToken(tokens, tokenIndex);
    unaryExpr(tokens, tokenIndex);
  }
}

void unaryExpr(const vector<Token> &tokens, int &tokenIndex)
{
  if (peekToken(tokens, tokenIndex).tipo == 1 || peekToken(tokens, tokenIndex).tipo == 2) // '+' ou '-'
  {
    nextToken(tokens, tokenIndex);
    unaryExpr(tokens, tokenIndex);
  }
  else
  {
    fator(tokens, tokenIndex);
  }
}

void fator(const vector<Token> &tokens, int &tokenIndex)
{
  Token token = nextToken(tokens, tokenIndex);
  if (token.tipo == 104 || token.tipo == 103 || token.tipo == 102 || token.tipo == 101 || token.tipo == 99 || token.tipo == 6) // Números, IDENT, STR
    return;
  if (token.tipo == 61) // '('
  {
    expr(tokens, tokenIndex);
    token = nextToken(tokens, tokenIndex);
    if (token.tipo != 62) // ')'
      erroSintaxe(token, "<fator>. Esperava ')', recebeu um ");
  }
  else
  {
    erroSintaxe(token, "<fator>. Token recebido: ");
  }
}
