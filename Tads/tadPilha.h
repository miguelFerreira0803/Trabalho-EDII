#define MAXPILHA 100
#include <stdio.h>
#include "tree.h"


struct TpPilha
{
    int topo;
    Tree *PILHA[MAXPILHA];
};
typedef struct TpPilha Pilha;

void Init(Pilha *P)
{
    P -> topo = -1;
}

void Push(Pilha *P, Tree *T)
{
    if(!Cheia(P -> topo))
        P -> PILHA[++P -> topo] = T;
}

Tree * Pop(Pilha *P)
{
    if(!Vazia(P -> topo))
        return P -> PILHA[P -> topo--];
    return NULL;
}

char Vazia(int topo)
{
    return topo == -1;
}

char Cheia(int topo)
{
    return topo == MAXPILHA - 1;
}