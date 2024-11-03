 int main() {
        int a = 10.12; // Comentário de linha
        float b = 052; // Número octal

        // Testando strings e caracteres
        string str1 = "Texto com quebra de linha\\n e outros caracteres especiais.";
        string str2 = "Este é um teste de string com 'caracteres' entre aspas simples.";
        
        // Teste de números
        int numeroInteiro = 42;
        float numeroFlutuante = 3.14;
        int numeroHexadecimal = 0x1A; // Número hexadecimal

        // Condicional e laço
        if (a == 10) {
            b += 2.5;
            system.out.println("Valor de a: " + a);
            system.out.println("Valor de b: " + b);
        } else {
            system.out.println("A variável 'a' não é igual a 10.");
        }

        // Testando a função print
        print("Texto de teste");
        print('Texto entre aspas simples');
        /*
        Bloco de comentários
        que deve ser ignorado
        */
    }

    // Função de teste
    int print(string message) {
        system.out.println(message);
    }

