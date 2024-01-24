#ifndef REGISTRO_H
#define REGISTRO_H

#include <stdbool.h>

typedef struct {
  unsigned long int numero_inscricao;
  double nota;
  char estado[3];
  char cidade[51];
  char curso[31];
} Registro;

#define N_TOTAL_FITAS 20
#define N_FITAS_ENTRADA 10
#define N_FITAS_SAIDA 10

#define TAM_MEMORIA_PRINCIPAL 10

typedef struct celula {
  unsigned int qtde_itens_bloco;
  struct celula *proximo;
} Celula;

typedef struct lista {
  Celula *cabeca;
  Celula *ultimo;
} Lista;

typedef struct item_heap {
  Registro registro;
  bool marcado;
} ItemHeap;

typedef struct heap {
  ItemHeap itens[TAM_MEMORIA_PRINCIPAL];
  unsigned short n;
  unsigned short marcados; // Responsavel por contar a quantidade de itens
                           // marcados no vetor.
} Heap;

// Aloca uma quantidade "n" de registros.
Registro *alocarRegistros(unsigned int);

// Desaloca uma quantidade "n" de registros.
void desalocarRegistros(Registro *);

#endif // REGISTRO_H
