#include <iostream>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <fstream>
#include "analisesintatica.h"
#include "TokenTypes.h"

using namespace std;
int labelCount = 0;
int tempCount = 0;
vector<tuple<string, string, string, string>> codigoIntermediario;
unordered_set<string> variaveisDeclaradas;

string novaLabel()
{
  return "__label" + to_string(labelCount++);
}

string novaTemp()
{
  return "__temp" + to_string(tempCount++);
}

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
string formatarString(const string &valor)
{
  if (valor.empty())
  {
    return "\"\""; // Valores vazios devem ser representados como ""
  }

  string resultado = valor;
  if (resultado == "\n")
  {
    return "\n";
  }
  // Remove aspas duplas do início e fim, se existirem
  if (resultado.front() == '"' && resultado.back() == '"')
  {
    resultado = resultado.substr(1, resultado.size() - 2);
  }

  // Envolve tudo com aspas simples
  return "'" + resultado + "'";
}
void addCodigoIntermediario(const string &op, const string &arg1, const string &arg2, const string &result)
{
  codigoIntermediario.emplace_back(
      formatarString(op),
      formatarString(arg1),
      formatarString(arg2),
      formatarString(result));
}

void erroSintatico(const Token &token, const string &contexto)
{
  throw runtime_error("Erro sintatico no contexto " + contexto + token.lexema +
                      " na linha " + to_string(token.linha) +
                      ", coluna " + to_string(token.coluna) + ".");
}

void salvarCodigoIntermediario(const string &nomeArquivo)
{
  ofstream arquivo(nomeArquivo); // Abre o arquivo para escrita
  if (!arquivo)
  {
    cerr << "Erro ao abrir o arquivo para escrita!" << endl;
    return;
  }

  for (const auto &tupla : codigoIntermediario)
  {
    string op, arg1, arg2, result;
    tie(op, arg1, arg2, result) = tupla;
    arquivo << "(" << op << ", " << arg1 << ", " << arg2 << ", " << result << ")\n";
  }
  arquivo.close();
  cout << "Codigo intermediario salvo em " << nomeArquivo << endl;
}

// Função principal
void program(const vector<Token> &tokens, int &tokenIndex)
{
  Token token = nextToken(tokens, tokenIndex);
  // Referente ao <type>
  if (token.tipo != TOKEN_INT && token.tipo != TOKEN_FLOAT && token.tipo != TOKEN_STRING) // int, float, string
    erroSintatico(token, "<program>. Esperava um tipo e recebeu um ");

  token = nextToken(tokens, tokenIndex);
  if (token.tipo != TOKEN_IDENTIFIER) // IDENT
    erroSintatico(token, "<program>. Esperava IDENT e recebeu um ");

  token = nextToken(tokens, tokenIndex);
  if (token.tipo != TOKEN_OPEN_PAREN) // '('
    erroSintatico(token, "<program>. Esperava '(' e recebeu um ");

  token = nextToken(tokens, tokenIndex);
  if (token.tipo != TOKEN_CLOSE_PAREN) // ')'
    erroSintatico(token, "<program>. Esperava ')' e recebeu um ");

  block(tokens, tokenIndex);

  if (tokenIndex < tokens.size())
  {
    Token restante = nextToken(tokens, tokenIndex);
    erroSintatico(restante, "<program>. Codigo inesperado encontrado apos o termino do programa. Token: ");
  }
}

void block(const vector<Token> &tokens, int &tokenIndex)
{
  Token token = nextToken(tokens, tokenIndex);
  if (token.tipo != TOKEN_OPEN_BRACE) // '{'
    erroSintatico(token, "<block>. Esperava '{' e recebeu um ");

  stmtList(tokens, tokenIndex);

  token = nextToken(tokens, tokenIndex);
  if (token.tipo != TOKEN_CLOSE_BRACE) // '}'
    erroSintatico(token, "<block>. Esperava '}' e recebeu um ");
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
      erroSintatico(token, "<stmt>. Esperava ';', recebeu um ");
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
    erroSintatico(token, "<stmt>. Esperava um <stmt>, recebeu um: ");
  }
}

void declaration(const vector<Token> &tokens, int &tokenIndex)
{
  Token tipo = nextToken(tokens, tokenIndex);
  if (tipo.tipo != TOKEN_INT && tipo.tipo != TOKEN_FLOAT && tipo.tipo != TOKEN_STRING)
  {
    erroSintatico(tipo, "<declaration>. Esperava um <type> e recebeu um ");
  }

  vector<string> variaveis;
  while (true)
  {
    Token ident = nextToken(tokens, tokenIndex);
    if (ident.tipo != TOKEN_IDENTIFIER)
    {
      erroSintatico(ident, "<declaration>. Esperava um IDENT, recebeu um ");
    }

    string varName = ident.lexema;

    if (variaveisDeclaradas.count(varName))
    {
      cerr << "Erro: Variavel '" << varName << "' ja declarada anteriormente."
           << " Linha: " << ident.linha << ", Coluna: " << ident.coluna << endl;
      exit(EXIT_FAILURE);
    }

    for (int i = 0; i < tokenIndex - 1; ++i)
    {
      if (tokens[i].tipo == TOKEN_IDENTIFIER && tokens[i].lexema == varName &&
          !variaveisDeclaradas.count(varName))
      {
        cerr << "Erro: Variável '" << varName << "' foi usada antes de ser declarada. "
             << "Linha: " << tokens[i].linha << ", Coluna: " << tokens[i].coluna << endl;
        exit(EXIT_FAILURE);
      }
    }

    variaveisDeclaradas.insert(varName);
    variaveis.push_back(varName);

    if (peekToken(tokens, tokenIndex).tipo != TOKEN_COMMA)
      break;

    nextToken(tokens, tokenIndex); // Consome a vírgula
  }

  Token pontoVirgula = nextToken(tokens, tokenIndex);
  if (pontoVirgula.tipo != TOKEN_SEMICOLON)
  {
    erroSintatico(pontoVirgula, "<declaration>. Esperava ';', recebeu um ");
  }

  string valorInicial = (tipo.tipo == TOKEN_STRING) ? "''" : "0";
  for (const auto &var : variaveis)
  {
    addCodigoIntermediario("=", var, valorInicial, "");
  }
}

void identList(const vector<Token> &tokens, int &tokenIndex)
{
  Token token = nextToken(tokens, tokenIndex);
  if (token.tipo != TOKEN_IDENTIFIER) // IDENT
    erroSintatico(token, "<identList>. Esperava um identificador e recebeu um ");

  while (peekToken(tokens, tokenIndex).tipo == TOKEN_COMMA)
  { // ','
    nextToken(tokens, tokenIndex);
    token = nextToken(tokens, tokenIndex);
    if (token.tipo != TOKEN_IDENTIFIER) // IDENT
      erroSintatico(token, "<identList>. Esperava um identificador e recebeu um ");
  }
}
void forStmt(const vector<Token> &tokens, int &tokenIndex)
{
  nextToken(tokens, tokenIndex); // Consome 'for'
  nextToken(tokens, tokenIndex); // Consome '('

  if (peekToken(tokens, tokenIndex).tipo == TOKEN_IDENTIFIER)
    atrib(tokens, tokenIndex);

  nextToken(tokens, tokenIndex); // Consome ';'

  string labelInicio = novaLabel();
  string labelCorpo = novaLabel();
  string labelIncremento = novaLabel();
  string labelFim = novaLabel();

  addCodigoIntermediario("LABEL", labelInicio, "", ""); // Início da condição

  string condicao = expr(tokens, tokenIndex);
  nextToken(tokens, tokenIndex); // Consome ';'

  addCodigoIntermediario("IF", condicao, labelCorpo, labelFim);
  addCodigoIntermediario("LABEL", labelIncremento, "", "");
  string incremento = expr(tokens, tokenIndex);
  nextToken(tokens, tokenIndex);
  addCodigoIntermediario("EXPR", incremento, "", "");
  addCodigoIntermediario("JUMP", labelInicio, "", "");
  addCodigoIntermediario("LABEL", labelCorpo, "", "");
  stmt(tokens, tokenIndex);
  addCodigoIntermediario("JUMP", labelIncremento, "", "");
  addCodigoIntermediario("LABEL", labelFim, "", "");
}

void ifStmt(const vector<Token> &tokens, int &tokenIndex)
{
  nextToken(tokens, tokenIndex);
  nextToken(tokens, tokenIndex);

  string condicao = expr(tokens, tokenIndex);
  nextToken(tokens, tokenIndex);

  string labelTrue = novaLabel();
  string labelFalse = novaLabel();
  string labelEnd = novaLabel();

  // IF com duas labels: (condição, labelTrue, labelFalse)
  addCodigoIntermediario("IF", condicao, labelTrue, labelFalse);

  addCodigoIntermediario("LABEL", labelTrue, "", "");
  stmt(tokens, tokenIndex);
  addCodigoIntermediario("JUMP", labelEnd, "", "");

  addCodigoIntermediario("LABEL", labelFalse, "", "");
  if (tokens[tokenIndex].lexema == "else")
  {
    nextToken(tokens, tokenIndex);
    stmt(tokens, tokenIndex);
  }

  addCodigoIntermediario("LABEL", labelEnd, "", "");
}

void ioStmt(const vector<Token> &tokens, int &tokenIndex)
{
  Token token = nextToken(tokens, tokenIndex);
  if (token.tipo != TOKEN_SYSTEM)
  {
    erroSintatico(token, "<ioStmt>. Esperava 'system', recebeu um ");
  }

  token = nextToken(tokens, tokenIndex);
  if (token.tipo != TOKEN_DOT)
  {
    erroSintatico(token, "<ioStmt>. Esperava '.', recebeu um ");
  }

  token = nextToken(tokens, tokenIndex);
  if (token.tipo == TOKEN_IN)
  {
    // system.in.scan
    token = nextToken(tokens, tokenIndex);
    if (token.tipo != TOKEN_DOT)
      erroSintatico(token, "<ioStmt>. Esperava um '.', recebeu um ");

    Token scanToken = nextToken(tokens, tokenIndex);
    if (scanToken.tipo != TOKEN_SCAN)
    {
      erroSintatico(scanToken, "<ioStmt>. Esperava 'scan', recebeu um ");
    }

    Token abrePar = nextToken(tokens, tokenIndex);
    if (abrePar.tipo != TOKEN_OPEN_PAREN)
    {
      erroSintatico(abrePar, "<ioStmt>. Esperava '(', recebeu um ");
    }

    Token tipo = nextToken(tokens, tokenIndex);
    if (tipo.tipo != TOKEN_INT && tipo.tipo != TOKEN_FLOAT && tipo.tipo != TOKEN_STRING)
    {
      erroSintatico(tipo, "<ioStmt>. Esperava um tipo, recebeu um ");
    }

    Token comma = nextToken(tokens, tokenIndex);
    if (comma.tipo != TOKEN_COMMA)
    {
      erroSintatico(comma, "<ioStmt>. Esperava ',', recebeu um ");
    }

    Token var = nextToken(tokens, tokenIndex);
    if (var.tipo != TOKEN_IDENTIFIER)
    {
      erroSintatico(var, "<ioStmt>. Esperava um IDENT, recebeu um ");
    }

    Token fechaPar = nextToken(tokens, tokenIndex);
    if (fechaPar.tipo != TOKEN_CLOSE_PAREN)
    {
      erroSintatico(fechaPar, "<ioStmt>. Esperava ')', recebeu um ");
    }

    Token pontoVirgula = nextToken(tokens, tokenIndex);
    if (pontoVirgula.tipo != TOKEN_SEMICOLON)
    {
      erroSintatico(pontoVirgula, "<ioStmt>. Esperava ';', recebeu um ");
    }

    // Geração do código intermediário para entrada
    addCodigoIntermediario("CALL", "SCAN", "", var.lexema);
  }
  else if (token.tipo == TOKEN_OUT)
  {
    // system.out.print
    token = nextToken(tokens, tokenIndex);
    if (token.tipo != TOKEN_DOT)
      erroSintatico(token, "<ioStmt>. Esperava um '.', recebeu um ");

    Token printToken = nextToken(tokens, tokenIndex);
    if (printToken.tipo != TOKEN_PRINT)
    {
      erroSintatico(printToken, "<ioStmt>. Esperava 'print', recebeu um ");
    }

    Token abrePar = nextToken(tokens, tokenIndex);
    if (abrePar.tipo != TOKEN_OPEN_PAREN)
    {
      erroSintatico(abrePar, "<ioStmt>. Esperava '(', recebeu um ");
    }

    // Lista de saída
    outList(tokens, tokenIndex);

    Token fechaPar = nextToken(tokens, tokenIndex);
    if (fechaPar.tipo != TOKEN_CLOSE_PAREN)
    {
      erroSintatico(fechaPar, "<ioStmt>. Esperava ')', recebeu um ");
    }

    Token pontoVirgula = nextToken(tokens, tokenIndex);
    if (pontoVirgula.tipo != TOKEN_SEMICOLON)
    {
      erroSintatico(pontoVirgula, "<ioStmt>. Esperava ';', recebeu um ");
    }
  }
  else
  {
    erroSintatico(token, "<ioStmt>. Esperava 'in' ou 'out', recebeu um ");
  }
}

void outList(const vector<Token> &tokens, int &tokenIndex)
{
  Token token = nextToken(tokens, tokenIndex);
  if (token.tipo != TOKEN_STR_LITERAL && token.tipo != TOKEN_IDENTIFIER && token.tipo != TOKEN_HEX && token.tipo != TOKEN_OCTAL && token.tipo != TOKEN_FLOAT_LITERAL && token.tipo != TOKEN_INTEGER_LITERAL)
  {
    erroSintatico(token, "<outList>. Esperava um STR, IDENT ou NUM, recebeu um ");
  }
  else if (token.tipo == TOKEN_IDENTIFIER)
  {
    addCodigoIntermediario("CALL", "PRINT", "", token.lexema);
  }
  else
  {
    addCodigoIntermediario("CALL", "PRINT", token.lexema, "");
  }

  // Se houver mais elementos na lista (vírgula), continua
  if (peekToken(tokens, tokenIndex).tipo == TOKEN_COMMA)
  {
    nextToken(tokens, tokenIndex);
    outList(tokens, tokenIndex);
  }
}
void whileStmt(const vector<Token> &tokens, int &tokenIndex)
{
  nextToken(tokens, tokenIndex); // Consome 'while'
  nextToken(tokens, tokenIndex); // Consome '('

  string labelInicio = novaLabel();
  string labelCorpo = novaLabel();
  string labelFim = novaLabel();

  addCodigoIntermediario("LABEL", labelInicio, "", "");

  string condicao = expr(tokens, tokenIndex);
  nextToken(tokens, tokenIndex); // Consome ')'

  addCodigoIntermediario("IF", condicao, labelCorpo, labelFim);
  addCodigoIntermediario("LABEL", labelCorpo, "", "");
  stmt(tokens, tokenIndex);
  addCodigoIntermediario("JUMP", labelInicio, "", "");

  addCodigoIntermediario("LABEL", labelFim, "", "");
}

void atrib(const vector<Token> &tokens, int &tokenIndex)
{
  Token var = nextToken(tokens, tokenIndex);
  if (var.tipo != TOKEN_IDENTIFIER)
  { // IDENT
    erroSintatico(var, "<atrib>. Esperava uma variável, recebeu um ");
  }

  Token op = nextToken(tokens, tokenIndex);
  if (op.tipo < 24 || op.tipo > 29)
  { // Operadores de atribuição +=, -=, *=, /=, %=
    erroSintatico(op, "<atrib>. Esperava um operador de atribuição, recebeu um ");
  }

  string result = expr(tokens, tokenIndex);

  addCodigoIntermediario(op.lexema, var.lexema, result, "");
}

string expr(const vector<Token> &tokens, int &tokenIndex)
{
  string left = orExpr(tokens, tokenIndex);

  while (peekToken(tokens, tokenIndex).tipo == TOKEN_OR)
  {
    Token op = nextToken(tokens, tokenIndex);
    string right = orExpr(tokens, tokenIndex);

    string temp = novaTemp();
    addCodigoIntermediario(op.lexema, temp, left, right);
    left = temp;
  }

  return left;
}

string orExpr(const vector<Token> &tokens, int &tokenIndex)
{
  string left = andExpr(tokens, tokenIndex);

  while (peekToken(tokens, tokenIndex).tipo == TOKEN_OR)
  { // '||'
    Token op = nextToken(tokens, tokenIndex);
    string right = andExpr(tokens, tokenIndex);

    string temp = novaTemp();
    addCodigoIntermediario(op.lexema, temp, left, right);
    left = temp;
  }

  return left;
}

string andExpr(const vector<Token> &tokens, int &tokenIndex)
{
  string left = notExpr(tokens, tokenIndex);

  while (peekToken(tokens, tokenIndex).tipo == TOKEN_AND)
  { // '&&'
    Token op = nextToken(tokens, tokenIndex);
    string right = notExpr(tokens, tokenIndex);

    string temp = novaTemp();
    addCodigoIntermediario(op.lexema, temp, left, right);
    left = temp;
  }

  return left;
}

string notExpr(const vector<Token> &tokens, int &tokenIndex)
{
  if (peekToken(tokens, tokenIndex).tipo == TOKEN_NOT)
  { // '!'
    Token op = nextToken(tokens, tokenIndex);
    string right = notExpr(tokens, tokenIndex);

    string temp = novaTemp();
    addCodigoIntermediario(op.lexema, temp, "", right);
    return temp;
  }
  else
  {
    return relExpr(tokens, tokenIndex);
  }
}

string relExpr(const vector<Token> &tokens, int &tokenIndex)
{
  string left = addExpr(tokens, tokenIndex);

  while (peekToken(tokens, tokenIndex).tipo >= 11 && peekToken(tokens, tokenIndex).tipo <= 16)
  { // Operadores relacionais
    Token op = nextToken(tokens, tokenIndex);
    string right = addExpr(tokens, tokenIndex);

    string temp = novaTemp();
    addCodigoIntermediario(op.lexema, temp, left, right);
    left = temp;
  }

  return left;
}

string addExpr(const vector<Token> &tokens, int &tokenIndex)
{
  string left = multExpr(tokens, tokenIndex);

  while (peekToken(tokens, tokenIndex).tipo == TOKEN_PLUS || peekToken(tokens, tokenIndex).tipo == TOKEN_MINUS)
  { // '+' ou '-'
    Token op = nextToken(tokens, tokenIndex);
    string right = multExpr(tokens, tokenIndex);

    string temp = novaTemp();
    addCodigoIntermediario(op.lexema, temp, left, right);
    left = temp;
  }

  return left;
}

string multExpr(const vector<Token> &tokens, int &tokenIndex)
{
  string left = unaryExpr(tokens, tokenIndex);

  while (peekToken(tokens, tokenIndex).tipo == TOKEN_MULTIPLY ||
         peekToken(tokens, tokenIndex).tipo == TOKEN_DIVIDE ||
         peekToken(tokens, tokenIndex).tipo == TOKEN_MODULO)
  { // '*', '/', '%'
    Token op = nextToken(tokens, tokenIndex);
    string right = unaryExpr(tokens, tokenIndex);

    string temp = novaTemp();
    addCodigoIntermediario(op.lexema, temp, left, right);
    left = temp;
  }

  return left;
}

string unaryExpr(const vector<Token> &tokens, int &tokenIndex)
{
  if (peekToken(tokens, tokenIndex).tipo == TOKEN_PLUS || peekToken(tokens, tokenIndex).tipo == TOKEN_MINUS)
  { // '+' ou '-'
    Token op = nextToken(tokens, tokenIndex);
    string right = unaryExpr(tokens, tokenIndex);

    string temp = novaTemp();
    addCodigoIntermediario(op.lexema, temp, "", right);
    return temp;
  }
  else
  {
    return fator(tokens, tokenIndex);
  }
}

string fator(const vector<Token> &tokens, int &tokenIndex)
{
  Token token = nextToken(tokens, tokenIndex);

  // Se for número, identificador ou string, retorna diretamente o lexema
  if (token.tipo == TOKEN_INTEGER_LITERAL || token.tipo == TOKEN_FLOAT_LITERAL || token.tipo == TOKEN_OCTAL || token.tipo == TOKEN_HEX || token.tipo == TOKEN_IDENTIFIER || token.tipo == TOKEN_STR_LITERAL) // Números, IDENT, ST
  {
    return token.lexema;
  }

  if (token.tipo == TOKEN_OPEN_PAREN)
  { // '('
    string result = expr(tokens, tokenIndex);
    token = nextToken(tokens, tokenIndex);
    if (token.tipo != TOKEN_CLOSE_PAREN)
    { // ')'
      erroSintatico(token, "<fator>. Esperava ')', recebeu um ");
    }
    return result;
  }

  erroSintatico(token, "<fator>. Token inesperado: ");
  return "";
}
