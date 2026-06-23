#ifndef EXPENSE_H
#define EXPENSE_H

typedef enum {
    ALIMENTACAO = 0,
    CASA = 1,
    TRANSPORTE = 2,
    OUTROS = 3
} Categoria;

typedef enum {
    NENHUMA = 0,
    DIARIA = 1,
    SEMANAL = 2,
    MENSAL = 3,
    ANUAL = 4
} Frequencia;

typedef struct data {
    int dia;
    int mes;
    int ano;
} Data;

typedef struct despesa {
    int id;
    char titulo[100];
    Categoria categoria;
    double valor;
    Data data;
    int recorrente;
    Frequencia frequencia;
} Despesa;

#endif