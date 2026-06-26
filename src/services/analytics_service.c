#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "analytics_service.h"
#include "../storage/expense_repository.h"
#include "../math/math_utils.h"
#include "../utils/utils.h"

#define MAX_PERIODOS 1000

typedef struct {
    int    ano;
    int    mes;
    double total;
} PeriodoTotal;

/* ==========================================================================
 * FUNÇÕES AUXILIARES DE AGREGAÇÃO
 * ========================================================================== */

static void acumular_periodo(PeriodoTotal *periodos, int *num_periodos, int ano, int mes, double valor) {
    for (int j = 0; j < *num_periodos; j++) {
        if (periodos[j].ano == ano && periodos[j].mes == mes) {
            periodos[j].total += valor;
            return;
        }
    }
    if (*num_periodos < MAX_PERIODOS) {
        periodos[*num_periodos].ano   = ano;
        periodos[*num_periodos].mes   = mes;
        periodos[*num_periodos].total = valor;
        (*num_periodos)++;
    }
}

static void adicionar_valor_dinamico(double **todos_valores, int *idx, int *capacidade, double valor) {
    if (*idx >= *capacidade) {
        *capacidade *= 2;
        *todos_valores = (double *)realloc(*todos_valores, (*capacidade) * sizeof(double));
    }
    (*todos_valores)[(*idx)++] = valor;
}

/* --------------------------------------------------------------------------
 * acumular_totais
 *   Itera sobre o vetor de despesas e preenche os totais projetando
 *   ocorrências recorrentes usando mktime.
 * -------------------------------------------------------------------------- */
static void acumular_totais(const Despesa *despesas, int contagem,
                            double **todos_valores, int *qtd_valores,
                            double totais_mensais[12],
                            PeriodoTotal *periodos, int *num_periodos,
                            int mes_atual, int ano_atual,
                            int mes_anterior, int ano_anterior,
                            double *total_mes_atual,
                            double *total_mes_anterior) {
    int capacidade = contagem > 0 ? contagem : 10;
    int idx_valores = 0;
    time_t limite = obter_limite_tempo_atual();

    for (int i = 0; i < contagem; i++) {
        time_t t_desp = converter_data_para_tempo(despesas[i].data);
        struct tm tm_desp = *localtime(&t_desp);

        while (t_desp <= limite) {
            adicionar_valor_dinamico(todos_valores, &idx_valores, &capacidade, despesas[i].valor);

            int mes = tm_desp.tm_mon + 1;
            int ano = tm_desp.tm_year + 1900;

            if (mes >= 1 && mes <= 12) {
                totais_mensais[mes - 1] += despesas[i].valor;
            }

            acumular_periodo(periodos, num_periodos, ano, mes, despesas[i].valor);

            if (ano == ano_atual && mes == mes_atual) {
                *total_mes_atual += despesas[i].valor;
            } else if (ano == ano_anterior && mes == mes_anterior) {
                *total_mes_anterior += despesas[i].valor;
            }

            if (!avancar_data_despesa(&tm_desp, &despesas[i])) {
                break;
            }
            t_desp = mktime(&tm_desp);
        }
    }
    *qtd_valores = idx_valores;
}

/* --------------------------------------------------------------------------
 * calcular_extremos_mensais
 *   Percorre os períodos agregados e identifica o mês com maior e menor
 *   gasto total, preenchendo maior_gasto e menor_gasto em *analytics.
 * -------------------------------------------------------------------------- */
static void calcular_extremos_mensais(const PeriodoTotal *periodos,
                                      int num_periodos,
                                      AnaliseDespesas *analytics) {
    double max_total = -1.0;
    double min_total = -1.0;

    for (int i = 0; i < num_periodos; i++) {
        if (max_total < 0.0 || periodos[i].total > max_total) {
            max_total = periodos[i].total;
            analytics->maior_gasto.mes = periodos[i].mes;
            analytics->maior_gasto.ano = periodos[i].ano;
        }
        if (min_total < 0.0 || periodos[i].total < min_total) {
            min_total = periodos[i].total;
            analytics->menor_gasto.mes = periodos[i].mes;
            analytics->menor_gasto.ano = periodos[i].ano;
        }
    }
}


AnaliseDespesas calcular_analise_geral() {
    AnaliseDespesas analytics = {0};

    int contagem = 0;
    Despesa *despesas = listar_todas_despesas(&contagem);
    if (contagem == 0 || despesas == NULL) {
        return analytics;
    }

    int mes_atual, ano_atual;
    obter_periodo_atual(&mes_atual, &ano_atual);

    int mes_anterior, ano_anterior;
    obter_mes_anterior(mes_atual, ano_atual, &mes_anterior, &ano_anterior);

    analytics.mes_referencia = mes_atual;
    analytics.ano_referencia = ano_atual;

    double *todos_valores = (double *)malloc((contagem > 0 ? contagem : 10) * sizeof(double));
    if (!todos_valores) {
        free(despesas);
        return analytics;
    }

    double totais_mensais[12]          = {0};
    PeriodoTotal periodos[MAX_PERIODOS] = {0};
    int num_periodos                    = 0;
    double total_mes_atual              = 0.0;
    double total_mes_anterior           = 0.0;
    int qtd_valores                     = 0;

    /* Passa única sobre os dados */
    acumular_totais(despesas, contagem,
                    &todos_valores, &qtd_valores,
                    totais_mensais,
                    periodos, &num_periodos,
                    mes_atual, ano_atual,
                    mes_anterior, ano_anterior,
                    &total_mes_atual, &total_mes_anterior);

    /* Extremos (maior/menor mês) */
    calcular_extremos_mensais(periodos, num_periodos, &analytics);

    /* Métricas matemáticas */
    analytics.despesa_acumulada  = integral_discreta(todos_valores, qtd_valores); /* soma histórica total projetada */
    analytics.total_despesas     = total_mes_atual;                            /* soma apenas do mês atual */
    analytics.media_mensal       = calcular_media_mensal(analytics.despesa_acumulada,
                                                         num_periodos);
    analytics.taxa_crescimento   = derivada_discreta(total_mes_atual, total_mes_anterior);
    analytics.previsao_tendencia = aproximacao_linear_local(total_mes_atual,
                                                            analytics.taxa_crescimento);
    analytics.mes_sazonal        = analise_sazonalidade(totais_mensais, 12);

    free(todos_valores);
    free(despesas);
    return analytics;
}
