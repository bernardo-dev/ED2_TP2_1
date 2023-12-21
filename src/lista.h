#ifndef LISTA_H
#define LISTA_H

typedef struct item* celula;

typedef struct item {
    int valor;
    celula ant, prox;
} item;

typedef struct lista {
    celula primeiro;
} lista;

void listaAdd(lista *, int);
int removePrimeiro(lista *);
int removeElemento(lista *, int);
int listaTam(lista *);
int listaPosicao(lista *, int);
void listaImprime(lista *);
void limparList(lista *);

#endif