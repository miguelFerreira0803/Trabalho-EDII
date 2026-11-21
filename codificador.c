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
    union byte byte;
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
void insereArvoreH(Arvh **raiz, Arvh *subArvore);


char verificaArquivo(FILE *arq)
{
    if(arq == NULL)
        return 1;
    return 0;
}

char verificaMaiusculo(int c)
{
    if(c >= 65 && c <= 90)
        return 1;
    return 0;
}


void lerArquivo(Lista **lista)
{
    char palavra[100];
    int i = 0, simbolo = 0,caracter;
    Lista *aux, *novo;
    Lista **atual;
    FILE *arq = fopen("Texto.txt","r");
    if(verificaArquivo(arq))
    {   
        printf("Arquivo aberto");
        while(!feof(arq))
        {
            caracter = fgetc(arq);
            //Lê a palavra inteira

            if(verificaMaiusculo(caracter))
                caracter -= 32;

            if(caracter != EOF &&  caracter != ' ' && caracter != '.' && caracter != ',')
            {
                palavra[i] = caracter;
                i++;
            }
            else if(i > 0) //Faz a inserção na lista dinamica
            {
                palavra[i] = '\0';
                aux = buscarPalavra(*lista, palavra);

                if(aux != NULL)
                {
                    atual = lista;

                    while(*atual != aux)
                        atual = &(*atual) -> prox;
                    *atual = aux -> prox;

                    aux -> freq++;
                    aux -> prox = NULL;

                    inserirOrdenado(lista,aux);
                }
                else
                {
                    criaNoLista(&novo, simbolo, palavra);

                    if(novo != NULL)
                    {
                        inserirOrdenado(lista,novo);
                        simbolo++;
                    }
                }

                i = 0;
            }

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

// Como a lista em ordem crescente, os dois menores sempre serão os dois primeiros
Lista *menores(Lista *lista, Lista **menor, Lista **segMenor)
{
    *menor = *segMenor = NULL;

    if (lista != NULL && lista->prox != NULL)
    {
        *menor = lista;
        *segMenor = lista->prox;

        lista = lista->prox->prox;

        (*menor)->prox = NULL;
        (*segMenor)->prox = NULL;
    }

    return lista;
}

void exibirLista(Lista *lista)
{
    while (lista != NULL)
    {
        printf("Simbolo: %d\n", lista->simbolo);
        printf("Palavra: %s\n", lista->palavra);
        printf("Frequencia: %d\n", lista->freq);

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

void criarArvoreH(Arvh ** raiz, Lista *lista)
{
    Lista *menor, *segMenor;
    Arvh *novoMenor, *novoSegMenor, *arvore;
    Lista_arvh *raizes;
    while (lista != NULL && lista->prox != NULL)
    {
        lista = menores(lista, &menor, &segMenor);

        if (menor != NULL && segMenor != NULL)
        {
            criaNoArvoreFolha(&novoMenor, menor);
            criaNoArvoreFolha(&novoSegMenor, segMenor);
        }
        else
        {

        }
        
        criaNoArvore(&arvore, novoMenor, novoSegMenor);

        insereArvoreH(&*raiz, arvore);

    }
}

void insereArvoreH(Arvh ** raiz, Arvh *subArvore)
{
    
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

int main()
{
    Lista *lista = NULL;
    Arvh *arvh = init();

    printf("Codificacao\n");

    lerArquivo(&lista);
    exibirLista(lista);

    criarArvoreH(&arvh, lista);

    return 0;
}