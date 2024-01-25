#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "umaFita.h"

#define F1_N_TOTAL_FITAS 20
#define F1_N_FITAS_ENTRADA 19
#define F1_N_FITAS_SAIDA 1

#define F1_TAM_MEMORIA_PRINCIPAL 19

typedef struct celula_f1 {
    unsigned int qtde_itens_bloco;
    struct celula_f1* proximo;
} CelulaF1;

typedef struct lista_f1 {
    CelulaF1 *cabeca;
    CelulaF1 *ultimo;
} ListaF1;

typedef struct item_heap_f1 {
    Registro registro;
    bool marcado;
} ItemHeapF1;

typedef struct heap_f1 {
    ItemHeapF1 itens[F1_TAM_MEMORIA_PRINCIPAL];
    unsigned short n;
    unsigned short marcados;    // Responsavel por contar a quantidade de itens marcados no vetor.
} HeapF1;

CelulaF1* alocarCelulaF1()
{
    CelulaF1 *celula = (CelulaF1*) malloc(sizeof(CelulaF1));

    if(celula != NULL)
    {
        celula->qtde_itens_bloco = 0;
        celula->proximo = NULL;
    }

    return celula;
}

void iniciaListasF1(ListaF1 blocos_fita_entrada[F1_N_FITAS_ENTRADA], ListaF1 *blocos_fita_saida)
{
    blocos_fita_saida->cabeca = alocarCelulaF1();

    if(blocos_fita_saida->cabeca == NULL)
        exit(1);

    blocos_fita_saida->ultimo = NULL;

    for(unsigned short i = 0 ; i < F1_N_FITAS_ENTRADA ; i++)
    {
        blocos_fita_entrada[i].cabeca = alocarCelulaF1();

        // Se nao foi possivel alocar memoria, o programa termina
        if(blocos_fita_entrada[i].cabeca == NULL)
            exit(1);
            
        blocos_fita_entrada[i].ultimo = NULL;
    }
}

bool listaEstaVaziaF1(ListaF1 *lista)
{
    return lista->cabeca->proximo == NULL;
}

void insereListaF1(ListaF1 *lista, unsigned int qtde_itens_bloco)
{
    if(listaEstaVaziaF1(lista))
    {
        lista->ultimo = alocarCelulaF1();

        // Se nao foi possivel alocar memoria, o programa termina
        if(lista->ultimo == NULL)
            exit(1);

        lista->cabeca->proximo = lista->ultimo;
        lista->ultimo->qtde_itens_bloco = qtde_itens_bloco;
    }
    else
    {
        lista->ultimo->proximo = alocarCelulaF1();

        // Se nao foi possivel alocar memoria, o programa termina
        if(lista->ultimo->proximo == NULL)
            exit(1);

        lista->ultimo = lista->ultimo->proximo;
        lista->ultimo->qtde_itens_bloco = qtde_itens_bloco;
    }
}

void removeListaF1Inicio(ListaF1 *lista)
{
    CelulaF1 *auxiliar;

    auxiliar = lista->cabeca->proximo;
    lista->cabeca->proximo = auxiliar->proximo;

    free(auxiliar);
}

bool fitaPossuiSomenteUmBlocoF1(ListaF1 *blocos_por_fita, unsigned int qtde_total_registros)
{
    if(blocos_por_fita->cabeca->proximo == NULL)
        return false;

    return blocos_por_fita->cabeca->proximo->qtde_itens_bloco == qtde_total_registros;
}

bool todasAsFitasDeEntradaEstaoVaziasF1(ListaF1 blocos_fitas_entrada[F1_N_FITAS_ENTRADA])
{
    for(unsigned short i = 0 ; i < F1_N_FITAS_ENTRADA ; i++)
        if(!listaEstaVaziaF1(&blocos_fitas_entrada[i]))
            return false;

    return true;
}

void iniciaHeapF1(HeapF1 *heap)
{
    heap->n = heap->marcados = 0;
}

void removerFitasEntradaF1(FILE *fitas_entrada[F1_N_FITAS_ENTRADA])
{
    char nome_arquivo[51];

    for(unsigned short i = 1 ; i <= F1_N_FITAS_ENTRADA ; i++)
    {
        // Gera o nome do arquivo de entrada, com um indice igual a i.
        sprintf(nome_arquivo, "e_fita_%hu.bin", i);

        // Fecha o arquivo antes de remove-lo.
        fclose(fitas_entrada[i - 1]);

        remove(nome_arquivo);
    }

    remove("s_fita.bin");
}

void abrirFitasF1(FILE *fitas_entrada[F1_N_FITAS_ENTRADA], FILE **fita_saida)
{
    char nome_arquivo[51];

    for(unsigned short i = 1 ; i <= F1_N_FITAS_ENTRADA ; i++)
    {
        // Gera o nome do arquivo de entrada, com um indice igual a i.
        sprintf(nome_arquivo, "e_fita_%hu.bin", i);

        fitas_entrada[i - 1] = fopen(nome_arquivo, "w+b");

        if(fitas_entrada[i - 1] == NULL)
            exit(1);
    }

    *fita_saida = fopen("s_fita.bin", "w+b");

    if(*fita_saida == NULL)
        exit(1);
}

void heapRefazF1(HeapF1 *heap, short esq, short dir, Metrica *metricas)
{
    short i;
    short j;
    ItemHeapF1 auxiliar;

    i = esq;
    j = i * 2 + 1;

    auxiliar = heap->itens[i];

    while(j <= dir)
    {
        if(j < dir)
        {
            metricas->n_comparacao++;
            if(!(heap->itens[j].marcado == false && heap->itens[j + 1].marcado == true))
            {
                /*
                    Os fluxos condicionais seguintes servem para indicar os casos em que o
                    filho mais a direita eh menor que o filho mais a esquerda
                */
                if((heap->itens[j].marcado == true && heap->itens[j + 1].marcado == false) ||
                    heap->itens[j].registro.nota > heap->itens[j + 1].registro.nota)
                    j++;
            }
        }

       metricas->n_comparacao++;
       if(!(auxiliar.marcado == true && heap->itens[j].marcado == false))
       {
            /* 
                Se o registro pai nao estiver marcado e seu menor filho estiver
                OU se a nota do registro pai ja for menor que a do seu menor filho,
                quebra o looping.
            */
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

void heapConstroiF1(HeapF1 *heap, Metrica *metricas)
{
    short esq;
    
    esq = (heap->n / 2) - 1;

    while(esq >= 0)
    {
        heapRefazF1(heap, esq, heap->n - 1, metricas);
        esq--;
    }
}

// O item retirado sempre sera o no raiz do HeapF1.
ItemHeapF1 heapRetiraF1(HeapF1 *heap)
{
    ItemHeapF1 auxiliar;

    auxiliar = heap->itens[0];
    heap->itens[0] = heap->itens[heap->n - 1];
    heap->n--;

    return auxiliar;
}

/*
    O item inserido sempre sera no no raiz do HeapF1.
    Isso permite que a reconstrucao do HeapF1 aconteca
    de maneira adequada.
*/
void heapInsereF1(HeapF1 *heap, ItemHeapF1 *item)
{
    heap->itens[0] = *item;
    heap->n++;
}

void desmarcarItensHeapF1(HeapF1 *heap)
{
    for(unsigned short i = 0 ; i < heap->n ; i++)
        heap->itens[i].marcado = false;

    heap->marcados = 0;
}

// Funcao responsavel por gerar os blocos pre-ordenados.
void geracaoBlocosOrdenadosF1(FILE *arquivo_binario, unsigned int qtde_registros, FILE *fitas_entrada[F1_N_FITAS_ENTRADA], ListaF1 blocos_por_fita[F1_N_FITAS_ENTRADA], Metrica *metricas)
{
    unsigned int indice_fita;   // Indice que marca em qual das fitas esta se inserindo blocos
    HeapF1 heap;

    indice_fita = 0;
    iniciaHeapF1(&heap);

    // Se a quantidade de registros no arquivo couber em memoria principal.
    if(qtde_registros <= F1_TAM_MEMORIA_PRINCIPAL)
    {
        // Le todos itens do arquivo e os coloca em memoria principal.
        for(unsigned short i = 0 ; i < qtde_registros ; i++)
        {
            // Se a leitura foi mal-sucedida, termina o programa
            if(fread(&heap.itens[i].registro, sizeof(Registro), 1, arquivo_binario) == 0)
                exit(1);

            metricas->n_leitura++;
            heap.n++;
        }

        ItemHeapF1 auxiliar;

        // Insertion Sort
        for(unsigned short i = 1 ; i < heap.n ; i++)
        {
            for(unsigned short j = i ; j > 0 ; j--)
            {
                metricas->n_comparacao++;
                if(heap.itens[j].registro.nota < heap.itens[j - 1].registro.nota)
                {
                    auxiliar = heap.itens[j];
                    heap.itens[j] = heap.itens[j - 1];
                    heap.itens[j - 1] = auxiliar;
                }
            }
        }

        // Insere registros ordenados na primeira fita
        for(unsigned short i = 0 ; i < heap.n ; i++) {
            fwrite(&heap.itens[i].registro, sizeof(Registro), 1, fitas_entrada[indice_fita]);
            metricas->n_escrita++;
        }

        insereListaF1(&blocos_por_fita[indice_fita], qtde_registros);
    }
    else
    {
        // Inicialmente, insere todos os primeiros F1_TAM_MEMORIA_PRINCIPAL registros no vetor do heap.
        for(unsigned short i = 0 ; i < F1_TAM_MEMORIA_PRINCIPAL ; i++)
        {
            // Se a leitura foi mal-sucedida, termina o programa
            if(fread(&heap.itens[i].registro, sizeof(Registro), 1, arquivo_binario) == 0)
                exit(1);

            metricas->n_leitura++;

            heap.itens[i].marcado = false;

            heap.n++;
        }
        
        // Parte do codigo que realiza a pre-ordenacao dos blocos propriamente dita
        {
            unsigned int contador;          // Conta a quantidade de registros lidos do arquivo.
            unsigned int itens_por_bloco;   // Conta a quantidade de itens lidos para aquele bloco especifico.
            ItemHeapF1 ultimo_lido;
            ItemHeapF1 ultimo_retirado;

            contador = F1_TAM_MEMORIA_PRINCIPAL + 1;
            itens_por_bloco = 0;

            heapConstroiF1(&heap, metricas);

            // Le o restante dos registros do arquivo
            while(contador <= qtde_registros)
            {
                /*
                    Se a quantidade de marcados for igual a de todos os elementos presentes no heap,
                    desmarca todos e inicia um novo bloco.
                */
                if(heap.marcados == heap.n)
                {
                    // Registra a quantidade de itens que aquele bloco na fita indice_fita possui.
                    insereListaF1(&blocos_por_fita[indice_fita], itens_por_bloco);

                    desmarcarItensHeapF1(&heap);
                    // Reinicia a contagem de itens por bloco
                    itens_por_bloco = 0;
                    // Passa a escrever na proxima fita
                    indice_fita++;

                    if(indice_fita == F1_N_FITAS_ENTRADA)
                        indice_fita = 0;
                }
                
                // Escreve o menor elemento do heap na fita indice_fita.
                fwrite(&heap.itens[0].registro, sizeof(Registro), 1, fitas_entrada[indice_fita]);
                itens_por_bloco++;
                metricas->n_escrita++;

                // Retira o menor item do heap e o retorna para a variavel "ultimo_retirado"
                ultimo_retirado = heapRetiraF1(&heap);

                fread(&ultimo_lido.registro, sizeof(Registro), 1, arquivo_binario);
                metricas->n_leitura++;

                // Se o ultimo registro lido do arquivo for menor do que o item retirado do heap.
                metricas->n_comparacao++;
                if(ultimo_lido.registro.nota < ultimo_retirado.registro.nota)
                {
                    ultimo_lido.marcado = true;
                    heap.marcados++;
                }
                else
                    ultimo_lido.marcado = false;

                // Insere o ultimo item lido do arquivo no heap.
                heapInsereF1(&heap, &ultimo_lido);

                heapRefazF1(&heap, 0, heap.n - 1, metricas);

                contador++;
            }

            // Apos lidos todos os itens, esvazia o heap, inserindo os seus elementos na fita necessaria.
            while(heap.n > 0)
            {
                if(heap.marcados == heap.n)
                {
                    // Registra a quantidade de itens que aquele bloco na fita indice_fita possui.
                    insereListaF1(&blocos_por_fita[indice_fita], itens_por_bloco);

                    desmarcarItensHeapF1(&heap);
                    // Reinicia a contagem de itens por bloco
                    itens_por_bloco = 0;
                    // Passa a escrever na proxima fita
                    indice_fita++;

                    if(indice_fita == F1_N_FITAS_ENTRADA)
                        indice_fita = 0;
                }

                ultimo_retirado = heapRetiraF1(&heap);

                fwrite(&ultimo_retirado.registro, sizeof(Registro), 1, fitas_entrada[indice_fita]);
                metricas->n_escrita++;

                itens_por_bloco++;
                
                heapRefazF1(&heap, 0, heap.n - 1, metricas);
            }

            // Registra a quantidade de itens que aquele bloco na fita indice_fita possui.
            insereListaF1(&blocos_por_fita[indice_fita], itens_por_bloco);
        }
    }
}

void realocarPonteirosFitasF1(FILE *fitas_entrada[F1_N_FITAS_ENTRADA])
{
    for(unsigned short i = 0 ; i < F1_N_FITAS_ENTRADA ; i++)
        rewind(fitas_entrada[i]);
}

void redistribuirBlocos(FILE *fitas_entrada[F1_N_FITAS_ENTRADA], FILE *fita_saida, ListaF1 blocos_fitas_entrada[F1_N_FITAS_ENTRADA], ListaF1 *blocos_fita_saida, Metrica *metricas)
{
    unsigned int indice_fita;      // Indice que marca em qual das fitas esta se inserindo blocos
    unsigned int itens_por_bloco;
    Registro buffer;

    itens_por_bloco = 0;
    indice_fita = 0;

    realocarPonteirosFitasF1(fitas_entrada);
    rewind(fita_saida);

    while(!listaEstaVaziaF1(blocos_fita_saida))
    {
        while(itens_por_bloco < blocos_fita_saida->cabeca->proximo->qtde_itens_bloco)
        {
            fread(&buffer, sizeof(Registro), 1, fita_saida);
            fwrite(&buffer, sizeof(Registro), 1, fitas_entrada[indice_fita]);
            itens_por_bloco++;
            
            metricas->n_leitura++;
            metricas->n_escrita++;
        }

        removeListaF1Inicio(blocos_fita_saida);

        insereListaF1(&blocos_fitas_entrada[indice_fita], itens_por_bloco);
        itens_por_bloco = 0;
        indice_fita++;

        if(indice_fita == F1_N_FITAS_ENTRADA)
            indice_fita = 0;
    }
}

void intercalacaoBalanceada(FILE *fitas_entrada[F1_N_FITAS_ENTRADA], FILE *fita_saida, ListaF1 blocos_por_fita[F1_N_FITAS_ENTRADA], ListaF1 *blocos_fita_saida, Metrica *metricas)
{
    unsigned short marcados;                     // Calcula a quantidade de fitas com blocos disponiveis restantes.
    unsigned short k;                            // Guarda o indice da fita que possui o elemento com o menor valor em um dado momento.
    unsigned int itens_por_bloco;                // Calcula a quantidade de itens por bloco na fita de saida.
    ItemHeapF1 memoria[F1_TAM_MEMORIA_PRINCIPAL];     // Guarda os registros das fitas. Cada posicao eh um indice de uma fita.
    
    marcados = 0;

    // Preenche cada posicao do vetor com o primeiro elemento de cada fita que possui pelo menos um bloco disponivel.
    for(unsigned short i = 0 ; i < F1_TAM_MEMORIA_PRINCIPAL ; i++)
    {
        if(listaEstaVaziaF1(&blocos_por_fita[i]))
        {
            memoria[i].marcado = true;
            marcados++;
        }
        else
        {
            fread(&memoria[i].registro, sizeof(Registro), 1, fitas_entrada[i]);
            memoria[i].marcado = false;
            metricas->n_leitura++;

            // Remove a quantidade de itens no bloco, ja que acabou de se ler um.
            blocos_por_fita[i].cabeca->proximo->qtde_itens_bloco--;
        }
    }

    itens_por_bloco = 0;

    while(marcados != F1_TAM_MEMORIA_PRINCIPAL)
    {
        k = 0;

        // Localiza a primeira fita com um bloco disponivel
        while(memoria[k].marcado == true)
            k++;

        // Localiza a fita com o menor valor num dado momento
        for(unsigned short i = k + 1 ; i < F1_TAM_MEMORIA_PRINCIPAL ; i++)
        {
            if(memoria[i].marcado == false && memoria[i].registro.nota < memoria[k].registro.nota)
                k = i;

            if(memoria[i].marcado == false)
                metricas->n_comparacao++;
        }

        // Escreve o menor elemento na fita de saida.
        fwrite(&memoria[k].registro, sizeof(Registro), 1, fita_saida);
        itens_por_bloco++;
        metricas->n_escrita++;

        // Le o proximo item da fita de entrada caso ela tenha itens restantes em seu bloco.
        if(blocos_por_fita[k].cabeca->proximo->qtde_itens_bloco > 0)
        {
            fread(&memoria[k].registro, sizeof(Registro), 1, fitas_entrada[k]);
            blocos_por_fita[k].cabeca->proximo->qtde_itens_bloco--;
            metricas->n_leitura++;
        }
        // Caso contrario, significa que o elemento que acabou de ser escrito na fita de saida foi o ultimo do bloco.
        else
        {
            memoria[k].marcado = true;
            removeListaF1Inicio(&blocos_por_fita[k]);
            marcados++;
        }
    }

    if(itens_por_bloco != 0)
        insereListaF1(blocos_fita_saida, itens_por_bloco);
}

FILE* f1fitas(FILE *arquivo_binario, unsigned int qtde_registros, Metrica *metricas)
{
    FILE *fitas_entrada[F1_N_FITAS_ENTRADA];
    FILE *fita_saida;
    ListaF1 blocos_fitas_entrada[F1_N_FITAS_ENTRADA];    // Cada posicao do vetor eh uma lista encadeada de blocos presentes em cada fita.
    ListaF1 blocos_fita_saida;                        // ListaF1 encadeada de blocos que a fita de saida ira possuir.
    // -------- METRICAS --------- //
    clock_t inicio;
    clock_t fim;

    // Inicio da contagem de tempo de execucao do metodo.
    inicio = clock();

    // Cria os arquivos necessarios e os abre.
    abrirFitasF1(fitas_entrada, &fita_saida);

    iniciaListasF1(blocos_fitas_entrada, &blocos_fita_saida);

    rewind(arquivo_binario);

    geracaoBlocosOrdenadosF1(arquivo_binario, qtde_registros, fitas_entrada, blocos_fitas_entrada, metricas);

    while(true)
    {
        rewind(fita_saida);
        realocarPonteirosFitasF1(fitas_entrada);

        while(!todasAsFitasDeEntradaEstaoVaziasF1(blocos_fitas_entrada))
            intercalacaoBalanceada(fitas_entrada, fita_saida, blocos_fitas_entrada, &blocos_fita_saida, metricas);

        if(fitaPossuiSomenteUmBlocoF1(&blocos_fita_saida, qtde_registros))
            break;

        redistribuirBlocos(fitas_entrada, fita_saida, blocos_fitas_entrada, &blocos_fita_saida, metricas);
    }

    removerFitasEntradaF1(fitas_entrada);

    // Fim da contagem de tempo de execucao do metodo.
    fim = clock();
    metricas->t_execucao += ((double) (fim - inicio)) / CLOCKS_PER_SEC;

    return fita_saida;
}