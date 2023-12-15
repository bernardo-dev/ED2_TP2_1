#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
  */
  if ((arquivo_binario =
           converterParaBinario(arquivo, entrada.qtde_registros)) == NULL) {
    printf("Erro ao tentar abrir o arquivo binario ou falha na alocacao "
           "dinamica de memoria! Abortando o programa...\n");
    exit(1);
  }

  fclose(arquivo);
  fclose(arquivo_binario);

  return 0;
}
