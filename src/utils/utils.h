#ifndef UTILS_H
#define UTILS_H

#include <time.h>
#include "../models/expense.h"

void limpar_buffer();
int validar_data(Data data);
void obter_periodo_atual(int *mes, int *ano);
void obter_mes_anterior(int mes_atual, int ano_atual, int *mes_ant, int *ano_ant);
int ler_data_opcional(Data *data);
void ler_data(Data *data);
const char* obter_nome_categoria(Categoria c);
const char* obter_nome_frequencia(Frequencia f);

// Funções de manipulação de tempo/data
time_t obter_limite_tempo_atual(void);
time_t converter_data_para_tempo(Data data);
int avancar_data_despesa(struct tm *tm_desp, const Despesa *despesa);

#endif
