#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "registro.h"
#include "lista.h"
#include "heapSort.h"

// Gera arquivo temporario das n primeiras linhas do arquivo principal
void geraArquivo(FILE *in, FILE *temp, int n) {

    Registro auxiliar;
    for(int i = 0; i < n; i++){
        fscanf(in, "%d %f", &auxiliar.inscricao, &auxiliar.nota);
        fgets(auxiliar.estado, 3, in);
        fseek(in, 1, 1);
        fgets(auxiliar.cidade, 50, in);
        fseek(in, 2, 1);
        fscanf(in, "%[A-Z] %[\n]", &auxiliar.curso);
        fwrite(&auxiliar, sizeof(Registro), 1, temp);
    }
}

// Checa se todos os elementos estao marcados
short temElementoNaMemoria(short marcados[], int n) {
    for(int i = 0; i < n; i++)
        if(marcados[i] != -1)
            return 1;
    
    return 0;
}

// Checa se todos os elementos do vetor foram marcados
short todosMarcados(short marcados[], int n){
    for(int i = 0; i < n; i++)
        if(!marcados[i])
            return 0;
    return 1;
}

void gerarblocos(int n, FILE *temp, FILE **fitas, int elementos[DOBRO], lista blocos[DOBRO]){

    Registro memoria[FITAS];
    short marcados[FITAS];
    for(int i = 0; i < DOBRO; i++){
        elementos[i] = 0;
        limparList(&blocos[i]);
    }
    rewind(temp);

    int fita = 0;
    Registro proximo;

    // Pega os primeiros elementos de acordo com a quantidade de fitas
    fread(memoria, sizeof(Registro), FITAS, temp);
    for(int i = 0; i < FITAS; i++){
        marcados[i] = 0;
        // Colocou os primeiros elementos no arquivo temporario, como sao primeiros nenhum esta marcado
    }

    // Enquando nao tiver lido todo o arquivo ou enquanto tiver elementos nao marcados na memoria
    while (ftell(temp)/sizeof(Registro) < (unsigned) n || temElementoNaMemoria(marcados, FITAS)){
        for(int i = 0; i < FITAS; i++){
            if(marcados[i] == 1) 
                marcados[i] = 0; // desmarca
        }

        listaAdd(&blocos[fita], elementos[fita]);
        // Enquanto algum elemento da memoria nao estiver marcado
        while(!todosMarcados(marcados, FITAS)){
            heapSortM(memoria, marcados, FITAS);
            // Escreve o menor elemento na fita alvo
            fwrite(&memoria[0], sizeof(Registro), 1, fitas[fita]);
            // Contabiliza os elementos em cada fita
            elementos[fita]++;
            
            // se o arquivo nao acabou
            if(ftell(temp)/sizeof(Registro) < (unsigned) n) {
                fread(&proximo, sizeof(Registro), 1, temp); // Le proximo elemento do arquivo
                if(proximo.nota > memoria[0].nota) // Se o proximo elemento for maior que o primeiro, ele é marcado
                    marcados[0] = 1;
                memoria[0] = proximo; // Coloca o proximo como primeiro elemento da memoria
            } else {
                marcados[0] = -1;  // Nao tem elemento na memoria
            }
        }
        fita = (fita + 1)%(DOBRO/2);
    }
}

int main() {
    FILE *input;
    //FILE *out;
    FILE *temp; // arquivo binario temporario
    //Registro vetor[MAX_RECORDS];
    int quantidade;

    input = ("PROVAO.TXT", "r+");
    scanf("%d", &quantidade);

    geraArquivo(input, temp, quantidade);
    rewind(temp);

    // Criacao de arquivos para fitas temporarias
    FILE **fitas = (FILE**) malloc(sizeof(FILE*)*DOBRO); // Vetor de arquivos de fitas
    for(int i = 0; i < DOBRO; i++){
        char nome[11]; 
        sprintf(nome, "fita%.2i.bin", i);   // Separar as fitas em arquivos
        fitas[i] = fopen(nome, "w+b");
    }

    int elementos[DOBRO];
    for(int i = 0; i < DOBRO; i++)
        elementos[i] = 0;
    lista blocos[DOBRO];
    for(int i = 0; i < DOBRO; i++)
        blocos[i].primeiro = NULL;

    gerarblocos(quantidade, temp, fitas, elementos, blocos);


}