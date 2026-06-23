#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include <time.h>

void limpar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

int validar_data(Data data) {
    if (data.ano < 1900 || data.ano > 2100) return 0;
    if (data.mes < 1 || data.mes > 12) return 0;
    if (data.dia < 1 || data.dia > 31) return 0;

    if ((data.mes == 4 || data.mes == 6 || data.mes == 9 || data.mes == 11) && data.dia > 30) return 0;

    if (data.mes == 2) {
        int is_leap = (data.ano % 4 == 0 && data.ano % 100 != 0) || (data.ano % 400 == 0);
        if (data.dia > (is_leap ? 29 : 28)) return 0;
    }

    return 1;
}

void obter_periodo_atual(int *mes, int *ano) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    *mes = tm.tm_mon + 1;
    *ano = tm.tm_year + 1900;
}

void obter_mes_anterior(int mes_atual, int ano_atual, int *mes_ant, int *ano_ant) {
    *mes_ant = mes_atual - 1;
    *ano_ant = ano_atual;
    if (*mes_ant == 0) {
        *mes_ant = 12;
        (*ano_ant)--;
    }
}

// Analisa uma string no formato dd/mm/aaaa
static int parse_data_string(char *buffer, Data *data) {
    char *p = buffer;
    data->dia = atoi(p);
    p = strchr(p, '/');
    if (!p) return 0;
    p++;
    data->mes = atoi(p);
    p = strchr(p, '/');
    if (!p) return 0;
    p++;
    data->ano = atoi(p);
    return 1;
}

void ler_data(Data *data) {
    char buffer[256];
    do {
        printf("Insira a data (dd/mm/aaaa): ");
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            if (parse_data_string(buffer, data) && validar_data(*data)) {
                break;
            }
        }
    } while(1);
}

// Retorna 1 se leu com sucesso, 0 se pulou
int ler_data_opcional(Data *data) {
    char buffer[256];
    do {
        printf("Insira a data (dd/mm/aaaa) (0/0/0 para pular): ");
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            if (parse_data_string(buffer, data)) {
                if (data->dia == 0 && data->mes == 0 && data->ano == 0) return 0;
                if (validar_data(*data)) return 1;
            }
        }
    } while(1);
}

const char* obter_nome_categoria(Categoria c) {
    switch(c) {
        case ALIMENTACAO: return "ALIMENTACAO";
        case CASA: return "CASA";
        case TRANSPORTE: return "TRANSPORTE";
        case OUTROS: return "OUTROS";
        default: return "DESCONHECIDO";
    }
}

const char* obter_nome_frequencia(Frequencia f) {
    switch(f) {
        case NENHUMA: return "NENHUMA";
        case DIARIA: return "DIARIA";
        case SEMANAL: return "SEMANAL";
        case MENSAL: return "MENSAL";
        case ANUAL: return "ANUAL";
        default: return "DESCONHECIDA";
    }
}

time_t obter_limite_tempo_atual(void) {
    time_t t_now = time(NULL);
    struct tm *tm_now = localtime(&t_now);
    struct tm tm_limite = {0};
    tm_limite.tm_year = tm_now->tm_year;
    tm_limite.tm_mon  = tm_now->tm_mon;
    tm_limite.tm_mday = tm_now->tm_mday;
    tm_limite.tm_hour = 23;
    tm_limite.tm_min  = 59;
    tm_limite.tm_sec  = 59;
    return mktime(&tm_limite);
}

time_t converter_data_para_tempo(Data data) {
    struct tm tm_desp = {0};
    tm_desp.tm_year = data.ano - 1900;
    tm_desp.tm_mon  = data.mes - 1;
    tm_desp.tm_mday = data.dia;
    tm_desp.tm_hour = 12; /* meio dia para evitar fuso */
    tm_desp.tm_isdst = -1;
    return mktime(&tm_desp);
}

int avancar_data_despesa(struct tm *tm_desp, const Despesa *despesa) {
    if (!despesa->recorrente) return 0;

    if (despesa->frequencia == DIARIA) {
        tm_desp->tm_mday += 1;
    } else if (despesa->frequencia == SEMANAL) {
        tm_desp->tm_mday += 7;
    } else if (despesa->frequencia == MENSAL) {
        tm_desp->tm_mon += 1;
    } else if (despesa->frequencia == ANUAL) {
        tm_desp->tm_year += 1;
    } else {
        return 0;
    }
    tm_desp->tm_isdst = -1;
    return 1;
}