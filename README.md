
# Analisador Léxico e Sintático em C++ 
Este é um projeto de um analisador léxico e sintático escrito em C++. <br/>
O objetivo é analisar um código-fonte em uma linguagem semelhante a Java, identificar tokens e classificá-los conforme suas características e após finalizada a análise léxica, realizar a análise sintática. <br/>
O analisador ignora comentários e mantém a contagem de linhas e colunas para relatórios de erros. <br/>


### Pré-requisitos 
+ Compilador C++: Você precisará de um compilador C++ para compilar e executar o código.
+ Ambiente de Desenvolvimento: Um ambiente de desenvolvimento integrado (IDE) como Visual Studio Code, Code::Blocks ou CLion.


Como Executar:
```
g++ main.cpp analisador_lexico.cpp analisador_sintatico.cpp -o analisador
./analisador
```
Depois você pode escolher qual caso de teste deseja executar, como por exemplo:

```
./analisador
teste.java
```
```
./analisador
exemplo1.java
```

## Estrutura do Código:
### Analisador Léxico:
+ Token: Estrutura que representa um token com seu tipo, lexema, linha e coluna.
+ criarTabelaDeTokens(): Função que cria uma tabela de tokens (mapeamento de lexemas para seus tipos).
+ Verificação de Tipos: Funções que determinam se uma string é um número inteiro, flutuante, hexadecimal ou octal.
+ imprimirTokens(): Função para exibir os tokens identificados.
+ lerArquivo(): Função que lê o conteúdo de um arquivo.
+ analiseLexica(): Função principal que processa o código-fonte e gera a lista de tokens.

### Analisador Sintático:
+ O analisador sintático utiliza recursão descendente para verificar a conformidade do código com a gramática definida.
+ As funções de parsing são responsáveis por processar diferentes construções da linguagem, como:
  + program: Verifica a estrutura do programa, incluindo a presença da função `main`.
  + block: Processa blocos de código delimitados por chaves `{}`.
  + stmtList: Processa uma lista de declarações ou instruções.
  + stmt: Processa diferentes tipos de declarações e instruções (como `for`, `while`, `if`, atribuições e chamadas de função).
  + declaration: Verifica declarações de variáveis.
  + identList: Processa a lista de identificadores nas declarações.
  + forStmt: Processa laços `for`.
  + ioStmt: Processa operações de entrada e saída (`print` e `scan`).
  + whileStmt: Processa laços `while`.
  + atrib: Processa atribuições de valores a variáveis.
+ Controle de Erros: A função erroSintaxe() é utilizada para detectar erros de sintaxe e exibir mensagens informativas com o contexto do erro.
