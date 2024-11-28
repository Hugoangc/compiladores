#include <iostream>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include "analiselexica.h"
#ifndef ANALISESINTATICA_H
#define ANALISESINTATICA_H

Token nextToken(const vector<Token> &tokens);
Token peekToken(const vector<Token> &tokens);
void erroSintaxe(const Token &token, const string &contexto = "");
void program(const vector<Token> &tokens);
void declaration(const vector<Token> &tokens);
void block(const vector<Token> &tokens);
void stmt(const vector<Token> &tokens);
void forStmt(const vector<Token> &tokens);
void whileStmt(const vector<Token> &tokens);
void ifStmt(const vector<Token> &tokens);
void verificaTipo(const Token &tipoVariavel, const Token &valor);
string tipoParaString(int tipo);
void expr(const vector<Token> &tokens);
void relExpr(const vector<Token> &tokens);
void addExpr(const vector<Token> &tokens);
void multExpr(const vector<Token> &tokens);
void optAtrib(const vector<Token> &tokens);
void atrib(const vector<Token> &tokens);
void orOp(std::vector<std::string> &tokens, size_t &index);
void andOp(std::vector<std::string> &tokens, size_t &index);
void notOp(std::vector<std::string> &tokens, size_t &index);
void rel(std::vector<std::string> &tokens, size_t &index);
void add(std::vector<std::string> &tokens, size_t &index);
void mult(std::vector<std::string> &tokens, size_t &index);
void uno(std::vector<std::string> &tokens, size_t &index);
void fator(std::vector<std::string> &tokens, size_t &index);
#endif