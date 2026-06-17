#include <stdio.h>
#include "ui/menu.h"
#include "utils/utils.h"

int main(){
    int opcao;

    printf("Bem vindo ao FinCalc\n");

    do{
        printf("\nSelecione uma categoria\n");
        printf("1. Despesas\n");
        printf("2. Análises\n");
        printf("3. Sair\n");
        printf("Escolha uma opcao: ");
        if (scanf("%d", &opcao) != 1) {
            limpar_buffer();
            opcao = 0;
            continue;
        }
        limpar_buffer();

        switch (opcao) {
            case 1:
                menu_despesas();
                break;
            case 2:
                menu_analises();
                break;
            case 3:
                printf("Saindo...\n");
                break;
            default:
                printf("Opção inválida!\n");
                break;
        }

    }while (opcao != 3);

    return 0;
}