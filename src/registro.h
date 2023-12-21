#ifndef REGISTRO_H
#define REGISTRO_H

#define FITAS 10
#define DOBRO (FITAS * 2)

typedef struct {
    long int inscricao;
    float nota;
    char estado[3];
    char cidade[51];
    char curso[31];
} Registro;

#endif