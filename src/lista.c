#include "include/util.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

Celula *alocarCelula(Metrica *metrica) {
  Celula *celula = (Celula *)malloc(sizeof(Celula));
  metrica->n_leitura++;

  if (celula != NULL) {
    celula->qtde_itens_bloco = 0;
    celula->proximo = NULL;
  }

  metrica->n_escrita++;
  return celula;
}

void iniciaListas(Lista blocos_fita_entrada[N_FITAS_ENTRADA],
                  Lista blocos_fita_saida[N_FITAS_SAIDA], Metrica *metricas) {
  // ----- Mudanca -------
  // Como no caso de 2f fitas existem mais de um bloco, precisa-se passar de um
  // por um
  // ------ Codigo f1 -------------
  // blocos_fita_saida->cabeca = alocarCelula();

  // if(blocos_fita_saida->cabeca == NULL)
  // exit(1);

  // blocos_fita_saida->ultimo = NULL;

  for (unsigned short i = 0; i < N_FITAS_SAIDA; i++) {

    blocos_fita_saida[i].cabeca = alocarCelula(metricas);

    if (blocos_fita_saida[i].cabeca == NULL)
      exit(1);

    blocos_fita_saida[i].ultimo = NULL;
  }

  for (unsigned short i = 0; i < N_FITAS_ENTRADA; i++) {
    blocos_fita_entrada[i].cabeca = alocarCelula(metricas);

    // Se nao foi possivel alocar memoria, o programa termina
    if (blocos_fita_entrada[i].cabeca == NULL)
      exit(1);

    blocos_fita_entrada[i].ultimo = NULL;
  }

  metricas->n_escrita += N_FITAS_ENTRADA + N_FITAS_SAIDA;
}

bool listaEstaVazia(Lista *lista) { return lista->cabeca->proximo == NULL; }

void insereLista(Lista *lista, unsigned int qtde_itens_bloco,
                 Metrica *metrica) {
  if (listaEstaVazia(lista)) {
    lista->ultimo = alocarCelula(metrica);

    // Se nao foi possivel alocar memoria, o programa termina
    if (lista->ultimo == NULL)
      exit(1);

    lista->cabeca->proximo = lista->ultimo;
    lista->ultimo->qtde_itens_bloco = qtde_itens_bloco;
  } else {
    lista->ultimo->proximo = alocarCelula(metrica);

    // Se nao foi possivel alocar memoria, o programa termina
    if (lista->ultimo->proximo == NULL)
      exit(1);

    lista->ultimo = lista->ultimo->proximo;
    lista->ultimo->qtde_itens_bloco = qtde_itens_bloco;
  }
}

void removeListaInicio(Lista *lista) {
  Celula *auxiliar;

  auxiliar = lista->cabeca->proximo;
  lista->cabeca->proximo = auxiliar->proximo;

  free(auxiliar);
}

bool fitaPossuiSomenteUmBloco(Lista *blocos_por_fita,
                              unsigned int qtde_total_registros) {
  if (blocos_por_fita->cabeca->proximo == NULL)
    return false;
  // Verificar se a segunda fita esta vazia
  return blocos_por_fita->cabeca->proximo->qtde_itens_bloco ==
         qtde_total_registros;
}

bool apenasUmaFitaPossuiBlocos(Lista blocos_fitas_saida[N_FITAS_SAIDA]) {
  return blocos_fitas_saida[0].cabeca->proximo != NULL &&
         blocos_fitas_saida[1].cabeca->proximo == NULL;
}

bool todasAsFitasDeEntradaEstaoVazias(
    Lista blocos_fitas_entrada[N_FITAS_ENTRADA], Metrica *metricas) {
  for (unsigned short i = 0; i < N_FITAS_ENTRADA; i++)
    if (!listaEstaVazia(&blocos_fitas_entrada[i])) {
      metricas->n_comparacao++;
      return false;
    }

  return true;
}