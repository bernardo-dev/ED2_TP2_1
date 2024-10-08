#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/duasFitas.h"
#include "include/quicksort.h"
#include "include/umaFita.h"
#include "include/util.h"

int main(int argc, char *argv[]) {
  FILE *arquivo;
  FILE *arquivo_binario;
  FILE *aux;
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
  switch (entrada.situacao) {
  case 1: {
    if ((arquivo = fopen("PROVAO_ASCENDENTE.TXT", "r+")) == NULL) {
      printf("Erro ao tentar abrir o arquivo de texto plano PROVAO.TXT! "
             "Abortando o programa...\n");
      exit(1);
    }
    break;
  }
  case 2: {
    if ((arquivo = fopen("PROVAO_DESCENDENTE.TXT", "r+")) == NULL) {
      printf("Erro ao tentar gerar o arquivo de texto plano ordenado! "
             "Abortando o programa...\n");
      exit(1);
    }
    break;
  }
  case 3: {
    if ((arquivo = fopen("PROVAO.TXT", "r+")) == NULL) {
      printf("Erro ao tentar abrir o arquivo de texto plano PROVAO.TXT! "
             "Abortando o programa...\n");
      exit(1);
    }
    break;
  }
  }

  if (strcmp(entrada.opcional, "-p") == 0) {
    imprimirArquivoTexto(arquivo, entrada.qtde_registros);
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

  Metrica metricas;
  resetaMetricas(&metricas);

  switch (entrada.metodo) {
  case 1: {
    printf("Executando o método 2f-fitas\n");

    aux = doisFfitas(arquivo_binario, entrada.qtde_registros, &metricas);

    converterParaTexto(aux, entrada.qtde_registros);

    exibirMetricas(&metricas);
    break;
  }
  case 2: {
    printf("Executando o método f+1 fitas\n");

    FILE *fita_saida;

    fita_saida = f1fitas(arquivo_binario, entrada.qtde_registros, &metricas);

    if (fita_saida == NULL) {
      printf("Algum erro inesperado ocorreu ao executar o metodo F + 1 fitas! "
             "Abortando o programa...\n");
      exit(1);
    }

    converterParaTexto(fita_saida, entrada.qtde_registros);

    fclose(fita_saida);

    exibirMetricas(&metricas);
    break;
  }
  case 3: {
    printf("Executando o método QuickSort Externo\n");
    fclose(arquivo_binario);

    if (quickSortExterno(entrada.qtde_registros, &metricas) == false) {
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

    exibirMetricas(&metricas);
    break;
  }
  }

  char nome_arquivo[21];
  sprintf(nome_arquivo, "PROVAO_%u.txt", entrada.qtde_registros);

  FILE *arquivo_ordenado;
  arquivo_ordenado = fopen(nome_arquivo, "r");

  if (strcmp(entrada.opcional, "-p") == 0) {
    imprimirArquivoTexto(arquivo_ordenado, entrada.qtde_registros);
  }
  
  fclose(arquivo_ordenado);
  fclose(arquivo);
  fclose(arquivo_binario);

  return 0;
}
