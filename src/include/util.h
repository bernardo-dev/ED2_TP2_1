#ifndef UTIL_H
#define UTIL_H

#include "registro.h"
#include <stdio.h>
#include <stdbool.h>


typedef struct entrada {
  short int metodo;
  unsigned int qtde_registros;
  short int situacao;
  char opcional[3];
} Entrada;

// Le "n" linhas do arquivo "PROVAO.TXT"
void lerNElementosDoArquivo(FILE *arquivo, Registro registros[],
                            unsigned int n);

// Converte o arquivo texto "PROVAO.TXT" para um arquivo equivalente em binario.
FILE *converterParaBinario(FILE *arquivo, unsigned int qtde_registros_arquivo);

// Le e verifica se a entrada passada pelo terminal eh valida.
bool lerEntrada(Entrada *entrada, int argc, char *argv[]);

#endif // UTIL_H