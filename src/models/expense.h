#ifndef EXPENSE_H
#define EXPENSE_H

enum Categoria{
    ALIMENTACAO,
    CASA,
    TRANSPORTE,
    OUTROS
};
enum Frequencia{
    NENHUMA,
    DIARIA,
    SEMANAL,
    MENSAL,
    ANUAL
};
typedef struct data{
    int dia;
    int mes;
    int ano;
}Data;

typedef struct despesa{

    int id;

    char titulo[100];
    enum Categoria categoria;

    double valor;

    Data data;

    int recorrente;

    enum Frequencia frequencia;

} Despesa;

#endif