#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
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

void heapRefazM(Registro *v, short *marcados, int esq, int dir){
    int i = esq;
    int j = i * 2 + 1;

    Registro aux = v[i];
    short auxM = marcados[i];

    while(j <= dir){
        if((j < dir && v[j].nota < v[j+1].nota) || marcados[j+1] == 1) 
            j++;
        if(aux.nota >= v[j].nota || auxM == 1)
            break;
        v[i] = v[j];
        marcados[i] = marcados[j];
        i = j;
        j = i*2+1;
    }
    v[i] = aux;
    marcados[i] = auxM;
}

void heapConstroiM(Registro *v, short *marcados, int n){
    int esq;
    esq = (n/2) - 1;
    while(esq >= 0){
        heapRefazM(v, marcados, esq, n-1);
        esq--;
    }
}

void heapSortM(Registro *vetor, short *marcados, int n) {
    Registro aux;
    short auxM;
    heapConstroiM(vetor, marcados, n);
    while(n > 1){
        aux = vetor[n-1];
        auxM = marcados[n-1];
        vetor[n-1] = vetor[0];
        marcados[n-1] = marcados[0];
        vetor[0] = aux;
        marcados[0] = auxM;
        n--;
        heapRefazM(vetor, marcados, 0, n-1);
    }
}