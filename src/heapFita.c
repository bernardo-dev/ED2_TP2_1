#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "include/registro.h"
#include "include/lista.h"
#include "include/util.h"

void iniciaHeap(Heap *heap)
{
    heap->n = heap->marcados = 0;
}

void heapRefaz(Heap *heap, short esq, short dir, Metrica *metrica)
{
    short i;
    short j;
    ItemHeap auxiliar;

    i = esq;
    j = i * 2 + 1;

    auxiliar = heap->itens[i];

    while(j <= dir)
    {
        metrica->n_comparacao++;
        if(j < dir)
        {
            metrica->n_comparacao++;
            if(!(heap->itens[j].marcado == false && heap->itens[j + 1].marcado == true))
            {
                /*
                    Os fluxos condicionais seguintes servem para indicar os casos em que o
                    filho mais a direita eh menor que o filho mais a esquerda
                */
               metrica->n_comparacao++;
                if((heap->itens[j].marcado == true && heap->itens[j + 1].marcado == false) ||
                    heap->itens[j].registro.nota > heap->itens[j + 1].registro.nota)
                    j++;
            }
        }

        metrica->n_comparacao++;

       if(!(auxiliar.marcado == true && heap->itens[j].marcado == false))
       {
            /* 
                Se o registro pai nao estiver marcado e seu menor filho estiver
                OU se a nota do registro pai ja for menor que a do seu menor filho,
                quebra o looping.
            */
           metrica->n_comparacao++;
            if((auxiliar.marcado == false && heap->itens[j].marcado == true) ||
            (auxiliar.registro.nota <= heap->itens[j].registro.nota))
                break;
       }

        heap->itens[i] = heap->itens[j];
        i = j;
        j = i * 2 + 1;
    }

    heap->itens[i] = auxiliar;
}

void heapConstroi(Heap *heap, Metrica *metrica)
{
    short esq;
    
    esq = (heap->n / 2) - 1;

    while(esq >= 0)
    {
        heapRefaz(heap, esq, heap->n - 1, metrica);
        esq--;
    }
}

// O item retirado sempre sera o no raiz do Heap.
ItemHeap heapRetira(Heap *heap)
{
    ItemHeap auxiliar;

    auxiliar = heap->itens[0];
    heap->itens[0] = heap->itens[heap->n - 1];
    heap->n--;

    return auxiliar;
}

/*
    O item inserido sempre sera no no raiz do Heap.
    Isso permite que a reconstrucao do Heap aconteca
    de maneira adequada.
*/
void heapInsere(Heap *heap, ItemHeap *item)
{
    heap->itens[0] = *item;
    heap->n++;
}

void desmarcarItensHeap(Heap *heap)
{
    for(unsigned short i = 0 ; i < heap->n ; i++)
        heap->itens[i].marcado = false;

    heap->marcados = 0;
}

void removerFitasEntrada(FILE *fitas_entrada[N_FITAS_ENTRADA])
{
    char nome_arquivo[51];

    for(unsigned short i = 1 ; i <= N_FITAS_ENTRADA ; i++)
    {
        // Gera o nome do arquivo de entrada, com um indice igual a i.
        sprintf(nome_arquivo, "e_fita_%hu.bin", i);

        // Fecha o arquivo antes de remove-lo.
        fclose(fitas_entrada[i - 1]);

        remove(nome_arquivo);
    }

    remove("s_fita.bin");
}

void removerFitasSaida(FILE *fitas_saida[N_FITAS_SAIDA]){

    char nome_arquivo[51];

    for(unsigned short i = 2; i <= N_FITAS_SAIDA; i++)
    {
        sprintf(nome_arquivo, "e_fita_saida%hu.bin", i);

        fclose(fitas_saida[i - 1]);

        remove(nome_arquivo);
    }

    remove("s_fita_saida.bin");
}

void abrirFitas(FILE *fitas_entrada[N_FITAS_ENTRADA], FILE *fitas_saida[N_FITAS_SAIDA], Metrica *metrica)
{
    char nome_arquivo[51];

    for(unsigned short i = 1 ; i <= N_FITAS_ENTRADA ; i++)
    {
        // Gera o nome do arquivo de entrada, com um indice igual a i.
        sprintf(nome_arquivo, "e_fita_%hu.bin", i);

        fitas_entrada[i - 1] = fopen(nome_arquivo, "w+b");
        metrica->n_escrita++;

        if(fitas_entrada[i - 1] == NULL){
            metrica->n_comparacao++;
            exit(1);
        }

        metrica->n_leitura++;
    }

    // --- Mudanca ---
    // Como o numero de fitas de saida aqui é maior que 1, repete o mesmo processo da fita de entrada
    // ----- Codigo f1 -------
    //*fita_saida = fopen("s_fita.bin", "w+b");

    //if(*fita_saida == NULL)
        //exit(1);
    // ---------------------------

    for(unsigned short i = 1; i <= N_FITAS_SAIDA; i++)
    {
        sprintf(nome_arquivo, "e_fita_saida%hu.bin", i);

        fitas_saida[i-1] = fopen(nome_arquivo, "w+b");
        metrica->n_escrita++;

        if(fitas_saida[i-1] == NULL){
            metrica->n_comparacao++;
            exit(1);
        }

        metrica->n_leitura++;
    }
}

void converterFitaParaTexto(FILE *fita_saida, Lista *blocos_fita_saida)
{
    Registro buffer;
    unsigned int c_itens;
    unsigned int c_blocos;

    rewind(fita_saida);

    c_blocos = 1;
    // Varre a lista
    while(!listaEstaVazia(blocos_fita_saida))
    {
        c_itens = 0;

        // Varre o bloco
        while(c_itens < blocos_fita_saida->cabeca->proximo->qtde_itens_bloco)
        {
            fread(&buffer, sizeof(Registro), 1, fita_saida);
            c_itens++;
        }

        c_blocos++;
        removeListaInicio(blocos_fita_saida);
    }
}

void realocarPonteirosFitas(FILE *fitas_entrada[N_FITAS_ENTRADA])
{
    for(unsigned short i = 0 ; i < N_FITAS_ENTRADA ; i++)
        rewind(fitas_entrada[i]);
}
