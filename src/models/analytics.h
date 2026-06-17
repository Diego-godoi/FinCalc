#ifndef ANALYTICS_H
#define ANALYTICS_H

#include "expense.h"

typedef struct {

    int mes_referencia;
    int ano_referencia;

    double total_despesas;

    double media_mensal;

    double taxa_crescimento;

    double despesa_acumulada;

    double previsao_tendencia;

    int mes_sazonal;

    Data menor_gasto;
    Data maior_gasto;

} AnaliseDespesas;


#endif