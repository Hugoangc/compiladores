int main() {
        int numBloco0;
        int divBloco0;
        int resto;
        system.out.print("Entre com o inteiro: ");
        system.in.scan(int, numBloco0);
        system.out.print(numBloco0);
        system.out.print(" = ");

        if (numBloco0 > 1) {
            divBloco0  = 2;  
            while (numBloco0 > 1) {  
                 resto = numBloco0 % divBloco0;
                if (resto == 0) { 
                    system.out.print(divBloco0);  
                    numBloco0 = numBloco0 / divBloco0; 
                    if (numBloco0 > 1) {
                        system.out.print(" * ");  
                    }
                } else {
                    divBloco0 = divBloco0 + 1;  
                }
            }
        }
        // system.out.print("\n");
        // system.out.print(numBloco0);
}
