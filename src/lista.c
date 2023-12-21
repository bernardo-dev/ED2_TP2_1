#include <stdio.h>
#include "registro.h"
#include "lista.h"

void listaAdd(lista *list, int valor){
    
    // Cria um novo no
    celula aux = (celula)malloc(sizeof(item));
    aux->valor = valor;
    aux->ant = NULL;
    aux->prox = NULL;

    // Se a primeira posicao estiver vazia, adiciona na primeira
    if(list->primeiro == NULL) 
        list->primeiro = aux;
    else {
        // Se nao, percorre a lista ate a ultima posicao
        celula aux2 = list->primeiro;
        while(aux2->prox != NULL)
            aux2 = aux2->prox;
        
        aux2->prox = aux;
        aux->ant = aux2; 
    }
}

int removePrimeiro(lista *list){

    // Nao existe nenhum elemento na lista
    if(list->primeiro == NULL)
        return -1;

    // Remove o primeiro elemento e atualiza os ponteiros
    celula aux = list->primeiro;
    int x = aux->valor;
    list->primeiro = aux->prox;

    // Retorna o elemento removido
    aux = NULL;
    return x;
}

int removeElemento(lista *list, int valor){

    if(list->primeiro == NULL)
        return -1;

    // Percorre a lista ate achar valor procurado ou perceber que valor nao existe na lista
    celula aux = list->primeiro;
    while(aux != NULL && aux->valor != valor)
        aux = aux->prox;
    if(aux == NULL)
        return -1;
    
    // Atualiza ponteiros caso esteja na primeira posicao
    if(aux == list->primeiro)
        list->primeiro = list->primeiro->prox;

    if(aux->ant != NULL)
        aux->ant->prox = aux->prox;
    if(aux->prox != NULL)
        aux->prox->ant = aux->ant;

    // Retorna valor removido
    int x = aux->valor;
    free(aux);
    return x;
}

int listaTam(lista *list){

    if(list->primeiro == NULL)
        return 0;
    
    int n;
    celula aux = list->primeiro;

    for(n = 0; aux != NULL; n++)
        aux = aux->prox;

    // Retorna tamanho da lista
    return n;
}

int listaPosicao(lista *list, int indice){

    if(list->primeiro == NULL || indice >= listaTam(list) || indice < 0)
        return -1;
    
    // Percorre lista ate posicao desejada
    celula aux = list->primeiro;
    for(int i = 0; i < indice; i++)
        aux = aux->prox;

    return aux->valor;
}

void listaImprime(lista *list){
    printf("[");
    if(list->primeiro != NULL){
        for(celula i = list->primeiro; i != NULL; i = i->prox){
            printf("%i ", i->valor);
        }
    }
    printf("]");
}

void limparList(lista *list){
    if(list->primeiro == NULL)
        return;
    
    while(list->primeiro != NULL)
        removePrimeiro(list);
}
