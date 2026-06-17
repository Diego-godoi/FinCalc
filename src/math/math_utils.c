#include "math_utils.h"

// Derivada discreta (Diferença Regressiva) - taxa de variação (f(x) - f(x-1))
double derivada_discreta(double current_value, double previous_value) {
    return current_value - previous_value;
}

// Integral discreta (Somatório) - total acumulado
double integral_discreta(double* values, int contagem) {
    double sum = 0.0;
    for (int i = 0; i < contagem; i++) {
        sum += values[i];
    }
    return sum;
}

// Aproximação linear local - f(x+1) = f(x) + f'(x) * delta_x (delta_x = 1 mês)
double aproximacao_linear_local(double current_value, double derivative) {
    return current_value + derivative; // delta_x = 1
}

// Análise de sazonalidade - identifica o mês com a maior soma de despesas
int analise_sazonalidade(double* totais_mensais, int num_months) {
    int mes_sazonal = 1;
    double max_expense = -1.0;
    for (int i = 0; i < num_months; i++) {
        if (totais_mensais[i] > max_expense) {
            max_expense = totais_mensais[i];
            mes_sazonal = i + 1;
        }
    }
    return mes_sazonal;
}
