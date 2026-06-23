#ifndef MATH_UTILS_H
#define MATH_UTILS_H

// Derivada discreta (Diferença Regressiva) - taxa de variação
double derivada_discreta(double current_value, double previous_value);

// Integral discreta (Somatório)
double integral_discreta(double* values, int contagem);

// Aproximação linear local - previsão para o próximo mês
double aproximacao_linear_local(double current_value, double derivative);

// Análise de sazonalidade - retorna o mês com maior aumento ou maior despesa
int analise_sazonalidade(double* totais_mensais, int num_months);

// Média mensal - divide o total acumulado pelos meses com ao menos uma despesa
double calcular_media_mensal(double total_acumulado, const double totais_mensais[12]);

#endif
