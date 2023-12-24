#include "include/registro.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define TAM_MEMORIA_PRINCIPAL 10

typedef struct {
  Registro vetor[TAM_MEMORIA_PRINCIPAL];
  unsigned int tamMax;
  unsigned int tamOcupado;
} Area;

void inicializarArea(Area *pArea) {
  pArea->tamMax = TAM_MEMORIA_PRINCIPAL;
  pArea->tamOcupado = 0;
  for (int i = 0; i < 10; i++) {
    pArea->vetor[i].nota = 200;
    memset(pArea->vetor[i].estado, '\0', 3);
    memset(pArea->vetor[i].cidade, '\0', 51);
    memset(pArea->vetor[i].curso, '\0', 31);
  }
}

void imprimirArea(Area *pArea) {
  if (pArea->tamOcupado == 0) {
    printf("Area vazia\n");
    return;
  }

  printf("Area interna\n");

  for (unsigned int i = 0; i < pArea->tamOcupado; i++) {
    printf("Registro %d \n", i + 1);
    printf("\tNota [%lf]\n", pArea->vetor[i].nota);
    printf("\tEstado [%s]\n", pArea->vetor[i].estado);
    printf("\tCidade [%s]\n", pArea->vetor[i].curso);
    printf("---------\n");
  }
}

// Busca de O(log n)
unsigned int buscaBinaria(Area *pArea, double nota) {
  int insercao;
  int inicio = 0;
  int fim = pArea->tamOcupado - 1;
  int meio = 0;

  while (inicio <= fim) {
    meio = inicio + (fim - inicio) / 2;
    printf("meio = %d\n", meio);

    if (pArea->vetor[meio].nota == nota) {
      // Registro com nota igual encontrada na posicao meio
      return insercao = meio;
    }

    if (pArea->vetor[meio].nota < nota) {
      inicio = meio + 1; // Descartar metade inferior
    } else {
      fim = meio - 1; // Descartar metade superior
    }
  }

  // Retornar a posicao onde a chave deve ser inserida
  return insercao = inicio;
}

// Insercao de O(n)
void inserirOrdenadoNaArea(Area *pArea, Registro *pRegistro) {
  if (pArea->tamOcupado == pArea->tamMax) {
    return;
  }

  if (pArea->tamOcupado == 0) {
    pArea->vetor[0] = *pRegistro;
    pArea->tamOcupado++;
    return;
  }

  // Posicao que deve ser inserido o novo registro
  unsigned int posicao = buscaBinaria(pArea, pRegistro->nota);
  printf("Posicao de insercao = %u\n", posicao);

  // Desloca os elementos maiores para abrir espaco para o novo registro
  for (unsigned int i = pArea->tamOcupado; i > posicao; i--) {
    pArea->vetor[i] = pArea->vetor[i - 1];
  }

  // Insere o registro na posicao correta
  pArea->vetor[posicao] = *pRegistro;

  pArea->tamOcupado++;
}

void retiraUltimoArea(Area *pArea, Registro *pRegistro){
  *pRegistro = pArea->vetor[pArea->tamOcupado - 1];
  pArea->tamOcupado--;
}

void retiraPrimeiroArea(Area *pArea, Registro *pRegistro) {
  *pRegistro = pArea->vetor[0];
  for (int i = 0; i < pArea->tamOcupado; i++) {
    pArea->vetor[i] = pArea->vetor[i + 1];
  }
  pArea->tamOcupado--;
}

void lerSuperior(FILE *pArqLs, Registro *pUltimoLido, int *pLs,
                 bool *pOndeLer) {
  fseek(pArqLs, (*pLs - 1) * sizeof(Registro), SEEK_SET);
  fread(pUltimoLido, sizeof(Registro), 1, pArqLs);
  (*pLs)--;
  *pOndeLer = false;
}

void lerInferior(FILE *pArqLi, Registro *pUltimoLido, int *pLi,
                 bool *pOndeLer) {
  fread(pUltimoLido, sizeof(Registro), 1, pArqLi);
  (*pLi)++;
  *pOndeLer = true;
}

void escreveMax(FILE *pArqEs, Registro registro, int *pEs) {
  fseek(pArqEs, (*pEs - 1) * sizeof(Registro), SEEK_SET);
  fwrite(&registro, sizeof(Registro), 1, pArqEs);
  (*pEs)--;
}

void escreveMin(FILE *pArqEi, Registro registro, int *pEi) {
  fwrite(&registro, sizeof(Registro), 1, pArqEi);
  (*pEi)++;
}

void retiraMax(Area *pArea, Registro *pRegistro, int *pNRArea){
  retiraUltimoArea(pArea, pRegistro);
  *pNRArea = pArea->tamOcupado;
}

void retiraMin(Area *pArea, Registro *pRegistro, int *pNRArea){
  retiraPrimeiroArea(pArea, pRegistro);
  *pNRArea = pArea->tamOcupado;
}

void particao(FILE *pArqLi, FILE *pArqLs, FILE *pArqEi, FILE *pArqEs,
              Area *pArea, int esquerda, int direita, int *pI, int *pJ) {
  unsigned int leituraSuperior = direita;
  unsigned int escritaSuperior = direita;

  unsigned int leituraInferior = esquerda;
  unsigned int escritaInferior = esquerda;

  int ocupacaoArea = 0;
  double limiteInferior = -1;
  double limiteSuperior = 101;

  bool ondeLer = true;
  Registro ultimoLido, registro;

  fseek(pArqLi, (leituraInferior - 1) * sizeof(Registro), SEEK_SET);
  fseek(pArqEi, (escritaSuperior - 1) * sizeof(Registro), SEEK_SET);

  *pI = esquerda - 1;
  *pJ = direita + 1;
  while (leituraSuperior >= leituraInferior) {
    if (pArea->tamOcupado < pArea->tamMax) {
      if (ondeLer) {
        lerSuperior(pArqEs, &ultimoLido, &leituraSuperior, &ondeLer);
      }
    }
  }
}

void quicksortExterno(FILE *pArqLi, FILE *pArqLs, FILE *pArqEi, FILE *pArqEs,
                      int esquerda, int direita) {
  int i, j;

  Area area;

  if (direita - esquerda < 1)
    return;

  inicializarArea(&area);

  imprimirArea(&area);
  printf("\n\n\n");

  particao(pArqLi, pArqLs, pArqEi, pArqEs, &area, esquerda, direita, &i, &j);

  imprimirArea(&area);
}

/*
void quicksortExterno() {
  Area area;

  inicializarArea(&area);

  Registro registro;
  FILE *pArquivo = fopen("PROVAO.bin", "rb");

  imprimirArea(&area);

  for (int i = 0; i < 10; i++) {
    fread(&registro, sizeof(Registro), 1, pArquivo);
    inserirOrdenadoNaArea(&area, &registro);
    imprimirArea(&area);
  }

  imprimirArea(&area);
}
*/
