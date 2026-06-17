#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../models/expense.h"
#include "../storage/expense_repository.h"

int adicionar_despesa(Despesa despesa){
    despesa.id = gerar_id();
    if(!salvar_despesa(despesa)){
        fprintf(stderr, "Erro ao salvar despesa!\n");
        return 0;
    }
    printf("Despesa adicionada com sucesso!\n");
    return 1;
}

int remover_despesa(int id){
    Despesa *despesa = obter_despesa_por_id(id);
    if(despesa == NULL){
        printf("Despesa nao encontrada!\n");
        return 0;
    }
    free(despesa);
    if(!remover_despesa_por_id(id)){
        fprintf(stderr, "Erro ao remover despesa!\n");
        return 0;
    }
    printf("Despesa removida com sucesso!\n");
    return 1;
}

int editar_despesa(int id, Despesa despesa){
    Despesa *despesa_antiga = obter_despesa_por_id(id);
    if(despesa_antiga == NULL){
        printf("Despesa nao encontrada!\n");
        return 0;
    }
    if(strlen(despesa.titulo) > 0 && strcmp(despesa.titulo, "\n") != 0){
        strcpy(despesa_antiga->titulo, despesa.titulo);
    }
    if ((int)despesa.categoria != -1) {
        despesa_antiga->categoria = despesa.categoria;
    }
    if (despesa.valor != -1.0) {
        despesa_antiga->valor = despesa.valor;
    }
    if (despesa.data.dia != 0) {
        despesa_antiga->data = despesa.data;
    }
    if (despesa.recorrente != -1) {
        despesa_antiga->recorrente = despesa.recorrente;
        despesa_antiga->frequencia = despesa.frequencia;
    }
    
    if(!editar_despesa_por_id(id, *despesa_antiga)){
        fprintf(stderr, "Erro ao editar despesa!\n");
        free(despesa_antiga);
        return 0;
    }
    free(despesa_antiga);
    printf("Despesa editada com sucesso!\n");
    return 1;
}

Despesa* listar_despesas(int *contagem){
    return listar_todas_despesas(contagem);
}

int existe_por_id(int id){
    Despesa *despesa = obter_despesa_por_id(id);
    if (despesa != NULL) {
        free(despesa);
        return 1;
    }
    return 0;
}