# Trabalho-EDII: Codificação de Huffman de Palavras
 
Trabalho prático em dupla da disciplina **Estruturas de Dados II** (2026, 2º semestre), orientado pelo professor Francisco Assis da Silva.
 
O projeto implementa **compressão de texto com o algoritmo de Huffman**, usando **palavras e espaços em branco como símbolos** (e não caracteres isolados). Ele é dividido em dois programas em C, executados separadamente:
 
| Programa | Arquivo | O que faz |
|---|---|---|
| 1. Codificador | `codificador.c` | Lê um texto, conta as frequências, monta a árvore de Huffman, gera os códigos, grava a tabela em disco e codifica uma frase |
| 2. Decodificador | `decodificador.c` | Lê a tabela e a frase codificada gravadas pelo programa 1 e reconstrói a frase original |
 
---
 
## Sumário
 
- [Como o Huffman funciona](#como-o-huffman-funciona)
- [Por que palavras como símbolos?](#por-que-palavras-como-símbolos)
- [Visão geral do fluxo](#visão-geral-do-fluxo)
- [Programa 1: Codificador](#programa-1-codificador)
- [Programa 2: Decodificador](#programa-2-decodificador)
- [Formato dos arquivos gerados](#formato-dos-arquivos-gerados)
- [Estruturas de dados utilizadas](#estruturas-de-dados-utilizadas)
- [Como compilar e executar](#como-compilar-e-executar)
- [Estrutura do repositório](#estrutura-do-repositório)
- [Observações e limitações](#observações-e-limitações)
- [Autores](#autores)
---
 
## Como o Huffman funciona
 
A ideia central é atribuir **códigos curtos aos símbolos mais frequentes** e **códigos longos aos mais raros**. Em vez de usar um tamanho fixo para cada símbolo (como os 8 bits do ASCII), cada símbolo recebe uma sequência de bits de tamanho variável, e o texto inteiro ocupa menos espaço.
 
O algoritmo é guloso e funciona assim:
 
1. Conta-se a frequência de cada símbolo.
2. Cada símbolo vira uma árvore de um único nó (uma folha) com a sua frequência.
3. Repete-se até sobrar uma única árvore:
   - retiram-se as **duas árvores de menor frequência**;
   - cria-se um novo nó pai cuja frequência é a **soma** das duas e cujos filhos são essas duas árvores;
   - o novo nó volta para o conjunto de árvores.
4. A árvore que sobra é a árvore de Huffman. O código de cada símbolo é o caminho da **raiz até a sua folha**, onde **esquerda = `0`** e **direita = `1`**.
Como os símbolos ficam apenas nas folhas, **nenhum código é prefixo de outro**. Isso garante que a decodificação nunca é ambígua: basta percorrer a árvore bit a bit, e ao chegar numa folha o símbolo está identificado.
 
Exemplo ilustrativo (não é a saída real do programa):
 
```
Símbolos: A(5)  B(2)  C(1)  D(1)
 
Passo 1: junta C(1) + D(1)   -> nó(2)
Passo 2: junta B(2) + nó(2)  -> nó(4)
Passo 3: junta nó(4) + A(5)  -> raiz(9)
 
Códigos possíveis:  A = 1   B = 01   C = 000   D = 001
```
 
---
 
## Por que palavras como símbolos?
 
O Huffman "simples" usa caracteres como símbolos. Aqui foi usado o **Huffman de palavras**: cada símbolo é uma palavra do texto, e **cada espaço em branco também é um símbolo**. Isso costuma gerar taxas de compressão melhores em textos com muita repetição de palavras, e por isso o texto de entrada deve conter palavras repetidas.
 
Na árvore, as folhas guardam apenas um **código numérico (símbolo)**. A associação entre símbolo e palavra fica na tabela gravada em disco.
 
---
 
## Visão geral do fluxo
 
```
                     PROGRAMA 1 (codificador)
Texto.txt ──► lista de palavras/frequências ──► árvore de Huffman ──► códigos
                                                                         │
                  ┌──────────────────────────────────────────────────────┤
                  ▼                                                      ▼
          registros.bin                                   frase de teste ──► bits ──► frase.bin
     (símbolo, palavra, código)                                       (gravada byte a byte)
                  │                                                      │
                  └───────────────────────┬──────────────────────────────┘
                                          ▼
                     PROGRAMA 2 (decodificador)
          monta árvore pelos códigos ──► percorre os bits ──► frase decodificada
```
 
---
 
## Programa 1: Codificador
 
Arquivo: `codificador.c`
 
### 1. Leitura do texto e contagem de frequências
 
O texto é lido de um arquivo (`Texto.txt`) caractere por caractere. A leitura separa as **palavras** e identifica os **espaços em branco**; os separadores são o espaço, o ponto, a vírgula e as quebras de linha.
 
Cada palavra ou espaço encontrado é contabilizado numa **lista encadeada ordenada por frequência (ordem crescente)**:
 
- se a palavra já existe, sua frequência é incrementada e o nó é reposicionado na lista;
- se não existe, um novo registro é criado com frequência 1 e recebe um **símbolo numérico** único, atribuído na ordem de aparição.
### 2. Lista de registros
 
Cada nó da lista guarda:
 
| Campo | Descrição |
|---|---|
| `simbolo` | Código numérico que identifica a palavra |
| `palavra` | A palavra (ou `" "` para o espaço) |
| `freq` | Quantas vezes aparece no texto |
| `codigo` | Código de Huffman, como string de `'0'` e `'1'` |
 
### 3. Construção da árvore de Huffman
 
A árvore é construída usando **uma segunda lista encadeada ordenada**, que funciona como uma "floresta" de árvores:
 
1. Cada registro da lista de frequências vira uma folha (que guarda apenas frequência e símbolo) e é inserida ordenadamente na floresta.
2. Enquanto houver mais de uma árvore, removem-se as duas primeiras (as de menor frequência), cria-se o nó pai com a soma das frequências e ele é reinserido de forma ordenada.
3. A última árvore que sobra é a raiz.
### 4. Geração dos códigos
 
Um percurso recursivo na árvore monta o código de cada folha (`0` ao descer à esquerda, `1` ao descer à direita) e o copia para o registro correspondente na lista. Há um tratamento para o caso extremo de um único símbolo, que recebe o código `"0"`.
 
### 5. Exibição na tela
 
O programa mostra:
 
- **a lista de registros** (símbolo, palavra, frequência e código);
- **a árvore de Huffman em formato horizontal**, com o ramo da direita (`1`) no topo e o da esquerda (`0`) embaixo. As folhas aparecem como `[símbolo] f=frequência` e os nós internos como `(soma das frequências)`.
### 6. Gravação da tabela em disco
 
Os registros são gravados em **`registros.bin`** (arquivo binário). A **frequência não é gravada**, apenas símbolo, palavra e código.
 
### 7. Codificação de uma frase
 
Uma **frase diferente da usada na construção da árvore** (fixa no código, sem `scanf`/`gets`) é codificada:
 
1. A frase é separada em palavras e espaços, com a mesma regra da leitura do texto.
2. O código de cada palavra/espaço é buscado na lista e concatenado numa sequência de bits.
3. A sequência é empacotada em bytes e gravada em **`frase.bin`**. O empacotamento usa uma `union` que sobrepõe 8 campos de 1 bit a um `unsigned char`, permitindo montar cada byte bit a bit.
4. O arquivo guarda no início a **quantidade de bits válidos**, porque o último byte quase nunca é preenchido por completo.
---
 
## Programa 2: Decodificador
 
Arquivo: `decodificador.c`
 
### 1. Reconstrução da árvore
 
O programa lê o `registros.bin` e monta uma árvore binária a partir dos **códigos** gravados. Para cada registro, percorre-se o código bit a bit a partir da raiz (`0` vai para a esquerda, `1` para a direita), criando os nós que ainda não existem. O último nó do caminho passa a ser uma folha com o símbolo.
 
### 2. Leitura da frase codificada
 
O programa abre o `frase.bin`, lê a quantidade de bits válidos e depois os bytes, extraindo os bits na mesma ordem em que foram gravados (`b0` até `b7`).
 
### 3. Decodificação
 
Para cada bit, desce-se na árvore. Ao chegar numa **folha**:
 
1. o programa **abre o `registros.bin` e busca o registro com aquele símbolo**, obtendo a palavra (ou o espaço);
2. a palavra é concatenada na string da frase;
3. o percurso volta à raiz.
O processo termina ao consumir todos os bits válidos, ignorando os bits de preenchimento do último byte. Ao final, a **frase decodificada é exibida na tela**.
 
---
 
## Formato dos arquivos gerados
 
### `registros.bin`
 
Sequência de registros de tamanho fixo, sem cabeçalho:
 
```c
struct registro {
    int  simbolo;
    char palavra[30];
    char codigo[128];
};
```
 
> A struct `Registro` precisa ser **idêntica nos dois programas**; caso contrário, a leitura com `fread` fica desalinhada.
 
### `frase.bin`
 
```
[ int: quantidade de bits válidos ][ byte 1 ][ byte 2 ] ... [ byte N ]
```
 
Dentro de cada byte, o **primeiro bit da sequência** fica em `b0`, o segundo em `b1`, e assim por diante até `b7`. O último byte pode ter bits de preenchimento no final, que são descartados com base na quantidade de bits válidos.
 
---
 
## Estruturas de dados utilizadas
 
| Estrutura | Uso |
|---|---|
| Lista encadeada ordenada de registros (`Lista`) | Guarda símbolo, palavra, frequência e código; mantida em ordem crescente de frequência |
| Lista encadeada ordenada de árvores (`Lista_arvh`) | "Floresta" usada na construção da árvore de Huffman |
| Árvore binária (`Arvh`) | Árvore de Huffman, com nós `union` (frequência total para nós internos, frequência e símbolo para folhas) |
| `union byte` | Montagem e leitura de bytes bit a bit |
 
---
 
## Como compilar e executar
 
Requisitos: um compilador C (por exemplo, `gcc`).
 
```bash
# 1. Compilar os dois programas
gcc codificador.c -o codificador
gcc decodificador.c -o decodificador
 
# 2. Executar o codificador (gera registros.bin e frase.bin)
./codificador
 
# 3. Executar o decodificador na MESMA pasta (lê registros.bin e frase.bin)
./decodificador
```
 
Observações:
 
- Os programas **não pedem nenhuma entrada** do usuário: a frase a codificar está fixa no código, e basta executá-los para ver os resultados.
- O `codificador` lê o texto em `../Texto.txt`, ou seja, **uma pasta acima** da pasta de execução. Ajuste o caminho no `lerArquivo` se a organização do seu repositório for diferente.
- O `decodificador` precisa ser executado depois do `codificador` e **na mesma pasta**, pois depende dos arquivos que ele gera.
---
 
## Estrutura do repositório
 
```
Trabalho-EDII/
├── Texto.txt           # texto usado para montar a árvore (deve ter palavras repetidas)
├── codificador.c       # programa 1
├── decodificador.c     # programa 2
└── README.md
```
 
> Ajuste a árvore de pastas acima para refletir a organização real do seu repositório.
 
---
 
## Observações e limitações
 
- A frase a ser codificada deve conter **apenas palavras presentes no texto** usado para montar a árvore; palavras desconhecidas não têm código e são ignoradas (o programa avisa na tela).
- A frase de codificação deve ser **diferente** da usada na montagem da árvore.
- Palavras são limitadas a 29 caracteres e códigos a 127 bits (tamanho dos vetores das structs).
- A construção da árvore usa inserção ordenada em lista encadeada, então tem custo quadrático no número de símbolos distintos. Isso é adequado para o tamanho do trabalho, mas um heap seria mais eficiente para grandes volumes.
- A decodificação busca cada palavra diretamente no arquivo `registros.bin`, conforme pedido no enunciado, o que é mais lento do que manter a tabela em memória.
---
 
## Autores
 
<table>
  <tr>
    <td align="center">
      <a href="https://github.com/miguelFerreira0803">
        <img src="https://github.com/miguelFerreira0803.png" width="100" alt="Foto1"><br>
        <sub><b>Miguel Alves</b></sub>
      </a>
    </td>
    <td align="center">
      <a href="https://github.com/CaioBrito05">
        <img src="https://github.com/CaioBrito05.png" width="100" alt="Foto2"><br>
        <sub><b>Caio Brito</b></sub>
      </a>
    </td>
  </tr>
</table>


Disciplina: Estruturas de Dados II, 2026/2
Professor: Francisco Assis da Silva
