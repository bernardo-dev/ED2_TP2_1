#ifndef DUASFITAS_H
#define DUASFITAS_H

#include "registro.h"
#include "util.h"
#include <stdio.h>

void intercalacaoBalanceada2f(FILE *[N_FITAS_ENTRADA], FILE *[N_FITAS_SAIDA],
                              Lista[N_FITAS_ENTRADA], Lista[N_FITAS_SAIDA],
                              unsigned short, Metrica *);
FILE *doisFfitas(FILE *, unsigned int, Metrica *);

#endif
