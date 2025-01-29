#include <iostream>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include "analiselexica.h"
#ifndef ANALISESINTATICA_H
#define ANALISESINTATICA_H
using namespace std;
void program(const vector<Token> &tokens, int &tokenIndex);
void block(const vector<Token> &tokens, int &tokenIndex);
void stmtList(const vector<Token> &tokens, int &tokenIndex);
void stmt(const vector<Token> &tokens, int &tokenIndex);
void declaration(const vector<Token> &tokens, int &tokenIndex);
void identList(const vector<Token> &tokens, int &tokenIndex);
void forStmt(const vector<Token> &tokens, int &tokenIndex);
void ifStmt(const vector<Token> &tokens, int &tokenIndex);
void ioStmt(const vector<Token> &tokens, int &tokenIndex);
void whileStmt(const vector<Token> &tokens, int &tokenIndex);
void atrib(const vector<Token> &tokens, int &tokenIndex);
void outList(const vector<Token> &tokens, int &tokenIndex);
void out(const vector<Token> &tokens, int &tokenIndex);
void expr(const vector<Token> &tokens, int &tokenIndex);
void orExpr(const vector<Token> &tokens, int &tokenIndex);
void andExpr(const vector<Token> &tokens, int &tokenIndex);
void notExpr(const vector<Token> &tokens, int &tokenIndex);
void relExpr(const vector<Token> &tokens, int &tokenIndex);
void addExpr(const vector<Token> &tokens, int &tokenIndex);
void multExpr(const vector<Token> &tokens, int &tokenIndex);
void unaryExpr(const vector<Token> &tokens, int &tokenIndex);
void fator(const vector<Token> &tokens, int &tokenIndex);

// Funções auxiliares
Token nextToken(const vector<Token> &tokens, int &tokenIndex);
Token peekToken(const vector<Token> &tokens, int &tokenIndex);
void erroSintaxe(const Token &token, const string &contexto);
#endif
