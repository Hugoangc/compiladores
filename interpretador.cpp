#include <iostream>
#include <vector>
#include <tuple>
#include <unordered_map>
#include <stdexcept>
#include <string>
#include <variant>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;

// Definições globais
using Operando = variant<int, string>; // Um operando pode ser inteiro ou string
using Instrucao = tuple<string, Operando, Operando, Operando>;
vector<Instrucao> tuplas;             // Lista de tuplas
unordered_map<string, int> labels;    // Mapeamento de labels para índices
unordered_map<string, int> variaveis; // Tabela de variáveis
int ponteiro = 0;                     // Ponteiro de execuçã o

// Funções auxiliares
void trim(string &s)
{
  while (!s.empty() && isspace(s.front()))
    s.erase(s.begin());
  while (!s.empty() && isspace(s.back()))
    s.pop_back();
}

int obterValor(const Operando &operando)
{
  if (holds_alternative<int>(operando))
  {
    return get<int>(operando); // Retorna diretamente se for inteiro
  }
  else if (holds_alternative<string>(operando))
  {
    string nomeVariavel = get<string>(operando);
    if (variaveis.find(nomeVariavel) != variaveis.end())
    {
      return variaveis[nomeVariavel]; // Busca na tabela de variáveis
    }
    throw runtime_error("Variavel nao encontrada: " + nomeVariavel);
  }
  throw runtime_error("Operando invalido.");
}

void criarVariavelSeNaoExistir(const string &var)
{
  if (variaveis.find(var) == variaveis.end())
  {
    variaveis[var] = 0; // Inicializa a variável com 0
  }
}
void executarAtribuicao(const Instrucao &instrucao)
{
  string guardar = get<string>(get<1>(instrucao));
  Operando operando1 = get<2>(instrucao);
  criarVariavelSeNaoExistir(guardar);
  variaveis[guardar] = obterValor(operando1);
}

void executarAritmetica(const Instrucao &instrucao)
{
  string operador = get<0>(instrucao);
  string guardar = get<string>(get<1>(instrucao));
  Operando operando1 = get<2>(instrucao);
  Operando operando2 = get<3>(instrucao);

  int v1 = obterValor(operando1);
  int v2 = obterValor(operando2);

  if (operador == "+")
    variaveis[guardar] = v1 + v2;
  else if (operador == "-")
    variaveis[guardar] = v1 - v2;
  else if (operador == "*")
    variaveis[guardar] = v1 * v2;
  else if (operador == "/")
    variaveis[guardar] = v1 / v2;
  else if (operador == "%")
    variaveis[guardar] = v1 % v2;
  else if (operador == "//")
    variaveis[guardar] = v1 / v2;
  else
    throw runtime_error("Operador desconhecido: " + operador);
}
void executarRelacional(const Instrucao &instrucao)
{
  string operador = get<0>(instrucao);
  string guardar = get<string>(get<1>(instrucao));
  Operando operando1 = get<2>(instrucao);
  Operando operando2 = get<3>(instrucao);

  // Log para verificar os operandos
  if (holds_alternative<string>(operando1))
  {
    string var1 = get<string>(operando1);
    operando1 = variaveis[var1]; // Resolve para o valor da variável
  }

  if (holds_alternative<string>(operando2))
  {
    string var2 = get<string>(operando2);
    operando2 = variaveis[var2]; // Resolve para o valor da variável
  }

  int v1 = obterValor(operando1);
  int v2 = obterValor(operando2);

  if (operador == "==")
    variaveis[guardar] = (v1 == v2);
  else if (operador == "<>")
    variaveis[guardar] = (v1 != v2);
  else if (operador == ">")
    variaveis[guardar] = (v1 > v2);
  else if (operador == ">=")
    variaveis[guardar] = (v1 >= v2);
  else if (operador == "<")
    variaveis[guardar] = (v1 < v2);
  else if (operador == "<=")
    variaveis[guardar] = (v1 <= v2);
  else
    throw runtime_error("Operador relacional desconhecido: " + operador);
}

void executarLogico(const Instrucao &instrucao)
{
  string operador = get<0>(instrucao);
  string guardar = get<string>(get<1>(instrucao));
  Operando operando1 = get<2>(instrucao);
  Operando operando2 = get<3>(instrucao);

  int v1 = obterValor(operando1);
  int v2 = (operador != "!") ? obterValor(operando2) : 0;

  if (operador == "||")
    variaveis[guardar] = (v1 || v2);
  else if (operador == "&&")
    variaveis[guardar] = (v1 && v2);
  else if (operador == "!")
    variaveis[guardar] = !v1;
  else
    throw runtime_error("Operador logico desconhecido: " + operador);
}

void executarUnario(const Instrucao &instrucao)
{
  string operador = get<0>(instrucao);
  string guardar = get<string>(get<1>(instrucao));
  Operando operando1 = get<2>(instrucao);

  int v1 = obterValor(operando1);

  if (operador == "+")
    variaveis[guardar] = +v1;
  else if (operador == "-")
    variaveis[guardar] = -v1;
  else if (operador == "!")
    variaveis[guardar] = !v1;
  else
    throw runtime_error("Operador unario desconhecido: " + operador);
}

void executarIf(const Instrucao &instrucao)
{
  // cout << "DEBUG: Variaveis antes do IF:" << endl;
  // for (const auto &par : variaveis)
  // {
  //   cout << par.first << " = " << par.second << endl;
  // }

  Operando condicao = get<1>(instrucao);          // Condição é o segundo operando
  string label1 = get<string>(get<2>(instrucao)); // Primeiro label
  string label2 = get<string>(get<3>(instrucao)); // Segundo label

  // Obter o valor da condição
  int valorCondicao;
  try
  {
    valorCondicao = obterValor(condicao); // Tenta obter o valor da condição
  }
  catch (const std::exception &e)
  {
    throw runtime_error("A condicao de IF deve ser numerica.");
  }

  // cout << "Condicao do IF: " << valorCondicao << endl;

  if (valorCondicao) // Condição verdadeira
  {
    // cout << "DEBUG: Condicao verdadeira, pulando para " << label1 << endl;
    ponteiro = labels[label1] - 1;
  }
  else // Condição falsa
  {
    // cout << "DEBUG: Condicao falsa, pulando para " << label2 << endl;
    ponteiro = labels[label2] - 1;
  }
}

void executarJump(const Instrucao &instrucao)
{
  string destino = get<string>(get<1>(instrucao));
  if (labels.find(destino) != labels.end())
  {
    ponteiro = labels[destino];
  }
  else
  {
    throw runtime_error("Label nao encontrado: " + destino);
  }
}

void executarCall(const Instrucao &instrucao)
{
  string comando = get<string>(get<1>(instrucao));
  Operando operando1 = get<2>(instrucao);

  if (comando == "PRINT")
  {
    Operando operando1 = get<2>(instrucao); // O que será impresso
    Operando operando2 = get<3>(instrucao); // Opcional: variável para exibir valor

    // Verifica e imprime o operando1 (se não for vazio ou as aspas literais "")
    if (holds_alternative<string>(operando1))
    {
      string str = get<string>(operando1);
      if (str == "\\n") // Caso especial: imprime uma nova linha
      {
        cout << endl;
      }
      else if (str != "" && str != "\"\"")
      {
        cout << str; // Imprime a string diretamente
      }
    }

    // Verifica e imprime o valor da variável (se existir)
    if (holds_alternative<string>(operando2))
    {
      string var = get<string>(operando2);
      if (variaveis.find(var) != variaveis.end()) // A variável existe
      {
        int valor = variaveis[var]; // O valor é do tipo int
        cout << valor;              // Imprime o valor do inteiro diretamente
      }
    }
  }

  else if (comando == "SCAN")
  {
    string destino = get<string>(get<3>(instrucao)); // Nome da variável
    int valor;
    cout << "Entrada para \"" << destino << "\": ";
    cin >> valor;

    // Atualizar a tabela de variáveis
    criarVariavelSeNaoExistir(destino);
    variaveis[destino] = valor;
  }
  else if (comando == "STOP")
  {
    cout << "Execucao interrompida." << endl;
    ponteiro = tuplas.size(); // Termina o loop
  }
  else
  {
    throw runtime_error("Chamada de sistema desconhecida: " + comando);
  }
}

void carregarLabels()
{
  for (int i = 0; i < tuplas.size(); ++i)
  {
    if (get<0>(tuplas[i]) == "LABEL")
    {
      string label = get<string>(get<1>(tuplas[i]));
      labels[label] = i;
    }
  }
}
bool ehNumero(const std::string &str)
{
  if (str.empty())
    return false;
  for (char c : str)
  {
    if (!std::isdigit(c))
      return false;
  }
  return true;
}
Operando interpretarOperando(const string &str)
{
  if (str.empty())
    return {}; // caso o operando seja vazio
  if (ehNumero(str))
  {
    return Operando(stoi(str)); // Converte números diretamente
  }
  else
  {
    if (variaveis.find(str) != variaveis.end())
    {
      return Operando(variaveis[str]); // A string é uma variável, retornar seu valor
    }
    else
    {
      return Operando(str); // Caso contrário, tratar como string literal
    }
  }
}

void carregarTuplas(const string &nomeArquivo)
{
  ifstream arquivo(nomeArquivo);
  if (!arquivo.is_open())
  {
    throw runtime_error("Erro ao abrir o arquivo: " + nomeArquivo);
  }

  string linha;
  while (getline(arquivo, linha))
  {
    // Remover a vírgula no final da linha
    if (linha.back() == ',')
      linha.pop_back();

    // Verificar se a linha começa com '(' e termina com ')'
    if (linha.empty() || linha.front() != '(' || linha.back() != ')')
    {
      throw runtime_error("Linha mal formatada: " + linha);
    }

    // Remover parênteses
    linha.erase(remove(linha.begin(), linha.end(), '('), linha.end());
    linha.erase(remove(linha.begin(), linha.end(), ')'), linha.end());

    // Separar os elementos da linha por vírgula
    istringstream stream(linha);
    string tempComando, tempOp1, tempOp2, tempOp3;
    getline(stream, tempComando, ',');
    getline(stream, tempOp1, ',');
    getline(stream, tempOp2, ',');
    getline(stream, tempOp3, ',');

    // Limpar e remover espaços extras
    auto limpar = [](string &s)
    {
      s.erase(remove(s.begin(), s.end(), '\''), s.end());
      trim(s);
    };

    limpar(tempComando);
    limpar(tempOp1);
    limpar(tempOp2);
    limpar(tempOp3);

    // Interpretar os operandos
    Operando op1 = interpretarOperando(tempOp1);
    Operando op2 = interpretarOperando(tempOp2);
    Operando op3 = interpretarOperando(tempOp3);

    // Adicionar a instrução à lista de tuplas
    tuplas.emplace_back(tempComando, op1, op2, op3);
  }

  arquivo.close();
}

void executar()
{
  while (ponteiro < tuplas.size())
  {
    const Instrucao &instrucao = tuplas[ponteiro];
    string comando = get<0>(instrucao);

    // Remover espaços no comando antes da comparação
    trim(comando);

    if (comando == "=")
      executarAtribuicao(instrucao);
    else if (comando == "+" || comando == "-" || comando == "*" || comando == "/" || comando == "%" || comando == "//")
      executarAritmetica(instrucao);
    else if (comando == "==" || comando == "<>" || comando == ">" || comando == ">=" || comando == "<" || comando == "<=")
      executarRelacional(instrucao);
    else if (comando == "||" || comando == "&&" || comando == "!")
      executarLogico(instrucao);
    else if (comando == "!" || comando == "-")
      executarUnario(instrucao);
    else if (comando == "IF")
      executarIf(instrucao);
    else if (comando == "JUMP")
      executarJump(instrucao);
    else if (comando == "CALL")
      executarCall(instrucao);
    else if (comando == "LABEL")
    { /* Não faz nada */
    }
    else
    {
      throw runtime_error("Comando desconhecido: '" + comando + "'");
    }

    ponteiro++;
  }
}

int main()
{
  try
  {
    carregarTuplas("text2.txt");
    carregarLabels();
    executar();
  }
  catch (const exception &e)
  {
    cerr << "Erro: " << e.what() << endl;
  }

  return 0;
}
