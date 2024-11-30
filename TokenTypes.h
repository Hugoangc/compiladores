#ifndef TOKENTYPES_H
#define TOKENTYPES_H

enum TokenType
{
  // OPERADORES E SIMBOLOS
  TOKEN_PLUS = 1,             //+
  TOKEN_MINUS = 2,            // -
  TOKEN_MULTIPLY = 3,         // *
  TOKEN_DIVIDE = 4,           // '/'
  TOKEN_MODULO = 5,           // %
  TOKEN_EQUALITY = 11,        // ==
  TOKEN_INEQUALITY = 12,      // !=
  TOKEN_GREATER = 13,         // >
  TOKEN_GREATER_EQUAL = 14,   // >=
  TOKEN_LESS = 15,            // <
  TOKEN_LESS_EQUAL = 16,      // <=
  TOKEN_OR = 21,              // ||
  TOKEN_AND = 22,             // &&
  TOKEN_NOT = 23,             // !
  TOKEN_ASSIGN = 24,          // =
  TOKEN_PLUS_ASSIGN = 25,     // +=
  TOKEN_MINUS_ASSIGN = 26,    // -=
  TOKEN_MULTIPLY_ASSIGN = 27, // *=
  TOKEN_DIVIDE_ASSIGN = 28,   // /=
  TOKEN_MODULO_ASSIGN = 29,   // %=
  TOKEN_OPEN_PAREN = 61,      // (
  TOKEN_CLOSE_PAREN = 62,     // )
  TOKEN_OPEN_BRACE = 63,      // {
  TOKEN_CLOSE_BRACE = 64,     // }
  TOKEN_SEMICOLON = 65,       // ;
  TOKEN_COMMA = 66,           // ,
  TOKEN_DOT = 67,             // .

  // PALAVRAS RESERVADAS E TIPOS

  TOKEN_INT = 31,
  TOKEN_FLOAT = 32,
  TOKEN_STRING = 33,
  TOKEN_IF = 41,
  TOKEN_ELSE = 42,
  TOKEN_FOR = 43,
  TOKEN_WHILE = 44,
  TOKEN_BREAK = 45,
  TOKEN_CONTINUE = 46,
  TOKEN_SYSTEM = 51,
  TOKEN_OUT = 52,
  TOKEN_PRINT = 53,
  TOKEN_IN = 55,
  TOKEN_SCAN = 56,
  TOKEN_MAIN = 57,

  // Aspas
  TOKEN_QUOTE_DOUBLE = 71,
  TOKEN_QUOTE_SINGLE = 72,

  // Tipos Literais
  TOKEN_HEX = 101,
  TOKEN_OCTAL = 102,
  TOKEN_FLOAT_LITERAL = 103,
  TOKEN_INTEGER_LITERAL = 104,
  TOKEN_IDENTIFIER = 99,
  TOKEN_STR_LITERAL = 6
};

#endif // TOKENTYPES_H
