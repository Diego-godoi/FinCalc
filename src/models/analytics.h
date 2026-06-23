#ifndef ANALYTICS_H
#define ANALYTICS_H

#include "expense.h"

typedef struct {

    int    mes_referencia;        /* mês de referência do cálculo            */
    int    ano_referencia;        /* ano de referência do cálculo             */

    double total_despesas;        /* soma das despesas DO MÊS ATUAL           */

    double media_mensal;          /* total acumulado / nº meses com despesas  */

    double taxa_crescimento;      /* derivada discreta: mês atual − anterior  */

    double despesa_acumulada;     /* soma histórica de TODAS as despesas      */

    double previsao_tendencia;    /* aproximação linear para o próximo mês    */

    int    mes_sazonal;           /* mês (1-12) com maior volume histórico    */

    Data   menor_gasto;           /* (mes, ano) do período de menor gasto     */
    Data   maior_gasto;           /* (mes, ano) do período de maior gasto     */

} AnaliseDespesas;


#endif