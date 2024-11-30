#include <iostream>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include "analisesintatica.h"
#include "TokenTypes.h"

using namespace std;

Token nextToken(const vector<Token> &tokens, int &tokenIndex)
{
  if (tokenIndex >= tokens.size())
    throw runtime_error("Fim inesperado do codigo.");
  return tokens[tokenIndex++];
}

Token peekToken(const vector<Token> &tokens, int &tokenIndex)
{
  if (tokenIndex >= tokens.size())
    throw runtime_error("Fim inesperado do codigo.");
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
  if (token.tipo != TOKEN_INT && token.tipo != TOKEN_FLOAT && token.tipo != TOKEN_STRING) // int, float, string
    erroSintaxe(token, "<program>. Esperava um tipo e recebeu um ");

  token = nextToken(tokens, tokenIndex);
  if (token.tipo != TOKEN_IDENTIFIER) // IDENT
    erroSintaxe(token, "<program>. Esperava IDENT e recebeu um ");

  token = nextToken(tokens, tokenIndex);
  if (token.tipo != TOKEN_OPEN_PAREN) // '('
    erroSintaxe(token, "<program>. Esperava '(' e recebeu um ");

  token = nextToken(tokens, tokenIndex);
  if (token.tipo != TOKEN_CLOSE_PAREN) // ')'
    erroSintaxe(token, "<program>. Esperava ')' e recebeu um ");

  block(tokens, tokenIndex);

  if (tokenIndex < tokens.size())
  {
    Token restante = nextToken(tokens, tokenIndex);
    erroSintaxe(restante, "<program>. Codigo inesperado encontrado apos o termino do programa. Token: ");
  }
}

void block(const vector<Token> &tokens, int &tokenIndex)
{
  Token token = nextToken(tokens, tokenIndex);
  if (token.tipo != TOKEN_OPEN_BRACE) // '{'
    erroSintaxe(token, "<block>. Esperava '{' e recebeu um ");

  stmtList(tokens, tokenIndex);

  token = nextToken(tokens, tokenIndex);
  if (token.tipo != TOKEN_CLOSE_BRACE) // '}'
    erroSintaxe(token, "<block>. Esperava '}' e recebeu um ");
}

void stmtList(const vector<Token> &tokens, int &tokenIndex)
{
  if (tokenIndex >= tokens.size() || peekToken(tokens, tokenIndex).tipo == TOKEN_CLOSE_BRACE) // Verifica '}' como marcador de fim
  {
    return; // Caso vazio
  }
  stmt(tokens, tokenIndex);
  stmtList(tokens, tokenIndex);
}

void stmt(const std::vector<Token> &tokens, int &tokenIndex)
{
  Token token = peekToken(tokens, tokenIndex);
  if (token.tipo == TOKEN_FOR)
  { // for
    forStmt(tokens, tokenIndex);
  }
  else if (token.tipo == TOKEN_SYSTEM)
  { // system (in/out)
    ioStmt(tokens, tokenIndex);
  }
  else if (token.tipo == TOKEN_WHILE)
  { // while
    whileStmt(tokens, tokenIndex);
  }
  else if (token.tipo == TOKEN_IDENTIFIER)
  { // Identificador (atribuição)
    atrib(tokens, tokenIndex);
    token = nextToken(tokens, tokenIndex);
    if (token.tipo != TOKEN_SEMICOLON) // ';'
      erroSintaxe(token, "<stmt>. Esperava ';', recebeu um ");
  }
  else if (token.tipo == TOKEN_IF)
  { // if
    ifStmt(tokens, tokenIndex);
  }
  else if (token.tipo == TOKEN_OPEN_BRACE)
  { // '{'
    block(tokens, tokenIndex);
  }
  else if (token.tipo == TOKEN_BREAK || token.tipo == TOKEN_CONTINUE)
  { // break/continue
    nextToken(tokens, tokenIndex);
  }
  else if (token.tipo == TOKEN_INT || token.tipo == TOKEN_FLOAT || token.tipo == TOKEN_STRING)
  { // Declaração
    declaration(tokens, tokenIndex);
  }
  else if (token.tipo == TOKEN_SEMICOLON)
  { // ';' (stmt vazio)
    nextToken(tokens, tokenIndex);
  }
  else
  {
    erroSintaxe(token, "<stmt>. Esperava um <stmt>, recebeu um: ");
  }
}

void declaration(const std::vector<Token> &tokens, int &tokenIndex)
{
  Token token = nextToken(tokens, tokenIndex);
  //<type>
  if (token.tipo != TOKEN_INT && token.tipo != TOKEN_FLOAT && token.tipo != TOKEN_STRING) // int, float, string
    erroSintaxe(token, "<declaration>. Esperava um <type> e recebeu um ");
  identList(tokens, tokenIndex);
  token = nextToken(tokens, tokenIndex);
  if (token.tipo != TOKEN_SEMICOLON) // ';'
    erroSintaxe(token, "<declaration>. Esperava um ';' e recebeu um ");
}

void identList(const vector<Token> &tokens, int &tokenIndex)
{
  Token token = nextToken(tokens, tokenIndex);
  if (token.tipo != TOKEN_IDENTIFIER) // IDENT
    erroSintaxe(token, "<identList>. Esperava um identificador e recebeu um ");
  while (peekToken(tokens, tokenIndex).tipo == TOKEN_COMMA)
  { // ','
    nextToken(tokens, tokenIndex);
    token = nextToken(tokens, tokenIndex);
    if (token.tipo != TOKEN_IDENTIFIER) // IDENT
      erroSintaxe(token, "<identList>. Esperava um identificador e recebeu um ");
  }
}

void forStmt(const vector<Token> &tokens, int &tokenIndex)
{
  Token token = nextToken(tokens, tokenIndex);
  if (token.tipo != TOKEN_FOR) // for
    erroSintaxe(token, "<forStmt>. Esperava um 'for', recebeu um ");

  token = nextToken(tokens, tokenIndex);
  if (token.tipo != TOKEN_OPEN_PAREN) // '('
    erroSintaxe(token, "<forStmt>. Esperava um '(', recebeu um ");

  if (peekToken(tokens, tokenIndex).tipo == TOKEN_IDENTIFIER)
  {
    atrib(tokens, tokenIndex);
  }
  token = nextToken(tokens, tokenIndex);
  if (token.tipo != TOKEN_SEMICOLON) // ';'
    erroSintaxe(token, "<forStmt>. Esperava um ';', recebeu um ");

  if (peekToken(tokens, tokenIndex).tipo != TOKEN_SEMICOLON)
  { // Se não for ';', espera expressão
    expr(tokens, tokenIndex);
  }
  token = nextToken(tokens, tokenIndex);
  if (token.tipo != TOKEN_SEMICOLON) // ';'
    erroSintaxe(token, "<forStmt>. Esperava um ';', recebeu um ");

  if (peekToken(tokens, tokenIndex).tipo == TOKEN_IDENTIFIER)
  { // Incremento (opcional)
    atrib(tokens, tokenIndex);
  }
  token = nextToken(tokens, tokenIndex);
  if (token.tipo != TOKEN_CLOSE_PAREN) // ')'
    erroSintaxe(token, "<forStmt>. Esperava um ')', recebeu um ");

  stmt(tokens, tokenIndex);
}

void ifStmt(const vector<Token> &tokens, int &tokenIndex)
{
  Token token = nextToken(tokens, tokenIndex);
  if (token.tipo != TOKEN_IF) // 'if'
    erroSintaxe(token, "<ifStmt>. Esperava um 'if', recebeu um ");

  token = nextToken(tokens, tokenIndex);
  if (token.tipo != TOKEN_OPEN_PAREN) // '('
    erroSintaxe(token, "<ifStmt>. Esperava um '(', recebeu um ");

  expr(tokens, tokenIndex); // Processa a expressão dentro do 'if'

  token = nextToken(tokens, tokenIndex);
  if (token.tipo != TOKEN_CLOSE_PAREN) // ')'
    erroSintaxe(token, "<ifStmt>. Esperava um ')', recebeu um ");

  stmt(tokens, tokenIndex); // Processa a instrução do 'if'

  if (peekToken(tokens, tokenIndex).tipo == TOKEN_ELSE) // 'else'
  {
    nextToken(tokens, tokenIndex);
    stmt(tokens, tokenIndex); // Processa a instrução do 'else'
  }
}

void ioStmt(const vector<Token> &tokens, int &tokenIndex)
{
  Token token = nextToken(tokens, tokenIndex);
  if (token.tipo != TOKEN_SYSTEM) // 'system'
    erroSintaxe(token, "<ioStmt>. Esperava um 'system', recebeu um ");

  token = nextToken(tokens, tokenIndex);
  if (token.tipo != TOKEN_DOT) // '.'
    erroSintaxe(token, "<ioStmt>. Esperava um '.', recebeu um ");

  token = nextToken(tokens, tokenIndex);
  if (token.tipo == TOKEN_IN) // 'in'
  {
    token = nextToken(tokens, tokenIndex);
    if (token.tipo != TOKEN_DOT) // '.'
      erroSintaxe(token, "<ioStmt>. Esperava um '.', recebeu um ");

    token = nextToken(tokens, tokenIndex);
    if (token.tipo != TOKEN_SCAN) // 'scan'
      erroSintaxe(token, "<ioStmt>. Esperava um 'scan', recebeu um ");

    token = nextToken(tokens, tokenIndex);
    if (token.tipo != TOKEN_OPEN_PAREN) // '('
      erroSintaxe(token, "<ioStmt>. Esperava um '(', recebeu um ");

    // Verifica o tipo (adicionando a verificação do <type>)
    token = nextToken(tokens, tokenIndex);
    if ((token.tipo != TOKEN_INT && token.tipo != TOKEN_FLOAT && token.tipo != TOKEN_STRING)) // Tipo, <type>: 'int', 'float' e 'string'
      erroSintaxe(token, "<ioStmt>. Esperava um tipo, recebeu um ");

    token = nextToken(tokens, tokenIndex); // ','
    if (token.tipo != TOKEN_COMMA)
      erroSintaxe(token, "<ioStmt>. Esperava uma ',', recebeu um ");

    // Verifica o IDENT
    token = nextToken(tokens, tokenIndex);
    if (token.tipo != TOKEN_IDENTIFIER) // IDENT
      erroSintaxe(token, "<ioStmt>. Esperava um 'IDENT', recebeu um ");

    token = nextToken(tokens, tokenIndex);
    if (token.tipo != TOKEN_CLOSE_PAREN) // ')'
      erroSintaxe(token, "<ioStmt>. Esperava um ')', recebeu um ");

    token = nextToken(tokens, tokenIndex);
    if (token.tipo != TOKEN_SEMICOLON) // ';'
      erroSintaxe(token, "<ioStmt>. Esperava um ';', recebeu um ");
  }
  else if (token.tipo == TOKEN_OUT) // 'out'
  {
    token = nextToken(tokens, tokenIndex);
    if (token.tipo != TOKEN_DOT) // '.'
      erroSintaxe(token, "<ioStmt>. Esperava um '.', recebeu um ");

    token = nextToken(tokens, tokenIndex);
    if (token.tipo != TOKEN_PRINT) // 'print'
      erroSintaxe(token, "<ioStmt>. Esperava um 'print', recebeu um ");

    token = nextToken(tokens, tokenIndex);
    if (token.tipo != TOKEN_OPEN_PAREN) // '('
      erroSintaxe(token, "<ioStmt>. Esperava um '(', recebeu um ");

    outList(tokens, tokenIndex);

    token = nextToken(tokens, tokenIndex);
    if (token.tipo != TOKEN_CLOSE_PAREN) // ')'
      erroSintaxe(token, "<ioStmt>. Esperava um ')', recebeu um ");

    token = nextToken(tokens, tokenIndex);
    if (token.tipo != TOKEN_SEMICOLON) // ';'
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
  if (token.tipo != TOKEN_STR_LITERAL && token.tipo != TOKEN_IDENTIFIER && token.tipo != TOKEN_HEX && token.tipo != TOKEN_OCTAL && token.tipo != TOKEN_FLOAT_LITERAL && token.tipo != TOKEN_INTEGER_LITERAL)
    erroSintaxe(token, "<outList>. Esperava um 'STR', 'IDENT', 'NUMint', 'NUMdec', 'NUMfloat', 'NUMoct' ou 'NUMhex', recebeu um ");

  token = peekToken(tokens, tokenIndex);
  if (token.tipo == TOKEN_COMMA) // Se for uma vírgula (',')
  {
    nextToken(tokens, tokenIndex);
    outList(tokens, tokenIndex);
  }
}

void whileStmt(const vector<Token> &tokens, int &tokenIndex)
{
  Token token = nextToken(tokens, tokenIndex);
  if (token.tipo != TOKEN_WHILE) // while
    erroSintaxe(token, "<whileStmt>. Esperava um 'while', recebeu um ");

  token = nextToken(tokens, tokenIndex);
  if (token.tipo != TOKEN_OPEN_PAREN) // '('
    erroSintaxe(token, "<whileStmt>. Esperava um '(', recebeu um ");

  expr(tokens, tokenIndex);

  token = nextToken(tokens, tokenIndex);
  if (token.tipo != TOKEN_CLOSE_PAREN) // ')'
    erroSintaxe(token, "<whileStmt>. Esperava um ')', recebeu um ");

  stmt(tokens, tokenIndex);
}

void atrib(const vector<Token> &tokens, int &tokenIndex)
{
  Token token = nextToken(tokens, tokenIndex);
  if (token.tipo != TOKEN_IDENTIFIER) // IDENT
    erroSintaxe(token, "<atrib>. Esperava uma variavel, recebeu um ");

  token = nextToken(tokens, tokenIndex);
  if (token.tipo < 24 || token.tipo > 29) // Operadores de atribuição +=, -+, *=, /=, %=
    erroSintaxe(token, "<atrib>. Esperava um operador, recebeu um ");

  expr(tokens, tokenIndex);
}

void expr(const vector<Token> &tokens, int &tokenIndex)
{
  orExpr(tokens, tokenIndex); // Inicia pela produção mais alta
}

void orExpr(const vector<Token> &tokens, int &tokenIndex)
{
  andExpr(tokens, tokenIndex);
  while (peekToken(tokens, tokenIndex).tipo == TOKEN_OR) // '||'
  {
    nextToken(tokens, tokenIndex);
    andExpr(tokens, tokenIndex);
  }
}

void andExpr(const vector<Token> &tokens, int &tokenIndex)
{
  notExpr(tokens, tokenIndex);
  while (peekToken(tokens, tokenIndex).tipo == TOKEN_AND) // '&&'
  {
    nextToken(tokens, tokenIndex);
    notExpr(tokens, tokenIndex);
  }
}

void notExpr(const vector<Token> &tokens, int &tokenIndex)
{
  if (peekToken(tokens, tokenIndex).tipo == TOKEN_NOT) // '!'
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
  while (peekToken(tokens, tokenIndex).tipo == TOKEN_PLUS || peekToken(tokens, tokenIndex).tipo == TOKEN_MINUS) // '+' ou '-'
  {
    nextToken(tokens, tokenIndex);
    multExpr(tokens, tokenIndex);
  }
}

void multExpr(const vector<Token> &tokens, int &tokenIndex)
{
  unaryExpr(tokens, tokenIndex);
  while (peekToken(tokens, tokenIndex).tipo == TOKEN_MULTIPLY || peekToken(tokens, tokenIndex).tipo == TOKEN_DIVIDE || peekToken(tokens, tokenIndex).tipo == TOKEN_MODULO) // '*' ou '/' ou '%'
  {
    nextToken(tokens, tokenIndex);
    unaryExpr(tokens, tokenIndex);
  }
}

void unaryExpr(const vector<Token> &tokens, int &tokenIndex)
{
  if (peekToken(tokens, tokenIndex).tipo == TOKEN_PLUS || peekToken(tokens, tokenIndex).tipo == TOKEN_MINUS) // '+' ou '-'
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
  if (token.tipo == TOKEN_INTEGER_LITERAL || token.tipo == TOKEN_FLOAT_LITERAL || token.tipo == TOKEN_OCTAL || token.tipo == TOKEN_HEX || token.tipo == TOKEN_IDENTIFIER || token.tipo == TOKEN_STR_LITERAL) // Números, IDENT, STR
    return;
  if (token.tipo == TOKEN_OPEN_PAREN) // '('
  {
    expr(tokens, tokenIndex);
    token = nextToken(tokens, tokenIndex);
    if (token.tipo != TOKEN_CLOSE_PAREN) // ')'
      erroSintaxe(token, "<fator>. Esperava ')', recebeu um ");
  }
  else
  {
    erroSintaxe(token, "<fator>. Programa encerrado de maneira inesperada. Ultimo token recebido: ");
  }
}
