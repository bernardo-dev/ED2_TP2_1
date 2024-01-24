#include "include/util.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Aceita ate 512KB por pagina, caso necessario realizar paginacao
static unsigned long int ITENS_POR_PAGINA = (512 * 1024) / sizeof(Registro);

// Le "n" linhas do arquivo "PROVAO.TXT"
void lerNElementosDoArquivo(FILE *arquivo, Registro registros[],
                            unsigned int n) {
  for (unsigned int i = 0; i < n; i++) {
    if (fscanf(arquivo, "%8lu %5lf %2s %50[A-Z'\\ -] %30s",
               &registros[i].numero_inscricao, &registros[i].nota,
               registros[i].estado, registros[i].cidade,
               registros[i].curso) == EOF)
      break;

    // Passa para a proxima linha do arquivo
    while (fgetc(arquivo) != '\n')
      ;
  }
}

// Converte o arquivo texto "PROVAO.TXT" para um arquivo equivalente em binario.
FILE *converterParaBinario(FILE *arquivo, unsigned int qtde_registros_arquivo) {
  Registro *pagina;
  FILE *arquivo_binario;

  if ((arquivo_binario = fopen("PROVAO.bin", "w+b")) == NULL)
    return NULL;

  // Eh possivel alocar uma pagina que possua "ITENS_POR_PAGINA" tamanho?
  if ((pagina = alocarRegistros(ITENS_POR_PAGINA)) == NULL)
    return NULL;

  memset(pagina, 0, sizeof(Registro) * ITENS_POR_PAGINA);

  /*
      Se a quantidade de registros for maior do que a pagina pode suportar,
      realiza o processo de paginacao para coletar "ITENS_POR_PAGINA" itens
      de cada vez e escrever no arquivo binario.
  */
  while (qtde_registros_arquivo > ITENS_POR_PAGINA) {
    lerNElementosDoArquivo(arquivo, pagina, ITENS_POR_PAGINA);
    fwrite(pagina, sizeof(Registro), ITENS_POR_PAGINA, arquivo_binario);
    qtde_registros_arquivo -= ITENS_POR_PAGINA;
  }

  lerNElementosDoArquivo(arquivo, pagina, qtde_registros_arquivo);

  fwrite(pagina, sizeof(Registro), qtde_registros_arquivo, arquivo_binario);

  desalocarRegistros(pagina);
  return arquivo_binario;
}

// Converte o arquivo binario "PROVAO.bin" para um arquivo equivalente em texto
// "PROVAO_{qntde_registros}.tx".
FILE *converterParaTexto(FILE *arquivo, unsigned int qtde_registros_arquivo) {
  unsigned int qtde_registros_lidos;
  char nome_arquivo[21];
  Registro *pagina;
  FILE *arquivo_texto;

  sprintf(nome_arquivo, "PROVAO_%u.txt", qtde_registros_arquivo);

  pagina = alocarRegistros(ITENS_POR_PAGINA);

  if (pagina == NULL)
    return NULL;

  if ((arquivo_texto = fopen(nome_arquivo, "w")) == NULL)
    return NULL;

  rewind(arquivo);

  while ((qtde_registros_lidos =
              fread(pagina, sizeof(Registro), ITENS_POR_PAGINA, arquivo)) != 0)
    for (unsigned int i = 0; i < qtde_registros_lidos; i++)
      // Para garantir que a ultima variavel esteja sempre na coluna 70 do
      // arquivo texto, foi usado o especificador de formato "%*s" de largura
      // minima do campo. O numero cinco foi calculado a partir de 
      // 70 - 8 - 5 - 2 - 50
      // O "-" antes do 50 garante que o campo seja alinhado a esquerda
      fprintf(arquivo_texto, "%08lu %05.1lf %2s %-50s %*s\n",
              pagina[i].numero_inscricao, pagina[i].nota, pagina[i].estado,
              pagina[i].cidade, 5, pagina[i].curso);

  desalocarRegistros(pagina);

  fclose(arquivo_texto);

  return arquivo_texto;
}

// Le e verifica se a entrada passada pelo terminal eh valida.
bool lerEntrada(Entrada *entrada, int argc, char *argv[]) {
  if (argc < 4 || argc > 5)
    return false;

  entrada->metodo = atoi(argv[1]);
  entrada->qtde_registros = atoi(argv[2]);
  entrada->situacao = atoi(argv[3]);

  if (argc == 5) {
    strcpy(entrada->opcional, argv[4]);

    // Converte para LOWERCASE de modo a facilitar a comparacao futura
    for (int i = 0; entrada->opcional[i] != '\0'; i++)
      entrada->opcional[i] = tolower(entrada->opcional[i]);

    // Se o argumento opcional for diferente de -P, entao retorna falso
    if (strcmp(entrada->opcional, "-p"))
      return false;
  } else
    entrada->opcional[0] = '\0';

  if (entrada->metodo < 1 || entrada->metodo > 3 || entrada->situacao < 1 ||
      entrada->situacao > 3)
    return false;

  return true;
}

void imprimirArquivoBinario(FILE *pArquivo) {
  Registro registro;
  while (fread(&registro, sizeof(Registro), 1, pArquivo)) {
    printf("Registro = %lf\n", registro.nota);
  }
}

void exibirMetricas(Metrica *metricas) {
  printf("\n--------- METRICAS ---------\n");
  printf("LEITURAS E ESCRITAS ENTRE AS MEMORIAS PRINCIPAL E SECUNDARIA\n");
  printf("NUMERO TOTAL DE LEITURAS: %lu\n", metricas->n_leitura);
  printf("NUMERO TOTAL DE ESCRITAS: %lu\n", metricas->n_escrita);
  printf("NUMERO TOTAL DE COMPARACOES: %lu\n", metricas->n_comparacao);
  printf("TEMPO TOTAL DE EXECUCAO: %.8fs\n", metricas->t_execucao);
}

void resetaMetricas(Metrica *metricas) {
  metricas->n_comparacao = 0;
  metricas->n_escrita = 0;
  metricas->n_leitura = 0;
  metricas->t_execucao = 0;
}