#ifndef ORDENACAOBLOCOS_H
#define ORDENACAOBLOCOS_H

#include <stdio.h>
#include "registro.h"
#include "util.h"

void geracaoBlocosOrdenados(FILE *, unsigned int, FILE *[N_FITAS_ENTRADA], Lista [N_FITAS_ENTRADA], Metrica *);
//void redistribuirBlocos(FILE *fitas_entrada[], FILE *, Lista[], Lista *);
void saida_entrada(FILE *[N_FITAS_ENTRADA], FILE *[N_FITAS_SAIDA], Lista[N_FITAS_ENTRADA], Lista[N_FITAS_SAIDA]);

#endif