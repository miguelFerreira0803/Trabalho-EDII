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
int lerBit(union byte u, int pos);
void buscarPalavraArquivo(int simbolo, char *palavra);
void decodificarFrase(Arvh *raiz, char *frase);

int ehFolha(Arvh *no)
{
    return no->esq == NULL && no->dir == NULL;
}

void criaNo(Arvh **no)
{
    *no = (Arvh *)malloc(sizeof(Arvh));

    (*no)->esq = (*no)->dir = NULL;
    (*no)->no.folha.freq = 0;
    (*no)->no.folha.simbolo = -1;
}

void inserirCodigo(Arvh *raiz, char *codigo, int simbolo)
{
    Arvh *atual = raiz, **prox;
    int i;

    for (i = 0; codigo[i] != '\0'; i++)
    {
        if (codigo[i] == '0')
            prox = &atual->esq;
        else
            prox = &atual->dir;

        if (*prox == NULL)
            criaNo(prox);

        atual = *prox;
    }

    atual->no.folha.simbolo = simbolo;
}

void montarArvore(Arvh **raiz)
{
    Registro reg;
    FILE *arq = fopen("registros.dat", "rb");

    if (arq != NULL)
    {
        criaNo(raiz);

        while (fread(&reg, sizeof(Registro), 1, arq) == 1)
            inserirCodigo(*raiz, reg.codigo, reg.simbolo);

        fclose(arq);
    }
    else
        printf("Erro ao abrir registros.dat\n");
}

void exibirArvore(Arvh *no, int nivel)
{
    int i;

    if (no != NULL)
    {
        exibirArvore(no->dir, nivel + 1);

        for (i = 0; i < nivel; i++)
            printf("      ");

        if (ehFolha(no))
            printf("[%d]\n", no->no.folha.simbolo);
        else
            printf("(*)\n");

        exibirArvore(no->esq, nivel + 1);
    }
}

int lerBit(union byte u, int pos)
{
    int bit;

    if (pos == 0)
        bit = u.bits.b0;
    else if (pos == 1)
        bit = u.bits.b1;
    else if (pos == 2)
        bit = u.bits.b2;
    else if (pos == 3)
        bit = u.bits.b3;
    else if (pos == 4)
        bit = u.bits.b4;
    else if (pos == 5)
        bit = u.bits.b5;
    else if (pos == 6)
        bit = u.bits.b6;
    else
        bit = u.bits.b7;

    return bit;
}

void buscarPalavraArquivo(int simbolo, char *palavra)
{
    Registro reg;
    int achou = 0;
    FILE *arq = fopen("registros.dat", "rb");

    palavra[0] = '\0';

    if (arq != NULL)
    {
        while (!achou && fread(&reg, sizeof(Registro), 1, arq) == 1)
        {
            if (reg.simbolo == simbolo)
            {
                strcpy(palavra, reg.palavra);
                achou = 1;
            }
        }
        fclose(arq);
    }
    else
        printf("Erro ao abrir registros.dat\n");
}

void decodificarFrase(Arvh *raiz, char *frase)
{
    union byte u;
    Arvh *atual = raiz;
    char palavra[30];
    int qtdBits, lidos = 0, i;
    FILE *arq = fopen("frase.dat", "rb");

    frase[0] = '\0';

    if (arq != NULL)
    {
        fread(&qtdBits, sizeof(int), 1, arq);

        while (lidos < qtdBits && fread(&u.num, sizeof(unsigned char), 1, arq) == 1)
        {
            i = 0;
            while (i < 8 && lidos < qtdBits)
            {
                if (lerBit(u, i) == 0)
                    atual = atual->esq;
                else
                    atual = atual->dir;

                if (ehFolha(atual))
                {
                    buscarPalavraArquivo(atual->no.folha.simbolo, palavra);
                    strcat(frase, palavra);
                    atual = raiz;
                }

                i++;
                lidos++;
            }
        }
        fclose(arq);
    }
    else
        printf("Erro ao abrir frase.dat\n");
}

int main()
{
    char frase[4096];
    Arvh *arvh = NULL;

    printf("Decodificacao\n");

    montarArvore(&arvh);

    printf("\nArvore montada pelos codigos (topo = direita/1, base = esquerda/0):\n\n");
    exibirArvore(arvh, 0);

    decodificarFrase(arvh, frase);
    printf("\nFrase decodificada: %s\n", frase);

    return 0;
}