#include <stdio.h>
#include <stdbool.h>
#include "registro.h"
#include "lista.h"

void heapRefaz(Registro *v, int esq, int dir){
    int i = esq;
    int j = i * 2 + 1;

    Registro aux = v[i];

    while (j <= dir){
        if(j < dir && v[j].nota < v[j+1].nota)
            j++;
        if(aux.nota >= v[j].nota)
            break;
        v[i] = v[j];
        i = j;
        j = i*2 + 1;
    }
    v[i] = aux;
}

void heapConstroi(Registro *v, int n){
    int esq;
    esq = (n/2) - 1;
    while (esq >= 0){
        heapRefaz(v, esq, n-1);
        esq--;
    }
}

void heapSort(Registro *v, int n){
    Registro aux;
    heapConstroi(v, n);
    while (n > 1){
        aux = v[n-1];
        v[n-1] = v[0];
        v[0] = aux;
        n--;
        heapRefaz(v, 0, n-1);
    }
}

void troca(Registro *a, Registro *b){
    Registro temp = *a;
    *a = *b;
    *b = temp;
}

void heapfy(Registro *vetor, short *marcados, int n, int i) {
    int maior = i;
    int esq = 2 * i + 1;
    int dir = 2 * i + 2;

    if(esq < n && (vetor[esq].nota > vetor[maior].nota || (vetor[esq].nota == vetor[maior].nota && marcados[esq])))
        maior = esq;

    if(dir < n && (vetor[dir].nota > vetor[maior].nota || (vetor[dir].nota == vetor[maior].nota && marcados[dir])))
        maior = dir;

    if(maior != i){
        troca(&vetor[i], &vetor[maior]);
        troca(&marcados[i], &marcados[maior]);
        heapfy(vetor, marcados, n, maior);
    }
}

void heapSortM(Registro *vetor, short *marcados, int n) {
    for(int i = n / 2 - 1; i >= 0; i--)
        heapfy(vetor, marcados, n, i);
    
    for(int i = n-1; i > 0; i--){
        troca(&vetor[0], &vetor[i]);
        troca(&marcados[0], &marcados[i]);
        heapfy(vetor, marcados, i, 0);
    }
}