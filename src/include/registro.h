#ifndef REGISTRO_H
#define REGISTRO_H

typedef struct {
  unsigned long int numero_inscricao;
  double nota;
  char estado[3];
  char cidade[51];
  char curso[31];
} Registro;

// Aloca uma quantidade "n" de registros.
Registro *alocarRegistros(unsigned int);

// Desaloca uma quantidade "n" de registros.
void desalocarRegistros(Registro *);


#endif // REGISTRO_H