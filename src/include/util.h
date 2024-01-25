#ifndef UTIL_H
#define UTIL_H

#include "registro.h"
#include <stdbool.h>
#include <stdio.h>

typedef struct entrada {
  short int metodo;
  unsigned int qtde_registros;
  short int situacao;
  char opcional[3];
} Entrada;

typedef struct {
  // Leitura e escrita entre as memorias principal e secundaria
  unsigned long int n_leitura;
  unsigned long int n_escrita;
  unsigned long int n_comparacao;
  double t_execucao;
} Metrica;

// Le "n" linhas do arquivo "PROVAO.TXT"
void lerNElementosDoArquivo(FILE *arquivo, Registro registros[],
                            unsigned int n);

// Converte o arquivo texto "PROVAO.TXT" para um arquivo equivalente em binario.
FILE *converterParaBinario(FILE *arquivo, unsigned int qtde_registros_arquivo);

// Converte o arquivo binario "PROVAO.bin" para um arquivo equivalente em texto.
FILE *converterParaTexto(FILE *arquivo, unsigned int qtde_registros_arquivo);

// Le e verifica se a entrada passada pelo terminal eh valida.
bool lerEntrada(Entrada *entrada, int argc, char *argv[]);

// Imprime apenas as notas dos registros do arquivo binario
void imprimirArquivoBinario(FILE *);

// Imprime as métricas no terminal
void exibirMetricas(Metrica *);

// Reseta as métricas para o valor inicial (0)
void resetaMetricas(Metrica *);

// Imprime o arquivo texto
void imprimirArquivoTexto(FILE *, unsigned int);

#endif // UTIL_H
