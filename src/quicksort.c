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
    memset(pArea->vetor[i].estado, '\0', 3);
    memset(pArea->vetor[i].cidade, '\0', 51);
    memset(pArea->vetor[i].curso, '\0', 31);
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

void inserirArea(Area *pArea, Registro *pUltLido, int *pNRArea) {
  // insere o registro 'pUltLido' de forma ordenada na área
  insereItem(*pUltLido, pArea);
  *pNRArea = pArea->tamOcupado;
}

void escreveMax(FILE *pArqEs, Registro *pRegistro, int *pEs) {
  fseek(pArqEs, (*pEs - 1) * sizeof(Registro), SEEK_SET);
  fwrite(pRegistro, sizeof(Registro), 1, pArqEs);
  (*pEs)--;
}

void escreveMin(FILE *pArqEi, Registro *pRegistro, int *pEi) {
  fwrite(pRegistro, sizeof(Registro), 1, pArqEi);
  (*pEi)++;
}

void retiraMax(Area *pArea, Registro *pRegistro, int *pNRArea) {
  retiraUltimo(pArea, pRegistro);
  *pNRArea = pArea->tamOcupado;
}

void retiraMin(Area *pArea, Registro *pRegistro, int *pNRArea) {
  retiraPrimeiro(pArea, pRegistro);
  *pNRArea = pArea->tamOcupado;
}

void particao(FILE *pArqLi, FILE *pArqLs, FILE *pArqEi, FILE *pArqEs,
              Area *pArea, int esquerda, int direita, int *pI, int *pJ) {
  int Ls, Es = direita;
  int Li, Ei = esquerda;

  int NRArea = 0;
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
      escreveMax(pArqEs, &UltLido, &Es);
      continue;
    }

    if (UltLido.nota < Linf) {
      *pI = Ei;
      escreveMin(pArqEi, &UltLido, &Ei);
    }

    inserirArea(pArea, &UltLido, &NRArea);

    if (Ei - esquerda < direita - Es) {
      retiraMin(pArea, &registro, &NRArea);
      escreveMin(pArqEi, &registro, &Ei);
      Linf = registro.nota;
    } else {
      retiraMax(pArea, &registro, &NRArea);
      escreveMax(pArqEi, &registro, &Es);
      Lsup = registro.nota;
    }
  }

  while (Ei <= Es) {
    retiraMin(pArea, &registro, &NRArea);
    escreveMin(pArqEi, &registro, &Ei);
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
