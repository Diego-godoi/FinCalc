#!/bin/bash
# ==============================================================
# Script de teste automatizado para FinCalc
# Testa: CRUD de despesas, análise financeira, cálculos matemáticos
# ==============================================================

BINARY="./fincalc"
DATA_FILE="data/despesas.dat"
PASS=0
FAIL=0
TOTAL=0

# Cores
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

pass() {
    PASS=$((PASS+1))
    TOTAL=$((TOTAL+1))
    echo -e "  ${GREEN}✓ PASS${NC}: $1"
}

fail() {
    FAIL=$((FAIL+1))
    TOTAL=$((TOTAL+1))
    echo -e "  ${RED}✗ FAIL${NC}: $1"
    if [ -n "$2" ]; then
        echo -e "    ${RED}Detalhes: $2${NC}"
    fi
}

info() {
    echo -e "  ${CYAN}ℹ INFO${NC}: $1"
}

section() {
    echo ""
    echo -e "${YELLOW}═══════════════════════════════════════════════════${NC}"
    echo -e "${YELLOW}  $1${NC}"
    echo -e "${YELLOW}═══════════════════════════════════════════════════${NC}"
}

# Limpa o arquivo de dados para começar do zero
reset_data() {
    > "$DATA_FILE"
}

# Executa fincalc com input piped, com timeout
run_fincalc() {
    printf '%s\n' "$@" | timeout 5 $BINARY 2>&1
}

# =========================================================================
# TESTE 0: Compilação
# =========================================================================
section "TESTE 0: Compilação"
if [ -f "$BINARY" ]; then
    pass "Binário fincalc existe"
else
    fail "Binário fincalc NÃO encontrado"
    echo "Execute 'make' primeiro."
    exit 1
fi

# =========================================================================
# TESTE 1: Adicionar despesas
# =========================================================================
section "TESTE 1: Adicionar Despesas"
reset_data

# Fluxo: menu_principal(1) -> menu_despesas(1) -> titulo, cat, valor, data, recorrente -> menu_despesas(5) -> menu_principal(3)

# Despesa 1: Aluguel mensal recorrente - R$ 1500.00 em 01/01/2025
OUTPUT=$(run_fincalc "1" "1" "Aluguel" "1" "1500.00" "01/01/2025" "s" "3" "5" "3")
if echo "$OUTPUT" | grep -q "Despesa adicionada com sucesso"; then
    pass "Despesa 1 (Aluguel R\$1500, recorrente mensal) adicionada"
else
    fail "Falha ao adicionar despesa 1 (Aluguel)" "$(echo "$OUTPUT" | tail -5)"
fi

# Despesa 2: Supermercado pontual - R$ 450.00 em 15/03/2025
OUTPUT=$(run_fincalc "1" "1" "Supermercado" "0" "450.00" "15/03/2025" "n" "5" "3")
if echo "$OUTPUT" | grep -q "Despesa adicionada com sucesso"; then
    pass "Despesa 2 (Supermercado R\$450, pontual) adicionada"
else
    fail "Falha ao adicionar despesa 2 (Supermercado)" "$(echo "$OUTPUT" | tail -5)"
fi

# Despesa 3: Transporte pontual - R$ 200.00 em 10/06/2025
OUTPUT=$(run_fincalc "1" "1" "Onibus" "2" "200.00" "10/06/2025" "n" "5" "3")
if echo "$OUTPUT" | grep -q "Despesa adicionada com sucesso"; then
    pass "Despesa 3 (Onibus R\$200, pontual) adicionada"
else
    fail "Falha ao adicionar despesa 3 (Onibus)" "$(echo "$OUTPUT" | tail -5)"
fi

# Despesa 4: Energia pontual - R$ 180.00 em 20/02/2025
OUTPUT=$(run_fincalc "1" "1" "Energia" "1" "180.00" "20/02/2025" "n" "5" "3")
if echo "$OUTPUT" | grep -q "Despesa adicionada com sucesso"; then
    pass "Despesa 4 (Energia R\$180, pontual) adicionada"
else
    fail "Falha ao adicionar despesa 4 (Energia)" "$(echo "$OUTPUT" | tail -5)"
fi

# =========================================================================
# TESTE 2: Listar despesas
# =========================================================================
section "TESTE 2: Listar Despesas"
OUTPUT=$(run_fincalc "1" "4" "5" "3")

if echo "$OUTPUT" | grep -q "Aluguel"; then
    pass "Despesa 'Aluguel' aparece na listagem"
else
    fail "Despesa 'Aluguel' NÃO encontrada na listagem"
fi

if echo "$OUTPUT" | grep -q "Supermercado"; then
    pass "Despesa 'Supermercado' aparece na listagem"
else
    fail "Despesa 'Supermercado' NÃO encontrada na listagem"
fi

if echo "$OUTPUT" | grep -q "Onibus"; then
    pass "Despesa 'Onibus' aparece na listagem"
else
    fail "Despesa 'Onibus' NÃO encontrada na listagem"
fi

if echo "$OUTPUT" | grep -q "Energia"; then
    pass "Despesa 'Energia' aparece na listagem"
else
    fail "Despesa 'Energia' NÃO encontrada na listagem"
fi

# Conta quantas linhas de despesas aparecem (linhas com ID numérico)
COUNT=$(echo "$OUTPUT" | grep -cE '^\s*[0-9]+\s*\|' || true)
if [ "$COUNT" -eq 4 ]; then
    pass "Total de 4 despesas listadas"
else
    fail "Esperado 4 despesas, encontrado $COUNT"
fi

# Verifica que os IDs são 1, 2, 3, 4
for id in 1 2 3 4; do
    if echo "$OUTPUT" | grep -qE "^\s*${id}\s*\|"; then
        pass "ID=$id presente na listagem"
    else
        fail "ID=$id ausente na listagem"
    fi
done

# =========================================================================
# TESTE 3: Editar despesa
# =========================================================================
section "TESTE 3: Editar Despesa"

# Editar despesa ID=2 (Supermercado) -> mudar valor para 500
# Fluxo: menu_principal(1) -> menu_despesas(3) -> id(2) -> titulo(0=skip) -> cat(-1=skip) -> valor(500) -> data(0/0/0=skip) -> recorrente(p=skip) -> menu_despesas(5) -> menu_principal(3)
OUTPUT=$(run_fincalc "1" "3" "2" "0" "-1" "500.00" "0/0/0" "p" "5" "3")
if echo "$OUTPUT" | grep -q "Despesa editada com sucesso"; then
    pass "Despesa ID=2 editada (valor 450 -> 500)"
else
    fail "Falha ao editar despesa ID=2" "$(echo "$OUTPUT" | tail -5)"
fi

# Verificar se o novo valor está correto
OUTPUT=$(run_fincalc "1" "4" "5" "3")
if echo "$OUTPUT" | grep "Supermercado" | grep -q "500.00"; then
    pass "Valor do Supermercado atualizado para 500.00"
else
    ACTUAL=$(echo "$OUTPUT" | grep "Supermercado")
    fail "Valor do Supermercado NÃO atualizado" "$ACTUAL"
fi

# =========================================================================
# TESTE 4: Remover despesa
# =========================================================================
section "TESTE 4: Remover Despesa"

# Remover despesa ID=3 (Onibus)
# Fluxo: menu_principal(1) -> menu_despesas(2) -> id(3) -> confirmacao(s) -> menu_despesas(5) -> menu_principal(3)
OUTPUT=$(run_fincalc "1" "2" "3" "s" "5" "3")
if echo "$OUTPUT" | grep -q "Despesa removida com sucesso"; then
    pass "Despesa ID=3 (Onibus) removida"
else
    fail "Falha ao remover despesa ID=3" "$(echo "$OUTPUT" | tail -5)"
fi

# Verificar que não aparece mais
OUTPUT=$(run_fincalc "1" "4" "5" "3")
if echo "$OUTPUT" | grep -q "Onibus"; then
    fail "Despesa 'Onibus' ainda aparece após remoção"
else
    pass "Despesa 'Onibus' não aparece mais na listagem"
fi

COUNT=$(echo "$OUTPUT" | grep -cE '^\s*[0-9]+\s*\|' || true)
if [ "$COUNT" -eq 3 ]; then
    pass "Total de 3 despesas após remoção"
else
    fail "Esperado 3 despesas, encontrado $COUNT"
fi

# =========================================================================
# TESTE 5: Remover despesa inexistente
# =========================================================================
section "TESTE 5: Remover Despesa Inexistente"
OUTPUT=$(run_fincalc "1" "2" "999" "s" "5" "3")
if echo "$OUTPUT" | grep -q "Despesa nao encontrada"; then
    pass "Remoção de ID inexistente tratada corretamente"
else
    fail "Não exibiu mensagem para ID inexistente" "$(echo "$OUTPUT" | tail -5)"
fi

# =========================================================================
# TESTE 6: Editar despesa inexistente
# =========================================================================
section "TESTE 6: Editar Despesa Inexistente"
OUTPUT=$(run_fincalc "1" "3" "999" "5" "3")
if echo "$OUTPUT" | grep -q "Despesa nao encontrada"; then
    pass "Edição de ID inexistente tratada corretamente"
else
    fail "Não exibiu mensagem para edição de ID inexistente" "$(echo "$OUTPUT" | tail -5)"
fi

# =========================================================================
# TESTE 7: Análise com dados controlados (3 despesas pontuais)
# =========================================================================
section "TESTE 7: Análise Geral (Dados Controlados - Pontuais)"
reset_data

# Despesa A: R$ 100.00 em 15/01/2025 (Janeiro)
run_fincalc "1" "1" "DespA" "0" "100.00" "15/01/2025" "n" "5" "3" > /dev/null
# Despesa B: R$ 200.00 em 20/02/2025 (Fevereiro)
run_fincalc "1" "1" "DespB" "1" "200.00" "20/02/2025" "n" "5" "3" > /dev/null
# Despesa C: R$ 300.00 em 10/03/2025 (Março)
run_fincalc "1" "1" "DespC" "2" "300.00" "10/03/2025" "n" "5" "3" > /dev/null

pass "3 despesas de controle adicionadas (Jan=100, Fev=200, Mar=300)"

# Rodar análise: menu_principal(2) -> menu_analises(1) -> menu_analises(2) -> menu_principal(3)
OUTPUT=$(run_fincalc "2" "1" "2" "3")

echo ""
echo "  --- Saída da análise ---"
echo "$OUTPUT" | grep -A20 "Resumo Geral" | head -10
echo "  ---"

# Total acumulado = 100 + 200 + 300 = 600
if echo "$OUTPUT" | grep "Total acumulado" | grep -q "600.00"; then
    pass "Integral discreta (acumulado) = R\$ 600.00 (100+200+300) ✓"
else
    ACTUAL=$(echo "$OUTPUT" | grep "Total acumulado" | grep -oE '[0-9]+\.[0-9]+')
    fail "Integral discreta esperada R\$ 600.00, encontrada R\$ $ACTUAL"
fi

# Mês atual é Jun/2026 - nenhuma despesa nesse mês, então total = 0
TOTAL_MES=$(echo "$OUTPUT" | grep "Total de despesas do mes atual" | grep -oE '[0-9]+\.[0-9]+')
info "Total do mês atual: R\$ $TOTAL_MES"

# Média mensal: 600 / 3 meses ativos = 200.00
if echo "$OUTPUT" | grep "Media de gastos" | grep -q "200.00"; then
    pass "Média mensal = R\$ 200.00 (600/3 meses) ✓"
else
    ACTUAL=$(echo "$OUTPUT" | grep "Media de gastos" | grep -oE '[0-9]+\.[0-9]+')
    fail "Média mensal esperada R\$ 200.00, encontrada R\$ $ACTUAL"
fi

# Maior gasto = Março (300)
if echo "$OUTPUT" | grep "Mes com maior gasto" | grep -q "03/2025"; then
    pass "Maior gasto em 03/2025 (R\$ 300) ✓"
else
    ACTUAL=$(echo "$OUTPUT" | grep "Mes com maior gasto")
    fail "Maior gasto esperado 03/2025" "$ACTUAL"
fi

# Menor gasto = Janeiro (100)
if echo "$OUTPUT" | grep "Mes com menor gasto" | grep -q "01/2025"; then
    pass "Menor gasto em 01/2025 (R\$ 100) ✓"
else
    ACTUAL=$(echo "$OUTPUT" | grep "Mes com menor gasto")
    fail "Menor gasto esperado 01/2025" "$ACTUAL"
fi

# =========================================================================
# TESTE 8: Análise com despesa recorrente mensal
# =========================================================================
section "TESTE 8: Análise com Despesa Recorrente Mensal"
reset_data

# Uma única despesa recorrente mensal de R$ 500 a partir de 01/01/2025
run_fincalc "1" "1" "Aluguel" "1" "500.00" "01/01/2025" "s" "3" "5" "3" > /dev/null
pass "Despesa recorrente mensal (R\$500, Jan/2025) adicionada"

OUTPUT=$(run_fincalc "2" "1" "2" "3")
echo ""
echo "  --- Saída da análise (recorrente mensal) ---"
echo "$OUTPUT" | grep -A10 "Resumo Geral" | head -10
echo "  ---"

# De Jan/2025 a Jun/2026 = 18 meses -> acumulado = 18 * 500 = 9000
ACUMULADO=$(echo "$OUTPUT" | grep "Total acumulado" | grep -oE '[0-9]+\.[0-9]+')
info "Total acumulado recorrente: R\$ $ACUMULADO"

# Deve ser múltiplo de 500
MOD=$(echo "$ACUMULADO" | awk '{printf "%.0f", $1 % 500}')
if [ "$MOD" -eq 0 ] && [ "$(echo "$ACUMULADO" | awk '{print ($1 > 0)}')" -eq 1 ]; then
    MESES=$(echo "$ACUMULADO" | awk '{printf "%.0f", $1 / 500}')
    pass "Acumulado R\$ $ACUMULADO = $MESES meses × R\$500 ✓"
else
    fail "Acumulado R\$ $ACUMULADO não é múltiplo exato de R\$500"
fi

# Média mensal deve ser ~500 (uniforme)
MEDIA=$(echo "$OUTPUT" | grep "Media de gastos" | grep -oE '[0-9]+\.[0-9]+')
info "Média mensal: R\$ $MEDIA"
if echo "$MEDIA" | awk '{exit ($1 >= 499 && $1 <= 501) ? 0 : 1}'; then
    pass "Média mensal R\$ $MEDIA ≈ R\$ 500 (recorrência uniforme) ✓"
else
    fail "Média mensal R\$ $MEDIA ≠ R\$ 500"
fi

# Total do mês atual deve ser R$ 500
TOTAL_MES=$(echo "$OUTPUT" | grep "Total de despesas do mes atual" | grep -oE '[0-9]+\.[0-9]+')
info "Total mês atual: R\$ $TOTAL_MES"
if echo "$TOTAL_MES" | awk '{exit ($1 >= 499 && $1 <= 501) ? 0 : 1}'; then
    pass "Total mês atual R\$ $TOTAL_MES ≈ R\$ 500 (projeção recorrente) ✓"
else
    fail "Total mês atual R\$ $TOTAL_MES (esperado ~R\$500)"
fi

# Crescimento ≈ 0 (constante)
CRESCIMENTO=$(echo "$OUTPUT" | grep "Crescimento" | grep -oE '[-]?[0-9]+\.[0-9]+')
info "Crescimento: R\$ $CRESCIMENTO"
if echo "$CRESCIMENTO" | awk '{v=$1; if(v<0)v=-v; exit (v < 1) ? 0 : 1}'; then
    pass "Derivada discreta ≈ 0 (despesa constante) ✓"
else
    fail "Derivada discreta = R\$ $CRESCIMENTO (esperado ≈ 0)"
fi

# =========================================================================
# TESTE 9: Análise sem dados
# =========================================================================
section "TESTE 9: Análise sem Dados"
reset_data

OUTPUT=$(run_fincalc "2" "1" "2" "3")
if echo "$OUTPUT" | grep "Total de despesas do mes atual" | grep -q "0.00"; then
    pass "Análise vazia retorna total = R\$ 0.00 ✓"
else
    fail "Análise vazia não retornou 0.00"
fi

if echo "$OUTPUT" | grep "Total acumulado" | grep -q "0.00"; then
    pass "Análise vazia retorna acumulado = R\$ 0.00 ✓"
else
    fail "Análise vazia não retornou acumulado 0.00"
fi

# =========================================================================
# TESTE 10: IDs incrementais
# =========================================================================
section "TESTE 10: IDs Incrementais"
reset_data

run_fincalc "1" "1" "Desp1" "0" "100" "01/01/2025" "n" "5" "3" > /dev/null
run_fincalc "1" "1" "Desp2" "0" "200" "01/02/2025" "n" "5" "3" > /dev/null
run_fincalc "1" "1" "Desp3" "0" "300" "01/03/2025" "n" "5" "3" > /dev/null

OUTPUT=$(run_fincalc "1" "4" "5" "3")

ID1=$(echo "$OUTPUT" | grep "Desp1" | awk -F'|' '{print $1}' | tr -d ' ')
ID2=$(echo "$OUTPUT" | grep "Desp2" | awk -F'|' '{print $1}' | tr -d ' ')
ID3=$(echo "$OUTPUT" | grep "Desp3" | awk -F'|' '{print $1}' | tr -d ' ')

if [ "$ID1" = "1" ] && [ "$ID2" = "2" ] && [ "$ID3" = "3" ]; then
    pass "IDs incrementais corretos: 1, 2, 3 ✓"
else
    fail "IDs não sequenciais: '$ID1', '$ID2', '$ID3'"
fi

# =========================================================================
# TESTE 11: Derivada discreta e previsão
# =========================================================================
section "TESTE 11: Derivada Discreta e Previsão Linear"
reset_data

# Mês anterior e mês atual
MES_ATUAL=$(date +%m | sed 's/^0//')
ANO_ATUAL=$(date +%Y)
if [ "$MES_ATUAL" -eq 1 ]; then
    MES_ANTERIOR=12
    ANO_ANTERIOR=$((ANO_ATUAL-1))
else
    MES_ANTERIOR=$((MES_ATUAL-1))
    ANO_ANTERIOR=$ANO_ATUAL
fi

info "Mês atual: $MES_ATUAL/$ANO_ATUAL, Mês anterior: $MES_ANTERIOR/$ANO_ANTERIOR"

# Despesa mês anterior: R$ 400
DATA_ANT=$(printf "%02d/%02d/%d" 15 "$MES_ANTERIOR" "$ANO_ANTERIOR")
run_fincalc "1" "1" "Gasto_ant" "0" "400.00" "$DATA_ANT" "n" "5" "3" > /dev/null
pass "Despesa mês anterior R\$400 ($DATA_ANT) adicionada"

# Despesa mês atual: R$ 600
DATA_ATUAL=$(printf "%02d/%02d/%d" 15 "$MES_ATUAL" "$ANO_ATUAL")
run_fincalc "1" "1" "Gasto_atual" "0" "600.00" "$DATA_ATUAL" "n" "5" "3" > /dev/null
pass "Despesa mês atual R\$600 ($DATA_ATUAL) adicionada"

OUTPUT=$(run_fincalc "2" "1" "2" "3")
echo ""
echo "  --- Saída da análise ---"
echo "$OUTPUT" | grep -A10 "Resumo Geral" | head -10
echo "  ---"

# Derivada = 600 - 400 = 200
CRESCIMENTO=$(echo "$OUTPUT" | grep "Crescimento" | grep -oE '[-]?[0-9]+\.[0-9]+')
if echo "$CRESCIMENTO" | awk '{exit ($1 >= 199 && $1 <= 201) ? 0 : 1}'; then
    pass "Derivada discreta = $CRESCIMENTO ≈ 200 (600-400) ✓"
else
    fail "Derivada discreta = $CRESCIMENTO (esperado ≈ 200)"
fi

# Total mês atual = 600
TOTAL_MES=$(echo "$OUTPUT" | grep "Total de despesas do mes atual" | grep -oE '[0-9]+\.[0-9]+')
if echo "$TOTAL_MES" | awk '{exit ($1 >= 599 && $1 <= 601) ? 0 : 1}'; then
    pass "Total mês atual = R\$ $TOTAL_MES ≈ R\$ 600 ✓"
else
    fail "Total mês atual = R\$ $TOTAL_MES (esperado ≈ R\$600)"
fi

# Acumulado = 400 + 600 = 1000
ACUMULADO=$(echo "$OUTPUT" | grep "Total acumulado" | grep -oE '[0-9]+\.[0-9]+')
if echo "$ACUMULADO" | awk '{exit ($1 >= 999 && $1 <= 1001) ? 0 : 1}'; then
    pass "Integral discreta = R\$ $ACUMULADO ≈ R\$ 1000 (400+600) ✓"
else
    fail "Integral discreta = R\$ $ACUMULADO (esperado ≈ R\$ 1000)"
fi

# Média = 1000 / 2 = 500
MEDIA=$(echo "$OUTPUT" | grep "Media de gastos" | grep -oE '[0-9]+\.[0-9]+')
if echo "$MEDIA" | awk '{exit ($1 >= 499 && $1 <= 501) ? 0 : 1}'; then
    pass "Média mensal = R\$ $MEDIA ≈ R\$ 500 (1000/2) ✓"
else
    fail "Média mensal = R\$ $MEDIA (esperado ≈ R\$ 500)"
fi

# =========================================================================
# TESTE 12: Sazonalidade e Extremos com múltiplas despesas
# =========================================================================
section "TESTE 12: Sazonalidade e Extremos"
reset_data

# Jan: R$50, Fev: R$100, Mar: R$800, Abr: R$30
run_fincalc "1" "1" "Jan" "0" "50.00" "15/01/2025" "n" "5" "3" > /dev/null
run_fincalc "1" "1" "Fev" "0" "100.00" "15/02/2025" "n" "5" "3" > /dev/null
run_fincalc "1" "1" "Mar" "0" "800.00" "15/03/2025" "n" "5" "3" > /dev/null
run_fincalc "1" "1" "Abr" "0" "30.00" "15/04/2025" "n" "5" "3" > /dev/null
pass "4 despesas de sazonalidade adicionadas (Jan=50,Fev=100,Mar=800,Abr=30)"

OUTPUT=$(run_fincalc "2" "1" "2" "3")
echo ""
echo "  --- Saída da análise ---"
echo "$OUTPUT" | grep -A10 "Resumo Geral" | head -10
echo "  ---"

# Maior gasto: Março (800)
if echo "$OUTPUT" | grep "Mes com maior gasto" | grep -q "03/2025"; then
    pass "Maior gasto = 03/2025 (R\$800) ✓"
else
    ACTUAL=$(echo "$OUTPUT" | grep "Mes com maior gasto")
    fail "Maior gasto esperado 03/2025" "$ACTUAL"
fi

# Menor gasto: Abril (30)
if echo "$OUTPUT" | grep "Mes com menor gasto" | grep -q "04/2025"; then
    pass "Menor gasto = 04/2025 (R\$30) ✓"
else
    ACTUAL=$(echo "$OUTPUT" | grep "Mes com menor gasto")
    fail "Menor gasto esperado 04/2025" "$ACTUAL"
fi

# Acumulado: 50 + 100 + 800 + 30 = 980
ACUMULADO=$(echo "$OUTPUT" | grep "Total acumulado" | grep -oE '[0-9]+\.[0-9]+')
if echo "$ACUMULADO" | awk '{exit ($1 >= 979 && $1 <= 981) ? 0 : 1}'; then
    pass "Acumulado = R\$ $ACUMULADO ≈ R\$ 980 (50+100+800+30) ✓"
else
    fail "Acumulado = R\$ $ACUMULADO (esperado ≈ R\$ 980)"
fi

# Média = 980 / 4 = 245
MEDIA=$(echo "$OUTPUT" | grep "Media de gastos" | grep -oE '[0-9]+\.[0-9]+')
if echo "$MEDIA" | awk '{exit ($1 >= 244 && $1 <= 246) ? 0 : 1}'; then
    pass "Média mensal = R\$ $MEDIA ≈ R\$ 245 (980/4) ✓"
else
    fail "Média mensal = R\$ $MEDIA (esperado ≈ R\$ 245)"
fi

# =========================================================================
# TESTE 13: Campos calculados mas não exibidos na UI
# =========================================================================
section "TESTE 13: Campos Calculados vs Exibidos"

info "Verificando se todos os campos calculados são exibidos na UI..."

if echo "$OUTPUT" | grep -qi "sazonal\|previs"; then
    pass "Campos sazonal/previsão exibidos na UI"
else
    info "Campos 'mes_sazonal' e 'previsao_tendencia' NÃO são exibidos no resumo."
    info "Eles são calculados na struct AnaliseDespesas mas omitidos do menu_analytics.c."
    fail "Campos calculados (mes_sazonal, previsao_tendencia) não são exibidos na UI"
fi

# =========================================================================
# TESTE 14: Validação de data (ano bissexto)
# =========================================================================
section "TESTE 14: Validação de Data"

# Testar 29/02/2024 (2024 é bissexto) - deve aceitar
OUTPUT=$(run_fincalc "1" "1" "Bissexto" "0" "50.00" "29/02/2024" "n" "5" "3")
if echo "$OUTPUT" | grep -q "Despesa adicionada com sucesso"; then
    pass "Data 29/02/2024 (ano bissexto) aceita ✓"
else
    fail "Data 29/02/2024 rejeitada (deveria aceitar - ano bissexto)"
fi

# =========================================================================
# TESTE 15: Despesa com valor zero
# =========================================================================
section "TESTE 15: Cenários de Borda"
reset_data

# Despesa com valor 0
OUTPUT=$(run_fincalc "1" "1" "Gratis" "0" "0.00" "15/01/2025" "n" "5" "3")
if echo "$OUTPUT" | grep -q "Despesa adicionada com sucesso"; then
    pass "Despesa com valor R\$0.00 adicionada (sem validação de valor mínimo)"
    info "NOTA: O sistema aceita valor zero - considere se isso deveria ser permitido"
else
    pass "Despesa com valor R\$0.00 rejeitada"
fi

# =========================================================================
# RESUMO FINAL
# =========================================================================
section "RESUMO FINAL"
echo ""
echo -e "  Total de testes: $TOTAL"
echo -e "  ${GREEN}Passou:  $PASS${NC}"
echo -e "  ${RED}Falhou:  $FAIL${NC}"
echo ""

if [ $FAIL -eq 0 ]; then
    echo -e "  ${GREEN}🎉 Todos os testes passaram!${NC}"
else
    echo -e "  ${YELLOW}⚠️  Há $FAIL falha(s) a verificar.${NC}"
fi
echo ""

# Limpar no final
reset_data
exit 0
