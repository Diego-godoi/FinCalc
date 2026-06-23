#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../models/expense.h"
#include "../services/expense_service.h"
#include "menu.h"
#include "../utils/utils.h"

void ui_registrar_despesa();
void ui_remover_despesa();
void ui_editar_despesa();
void ui_listar_despesas();

void menu_despesas(){
    int opcao;
    char buffer[10];

    do{
        printf("\n------- MENU DESPESAS --------\n");
        printf("1. Adicionar despesa\n");
        printf("2. Remover despesa\n");
        printf("3. Editar despesa\n");
        printf("4. Listar despesas\n");
        printf("5. Voltar\n");
        printf("Escolha uma opcao: ");

        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            opcao = 0;
            continue;
        }
        opcao = atoi(buffer);

        switch (opcao) {
            case 1:
                printf("\nAdicionando despesa...\n");
                ui_registrar_despesa();
                break;
            case 2:
                printf("\nRemovendo despesa...\n");
                ui_remover_despesa();
                break;
            case 3:
                printf("\nEditando despesa...\n");
                ui_editar_despesa();
                break;
            case 4:
                printf("\nListando despesas...\n");
                ui_listar_despesas();
                break;
            case 5:
                printf("Voltando...\n");
                break;
            default:
                printf("Opção inválida!\n");
                break;
        }
    }while(opcao != 5);
}

void ui_registrar_despesa(){
    Despesa despesa = {0}; // Define todos os valores como NULL
    char buffer[256];
    
    printf("----- Registrar despesa ----\n");
    printf("Insira o nome da despesa: ");
    fgets(despesa.titulo, sizeof(despesa.titulo), stdin);
    despesa.titulo[strcspn(despesa.titulo, "\n")] = 0; // remove o \n
    
    printf("Escolha uma categoria:\n");
    printf("0 - ALIMENTACAO\n1 - CASA\n2 - TRANSPORTE\n3 - OUTROS\nOpcao: ");
    fgets(buffer, sizeof(buffer), stdin);
    despesa.categoria = (Categoria)atoi(buffer); // converte char para int e depois para enum
    
    printf("Insira o valor: ");
    fgets(buffer, sizeof(buffer), stdin);
    despesa.valor = atof(buffer);
    
    ler_data(&despesa.data);
    
    do{
        printf("É recorrente (s ou n): ");
        fgets(buffer, sizeof(buffer), stdin);
    }while(buffer[0] != 's' && buffer[0] != 'n');
    
    if(buffer[0] == 's'){
        despesa.recorrente = 1;
        printf("Insira a categoria de recorrencia:\n");
        printf("1 - DIARIA\n2 - SEMANAL\n3 - MENSAL\n4 - ANUAL\nOpcao: ");
        fgets(buffer, sizeof(buffer), stdin);
        despesa.frequencia = (Frequencia)atoi(buffer);
    } else {
        despesa.recorrente = 0;
        despesa.frequencia = NENHUMA;
    }
    
    if(!adicionar_despesa(&despesa)){
        printf("Erro ao registrar despesa!\n");
        return;
    }

    printf("Despesa salva com sucesso!\n");
}

void ui_remover_despesa(){
    int id;
    char buffer[256];
    
    printf("----- Remover despesa ----\n");
    printf("Insira o ID da despesa: ");
    fgets(buffer, sizeof(buffer), stdin);
    id = atoi(buffer);
    
    printf("Tem certeza (s / n): ");
    fgets(buffer, sizeof(buffer), stdin);
    
    if(buffer[0] == 'n'){
        printf("Despesa nao removida!\n");
        return;
    }
    if(!remover_despesa(id)){
        return;
    }
    printf("Despesa removida com sucesso!\n");
}

void ui_editar_despesa(){
    int id;
    char buffer[256];
    Despesa despesa = {0};
    
    printf("----- Editar despesa ----\n");
    printf("Insira o ID da despesa: ");
    fgets(buffer, sizeof(buffer), stdin);
    id = atoi(buffer);
    
    if(!existe_por_id(id)){
        printf("Despesa nao encontrada!\n");
        return;
    }
    printf("Insira o novo nome da despesa (Digite 0 para pular): ");
    fgets(despesa.titulo, sizeof(despesa.titulo), stdin);
    despesa.titulo[strcspn(despesa.titulo, "\n")] = 0;
    if (strcmp(despesa.titulo, "0") == 0) despesa.titulo[0] = '\0';
    
    printf("Escolha uma categoria (-1 para pular):\n");
    printf("0 - ALIMENTACAO\n1 - CASA\n2 - TRANSPORTE\n3 - OUTROS\nOpcao: ");
    fgets(buffer, sizeof(buffer), stdin);
    despesa.categoria = (Categoria)atoi(buffer);
    
    printf("Insira o valor (-1 para pular): ");
    fgets(buffer, sizeof(buffer), stdin);
    despesa.valor = atof(buffer);
    
    ler_data_opcional(&despesa.data);
    
    do{
        printf("É recorrente (s, n ou p para pular): ");
        fgets(buffer, sizeof(buffer), stdin);
    }while(buffer[0] != 's' && buffer[0] != 'n' && buffer[0] != 'p');
    
    if(buffer[0] == 's'){
        despesa.recorrente = 1;
        printf("Insira a nova categoria de recorrencia:\n");
        printf("1 - DIARIA\n2 - SEMANAL\n3 - MENSAL\n4 - ANUAL\nOpcao: ");
        fgets(buffer, sizeof(buffer), stdin);
        despesa.frequencia = (Frequencia)atoi(buffer);
    } else if (buffer[0] == 'n') {
        despesa.recorrente = 0;
        despesa.frequencia = NENHUMA;
    } else {
        despesa.recorrente = -1;
    }

    if(!editar_despesa(id, &despesa)){
        return;
    }
    printf("Despesa editada com sucesso!\n");
}

void ui_listar_despesas(){
    int contagem = 0;
    printf("\n----- Listar despesas ----\n");
    Despesa *despesas = listar_despesas(&contagem);
    
    if(despesas == NULL || contagem == 0){
        printf("Nenhuma despesa encontrada!\n");
        return;
    }
    
    printf("%-5s | %-20s | %-12s | %-10s | %-10s | %-11s | %-12s\n", "ID", "Nome", "Categoria", "Valor", "Data", "Recorrencia", "Freq.");
    printf("------------------------------------------------------------------------------------------------------\n");
    for (int i = 0; i < contagem; i++) {
        printf("%-5d | %-20s | %-12s | %-10.2lf | %02d/%02d/%04d | %-11d | %-12s\n", 
            despesas[i].id, despesas[i].titulo, obter_nome_categoria(despesas[i].categoria), despesas[i].valor, 
            despesas[i].data.dia, despesas[i].data.mes, despesas[i].data.ano, 
            despesas[i].recorrente, obter_nome_frequencia(despesas[i].frequencia));
    }
    free(despesas);
}