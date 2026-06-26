#include <stdio.h>
#include <stdlib.h>
#include "../services/analytics_service.h"
#include "menu.h"
#include "../utils/utils.h"

void resumo_geral();

void menu_analises(){
    int opcao;

    do{
        printf("\n------- MENU ANALISES --------\n");
        printf("1. Analise Geral\n");
        printf("2. Voltar\n");
        printf("Escolha uma opcao: ");

        char buffer[256];
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            opcao = 0;
            continue;
        }
        opcao = atoi(buffer);

        switch (opcao) {
            case 1:
                resumo_geral();
                break;
            case 2:
                printf("Voltando...\n");
                break;
            default:
                printf("Opção inválida!\n");
                break;
        }
    }while(opcao != 2);
}


void resumo_geral(){
    AnaliseDespesas analytics = calcular_analise_geral();
    printf("\n--- Resumo Geral ---\n");
    printf("Total de despesas do mes atual:    R$ %.2lf\n", analytics.total_despesas);
    printf("Total acumulado (todos os meses):  R$ %.2lf\n", analytics.despesa_acumulada);
    printf("Crescimento de despesas em relacao ao mes passado: %.2lf\n", analytics.taxa_crescimento);
    printf("Mes com maior gasto: %02d/%04d\n", analytics.maior_gasto.mes, analytics.maior_gasto.ano);
    printf("Mes com menor gasto: %02d/%04d\n", analytics.menor_gasto.mes, analytics.menor_gasto.ano);
    printf("Media de gastos por mes: %.2lf\n", analytics.media_mensal);
    printf("Mes sazonal (maior volume historico): %02d\n", analytics.mes_sazonal);
    printf("Previsao para o proximo mes:          R$ %.2lf\n", analytics.previsao_tendencia);
    printf("\n-----------------------------\n");
}
