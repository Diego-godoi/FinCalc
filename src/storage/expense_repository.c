#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "expense_repository.h"

#define FILE_PATH "data/despesas.dat"

int gerar_id() {
    FILE *file = fopen(FILE_PATH, "rb");
    if (!file) return 1;

    Despesa desp;
    int max_id = 0;
    while (fread(&desp, sizeof(Despesa), 1, file)) {
        if (desp.id > max_id) {
            max_id = desp.id;
        }
    }
    fclose(file);
    return max_id + 1;
}

int salvar_despesa(Despesa despesa) {
    FILE *file = fopen(FILE_PATH, "ab");
    if (!file) return 0;
    
    int result = fwrite(&despesa, sizeof(Despesa), 1, file);
    fclose(file);
    return result > 0;
}

Despesa* obter_despesa_por_id(int id) {
    FILE *file = fopen(FILE_PATH, "rb");
    if (!file) return NULL;

    Despesa *desp = malloc(sizeof(Despesa));
    if (!desp) {
        fclose(file);
        return NULL;
    }

    while (fread(desp, sizeof(Despesa), 1, file)) {
        if (desp->id == id) {
            fclose(file);
            return desp;
        }
    }
    
    free(desp);
    fclose(file);
    return NULL;
}

int remover_despesa_por_id(int id) {
    FILE *file = fopen(FILE_PATH, "rb");
    if (!file) return 0;

    FILE *temp = fopen("data/temp.dat", "wb");
    if (!temp) {
        fclose(file);
        return 0;
    }

    Despesa desp;
    int found = 0;
    while (fread(&desp, sizeof(Despesa), 1, file)) {
        if (desp.id != id) {
            fwrite(&desp, sizeof(Despesa), 1, temp);
        } else {
            found = 1;
        }
    }
    
    fclose(file);
    fclose(temp);

    remove(FILE_PATH);
    rename("data/temp.dat", FILE_PATH);
    return found;
}

int editar_despesa_por_id(int id, Despesa despesa_atualizada) {
    FILE *file = fopen(FILE_PATH, "rb+");
    if (!file) return 0;

    Despesa desp;
    int found = 0;
    while (fread(&desp, sizeof(Despesa), 1, file)) {
        if (desp.id == id) {
            fseek(file, -sizeof(Despesa), SEEK_CUR);
            fwrite(&despesa_atualizada, sizeof(Despesa), 1, file);
            found = 1;
            break;
        }
    }
    
    fclose(file);
    return found;
}

Despesa* listar_todas_despesas(int *contagem) {
    FILE *file = fopen(FILE_PATH, "rb");
    *contagem = 0;
    if (!file) return NULL;

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (size == 0) {
        fclose(file);
        return NULL;
    }

    int num_items = size / sizeof(Despesa);
    Despesa *despesas = malloc(size);
    if (!despesas) {
        fclose(file);
        return NULL;
    }

    fread(despesas, sizeof(Despesa), num_items, file);
    fclose(file);

    *contagem = num_items;
    return despesas;
}
