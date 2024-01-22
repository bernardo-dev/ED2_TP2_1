#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/quicksort.h"
#include "include/registro.h"
#include "include/util.h"

int main(int argc, char *argv[]) {
  FILE *arquivo;
  FILE *arquivo_binario;
  Entrada entrada;

  // Le entrada e verifica se eh valida. Retorna "true" se for valida e
  // "false" caso contrario.
  if (lerEntrada(&entrada, argc, argv) == false) {
    printf("O modo de executar o programa esta errado ou algum argumento "
           "inserido nao e valido, execute de acordo:\n\n");
    printf("%s <metodo> <quantidade> <situacao> [-P|-p]\n", argv[0]);
    exit(1);
  }

  // Tenta abrir o arquivo "PROVAO.TXT" disponibilizado pelo docente.
  if ((arquivo = fopen("PROVAO.TXT", "r+")) == NULL) {
    printf("Erro ao tentar abrir o arquivo de texto plano PROVAO.TXT! "
           "Abortando o programa...\n");
    exit(1);
  }

  /*
      Cria uma versao binaria do arquivo de texto plano "PROVAO.TXT" cuja
      quantidade total de registros eh a quantidade passada por argumento
      para o programa.

      O nome do arquivo binario gerado eh "PROVAO.bin".
  */
  if ((arquivo_binario =
           converterParaBinario(arquivo, entrada.qtde_registros)) == NULL) {
    printf("Erro ao tentar abrir o arquivo binario ou falha na alocacao "
           "dinamica de memoria! Abortando o programa...\n");
    exit(1);
  }

  switch (entrada.metodo) {
  case 1: {
    printf("Executando o método 2f-fitas\n");
    break;
  }
  case 2: {
    printf("Executando o método f+1 fitas\n");
    break;
  }
  case 3: {
    fclose(arquivo_binario);

    if (quickSortExterno(entrada.qtde_registros) == false) {
      printf("Erro ao tentar executar o metodo QuickSort Externo. Abortando o "
             "programa...\n");
      exit(1);
    }

    if ((arquivo_binario = fopen("PROVAO.bin", "rb")) == NULL) {
      printf("Erro ao tentar abrir o arquivo binario novamente! Abortando o "
             "programa...\n");
      exit(1);
    }

    converterParaTexto(arquivo_binario, entrada.qtde_registros);

    break;
  }
  }

  fclose(arquivo);
  fclose(arquivo_binario);

  return 0;
}
