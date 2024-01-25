#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "include/duasFitas.h"
#include "include/heapFita.h"
#include "include/lista.h"
#include "include/ordenacaoBlocos.h"
#include "include/registro.h"
#include "include/util.h"

// void converterFitasParaTexto(FILE *fitas_entrada[N_FITAS_ENTRADA], Lista
// blocos_por_fita[N_FITAS_ENTRADA])
// {
//     Registro buffer;
//     unsigned int c_itens;
//     unsigned int c_blocos;

//     for(unsigned short i = 0 ; i < N_FITAS_ENTRADA ; i++)
//     {
//         rewind(fitas_entrada[i]);

//         c_blocos = 1;
//         // Varre a lista
//         while(!listaEstaVazia(&blocos_por_fita[i]))
//         {
//             c_itens = 0;

//             printf("BLOCO %u | QTDE DE ITENS : %u\n", c_blocos,
//             blocos_por_fita[i].cabeca->proximo->qtde_itens_bloco);

//             // Varre o bloco
//             while(c_itens <
//             blocos_por_fita[i].cabeca->proximo->qtde_itens_bloco)
//             {
//                 fread(&buffer, sizeof(Registro), 1, fitas_entrada[i]);
//                 printf("%lf\t%lu\n", buffer.nota, buffer.numero_inscricao);
//                 c_itens++;
//             }

//             c_blocos++;
//             removeListaInicio(&blocos_por_fita[i]);
//         }
//     }
// }

void intercalacaoBalanceada2f(FILE *fitas_entrada[N_FITAS_ENTRADA],
                              FILE *fitas_saida[N_FITAS_SAIDA],
                              Lista blocos_por_fita[N_FITAS_ENTRADA],
                              Lista blocos_fita_saida[N_FITAS_SAIDA],
                              unsigned short indice_fita_saida,
                              Metrica *metricas) {
  unsigned short marcados; // Calcula a quantidade de fitas com blocos
                           // disponiveis restantes.
  unsigned short k; // Guarda o indice da fita que possui o elemento com o menor
                    // valor em um dado momento.
  unsigned int itens_por_bloco; // Calcula a quantidade de itens por bloco na
                                // fita de saida.
  ItemHeap
      memoria[TAM_MEMORIA_PRINCIPAL]; // Guarda os registros das fitas. Cada
                                      // posicao eh um indice de uma fita.

  marcados = 0;

  // Preenche cada posicao do vetor com o primeiro elemento de cada fita que
  // possui pelo menos um bloco disponivel.
  for (unsigned short i = 0; i < TAM_MEMORIA_PRINCIPAL; i++) {
    if (listaEstaVazia(&blocos_por_fita[i])) {
      memoria[i].marcado = true;
      marcados++;
    } else {
      fread(&memoria[i].registro, sizeof(Registro), 1, fitas_entrada[i]);
      metricas->n_leitura++;
      memoria[i].marcado = false;

      // Remove a quantidade de itens no bloco, ja que acabou de se ler um.
      blocos_por_fita[i].cabeca->proximo->qtde_itens_bloco--;
    }
  }

  // ---------- Mudanca-------------
  // Como agr existe mais de uma fita de saida, precisa-se de um vetor para
  // contabilizar os itens por bloco
  // ----------- F1 fitas ----------
  itens_por_bloco = 0;

  // for(unsigned short i = 0; i < N_FITAS_SAIDA; i++){
  //     itens_por_bloco[i] = 0;
  // }

  while (marcados != TAM_MEMORIA_PRINCIPAL) {
    k = 0;

    // Localiza a primeira fita com um bloco disponivel
    while (memoria[k].marcado == true)
      k++;

    // Localiza a fita com o menor valor num dado momento
    for (unsigned short i = k + 1; i < TAM_MEMORIA_PRINCIPAL; i++) {
      metricas->n_comparacao++;
      if (memoria[i].marcado == false &&
          memoria[i].registro.nota < memoria[k].registro.nota) {
        k = i;
      }
    }

    fwrite(&memoria[k].registro, sizeof(Registro), 1,
           fitas_saida[indice_fita_saida]);
    metricas->n_escrita++;
    itens_por_bloco++;

    // -------- Mudanca ----------
    // Como tem mais fitas precisa colocar dentro do for
    // ----------- F1 Fitas ----------------------
    // Escreve o menor elemento na fita de saida.
    // fwrite(&memoria[k].registro, sizeof(Registro), 1, fita_saida);
    // itens_por_bloco++;

    // Le o proximo item da fita de entrada caso ela tenha itens restantes em
    // seu bloco.
    if (blocos_por_fita[k].cabeca->proximo->qtde_itens_bloco > 0) {
      metricas->n_leitura++;
      fread(&memoria[k].registro, sizeof(Registro), 1, fitas_entrada[k]);
      blocos_por_fita[k].cabeca->proximo->qtde_itens_bloco--;
    }
    // Caso contrario, significa que o elemento que acabou de ser escrito na
    // fita de saida foi o ultimo do bloco.
    else {
      memoria[k].marcado = true;
      removeListaInicio(&blocos_por_fita[k]);
      marcados++;
      // Contador pra saber em qual fita o elemento vai ser escrito
      //  cont_fita_saida++;
    }
  }

  if (itens_por_bloco != 0) {
    insereLista(&blocos_fita_saida[indice_fita_saida], itens_por_bloco,
                metricas);
  }
}

FILE *doisFfitas(FILE *arquivo_binario, unsigned int qtde_registros,
                 Metrica *metricas) {
  clock_t inicio;
  clock_t fim;

  // Inicio da contagem de tempo
  inicio = clock();

  FILE *fitas_entrada[N_FITAS_ENTRADA];
  FILE *fitas_saida[N_FITAS_SAIDA];
  Lista blocos_fitas_entrada[N_FITAS_ENTRADA]; // Cada posicao do vetor eh uma
                                               // lista encadeada de blocos
                                               // presentes em cada fita.
  Lista blocos_fitas_saida[N_FITAS_SAIDA]; // Lista encadeada de blocos que a
                                           // fita de saida ira possuir.
  unsigned short indice_fita_saida;

  // Cria os arquivos necessarios e os abre.
  abrirFitas(fitas_entrada, fitas_saida);

  iniciaListas(blocos_fitas_entrada, blocos_fitas_saida);

  rewind(arquivo_binario);

  geracaoBlocosOrdenados(arquivo_binario, qtde_registros, fitas_entrada,
                         blocos_fitas_entrada, metricas);

  // for(unsigned short i = 0 ; i < N_FITAS_ENTRADA ; i++)
  // {
  //     printf("FITA %hu\n", i);
  //     converterFitaParaTexto(fitas_entrada[i], &blocos_fitas_entrada[i]);
  // }

  // exit(1);

  while (true) {
    //----- Mudanca --------
    // Tem que chamar a funcao realocar para a fita de saida tbm
    // ---------- F1 fitas -------------
    // rewind(fitas_saida);
    realocarPonteirosFitas(fitas_saida);
    realocarPonteirosFitas(fitas_entrada);

    indice_fita_saida = 0;

    while (!todasAsFitasDeEntradaEstaoVazias(blocos_fitas_entrada)) {
      intercalacaoBalanceada2f(fitas_entrada, fitas_saida, blocos_fitas_entrada,
                               blocos_fitas_saida, indice_fita_saida, metricas);
      indice_fita_saida++;

      if (indice_fita_saida == N_FITAS_SAIDA) {
        metricas->n_comparacao++;
        indice_fita_saida = 0;
      }
    }

    if (apenasUmaFitaPossuiBlocos(blocos_fitas_saida)) {
      metricas->n_comparacao++;
      break;
    }

    // redistribuirBlocos(fitas_entrada, fitas_saida, blocos_fitas_entrada,
    // &blocos_fita_saida);
    saida_entrada(fitas_entrada, fitas_saida, blocos_fitas_entrada,
                  blocos_fitas_saida);
  }

  removerFitasEntrada(fitas_entrada);
  removerFitasSaida(fitas_saida);

  // Fim da contagem de tempo
  fim = clock();
  metricas->t_execucao += ((double)(fim - inicio)) / CLOCKS_PER_SEC;

  return fitas_saida[0];
}