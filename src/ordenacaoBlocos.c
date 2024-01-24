#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "include/heapFita.h"
#include "include/lista.h"
#include "include/registro.h"
#include "include/ordenacaoBlocos.h"
#include "include/util.h"

void geracaoBlocosOrdenados(FILE *arquivo_binario, unsigned int qtde_registros, FILE *fitas_entrada[N_FITAS_ENTRADA], Lista blocos_por_fita[N_FITAS_ENTRADA], Metrica *metrica)
{
    unsigned int indice_fita;   // Indice que marca em qual das fitas esta se inserindo blocos
    Heap heap;

    indice_fita = 0;
    iniciaHeap(&heap);

    // Se a quantidade de registros no arquivo couber em memoria principal.
    if(qtde_registros <= TAM_MEMORIA_PRINCIPAL)
    {   
        metrica->n_comparacao++;
        // Le todos itens do arquivo e os coloca em memoria principal.
        for(unsigned short i = 0 ; i < qtde_registros ; i++)
        {
            // Se a leitura foi mal-sucedida, termina o programa
            if(fread(&heap.itens[i].registro, sizeof(Registro), 1, arquivo_binario) == 0){
                metrica->n_leitura++;
                exit(1);
            }

            heap.n++;
        }

        ItemHeap auxiliar;

        // Insertion Sort
        for(unsigned short i = 1 ; i < heap.n ; i++)
        {
            for(unsigned short j = i ; j > 0 ; j--)
            {
                if(heap.itens[j].registro.nota < heap.itens[j - 1].registro.nota)
                {   
                    metrica->n_comparacao++;
                    auxiliar = heap.itens[j];
                    heap.itens[j] = heap.itens[j - 1];
                    heap.itens[j - 1] = auxiliar;
                }
            }
        }

        // Insere registros ordenados na primeira fita
        for(unsigned short i = 0 ; i < heap.n ; i++){
            fwrite(&heap.itens[i].registro, sizeof(Registro), 1, fitas_entrada[indice_fita]);
            metrica->n_escrita++;
        }
        insereLista(&blocos_por_fita[indice_fita], qtde_registros, metrica);
    }
    else
    {   
        metrica->n_comparacao++;
        // Inicialmente, insere todos os primeiros TAM_MEMORIA_PRINCIPAL registros no vetor do heap.
        for(unsigned short i = 0 ; i < TAM_MEMORIA_PRINCIPAL ; i++)
        {
            // Se a leitura foi mal-sucedida, termina o programa
            if(fread(&heap.itens[i].registro, sizeof(Registro), 1, arquivo_binario) == 0){
                metrica->n_comparacao++;
                exit(1);
            }

            heap.itens[i].marcado = false;

            heap.n++;
        }
        
        // Parte do codigo que realiza a pre-ordenacao dos blocos propriamente dita
        {
            unsigned int contador;          // Conta a quantidade de registros lidos do arquivo.
            unsigned int itens_por_bloco;   // Conta a quantidade de itens lidos para aquele bloco especifico.
            ItemHeap ultimo_lido;
            ItemHeap ultimo_retirado;

            contador = TAM_MEMORIA_PRINCIPAL + 1;
            itens_por_bloco = 0;

            heapConstroi(&heap, metrica);

            // Le o restante dos registros do arquivo
            while(contador <= qtde_registros)
            {
                /*
                    Se a quantidade de marcados for igual a de todos os elementos presentes no heap,
                    desmarca todos e inicia um novo bloco.
                */
                if(heap.marcados == heap.n)
                {
                    metrica->n_comparacao++;
                    // Registra a quantidade de itens que aquele bloco na fita indice_fita possui.
                    insereLista(&blocos_por_fita[indice_fita], itens_por_bloco, metrica);

                    desmarcarItensHeap(&heap);
                    // Reinicia a contagem de itens por bloco
                    itens_por_bloco = 0;
                    // Passa a escrever na proxima fita
                    indice_fita++;

                    if(indice_fita == N_FITAS_ENTRADA){
                        metrica->n_comparacao++;
                        indice_fita = 0;
                    }
                }
                
                // Escreve o menor elemento do heap na fita indice_fita.
                fwrite(&heap.itens[0].registro, sizeof(Registro), 1, fitas_entrada[indice_fita]);
                metrica->n_escrita++;
                itens_por_bloco++;

                // Retira o menor item do heap e o retorna para a variavel "ultimo_retirado"
                ultimo_retirado = heapRetira(&heap);

                fread(&ultimo_lido.registro, sizeof(Registro), 1, arquivo_binario);
                metrica->n_leitura++;

                // Se o ultimo registro lido do arquivo for menor do que o item retirado do heap.
                if(ultimo_lido.registro.nota < ultimo_retirado.registro.nota)
                {  
                    metrica->n_comparacao++;
                    ultimo_lido.marcado = true;
                    heap.marcados++;
                }
                else{
                    metrica->n_comparacao++;
                    ultimo_lido.marcado = false;
                }
                // Insere o ultimo item lido do arquivo no heap.
                heapInsere(&heap, &ultimo_lido);

                heapRefaz(&heap, 0, heap.n - 1, metrica);

                contador++;
            }

            // Apos lidos todos os itens, esvazia o heap, inserindo os seus elementos na fita necessaria.
            while(heap.n > 0)
            {
                if(heap.marcados == heap.n)
                {   
                    metrica->n_comparacao++;
                    // Registra a quantidade de itens que aquele bloco na fita indice_fita possui.
                    insereLista(&blocos_por_fita[indice_fita], itens_por_bloco, metrica);

                    desmarcarItensHeap(&heap);
                    // Reinicia a contagem de itens por bloco
                    itens_por_bloco = 0;
                    // Passa a escrever na proxima fita
                    indice_fita++;

                    if(indice_fita == N_FITAS_ENTRADA){
                        metrica->n_comparacao++;
                        indice_fita = 0;
                    }
                }

                ultimo_retirado = heapRetira(&heap);

                fwrite(&ultimo_retirado.registro, sizeof(Registro), 1, fitas_entrada[indice_fita]);
                metrica->n_escrita++;

                itens_por_bloco++;
                
                heapRefaz(&heap, 0, heap.n - 1,metrica);
            }

            // Registra a quantidade de itens que aquele bloco na fita indice_fita possui.
            insereLista(&blocos_por_fita[indice_fita], itens_por_bloco, metrica);
        }
    }
}

//void redistribuirBlocos(FILE *fitas_entrada[N_FITAS_ENTRADA], FILE *fita_saida, Lista blocos_fitas_entrada[N_FITAS_ENTRADA], Lista *blocos_fita_saida)
//{
    //unsigned int indice_fita;      // Indice que marca em qual das fitas esta se inserindo blocos
    //unsigned int itens_por_bloco;
    //Registro buffer;

    //itens_por_bloco = 0;
    //indice_fita = 0;

    //realocarPonteirosFitas(fitas_entrada);
    //rewind(fita_saida);

    //while(!listaEstaVazia(blocos_fita_saida))
    //{
        //while(itens_por_bloco < blocos_fita_saida->cabeca->proximo->qtde_itens_bloco)
        //{
            //fread(&buffer, sizeof(Registro), 1, fita_saida);
            //fwrite(&buffer, sizeof(Registro), 1, fitas_entrada[indice_fita]);
            //itens_por_bloco++;
        //}

        //removeListaInicio(blocos_fita_saida);

        //insereLista(&blocos_fitas_entrada[indice_fita], itens_por_bloco);
        //itens_por_bloco = 0;
        //indice_fita++;

        //if(indice_fita == N_FITAS_ENTRADA)
            //indice_fita = 0;
    //}
//}

// Tranforma a fita de saida em fita de entrada
void saida_entrada(FILE *fitas_entrada[N_FITAS_ENTRADA], FILE *fitas_saida[N_FITAS_SAIDA], Lista blocos_fitas_entrada[N_FITAS_ENTRADA], Lista blocos_fita_saida[N_FITAS_SAIDA]){

    FILE *arquivo_auxiliar;
    Lista lista_auxiliar;

    for(unsigned short i = 0 ; i < N_FITAS_SAIDA ; i++)
    {
        arquivo_auxiliar = fitas_entrada[i];
        fitas_entrada[i] = fitas_saida[i];
        fitas_saida[i] = arquivo_auxiliar;

        lista_auxiliar = blocos_fitas_entrada[i];
        blocos_fitas_entrada[i] = blocos_fita_saida[i];
        blocos_fita_saida[i] = lista_auxiliar;
    }
}
