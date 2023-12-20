#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
  */
  if ((arquivo_binario =
           converterParaBinario(arquivo, entrada.qtde_registros)) == NULL) {
    printf("Erro ao tentar abrir o arquivo binario ou falha na alocacao "
           "dinamica de memoria! Abortando o programa...\n");
    exit(1);
  }

  fclose(arquivo);
  fclose(arquivo_binario);

  switch (entrada.metodo) {
  case 1:
    printf("Executando o método 2f-fitas\n");
    break;
  case 2:
    printf("Executando o método f+1 fitas\n");
    break;
  case 3: {
    printf("Executando o método quicksort\n");

    char nomeArquivo[] = "PROVAO.bin";
    /* Declaracao dos tipos utilizados no quicksort externo */

    // Leitura inferior
    FILE *pArqLi = fopen(nomeArquivo, "r+b");

    // Leitura superior
    FILE *pArqLs = fopen(nomeArquivo, "r+b");

    // Escrita inferior
    FILE *pArqEi = fopen(nomeArquivo, "r+b");

    // Escrita superior
    FILE *pArqEs = fopen(nomeArquivo, "r+b");

    if (!(pArqLi && pArqLs && pArqEi && pArqEs)) {
      printf("Arquivo nao pode ser aberto\n");
      fclose(pArqLi);
      fclose(pArqLs);
      fclose(pArqEi);
      fclose(pArqEs);
      exit(1);
    }

    fseek(pArqLi, 0, SEEK_END);
    long final = ftell(pArqLi);
    final = final / sizeof(Registro);
    printf("Quantidade de registros = %ld\n", final);

    // quicksortExterno(pArqLi, pArqLs, pArqEi, pArqEs, 1, final);

    fflush(pArqLi);
    fseek(pArqLi, 0, SEEK_SET);
    imprimirArquivoBinario(pArqLi);

    fclose(pArqLi);
    fclose(pArqLs);
    fclose(pArqEi);
    fclose(pArqEs);
  } break;
  }

  return 0;
}
