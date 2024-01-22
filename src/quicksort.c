#include <stdlib.h>

#include "include/registro.h"
#include "include/util.h"

#define TAM_MEMORIA_PRINCIPAL 10

typedef struct {
  Registro area_pivos[TAM_MEMORIA_PRINCIPAL];
  unsigned int n;
} Area;

// Seta o valor da variavel "n" da variavel de tipo "Area" para 0.
void inicializarArea(Area *area_pivos) { area_pivos->n = 0; }

void troca(Registro *registros, unsigned int i, unsigned int j) {
  Registro auxiliar;

  auxiliar = registros[i];
  registros[i] = registros[j];
  registros[j] = auxiliar;
}

// Le "qtde_registros" registros do arquivo, armazenando-os no vetor
// "area_pivos" e os ordenando dentro dele.
void ordenarEmMemoriaPrincipal(FILE **arqLi,
                               Registro area_pivos[TAM_MEMORIA_PRINCIPAL],
                               unsigned int qtde_registros) {
  fread(area_pivos, sizeof(Registro), qtde_registros, *arqLi);

  // Ordena em memoria interna pelo metodo Insertion Sort.
  for (unsigned int i = 0; i < qtde_registros - 1; i++)
    for (unsigned int j = i + 1; j > 0; j--)
      if (area_pivos[j].nota < area_pivos[j - 1].nota)
        troca(area_pivos, j, j - 1);
}

void printV(Registro *r, unsigned int n) {
  for (unsigned int i = 0; i < n; i++) {
    printf("i: %u\n", i);
    printf("r->cidade: %s\n", r[i].cidade);
    printf("r->curso: %s\n", r[i].curso);
    printf("r->estado: %s\n", r[i].estado);
    printf("r->nota: %lf\n", r[i].nota);
    printf("r->cidade: %lu\n\n", r[i].numero_inscricao);
  }
}

bool syncBuffer(FILE **arqLi, FILE **arqEi, FILE **arqLs, FILE **arqEs) {
  if (fflush(*arqLi) || fflush(*arqEi) || fflush(*arqLs) || fflush(*arqEs))
    return false;

  return true;
}

void leParticaoInferior(FILE **arqLi, Registro *registro_lido,
                        unsigned int *leitura_inferior) {
  fseek(*arqLi, (*leitura_inferior - 1) * sizeof(Registro), SEEK_SET);
  fread(registro_lido, sizeof(Registro), 1, *arqLi);
  (*leitura_inferior)++;
}

void escreveParticaoInferior(FILE **arqEi, Registro *registro_escrito,
                             unsigned int *escrita_inferior) {
  fseek(*arqEi, (*escrita_inferior - 1) * sizeof(Registro), SEEK_SET);
  fwrite(registro_escrito, sizeof(Registro), 1, *arqEi);
  (*escrita_inferior)++;
}

void leParticaoSuperior(FILE **arqLs, Registro *registro_lido,
                        unsigned int *leitura_superior) {
  fseek(*arqLs, (*leitura_superior - 1) * sizeof(Registro), SEEK_SET);
  fread(registro_lido, sizeof(Registro), 1, *arqLs);
  (*leitura_superior)--;
}

void escreveParticaoSuperior(FILE **arqEs, Registro *registro_escrito,
                             unsigned int *escrita_superior) {
  fseek(*arqEs, (*escrita_superior - 1) * sizeof(Registro), SEEK_SET);
  fwrite(registro_escrito, sizeof(Registro), 1, *arqEs);
  (*escrita_superior)--;
}

void inserirArea(Area *area, Registro *registro) {
  unsigned int i = area->n;

  while (i > 0 && registro->nota <= area->area_pivos[i - 1].nota) {
    area->area_pivos[i] = area->area_pivos[i - 1];
    i--;
  }

  area->area_pivos[i] = *registro;
  area->n++;
}

void retirarArea(Area *area, unsigned int posicao) {
  unsigned int i = posicao;

  area->n--;

  while (i < area->n) {
    area->area_pivos[i] = area->area_pivos[i + 1];
    i++;
  }
}

void particao(FILE **arqLi, FILE **arqEi, FILE **arqLs, FILE **arqEs,
              Area *area, unsigned int esq, unsigned int dir, int *i, int *j, Metrica *metricas) {
  unsigned int leitura_inferior;
  unsigned int escrita_inferior;
  unsigned int leitura_superior;
  unsigned int escrita_superior;
  double limite_inferior;
  double limite_superior;
  // Utilizada para saber se ira ler o item da particao inferior ou superior do
  // subarquivo.
  bool alternar;
  Registro ultimo_lido;

  leitura_inferior = escrita_inferior = esq;
  leitura_superior = escrita_superior = dir;
  limite_inferior = -1.0;
  limite_superior = 101.0;
  alternar = true;
  /*
    Os dois comandos abaixo fazem os apontadores de leitura e escrita inferior
    apontarem para o inicio do subarquivo.
  */
  fseek(*arqLi, (leitura_inferior - 1) * sizeof(Registro), SEEK_SET);
  fseek(*arqEi, (escrita_inferior - 1) * sizeof(Registro), SEEK_SET);

  *i = esq - 1;
  *j = dir + 1;

  /*
    Verifica se a quantidade de itens do subarquivo cabe em memoria principal.

    O "+1" na expressao eh para contabilizar o primeiro elemento da diferenca,
    ja que ele eh excluido por ela.
  */
  if ((dir - esq) + 1 <= TAM_MEMORIA_PRINCIPAL) {
    unsigned int qtde_registros_subarquivo;

    qtde_registros_subarquivo = (dir - esq) + 1;

    // Le diretamente do arquivo, armazenando e ordenando os itens lidos dentro
    // da area dos pivos.
    ordenarEmMemoriaPrincipal(arqLi, area->area_pivos,
                              qtde_registros_subarquivo);
    // Escreve os registros ordenados no inicio do subarquivo.
    fwrite(area->area_pivos, sizeof(Registro), qtde_registros_subarquivo,
           *arqEi);
    // Sincroniza os buffers de cada apontador do arquivo binario.
    if (syncBuffer(arqLi, arqEi, arqLs, arqEs) == false)
      exit(1);

    *i = esq;
    *j = dir;

    return;
  }

  while (leitura_inferior <= leitura_superior) {
    if (area->n < TAM_MEMORIA_PRINCIPAL - 1) {
      if (alternar) {
        leParticaoInferior(arqLi, &ultimo_lido, &leitura_inferior);
        metricas->n_leitura++;
        alternar = false;
      } else {
        leParticaoSuperior(arqLs, &ultimo_lido, &leitura_superior);
        metricas->n_leitura++;
        alternar = true;
      }

      inserirArea(area, &ultimo_lido);
    } else {
      if (leitura_inferior == escrita_inferior) {
        leParticaoInferior(arqLi, &ultimo_lido, &leitura_inferior);
        metricas->n_leitura++;
        alternar = false;
      } else if (leitura_superior == escrita_superior) {
        leParticaoSuperior(arqLs, &ultimo_lido, &leitura_superior);
        metricas->n_leitura++;
        alternar = true;
      } else if (alternar) {
        leParticaoInferior(arqLi, &ultimo_lido, &leitura_inferior);
        metricas->n_leitura++;
        alternar = false;
      } else {
        leParticaoSuperior(arqLs, &ultimo_lido, &leitura_superior);
        metricas->n_leitura++;
        alternar = true;
      }

      metricas->n_comparacao++;
      if (ultimo_lido.nota < limite_inferior) {
        *i = escrita_inferior;
        escreveParticaoInferior(arqEi, &ultimo_lido, &escrita_inferior);

        if (syncBuffer(arqLi, arqEi, arqLs, arqEs) == false)
          exit(1);
      } else if (ultimo_lido.nota > limite_superior) {
        *j = escrita_superior;
        escreveParticaoSuperior(arqEs, &ultimo_lido, &escrita_superior);

        if (syncBuffer(arqLi, arqEi, arqLs, arqEs) == false)
          exit(1);
      } else {
        inserirArea(area, &ultimo_lido);

        if (escrita_inferior - esq <= dir - escrita_superior) {
          limite_inferior = area->area_pivos[0].nota;
          escreveParticaoInferior(arqEi, &area->area_pivos[0],
                                  &escrita_inferior);
          retirarArea(area, 0);

          if (syncBuffer(arqLi, arqEi, arqLs, arqEs) == false)
            exit(1);
        } else {
          limite_superior = area->area_pivos[area->n - 1].nota;
          escreveParticaoSuperior(arqEs, &area->area_pivos[area->n - 1],
                                  &escrita_superior);
          retirarArea(area, area->n - 1);

          if (syncBuffer(arqLi, arqEi, arqLs, arqEs) == false)
            exit(1);
        }
      }
    }
  }

  while (escrita_inferior <= escrita_superior) {
    escreveParticaoInferior(arqEi, &area->area_pivos[0], &escrita_inferior);
    retirarArea(area, 0);

    if (syncBuffer(arqLi, arqEi, arqLs, arqEs) == false)
      exit(1);
  }
}

void ordena(FILE **arqLi, FILE **arqEi, FILE **arqLs, FILE **arqEs, int esq,
            int dir, Metrica *metricas) {
  // Delimita, como um indice do arquivo, o termino da particao inferior.
  int i;
  // Delimita, como um indice do arquivo, o comeco da particao superior.
  int j;
  Area area_pivos;

  // Verifica se na particao ha somente um elemento. Se sim, ja esta ordenado.
  if (dir - esq < 1)
    return;

  inicializarArea(&area_pivos);

  particao(arqLi, arqEi, arqLs, arqEs, &area_pivos, esq, dir, &i, &j, metricas);

  /*
    Se o tamanho da particao inferior for menor ou igual ao tamanho da particao
    superior, ordena ela primeiro.

    Caso contrario, ordena a particao superior primeiro.
  */
  if (i - esq <= dir - j) {
    ordena(arqLi, arqEi, arqLs, arqEs, esq, i, metricas);
    ordena(arqLi, arqEi, arqLs, arqEs, j, dir, metricas);
  } else {
    ordena(arqLi, arqEi, arqLs, arqEs, j, dir, metricas);
    ordena(arqLi, arqEi, arqLs, arqEs, esq, i, metricas);
  }
}

bool quickSortExterno(unsigned int qtde_registros, Metrica *metricas) {
  // Ponteiro de leitura na particao inferior do arquivo "PROVAO.bin"
  FILE *arqLi;
  // Ponteiro de escrita na particao inferior do arquivo "PROVAO.bin"
  FILE *arqEi;
  // Ponteiro de leitura na particao superior do arquivo "PROVAO.bin"
  FILE *arqLs;
  // Ponteiro de escrita na particao superior do arquivo "PROVAO.bin"
  FILE *arqEs;

  // Fluxo condicional que verifica se foi possivel abrir todos os arquivos.
  if ((arqLi = fopen("PROVAO.bin", "rb")) == NULL)
    return false;
  else if ((arqEi = fopen("PROVAO.bin", "r+b")) == NULL) {
    fclose(arqLi);
    return false;
  } else if ((arqLs = fopen("PROVAO.bin", "rb")) == NULL) {
    fclose(arqLi);
    fclose(arqEi);
    return false;
  } else if ((arqEs = fopen("PROVAO.bin", "r+b")) == NULL) {
    fclose(arqLi);
    fclose(arqEi);
    fclose(arqLs);
    return false;
  }

  ordena(&arqLi, &arqEi, &arqLs, &arqEs, 1, qtde_registros, metricas);

  fclose(arqLi);
  fclose(arqEi);
  fclose(arqLs);
  fclose(arqEs);

  return true;
}