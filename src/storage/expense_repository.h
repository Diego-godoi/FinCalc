#ifndef EXPENSE_REPOSITORY_H
#define EXPENSE_REPOSITORY_H

#include "../models/expense.h"

int salvar_despesa(Despesa despesa);
Despesa* obter_despesa_por_id(int id);
int remover_despesa_por_id(int id);
int editar_despesa_por_id(int id, Despesa despesa);
Despesa* listar_todas_despesas(int *contagem);
int gerar_id();

#endif
