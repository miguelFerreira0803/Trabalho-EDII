#include <stdio.h>
#include <string.h>
#include <stdlib.h>

union byte
{
    unsigned char b0 : 1;
    unsigned char b1 : 1;
    unsigned char b2 : 1;
    unsigned char b3 : 1;
    unsigned char b4 : 1;
    unsigned char b5 : 1;
    unsigned char b6 : 1;
    unsigned char b7 : 1;
};



struct lista
{
    int simbolo;
    char palavra[30];
    int freq;
    char codigo[128];
    struct lista *prox;
};
typedef struct lista Lista;

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

struct lista_arvh
{
    Arvh *raiz;
    struct lista_arvh *prox;
}; 
typedef struct lista_arvh Lista_arvh; 

void lerArquivo(Lista **lista);
void criaNoLista(Lista **novo, int simbolo, char palavra[30]);
Lista *buscarPalavra(Lista *lista, char palavra[30]);
void inserirOrdenado(Lista **lista, Lista *novo);
void exibirLista(Lista *lista);
Lista *menores(Lista *lista, Lista **menor, Lista **segMenor);
void criarArvoreH(Arvh **arvh, Lista *lista);
void criaNoArvoreFolha(Arvh **arvh, Lista *lista);
void criaNoArvore(Arvh **arvh, Arvh *menor, Arvh *segMenor);
Arvh *removeMenor(Lista_arvh **lista);
void insereArvoreH(Lista_arvh **raiz, Arvh *subArvore);
void contabilizar(Lista **lista, char *palavra, int *simbolo);
int ehFolha(Arvh *no);
void gerarCodigos(Arvh *no, Lista *lista, char *cod, int nivel);

char verificaMaiusculo(int c)
{
    if(c >= 65 && c <= 90)
        return 1;
    return 0;
}

void contabilizar(Lista **lista, char *palavra, int *simbolo)
{
    Lista *aux = buscarPalavra(*lista, palavra), *novo;
    Lista **atual;

    if(aux != NULL)
    {
        atual = lista;
        while(*atual != aux && (*atual) -> prox != NULL)
            atual = &(*atual)->prox;
        *atual = aux->prox;

        aux->freq++;
        aux->prox = NULL;
        inserirOrdenado(lista, aux);
    }
    else
    {
        criaNoLista(&novo, *simbolo, palavra);
        if(novo != NULL)
        {
            inserirOrdenado(lista, novo);
            (*simbolo)++;
        }
    }
}


void lerArquivo(Lista **lista)
{
    char palavra[30];
    int i = 0, simbolo = 0,caracter;
    FILE *arq = fopen("../Texto.txt","r");
    if(arq != NULL)
    {   
        printf("Arquivo aberto\n\n");
        while(!feof(arq))
        {
            caracter = fgetc(arq);
            //Lê a palavra inteira
            if(caracter != EOF)
            {
            
                if(caracter != EOF &&  caracter != ' ' && caracter != '.' && caracter != ',' && caracter != 10 && caracter != 13)
                {
                    palavra[i] = caracter;
                    i++;
                }
                else if(i > 0) //Faz a inserção na lista dinamica
                {
                    palavra[i] = '\0';
                    contabilizar(lista, palavra, &simbolo);
                    i = 0;
                }

                if(caracter == ' ')
                    contabilizar(lista," ", &simbolo);
            }
        }
        if (i > 0)
        {
            palavra[i] = '\0';
            contabilizar(lista, palavra, &simbolo);
        }   
        fclose(arq);
    }
    else
    {
        printf("Arquivo nulo");
    }
}

// Ordem Crescente
void inserirOrdenado(Lista **lista, Lista *novo)
{
    Lista *aux;
    if (*lista == NULL)
    {
        *lista = novo;
    }
    else
    {
        if (novo->freq < (*lista)->freq)
        {
            novo->prox = *lista;
            *lista = novo;
        }
        else
        {
            aux = *lista;

            while (aux->prox != NULL && novo->freq >= aux->prox->freq)
                aux = aux->prox;

            novo->prox = aux->prox;
            aux->prox = novo;
        }
    }
}

void exibirLista(Lista *lista)
{
    while (lista != NULL)
    {
        printf("Simbolo: %d\n", lista->simbolo);
        printf("Palavra: %s\n", lista->palavra);
        printf("Frequencia: %d\n", lista->freq);
        printf("Codigo: %s\n", lista ->codigo);

        lista = lista->prox;
    }
}

void criaNoLista(Lista **novo, int simbolo, char palavra[30])
{
    *novo = (Lista *)malloc(sizeof(Lista));

    (*novo)->simbolo = simbolo;
    strcpy((*novo)->palavra, palavra);
    (*novo)->freq = 1;
    (*novo)->prox = NULL;
}

Lista *buscarPalavra(Lista *lista, char palavra[30])
{
    while (lista != NULL)
    {
        if (strcmp(lista->palavra, palavra) == 0)
        {
            return lista;
        }

        lista = lista->prox;
    }

    return NULL;
}

void criarArvoreH(Arvh **raiz, Lista *lista)
{
    Lista_arvh *floresta = NULL;
    Arvh *folha, *menor, *segMenor, *pai;

    // 1. Cada símbolo vira uma folha na floresta
    while (lista != NULL)
    {
        criaNoArvoreFolha(&folha, lista);
        insereArvoreH(&floresta, folha);
        lista = lista->prox;
    }

    // 2. Junta as duas menores até sobrar uma
    while (floresta != NULL && floresta->prox != NULL)
    {
        menor = removeMenor(&floresta);
        segMenor = removeMenor(&floresta);

        criaNoArvore(&pai, menor, segMenor);
        insereArvoreH(&floresta, pai);
    }

    // 3. A que sobrou é a raiz
    if (floresta != NULL)
    {
        *raiz = floresta->raiz;
        free(floresta);
    }
}

void insereArvoreH(Lista_arvh **lista, Arvh *subArvore)
{
    Lista_arvh *novo = (Lista_arvh *)malloc(sizeof(Lista_arvh));
    novo->raiz = subArvore;
    novo->prox = NULL;

    // Ordem crescente pela frequência
    if (*lista == NULL || subArvore->no.freqTotal < (*lista)->raiz->no.freqTotal)
    {
        novo->prox = *lista;
        *lista = novo;
    }
    else
    {
        Lista_arvh *aux = *lista;
        while (aux->prox != NULL &&
               subArvore->no.freqTotal >= aux->prox->raiz->no.freqTotal)
            aux = aux->prox;

        novo->prox = aux->prox;
        aux->prox = novo;
    }
}

Arvh *removeMenor(Lista_arvh **lista)
{
    Lista_arvh *aux = *lista;
    Arvh *arv = aux->raiz;

    *lista = aux->prox;
    free(aux);

    return arv;
}

void criaNoArvoreFolha(Arvh **arvh, Lista *lista)
{
    *arvh = (Arvh *)malloc(sizeof(Arvh));

    (*arvh)->dir = (*arvh)->esq = NULL;

    (*arvh)->no.folha.freq = lista->freq;
    (*arvh)->no.folha.simbolo = lista->simbolo;
}

void criaNoArvore(Arvh **arvh, Arvh *menor, Arvh *segMenor)
{
    *arvh = (Arvh *)malloc(sizeof(Arvh));

    (*arvh)->esq = menor;
    (*arvh)->dir = segMenor;

    (*arvh)->no.freqTotal = menor->no.folha.freq + segMenor->no.folha.freq;
}

Arvh *init()
{
    Arvh *arvh = (Arvh *)malloc(sizeof(Arvh));

    arvh->dir = arvh->esq = NULL;

    return arvh;
}

int ehFolha(Arvh *no)
{
    return no->esq == NULL && no->dir == NULL;
}

void gerarCodigos(Arvh *no, Lista *lista, char *cod, int nivel)
{
    if (no == NULL)
        return;

    if (ehFolha(no))
    {
        cod[nivel] = '\0';

        Lista *aux = lista;
        while (aux != NULL && aux->simbolo != no->no.folha.simbolo)
            aux = aux->prox;

        if (aux != NULL)
        {
            if (nivel == 0)
                strcpy(aux->codigo, "0");
            else
                strcpy(aux->codigo, cod);
        }
    }
    else
    {
        cod[nivel] = '0';
        gerarCodigos(no->esq, lista, cod, nivel + 1);

        cod[nivel] = '1';
        gerarCodigos(no->dir, lista, cod, nivel + 1);
    }
}


int main()
{
    char cod[128];
    Lista *lista = NULL;
    Arvh *arvh = NULL;

    printf("Codificacao\n");

    lerArquivo(&lista);
    //exibirLista(lista);

    criarArvoreH(&arvh, lista);
    gerarCodigos(arvh, lista, cod, 0);
    exibirLista(lista);

    return 0;
}