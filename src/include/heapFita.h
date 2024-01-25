#ifndef HEAPFITA_H
#define HEAPFITA_H

#include <stdio.h>
#include "registro.h"
#include "util.h"

// ----------------- Heap -----------------------
void iniciaHeap(Heap *);
void heapRefaz(Heap *, short, short, Metrica *);
void heapConstroi(Heap *, Metrica *);
ItemHeap heapRetira(Heap *);
void heapInsere(Heap *, ItemHeap *);
void desmarcarItensHeap(Heap *);

// ------------------ Fitas ----------------------
void removerFitasEntrada(FILE *[N_FITAS_ENTRADA]);
void abrirFitas(FILE *[N_FITAS_ENTRADA], FILE *[N_FITAS_SAIDA]);
void converterFitaParaTexto(FILE *, Lista *);
void realocarPonteirosFitas(FILE *[N_FITAS_ENTRADA]);
void removerFitasSaida(FILE *[N_FITAS_SAIDA]);

#endif