#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

typedef struct entrada {
    short int metodo;
    unsigned int qtde_registros;
    short int situacao;
    char opcional[3];
} Entrada;

typedef struct registro {
    unsigned long int numero_inscricao;
    double nota;
    char estado[3];
    char cidade[51];
    char curso[31];
} Registro;

// Aceita ate 512KB por pagina, caso necessario realizar paginacao
static unsigned long int ITENS_POR_PAGINA = (512 * 1024) / sizeof(Registro);

// Le "n" linhas do arquivo "PROVAO.TXT"
void lerNElementosDoArquivo(FILE *arquivo, Registro registros[], unsigned int n)
{
    for(int i = 0 ; i < n ; i++)
    {
        if(fscanf(arquivo, "%8lu %5lf %2s %50[A-Z ] %30s", &registros[i].numero_inscricao, &registros[i].nota, registros[i].estado, registros[i].cidade, registros[i].curso) == EOF)
            break;

        // Passa para a proxima linha do arquivo
        while(fgetc(arquivo) != '\n');
    }
}

// Aloca uma quantidade "n" de registros.
Registro* alocarRegistros(unsigned int n)
{
    Registro *registros = (Registro*) malloc(n * sizeof(Registro));

    return registros;
}

// Converte o arquivo texto "PROVAO.TXT" para um arquivo equivalente em binario.
FILE* converterParaBinario(FILE *arquivo, unsigned int qtde_registros_arquivo)
{
    Registro *pagina;
    FILE *arquivo_binario;

    if((arquivo_binario = fopen("PROVAO.bin", "w+b")) == NULL)
        return NULL;

    // Eh possivel alocar uma pagina que possua "ITENS_POR_PAGINA" tamanho?
    if((pagina = alocarRegistros(ITENS_POR_PAGINA)) == NULL)
        return NULL;

    /* 
        Se a quantidade de registros for maior do que a pagina pode suportar,
        realiza o processo de paginacao para coletar "ITENS_POR_PAGINA" itens
        de cada vez e escrever no arquivo binario.
    */
    while(qtde_registros_arquivo > ITENS_POR_PAGINA)
    {
        lerNElementosDoArquivo(arquivo, pagina, ITENS_POR_PAGINA);
        fwrite(pagina, sizeof(Registro), ITENS_POR_PAGINA, arquivo_binario);
        qtde_registros_arquivo -= ITENS_POR_PAGINA;
    }

    lerNElementosDoArquivo(arquivo, pagina, qtde_registros_arquivo);

    fwrite(pagina, sizeof(Registro), qtde_registros_arquivo, arquivo_binario);

    return arquivo_binario;
}

// Le e verifica se a entrada passada pelo terminal eh valida.
bool lerEntrada(Entrada *entrada, int argc, char* argv[])
{
    if(argc < 4 || argc > 5)
        return false;

    entrada->metodo = atoi(argv[1]);
    entrada->qtde_registros = atoi(argv[2]);
    entrada->situacao = atoi(argv[3]);

    if(argc == 5)
    {
        strcpy(entrada->opcional, argv[4]);

        // Converte para LOWERCASE de modo a facilitar a comparacao futura
        for(int i = 0 ; entrada->opcional[i] != '\0' ; i++)
            entrada->opcional[i] = tolower(entrada->opcional[i]);

        // Se o argumento opcional for diferente de -P, entao retorna falso
        if(strcmp(entrada->opcional, "-p"))
            return false;
    }
    else
        entrada->opcional[0] = '\0';

    if(entrada->metodo < 1 || entrada->metodo > 3 || entrada->situacao < 1 || entrada->situacao > 3)
        return false;

    return true;
}

int main(int argc, char *argv[])
{
    FILE *arquivo;
    FILE *arquivo_binario;
    Entrada entrada;

    // Le entrada enquanto verifica se eh valida. Retorna "true" se for valida e "false" caso contrario.
    if(lerEntrada(&entrada, argc, argv) == false)
    {
        printf("O modo de executar o programa esta errado ou algum argumento inserido nao e valido, execute de acordo:\n\n");
        printf("%s <metodo> <quantidade> <situacao> [-P|-p]\n", argv[0]);
        exit(1);
    }

    // Tenta abrir o arquivo "PROVAO.TXT" disponibilizado pelo docente.
    if((arquivo = fopen("PROVAO.TXT", "r+")) == NULL)
    {
        printf("Erro ao tentar abrir o arquivo de texto plano PROVAO.TXT! Abortando o programa...\n");
        exit(1);
    }

    /*
        Cria uma versao binaria do arquivo de texto plano "PROVAO.TXT" cuja
        quantidade total de registros eh a quantidade passada por argumento
        para o programa.
    */
    if((arquivo_binario = converterParaBinario(arquivo, entrada.qtde_registros)) == NULL)
    {
        printf("Erro ao tentar abrir o arquivo binario ou falha na alocacao dinamica de memoria! Abortando o programa...\n");
        exit(1);
    }

    return 0;
}