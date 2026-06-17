#include <stdio.h>
#include "utils.h"

void limpar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

// Tenho que validar somente datas futuras
int validar_data(Data data) {
    if (data.ano < 1900 || data.ano > 2100) return 0;
    if (data.mes < 1 || data.mes > 12) return 0;
    if (data.dia < 1 || data.dia > 31) return 0;

    // Simple validation for months with 30 days
    if ((data.mes == 4 || data.mes == 6 || data.mes == 9 || data.mes == 11) && data.dia > 30) return 0;

    // Leap year validation for February
    if (data.mes == 2) {
        int is_leap = (data.ano % 4 == 0 && data.ano % 100 != 0) || (data.ano % 400 == 0);
        if (data.dia > (is_leap ? 29 : 28)) return 0;
    }

    return 1;
}
