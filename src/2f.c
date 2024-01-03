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
        fscanf(in, "%ld %f", &auxiliar.inscricao, &auxiliar.nota);
        fgets(auxiliar.estado, 3, in);
        fseek(in, 1, 1);
        fgets(auxiliar.cidade, 50, in);
        fseek(in, 2, 1);
        fscanf(in, "%[A-Z] \n", auxiliar.curso);
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

int umaFitaPreenchida(int elementos[], int n, int inicio){
    int preenchida = -1;
    for(int i = 0; i < n; i++){
        if(elementos[inicio+i] > 0) {
            if(preenchida == -1)
                preenchida = i;
                else return -1;
        }
    }
    return (preenchida == -1) ? preenchida : inicio + preenchida;
}

short todasFitasPercorridas(FILE **fitas, int elementos[], int n, int inicio){
    for(int i = 0; i < n; i++){
        if(ftell(fitas[inicio+i])/sizeof(Registro) < (unsigned) elementos[inicio+i])
            return 0;
    }
    return 1;
}

short blocosEsgotados(short ativas[], int n){
    for(int i = 0; i < n; i++){
        if(ativas[i] != -1)
            return 0;
    }
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

void intercalablocos (int n, FILE *temp, FILE **fitas, int elementos[DOBRO], lista blocos[DOBRO]){

    Registro memoria[FITAS];
    int from[FITAS];
    short ativas[FITAS];
    short saltofitas = 1;
    int fita;
    for(int i = 0; i < DOBRO; i++){
        rewind(fitas[i]);
    }

    // f = -1 se nao tiver nenhuma fita preenchida
    int f = umaFitaPreenchida(elementos, FITAS, FITAS*saltofitas);
    while(f == -1) {
        fita = FITAS*saltofitas;
        int bloco = 0;
        while (!todasFitasPercorridas(fitas, elementos, FITAS, FITAS*(!saltofitas))){ // Nao percorreu todas as fitas

            listaAdd(&blocos[fita], ftell(fitas[fita])/sizeof(Registro));
            int limiteblocos[FITAS]; // termina o bloco de cada fita
            for(int i = 0; i < FITAS; i++){
                if(listaTam(&blocos[(FITAS*!saltofitas)+i])>bloco+1)
                    limiteblocos[i] = listaPosicao(&blocos[(FITAS*!saltofitas)+i], bloco+1);
                else
                    limiteblocos[i] = elementos[(FITAS*!saltofitas)+i];
            }

            // Primeiro elemento de cada fita de entrada
            for(int i = 0; i < FITAS; i++){
                if(ftell(fitas[(FITAS*!saltofitas)+i])/sizeof(Registro) < (unsigned) limiteblocos[i]) {  // Tem elemento no bloco?
                    fread(&memoria[i], sizeof(Registro), 1, fitas[(FITAS*!saltofitas)+i]); // Pega primeiro elemento
                    if(ftell(fitas[(FITAS*!saltofitas)+i])/sizeof(Registro) >= (unsigned) limiteblocos[i]) // Foi o ultimo elemento?
                        ativas[i] = 0;
                    else 
                        ativas[i] = 1;
                }
                else
                    ativas[i] = -1;
                from[i] = i+(!saltofitas*FITAS);
            }

            while(!blocosEsgotados(ativas, FITAS)){
                // heapSortM(memoria, ativas, from, FITAS, 0)
                fwrite(&memoria[0], sizeof(Registro), 1, fitas[fita]); // Menos elemento na fita alvo
                elementos[fita]++; // aumenta o tamanho da fita
                if(ativas[from[0]%FITAS]){ // se a fita do menor esta ativa
                    fread(&memoria[0], sizeof(Registro), 1, fitas[from[0]]); // pega o primeiro elemento
                    if(ftell(fitas[from[0]])/sizeof(Registro) == (unsigned) limiteblocos[from[0]%FITAS])
                        ativas[from[0]%FITAS] = 0; //fita inativa
                } else {
                    ativas[from[0]%FITAS] = -1; 
                }
            }
            fita = (fita%FITAS+1)%FITAS + FITAS*saltofitas; // proxima fita alvo
            bloco++;
        } 

        for(int i = 0; i < FITAS; i++){
            rewind(fitas[FITAS*saltofitas + i]);
            rewind(fitas[FITAS*(!saltofitas) + i]);
            elementos[FITAS * (!saltofitas) + i] = 0;
            limparList(&blocos[FITAS*(!saltofitas) + i]);
        }

        saltofitas = !saltofitas; // troca as fitas de entrada com as fitas de saida
        f = umaFitaPreenchida(elementos, FITAS, FITAS*(!saltofitas)); // resta apenas uma fita com elemento?
    }

    rewind(fitas[f]);
    rewind(temp);
    Registro r;
    for(int i = 0; i < n; i++){
        // joga elementos da fita f na fita principal
        fread(&r, sizeof(Registro), 1, fitas[f]);
        fwrite(&r, sizeof(Registro), 1, temp);
    }
}

int main() {
    FILE *input;
    //FILE *out;
    FILE *temp; // arquivo binario temporario
    //Registro vetor[MAX_RECORDS];
    int quantidade;

    input = fopen("PROVAO.TXT", "r+");
    temp = fopen("Temp.bin", "w+b");
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
    intercalablocos(quantidade, temp, fitas, elementos, blocos);

    for(int i = 0; i < DOBRO; i++)
        fclose(fitas[i]);
    fclose(input);
    fclose(temp);

}