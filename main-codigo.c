/******************************************************************************

                            Online C Compiler.
                Code, Compile, Run and Debug C program online.
Write your code in this editor and press "Run" button to compile and execute it.

*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARQUIVO_DADOS "pedidos.csv"
#define MAX_STR 100

/* =========================================================
 * 1. ESTRUTURAS DE DADOS
 * ========================================================= */

typedef struct {
    int id;
    char cliente[MAX_STR];
    char descricao[MAX_STR];
} Pedido;

typedef struct Node {
    Pedido pedido;
    struct Node* proximo;
} Node;

// Fila: Pedidos pendentes na cozinha
typedef struct {
    Node* inicio;
    Node* fim;
} Fila;

// Pilha: Histórico de pedidos concluídos para a função "Desfazer"
typedef struct {
    Node* topo;
} Pilha;

/* =========================================================
 * 2. FUNÇÕES BASE E ALOCAÇÃO
 * ========================================================= */

Node* criar_no(Pedido p) {
    Node* novo = (Node*)malloc(sizeof(Node));
    if (novo == NULL) {
        printf("Erro fatal: falha na alocacao de memoria.\n");
        exit(1);
    }
    novo->pedido = p;
    novo->proximo = NULL;
    return novo;
}

void inicializar_fila(Fila* f) {
    f->inicio = NULL;
    f->fim = NULL;
}

void inicializar_pilha(Pilha* p) {
    p->topo = NULL;
}

/* =========================================================
 * 3. OPERAÇÕES DE FILA (Pedidos Pendentes)
 * ========================================================= */

void enfileirar(Fila* f, Pedido p) {
    Node* novo = criar_no(p);
    if (f->fim == NULL) {
        f->inicio = novo;
        f->fim = novo;
    } else {
        f->fim->proximo = novo;
        f->fim = novo;
    }
}

// Inserção especial no início para a funcionalidade "Desfazer"
void enfileirar_inicio(Fila* f, Pedido p) {
    Node* novo = criar_no(p);
    if (f->inicio == NULL) {
        f->inicio = novo;
        f->fim = novo;
    } else {
        novo->proximo = f->inicio;
        f->inicio = novo;
    }
}

int fila_vazia(Fila* f) {
    return f->inicio == NULL;
}

Pedido desenfileirar(Fila* f) {
    Pedido p = {0, "", ""};
    if (fila_vazia(f)) return p;

    Node* removido = f->inicio;
    p = removido->pedido;
    
    f->inicio = f->inicio->proximo;
    if (f->inicio == NULL) {
        f->fim = NULL;
    }
    
    free(removido);
    return p;
}

// Remove um pedido específico da fila pelo seu ID (Tratando como Lista Simples)
// Retorna 1 se encontrou e removeu, ou 0 se não encontrou.
int cancelar_por_id(Fila* f, int id_procurado) {
    if (fila_vazia(f)) {
        return 0;
    }

    Node* atual = f->inicio;
    Node* anterior = NULL;

    // Percorre a fila até encontrar o ID ou chegar ao fim
    while (atual != NULL && atual->pedido.id != id_procurado) {
        anterior = atual;
        atual = atual->proximo;
    }

    // Se atual for NULL, significa que o ID não está na fila
    if (atual == NULL) {
        return 0;
    }

    // Se o elemento a ser removido for o primeiro da fila
    if (anterior == NULL) {
        f->inicio = atual->proximo;
        // Se a fila possuía apenas um elemento, atualiza o 'fim' para NULL
        if (f->inicio == NULL) {
            f->fim = NULL;
        }
    } 
    // Se o elemento estiver no meio ou no fim
    else {
        anterior->proximo = atual->proximo;
        // Se era o último elemento, o 'fim' passa a apontar para o anterior
        if (atual->proximo == NULL) {
            f->fim = anterior;
        }
    }

    free(atual); // Libera a memória do nó removido
    return 1;
}

void listar_fila(Fila* f) {
    if (fila_vazia(f)) {
        printf("\n=> Nenhum pedido pendente na cozinha no momento.\n");
        return;
    }
    
    printf("\n--- FILA DE PEDIDOS PENDENTES ---\n");
    Node* atual = f->inicio;
    while (atual != NULL) {
        printf("[ID: %d] Cliente: %s | Pedido: %s\n", 
               atual->pedido.id, atual->pedido.cliente, atual->pedido.descricao);
        atual = atual->proximo;
    }
    printf("---------------------------------\n");
}

/* =========================================================
 * 4. OPERAÇÕES DE PILHA (Histórico para Desfazer)
 * ========================================================= */

void empilhar(Pilha* p, Pedido ped) {
    Node* novo = criar_no(ped);
    novo->proximo = p->topo;
    p->topo = novo;
}

int pilha_vazia(Pilha* p) {
    return p->topo == NULL;
}

Pedido desempilhar(Pilha* p) {
    Pedido ped = {0, "", ""};
    if (pilha_vazia(p)) return ped;

    Node* removido = p->topo;
    ped = removido->pedido;
    
    p->topo = p->topo->proximo;
    free(removido);
    return ped;
}

/* =========================================================
 * 5. PERSISTÊNCIA EM ARQUIVO (CSV)
 * ========================================================= */

int carregar_dados(Fila* f) {
    FILE* file = fopen(ARQUIVO_DADOS, "r");
    if (file == NULL) {
        return 1; // Arquivo não existe, próximo ID é 1.
    }

    char linha[256];
    int maior_id = 0;
    
    while (fgets(linha, sizeof(linha), file)) {
        linha[strcspn(linha, "\n")] = 0; // Remove newline
        
        Pedido p;
        char* token = strtok(linha, ";");
        if (token != NULL) p.id = atoi(token);
        
        token = strtok(NULL, ";");
        if (token != NULL) strncpy(p.cliente, token, MAX_STR);
        
        token = strtok(NULL, ";");
        if (token != NULL) strncpy(p.descricao, token, MAX_STR);
        
        enfileirar(f, p);
        if (p.id > maior_id) maior_id = p.id;
    }
    
    fclose(file);
    return maior_id + 1; // Retorna o próximo ID disponível
}

void salvar_dados(Fila* f) {
    FILE* file = fopen(ARQUIVO_DADOS, "w");
    if (file == NULL) {
        printf("Erro ao salvar arquivo de dados.\n");
        return;
    }

    Node* atual = f->inicio;
    while (atual != NULL) {
        fprintf(file, "%d;%s;%s\n", atual->pedido.id, atual->pedido.cliente, atual->pedido.descricao);
        atual = atual->proximo;
    }
    fclose(file);
}

/* =========================================================
 * 6. LIMPEZA DE MEMÓRIA (Boa prática em C)
 * ========================================================= */

void liberar_memoria(Fila* f, Pilha* p) {
    while (!fila_vazia(f)) desenfileirar(f);
    while (!pilha_vazia(p)) desempilhar(p);
}

/* =========================================================
 * 7. FUNÇÃO PRINCIPAL / MENU
 * ========================================================= */

void limpar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int main() {
    Fila pedidos;
    Pilha historico;
    
    inicializar_fila(&pedidos);
    inicializar_pilha(&historico);
    
    int proximo_id = carregar_dados(&pedidos);
    int opcao = 0;

    do {
        printf("\n=== Sistema de Pedidos da Lanchonete ===\n");
        printf("1 - Adicionar novo pedido\n");
        printf("2 - Listar pedidos pendentes\n");
        printf("3 - Concluir proximo pedido da fila\n");
        printf("4 - Desfazer ultima conclusao\n");
        printf("5 - Cancelar pedido por ID\n");
        printf("6 - Sair\n");
        printf("Escolha: ");
        
        if (scanf("%d", &opcao) != 1) {
            opcao = -1; // Força erro no switch
        }
        limpar_buffer();

        switch (opcao) {
            case 1: {
                Pedido p;
                p.id = proximo_id++;
                printf("Nome do cliente: ");
                fgets(p.cliente, MAX_STR, stdin);
                p.cliente[strcspn(p.cliente, "\n")] = 0;
                
                printf("Descricao do pedido (ex: 2 X-Salada): ");
                fgets(p.descricao, MAX_STR, stdin);
                p.descricao[strcspn(p.descricao, "\n")] = 0;
                
                enfileirar(&pedidos, p);
                printf("=> Pedido #%d adicionado na fila!\n", p.id);
                break;
            }
            case 2:
                listar_fila(&pedidos);
                break;
            case 3:
                if (fila_vazia(&pedidos)) {
                    printf("=> A fila de pedidos esta vazia.\n");
                } else {
                    Pedido concluido = desenfileirar(&pedidos);
                    empilhar(&historico, concluido);
                    printf("=> Pedido #%d (Cliente: %s) CONCLUIDO e entregue!\n", concluido.id, concluido.cliente);
                }
                break;
            case 4:
                if (pilha_vazia(&historico)) {
                    printf("=> Nenhum pedido concluido recentemente para desfazer.\n");
                } else {
                    Pedido desfeito = desempilhar(&historico);
                    enfileirar_inicio(&pedidos, desfeito);
                    printf("=> Conclusao desfeita! Pedido #%d voltou para a FRENTE da fila.\n", desfeito.id);
                }
                break;
            case 5: {
                int id_cancelar;
                printf("Digite o ID do pedido que deseja cancelar: ");
                if (scanf("%d", &id_cancelar) == 1) {
                    if (cancelar_por_id(&pedidos, id_cancelar)) {
                        printf("=> Pedido #%d cancelado com sucesso!\n", id_cancelar);
                    } else {
                        printf("=> Erro: Pedido #%d nao encontrado na fila de pendentes.\n", id_cancelar);
                    }
                } else {
                    printf("=> Entrada invalida.\n");
                }
                limpar_buffer();
                break;
            }
            case 6:
                salvar_dados(&pedidos);
                
                printf("=> Dados salvos. Encerrando o sistema...\n");
                break;
            default:
                printf("=> Opcao invalida. Tente novamente.\n");
        }
    } while (opcao != 6);
    
    liberar_memoria(&pedidos, &historico);
    return 0;
}