#include <stdio.h>
#include <string.h>
#include <stdlib.h>

union byte
{
    struct
    {
        unsigned char b0 : 1;
        unsigned char b1 : 1;
        unsigned char b2 : 1;
        unsigned char b3 : 1;
        unsigned char b4 : 1;
        unsigned char b5 : 1;
        unsigned char b6 : 1;
        unsigned char b7 : 1;
    } bits;
    unsigned char num;
};

struct registro
{
    int simbolo;
    char palavra[30];
    char codigo[128];
};
typedef struct registro Registro;

struct folha
{
    int freq, simbolo;
};

union no
{
    int freqTotal;
    struct folha folha;
};

struct arvh
{
    struct arvh *esq, *dir;
    union no no;
};
typedef struct arvh Arvh;

void criaNo(Arvh **no);
int ehFolha(Arvh *no);
void inserirCodigo(Arvh *raiz, char *codigo, int simbolo);
void montarArvore(Arvh **raiz);
void exibirArvore(Arvh *no, int nivel);