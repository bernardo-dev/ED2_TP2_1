#ifndef ORDENACAOBLOCOS_H
#define ORDENACAOBLOCOS_H

#include "registro.h"
#include "util.h"
#include <stdio.h>

void geracaoBlocosOrdenados(FILE *, unsigned int, FILE *[N_FITAS_ENTRADA],
                            Lista[N_FITAS_ENTRADA], Metrica *);
// void redistribuirBlocos(FILE *fitas_entrada[], FILE *, Lista[], Lista *);
void saida_entrada(FILE *[N_FITAS_ENTRADA], FILE *[N_FITAS_SAIDA],
                   Lista[N_FITAS_ENTRADA], Lista[N_FITAS_SAIDA]);

#endif