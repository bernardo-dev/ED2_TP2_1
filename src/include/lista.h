#ifndef LISTA_H
#define LISTA_H

#include "registro.h"
#include "util.h"

Celula* alocarCelula();
void iniciaListas(Lista[N_FITAS_ENTRADA], Lista [N_FITAS_SAIDA]);
bool listaEstaVazia(Lista *);
void insereLista(Lista *, unsigned int, Metrica *);
void removeListaInicio(Lista *);
bool fitaPossuiSomenteUmBloco(Lista *, unsigned int);
bool todasAsFitasDeEntradaEstaoVazias(Lista[N_FITAS_ENTRADA]);
bool apenasUmaFitaPossuiBlocos(Lista [N_FITAS_SAIDA]);

#endif