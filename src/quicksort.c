#include "include/registro.h"
#include "include/util.h"

#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define TAMANHO_MAX 10

typedef struct {
  Registro vetor[TAMANHO_MAX];
  unsigned int tamMax;
  unsigned int tamOcupado;
} Area;

void FAVazia(Area *pArea) {
  pArea->tamMax = TAMANHO_MAX;
  pArea->tamOcupado = 0;
  for (int i = 0; i < 10; i++) {
    pArea->vetor[i].nota = 0;
    // memset(pArea->vetor[i].estado, '\0', 3);
    // memset(pArea->vetor[i].cidade, '\0', 51);
    // memset(pArea->vetor[i].curso, '\0', 31);
  }
}

void lerSup(FILE *pArqLs, Registro *pUltLido, int *pLs, bool *pOndeLer) {
  fseek(pArqLs, (*pLs - 1) * sizeof(Registro), SEEK_SET);
  fread(pUltLido, sizeof(Registro), 1, pArqLs);
  (*pLs)--;
  *pOndeLer = false;
}

void lerInf(FILE *pArqLi, Registro *pUltLido, int *pLi, bool *pOndeLer) {
  fread(pUltLido, sizeof(Registro), 1, pArqLi);
  (*pLi)++;
  *pOndeLer = true;
}

void insereItem(Registro *pUltLido, Area *pArea)
{
  unsigned int i = pArea->tamOcupado;

  while(i > 0 && pArea->vetor[i - 1].nota >= pUltLido->nota)
  {
    pArea->vetor[i] = pArea->vetor[i - 1];
    i--;
  }

  pArea->vetor[i] = *pUltLido;
  pArea->tamOcupado++;
}

void inserirArea(Area *pArea, Registro *pUltLido, unsigned int *pNRArea) {
  // insere o registro 'pUltLido' de forma ordenada na área
  insereItem(pUltLido, pArea);
  *pNRArea = pArea->tamOcupado;
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

void retiraPrimeiro(Area *pArea, Registro *pRegistro)
{
  *pRegistro = pArea->vetor[0];

  for(unsigned int i = 0 ; i < pArea->tamOcupado - 1 ; i++)
    pArea->vetor[i] = pArea->vetor[i + 1];

  pArea->tamOcupado--;
}

void retiraUltimo(Area *pArea, Registro *pRegistro)
{
  *pRegistro = pArea->vetor[pArea->tamOcupado - 1];
  pArea->tamOcupado--;
}

void retiraMax(Area *pArea, Registro *pRegistro, unsigned int *pNRArea) {
  retiraUltimo(pArea, pRegistro);
  *pNRArea = pArea->tamOcupado;
}

void retiraMin(Area *pArea, Registro *pRegistro, unsigned int *pNRArea) {
  retiraPrimeiro(pArea, pRegistro);
  *pNRArea = pArea->tamOcupado;
}

void particao(FILE *pArqLi, FILE *pArqLs, FILE *pArqEi, FILE *pArqEs,
              Area *pArea, int esquerda, int direita, int *pI, int *pJ) {
  int Ls;
  int Es;
  int Li;
  int Ei;

  Ls = direita;
  Es = direita;
  Ei = esquerda;
  Li = esquerda;

  unsigned int NRArea = 0;
  double Linf = -1;
  double Lsup = 101;

  bool OndeLer = true;
  Registro UltLido, registro;

  fseek(pArqLi, (Li - 1) * sizeof(Registro), SEEK_SET);
  fseek(pArqEi, (Es - 1) * sizeof(Registro), SEEK_SET);

  *pI = esquerda - 1;
  *pJ = direita + 1;

  while (Ls >= Li) {
    if (NRArea < pArea->tamMax - 1) {
      // NRArea < T
      if (OndeLer) {
        lerSup(pArqLs, &UltLido, &Ls, &OndeLer);
      } else {
        lerInf(pArqLi, &UltLido, &Li, &OndeLer);
      }
      inserirArea(pArea, &UltLido, &NRArea);
      continue;
    }
    if (Ls == Es) {
      lerSup(pArqLs, &UltLido, &Ls, &OndeLer);
    } else if (Li == Ei) {
      lerInf(pArqLi, &UltLido, &Li, &OndeLer);
    } else if (OndeLer) {
      lerSup(pArqEs, &UltLido, &Ls, &OndeLer);
    } else {
      lerInf(pArqLi, &UltLido, &Li, &OndeLer);
    }

    if (UltLido.nota > Lsup) {
      *pJ = Es;
      escreveMax(pArqEs, UltLido, &Es);
      continue;
    }

    if (UltLido.nota < Linf) {
      *pI = Ei;
      escreveMin(pArqEi, UltLido, &Ei);
      continue;
    }

    inserirArea(pArea, &UltLido, &NRArea);

    if (Ei - esquerda < direita - Es) {
      retiraMin(pArea, &registro, &NRArea);
      escreveMin(pArqEi, registro, &Ei);
      Linf = registro.nota;
    } else {
      retiraMax(pArea, &registro, &NRArea);
      escreveMax(pArqEi, registro, &Es);
      Lsup = registro.nota;
    }
  }

  while (Ei <= Es) {
    retiraMin(pArea, &registro, &NRArea);
    printf("Ei: %d\nEs: %d\nregistro.nota: %lf\n\n", Ei, Es, registro.nota);
    escreveMin(pArqEi, registro, &Ei);
  }
}

void quicksortExterno(FILE *pArqLi, FILE *pArqLs, FILE *pArqEi, FILE *pArqEs,
                      int esquerda, int direita) {
  int i, j;

  Area area;

  if (direita - esquerda < 1)
    return;

  FAVazia(&area);

  particao(pArqLi, pArqLs, pArqEi, pArqEs, &area, esquerda, direita, &i, &j);

  // Ordena primeiro o subarquivo menor
  if (i - esquerda < direita - j) {
    quicksortExterno(pArqLi, pArqLs, pArqEi, pArqEs, esquerda, i);
    quicksortExterno(pArqLi, pArqLs, pArqEi, pArqEs, j, direita);
  } else {
    quicksortExterno(pArqLi, pArqLs, pArqEi, pArqEs, j, direita);
    quicksortExterno(pArqLi, pArqLs, pArqEi, pArqEs, esquerda, i);
  }
}