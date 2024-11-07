
# Analisador Léxico em C++ 
Este é um projeto de um analisador léxico escrito em C++ para a disciplina de Compiladores do CEFET-MG campus Leopoldina. <br/>
O objetivo é analisar um código-fonte em uma linguagem semelhante a Java, identificar tokens e classificá-los conforme suas características. 
O analisador ignora comentários e mantém a contagem de linhas e colunas para relatórios de erros. <br/>


### Pré-requisitos 
+ Compilador C++: Você precisará de um compilador C++ para compilar e executar o código.
+ Ambiente de Desenvolvimento: Um ambiente de desenvolvimento integrado (IDE) como Visual Studio Code, Code::Blocks ou CLion.


Como Executar:
```
g++ analisador_lexico.cpp -o analisador	
./analisador
```
Depois você pode escolher qual caso de teste deseja executar, como por exemplo:

```
./analisador
teste.java
```
```
./analisador
teste3.java
```

Estrutura do Código:
+ Token: Estrutura que representa um token com seu tipo, lexema, linha e coluna.
+ criarTabelaDeTokens(): Função que cria uma tabela de tokens (mapeamento de lexemas para seus tipos).
+ Verificação de Tipos: Funções que determinam se uma string é um número inteiro, flutuante, hexadecimal ou octal.
+ imprimirTokens(): Função para exibir os tokens identificados.
+ lerArquivo(): Função que lê o conteúdo de um arquivo.
+ analiseLexica(): Função principal que processa o código-fonte e gera a lista de tokens.
