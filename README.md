# FinCalc

> Sistema de controle e análise de despesas financeiras em C.

---

## Sumário

- [Visão Geral](#visão-geral)
- [Funcionalidades](#funcionalidades)
- [Arquitetura](#arquitetura)
- [Estrutura do Projeto](#estrutura-do-projeto)
- [Modelos de Dados](#modelos-de-dados)
- [Módulos](#módulos)
  - [main](#main)
  - [models](#models)
  - [services](#services)
  - [storage](#storage)
  - [math](#math)
  - [ui](#ui)
  - [utils](#utils)
- [Compilação e Execução](#compilação-e-execução)
- [Persistência de Dados](#persistência-de-dados)
- [Licença](#licença)

---

## Visão Geral

O **FinCalc** é uma aplicação de linha de comando escrita em C puro que permite ao usuário registrar, editar, remover e listar despesas financeiras, além de gerar análises estatísticas sobre os gastos ao longo do tempo. As análises são baseadas em conceitos matemáticos discretos (integral, derivada e aproximação linear) aplicados sobre os dados históricos de despesas.

---

## Funcionalidades

| Funcionalidade | Descrição |
|---|---|
| Adicionar despesa | Registra uma nova despesa com título, categoria, valor, data e recorrência |
| Listar despesas | Exibe todas as despesas salvas |
| Editar despesa | Atualiza campos individuais de uma despesa existente |
| Remover despesa | Exclui uma despesa pelo ID |
| Análise Geral | Exibe métricas consolidadas: total acumulado, média mensal, crescimento, mês sazonal, maior e menor mês de gasto, e previsão para o próximo mês |

---

## Arquitetura

O projeto segue uma arquitetura em camadas, com separação clara de responsabilidades:

```
┌─────────────────────────┐
│          UI             │  menu_expense.c / menu_analytics.c
├─────────────────────────┤
│        Services         │  expense_service.c / analytics_service.c
├─────────────────────────┤
│        Storage          │  expense_repository.c
├─────────────────────────┤
│    Models / Math /      │  expense.h / analytics.h / math_utils.c
│       Utils             │  utils.c
└─────────────────────────┘
         │ persiste
         ▼
   data/despesas.dat  (arquivo binário)
```

---

## Estrutura do Projeto

```
FinCalc/
├── Makefile
├── README.md
├── LICENSE
├── fincalc                     ← binário gerado
├── data/
│   └── despesas.dat            ← base de dados binária (gerada em tempo de execução)
└── src/
    ├── main.c
    ├── models/
    │   ├── expense.h           ← structs Despesa e Data, enums Categoria e Frequencia
    │   └── analytics.h         ← struct AnaliseDespesas
    ├── services/
    │   ├── expense_service.c   ← CRUD de despesas (regras de negócio)
    │   ├── expense_service.h
    │   ├── analytics_service.c ← cálculo das análises financeiras
    │   └── analytics_service.h
    ├── storage/
    │   ├── expense_repository.c ← leitura/escrita binária em disco
    │   └── expense_repository.h
    ├── math/
    │   ├── math_utils.c        ← funções matemáticas discretas
    │   └── math_utils.h
    ├── ui/
    │   ├── menu.h
    │   ├── menu_expense.c      ← menus interativos de despesas
    │   └── menu_analytics.c    ← menus interativos de análises
    └── utils/
        ├── utils.c             ← helpers: limpeza de buffer, validação de data
        └── utils.h
```

---

## Modelos de Dados

### `Despesa` (`src/models/expense.h`)

```c
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
    Categoria categoria;         // ALIMENTACAO | CASA | TRANSPORTE | OUTROS
    double valor;
    Data data;                   // { dia, mes, ano }
    int recorrente;
    Frequencia frequencia;       // NENHUMA | DIARIA | SEMANAL | MENSAL | ANUAL
} Despesa;
```

### `AnaliseDespesas` (`src/models/analytics.h`)

```c
typedef struct {
    int    mes_referencia;
    int    ano_referencia;
    double total_despesas;       // igual a despesa_acumulada (compatibilidade)
    double media_mensal;         // total / nº de meses com gastos
    double taxa_crescimento;     // derivada discreta: mês atual − mês anterior
    double despesa_acumulada;    // integral discreta: soma de todos os valores
    double previsao_tendencia;   // aproximação linear: mês atual + taxa
    int    mes_sazonal;          // mês com maior volume histórico
    Data   menor_gasto;          // (mes, ano) do menor gasto registrado
    Data   maior_gasto;          // (mes, ano) do maior gasto registrado
} AnaliseDespesas;
```

---

## Módulos

### `main`

**Arquivo:** `src/main.c`

Ponto de entrada. Exibe o menu principal com as categorias **Despesas** e **Análises** e delega para as funções de UI correspondentes.

---

### `models`

Contém apenas headers com as definições de tipos compartilhados por toda a aplicação. Não possui lógica.

---

### `services`

#### `expense_service` — Regras de negócio de despesas

| Função | Descrição |
|---|---|
| `adicionar_despesa(const Despesa *)` | Gera ID automático e persiste a despesa |
| `remover_despesa(int id)` | Verifica existência e remove pelo ID |
| `editar_despesa(int id, const Despesa *)` | Aplica edição parcial (apenas campos não-nulos) |
| `listar_despesas(int *contagem)` | Retorna array alocado com todas as despesas |
| `existe_por_id(int id)` | Retorna 1 se a despesa existir, 0 caso contrário |

#### `analytics_service` — Análises financeiras

| Função | Descrição |
|---|---|
| `calcular_analise_geral()` | Carrega todas as despesas e retorna `AnaliseDespesas` preenchido |

**Funções internas (static):**

| Função | Responsabilidade |
|---|---|
| `obter_periodo_atual(mes, ano)` | Obtém mês e ano do sistema |
| `obter_mes_anterior(...)` | Calcula o mês/ano imediatamente anterior |
| `acumular_totais(...)` | Única passagem sobre os dados; preenche todos os buffers |
| `calcular_extremos_mensais(...)` | Identifica o maior e menor mês de gasto |
| `calcular_media_mensal(...)` | Divide o acumulado pelos meses com despesas |

---

### `storage`

**Arquivo:** `src/storage/expense_repository.c`

Camada de acesso a dados. Lê e escreve structs `Despesa` diretamente em arquivo binário (`data/despesas.dat`).

| Função | Descrição |
|---|---|
| `salvar_despesa(const Despesa *)` | Append binário no arquivo |
| `obter_despesa_por_id(int)` | Busca linear no arquivo; retorna ponteiro alocado |
| `remover_despesa_por_id(int)` | Reescreve o arquivo excluindo o registro |
| `editar_despesa_por_id(int, const Despesa *)` | Sobrescreve o registro no lugar (seek + write) |
| `listar_todas_despesas(int *)` | Carrega todas as despesas em memória |
| `gerar_id()` | Retorna `max_id + 1` a partir dos registros existentes |

---

### `math`

**Arquivo:** `src/math/math_utils.c`

Implementações matemáticas discretas usadas pela camada de análise.

| Função | Fórmula / Descrição |
|---|---|
| `integral_discreta(values, n)` | Σ values[i] — soma total acumulada |
| `derivada_discreta(atual, anterior)` | f(x) − f(x−1) — variação entre períodos |
| `aproximacao_linear_local(atual, deriv)` | f(x) + f′(x) — previsão para o próximo passo |
| `analise_sazonalidade(totais, n)` | Retorna o índice (mês) com maior soma histórica |

---

### `ui`

Menus interativos em terminal. Leem a entrada do usuário e chamam as funções da camada de serviços.

| Arquivo | Funções |
|---|---|
| `menu_expense.c` | `menu_despesas()` — CRUD completo via terminal |
| `menu_analytics.c` | `menu_analises()`, `resumo_geral()` — exibe as métricas calculadas |

---

### `utils`

**Arquivo:** `src/utils/utils.c`

| Função | Descrição |
|---|---|
| `limpar_buffer()` | Descarta caracteres restantes no `stdin` após leitura |
| `validar_data(Data)` | Valida dia/mês/ano com suporte a anos bissextos e meses de 30 dias |

---

## Compilação e Execução

### Pré-requisitos

- GCC (ou outro compilador C compatível com C99)
- `make`

### Compilar

```bash
make
```

O binário `fincalc` será gerado na raiz do projeto.

### Executar

```bash
./fincalc
```

### Limpar artefatos de build

```bash
make clean
```

---

## Persistência de Dados

As despesas são armazenadas em formato binário no arquivo `data/despesas.dat`. O diretório `data/` deve existir antes da primeira execução (já incluído no repositório).

> **Atenção:** o arquivo `.dat` é dependente da arquitetura e do compilador (alinhamento de struct). Não é portátil entre plataformas diferentes sem recompilação.

---

## Licença

Este projeto está licenciado sob os termos descritos em [LICENSE](LICENSE).
