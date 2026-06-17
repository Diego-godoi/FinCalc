#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "analytics_service.h"
#include "../storage/expense_repository.h"
#include "../math/math_utils.h"

AnaliseDespesas calcular_analise_geral() {
    AnaliseDespesas analytics = {0};
    int contagem = 0;
    Despesa *despesas = listar_todas_despesas(&contagem);

    if (contagem == 0 || despesas == NULL) {
        return analytics;
    }

    // Identificar o mês e ano atuais
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    int mes_atual = tm.tm_mon + 1;
    int ano_atual = tm.tm_year + 1900;
    
    // Mês anterior para a diferença regressiva
    int mes_anterior = mes_atual - 1;
    int ano_anterior = ano_atual;
    if (mes_anterior == 0) {
        mes_anterior = 12;
        ano_anterior -= 1;
    }

    analytics.mes_referencia = mes_atual;
    analytics.ano_referencia = ano_atual;

    double despesas_mes_atual = 0.0;
    double despesas_mes_anterior = 0.0;

    double *todos_valores = (double*)malloc(contagem * sizeof(double));
    double totais_mensais[12] = {0};

    typedef struct { int ano; int mes; double total; } AnoMesTotal;
    AnoMesTotal totais_unicos[1000] = {0};
    int num_unicos = 0;

    for (int i = 0; i < contagem; i++) {
        todos_valores[i] = despesas[i].valor;

        // Agregar totais por mês para análise de sazonalidade geral
        if (despesas[i].data.mes >= 1 && despesas[i].data.mes <= 12) {
            totais_mensais[despesas[i].data.mes - 1] += despesas[i].valor;
        }

        // Agregar totais por (ano, mes) para maior e menor gasto do mes
        int found = 0;
        for (int j = 0; j < num_unicos; j++) {
            if (totais_unicos[j].ano == despesas[i].data.ano && totais_unicos[j].mes == despesas[i].data.mes) {
                totais_unicos[j].total += despesas[i].valor;
                found = 1;
                break;
            }
        }
        if (!found && num_unicos < 1000) {
            totais_unicos[num_unicos].ano = despesas[i].data.ano;
            totais_unicos[num_unicos].mes = despesas[i].data.mes;
            totais_unicos[num_unicos].total = despesas[i].valor;
            num_unicos++;
        }

        // Classificar despesas do mês atual e anterior
        if (despesas[i].data.ano == ano_atual && despesas[i].data.mes == mes_atual) {
            despesas_mes_atual += despesas[i].valor;
        } else if (despesas[i].data.ano == ano_anterior && despesas[i].data.mes == mes_anterior) {
            despesas_mes_anterior += despesas[i].valor;
        }
    }

    double max_total = -1;
    double min_total = -1;
    for (int j = 0; j < num_unicos; j++) {
        if (max_total == -1 || totais_unicos[j].total > max_total) {
            max_total = totais_unicos[j].total;
            analytics.maior_gasto.mes = totais_unicos[j].mes;
            analytics.maior_gasto.ano = totais_unicos[j].ano;
        }
        if (min_total == -1 || totais_unicos[j].total < min_total) {
            min_total = totais_unicos[j].total;
            analytics.menor_gasto.mes = totais_unicos[j].mes;
            analytics.menor_gasto.ano = totais_unicos[j].ano;
        }
    }

    // Integral discreta (Somatório)
    analytics.despesa_acumulada = integral_discreta(todos_valores, contagem);
    analytics.total_despesas = analytics.despesa_acumulada; // Mantém a compatibilidade, ou poderia ser despesas_mes_atual

    // Média Mensal
    int meses_ativos = 0;
    for (int i = 0; i < 12; i++) {
        if (totais_mensais[i] > 0) meses_ativos++;
    }
    analytics.media_mensal = meses_ativos > 0 ? analytics.despesa_acumulada / meses_ativos : 0;

    // Derivada discreta (Diferença Regressiva)
    analytics.taxa_crescimento = derivada_discreta(despesas_mes_atual, despesas_mes_anterior);

    // Aproximação linear local
    analytics.previsao_tendencia = aproximacao_linear_local(despesas_mes_atual, analytics.taxa_crescimento);

    // Sazonalidade
    analytics.mes_sazonal = analise_sazonalidade(totais_mensais, 12);

    free(todos_valores);
    free(despesas);
    return analytics;
}
