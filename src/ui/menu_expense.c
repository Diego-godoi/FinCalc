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

    do{
        printf("\n------- MENU DESPESAS --------\n");
        printf("1. Adicionar despesa\n");
        printf("2. Remover despesa\n");
        printf("3. Editar despesa\n");
        printf("4. Listar despesas\n");
        printf("5. Voltar\n");
        printf("Escolha uma opcao: ");

        if (scanf("%d", &opcao) != 1) {
            limpar_buffer();
            opcao = 0;
            continue;
        }
        limpar_buffer();

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
    Despesa despesa = {0};
    Data data = {0};
    char opcao_str[10];
    
    printf("----- Registrar despesa ----\n");
    printf("Insira o nome da despesa: ");
    fgets(despesa.titulo, sizeof(despesa.titulo), stdin);
    despesa.titulo[strcspn(despesa.titulo, "\n")] = 0;
    
    printf("Escolha uma categoria (0=ALIMENTACAO, 1=CASA, 2=TRANSPORTE, 3=OUTROS): ");
    scanf("%d", (int*)&despesa.categoria);
    
    printf("Insira o valor: ");
    scanf("%lf", &despesa.valor);
    limpar_buffer();
    
    do{
        printf("Insira a data (dd/mm/aaaa): ");
        if (scanf("%d/%d/%d", &data.dia, &data.mes, &data.ano) != 3) {
            limpar_buffer();
            continue;
        }
        limpar_buffer();
    }while(!validar_data(data));
    despesa.data = data;
    
    do{
        printf("É recorrente (s ou n): ");
        fgets(opcao_str, sizeof(opcao_str), stdin);
    }while(opcao_str[0] != 's' && opcao_str[0] != 'n');
    
    if(opcao_str[0] == 's'){
        despesa.recorrente = 1;
        printf("Insira a categoria de recorrencia (1=DIARIA, 2=SEMANAL, 3=MENSAL, 4=ANUAL): ");
        scanf("%d", (int*)&despesa.frequencia);
        limpar_buffer();
    } else {
        despesa.recorrente = 0;
        despesa.frequencia = NENHUMA;
    }
    
    if(!adicionar_despesa(despesa)){
        printf("Erro ao registrar despesa!\n");
        return;
    }

    printf("Despesa salva com sucesso!\n");
}

void ui_remover_despesa(){
    int id;
    char opcao_str[10];
    
    printf("----- Remover despesa ----\n");
    printf("Insira o ID da despesa: ");
    scanf("%d", &id);
    limpar_buffer();
    
    printf("Tem certeza (s / n): ");
    fgets(opcao_str, sizeof(opcao_str), stdin);
    
    if(opcao_str[0] == 'n'){
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
    char opcao_str[10];
    Despesa despesa = {0};
    Data data = {0};
    
    printf("----- Editar despesa ----\n");
    printf("Insira o ID da despesa: ");
    scanf("%d", &id);
    limpar_buffer();
    
    if(!existe_por_id(id)){
        printf("Despesa nao encontrada!\n");
        return;
    }
    printf("Insira o novo nome da despesa (Digite 0 para pular): ");
    fgets(despesa.titulo, sizeof(despesa.titulo), stdin);
    despesa.titulo[strcspn(despesa.titulo, "\n")] = 0;
    if (strcmp(despesa.titulo, "0") == 0) despesa.titulo[0] = '\0';
    
    printf("Escolha uma categoria (-1 para pular): ");
    int cat;
    scanf("%d", &cat);
    despesa.categoria = (enum Categoria)cat;
    
    printf("Insira o valor (-1 para pular): ");
    scanf("%lf", &despesa.valor);
    limpar_buffer();
    
    do{
        printf("Insira a data (dd/mm/aaaa) (0/0/0 para pular): ");
        if (scanf("%d/%d/%d", &data.dia, &data.mes, &data.ano) != 3) {
            limpar_buffer();
            continue;
        }
        limpar_buffer();
        if (data.dia == 0 && data.mes == 0 && data.ano == 0) break;
    }while(!validar_data(data));
    despesa.data = data;
    
    do{
        printf("É recorrente (s, n ou p para pular): ");
        fgets(opcao_str, sizeof(opcao_str), stdin);
    }while(opcao_str[0] != 's' && opcao_str[0] != 'n' && opcao_str[0] != 'p');
    
    if(opcao_str[0] == 's'){
        despesa.recorrente = 1;
        printf("Insira a nova categoria de recorrencia: ");
        scanf("%d", (int*)&despesa.frequencia);
        limpar_buffer();
    } else if (opcao_str[0] == 'n') {
        despesa.recorrente = 0;
        despesa.frequencia = NENHUMA;
    } else {
        despesa.recorrente = -1;
    }

    if(!editar_despesa(id, despesa)){
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
    
    printf("%-5s | %-20s | %-10s | %-10s | %-10s | %-11s | %-10s\n", "ID", "Nome", "Categoria", "Valor", "Data", "Recorrencia", "Freq.");
    printf("-----------------------------------------------------------------------------------------\n");
    for (int i = 0; i < contagem; i++) {
        printf("%-5d | %-20s | %-10d | %-10.2lf | %02d/%02d/%04d | %-11d | %-10d\n", 
            despesas[i].id, despesas[i].titulo, despesas[i].categoria, despesas[i].valor, 
            despesas[i].data.dia, despesas[i].data.mes, despesas[i].data.ano, 
            despesas[i].recorrente, despesas[i].frequencia);
    }
    free(despesas);
}