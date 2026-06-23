#ifndef EXPENSE_SERVICE_H
#define EXPENSE_SERVICE_H

#include "../models/expense.h"

int adicionar_despesa(Despesa *despesa);
int remover_despesa(int id);
int editar_despesa(int id, const Despesa *despesa);
Despesa* listar_despesas(int *contagem);
int existe_por_id(int id);

#endif