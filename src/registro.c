#include "include/registro.h"
#include <stdlib.h>

// Aloca uma quantidade "n" de registros.
Registro *alocarRegistros(unsigned int n) {
  Registro *registros = (Registro *)malloc(n * sizeof(Registro));

  return registros;
}

// Desaloca uma quantidade "n" de registros.
void desalocarRegistros(Registro *registros) { free(registros); }

