#include "rainhas.h"

// verifica se a casa (lin,col) nao esta nas casas proibidas
int nao_proibido(unsigned int lin, unsigned int col, unsigned int k, casa *c) {
  for (unsigned int i = 0; i < k; i++) {
    if (c[i].linha == lin && c[i].coluna == col) { return 0; }
  }
  return 1;
}


// verifica se uma rainha na posicao (lin,col) nao ataca nenhuma das demais em r
int nao_ataca_demais(unsigned int lin, unsigned int col, unsigned int n, unsigned int *r) {
  unsigned int lin1;
  unsigned int col1;

  for (unsigned int i = 0; i < n, r[i] != 0; i++) {
    lin1 = i + 1;
    col1 = r[i];
    // nao estao na mesma coluna
    if (col1 == col) { return 0; }
    // nao estao na mesma diagonal
    if (abs(col1 - col) == abs(lin1 - lin)) { return 0; }
  }
  return 1;
}


//------------------------------------------------------------------------------
// computa uma resposta para a instância (n,c) do problema das n rainhas 
// com casas proibidas usando backtracking
//
//    n é o tamanho (número de linhas/colunas) do tabuleiro
//
//    c é um vetor de k 'struct casa' indicando as casas proibidas
//
//    r é um vetor de n posições (já alocado) a ser preenchido com a resposta:
//      r[i] = j > 0 indica que a rainha da linha i+1 fica na coluna j;
//      r[i] = 0     indica que não há rainha nenhuma na linha i+1
//
// devolve r

unsigned int *rainhas_bt(unsigned int n, unsigned int k, casa *c, unsigned int *r) {

  // se a ultima rainha já foi posta, achou uma solucao
  if (r[n-1] != 0) {
    return r;
  }

  // achar primeira linha sem rainha
  unsigned int lin = 0;
  while (r[lin] != 0) { lin++; }

  for (unsigned int col = 1; col <= n; col++) {
    if (nao_proibido(lin + 1, col, k, c) && nao_ataca_demais(lin + 1, col, n, r)) {
      r[lin] = col;
      if (rainhas_bt(n, k, c, r)) {
        return r;
      }
      r[lin] = 0;
    }
  }
  return NULL;
}
//------------------------------------------------------------------------------
// computa uma resposta para a instância (n,c) do problema das n
// rainhas com casas proibidas usando a modelagem do problema como
// conjunto independente de um grafo
//
// n, c e r são como em rainhas_bt()

unsigned int *rainhas_ci(unsigned int n, unsigned int k, casa *c, unsigned int *r) {

  n = n;
  k = k;
  c = c;

  return r;
}
