#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_RECORDS 10

typedef struct {
    long int inscricao;
    float nota;
    char estado[3];
    char cidade[51];
    char curso[31];
} Registro;

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

void sub_selecao(FILE *in, FILE *out){
    Registro vetor[MAX_RECORDS];
    Registro auxiliar;

    while(fread(&auxiliar, sizeof(Registro), 1, in) == 1) {  // Le cada registro do arquivos

    }
}

int main() {
    FILE *in;
    FILE *out;

    in = ("PROVAO.TXT", "r+");
    out = ("output.txt", "w+");


}