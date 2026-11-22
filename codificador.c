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
int insereArvoreH(Lista_arvh **lista, Arvh *subArvore);
void contabilizar(Lista **lista, char *palavra, int *simbolo);

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

void criarArvoreH(Arvh **raiz, Lista *lista)
{
    Lista_arvh *fila = NULL;
    Arvh *esq, *dir, *pai;
    int continuar = 1;

    *raiz = NULL;

    // Cria uma folha para cada palavra/símbolo da lista de frequências.
    while (lista != NULL && continuar)
    {
        Arvh *folha = NULL;
        Lista *proximo = lista->prox;

        criaNoArvoreFolha(&folha, lista);
        if (folha == NULL)
            continuar = 0;
        else
        {
            if (insereArvoreH(&fila, folha))
                lista = proximo;
            else
            {
                free(folha);
                continuar = 0;
            }
        }
    }

    // Junta sempre as duas subárvores de menor frequência.
    while (fila != NULL && fila->prox != NULL && continuar)
    {
        Lista_arvh *remover = fila;
        esq = remover->raiz;
        fila = remover->prox;
        free(remover);

        remover = fila;
        dir = remover->raiz;
        fila = remover->prox;
        free(remover);

        pai = NULL;
        criaNoArvore(&pai, esq, dir);
        if (pai == NULL)
            continuar = 0;
        else if (!insereArvoreH(&fila, pai))
        {
            free(pai);
            continuar = 0;
        }
    }

    if (continuar && fila != NULL)
    {
        *raiz = fila->raiz;
        free(fila);
    }

    // Se a construção falhou, libera os nós da fila que ainda não foram unidos.
    if (!continuar)
    {
        while (fila != NULL)
        {
            Lista_arvh *remover = fila;
            fila = fila->prox;
            free(remover->raiz);
            free(remover);
        }
    }
}

int insereArvoreH(Lista_arvh **lista, Arvh *subArvore)
{
    Lista_arvh *novo, **atual;
    int inserido = 0;

    if (lista != NULL && subArvore != NULL)
    {
        novo = (Lista_arvh *)malloc(sizeof(Lista_arvh));
        if (novo != NULL)
        {
            novo->raiz = subArvore;
            novo->prox = NULL;

            // Insere em ordem crescente de frequência.
            atual = lista;
            while (*atual != NULL &&
                   (*atual)->raiz->no.freqTotal <= subArvore->no.freqTotal)
                atual = &(*atual)->prox;

            novo->prox = *atual;
            *atual = novo;
            inserido = 1;
        }
    }

    return inserido;
}

void criaNoArvoreFolha(Arvh **arvh, Lista *lista)
{
    *arvh = (Arvh *)malloc(sizeof(Arvh));
    if (*arvh != NULL)
    {
        (*arvh)->dir = (*arvh)->esq = NULL;

        (*arvh)->no.folha.freq = lista->freq;
        (*arvh)->no.folha.simbolo = lista->simbolo;
    }
}

void criaNoArvore(Arvh **arvh, Arvh *menor, Arvh *segMenor)
{
    *arvh = (Arvh *)malloc(sizeof(Arvh));
    if (*arvh != NULL)
    {
        (*arvh)->esq = menor;
        (*arvh)->dir = segMenor;

        (*arvh)->no.freqTotal = menor->no.freqTotal + segMenor->no.freqTotal;
    }
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
    Arvh *arvh = NULL;

    printf("Codificacao\n");

    lerArquivo(&lista);
    exibirLista(lista);

    criarArvoreH(&arvh, lista);

    return 0;
}
