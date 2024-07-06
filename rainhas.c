#include "rainhas.h"

#include <string.h>

void mostra_resposta(unsigned int n, unsigned int* r) {
	for (unsigned int i = 0; i < n; i++) {
		printf("%u ", r[i]);
	}

	printf("\n");
}

// verifica se a casa (lin, col) nao esta nas casas proibidas
int nao_proibido(unsigned int lin, unsigned int col, unsigned int k, casa* c) {
	for (unsigned int i = 0; i < k; i++) {
		if (c[i].linha == lin && c[i].coluna == col) {
			return 0;
		}
	}
	return 1;
}

// verifica se uma rainha na posicao (lin, col) nao ataca nenhuma das demais em r
int nao_ataca_demais(unsigned int lin, unsigned int col, unsigned int n, unsigned int* r) {
	unsigned int lin1;
	unsigned int col1;

	for (unsigned int i = 0; i < n; i++) {
		lin1 = i + 1;
		col1 = r[i];
		// nao estao na mesma coluna
		if (col1 == col) {
			return 0;
		}
		// nao estao na mesma diagonal
		if (col1 && abs((int)(col1 - col)) == abs((int)(lin1 - lin))) {
			return 0;
		}
	}
	return 1;
}

// Atualiza a melhor solução encontrada até agora
void atualiza_r(int* max_rainhas, int num_rainhas, unsigned int* r, unsigned int* r_atual, int n) {
	if (num_rainhas > *max_rainhas) {
		*max_rainhas = num_rainhas;
		memcpy(r, r_atual, n * sizeof(*r));
	}
}

// Função principal de resolução usando backtracking
int solve_rainhas_bt(unsigned int n, unsigned int k, casa* c, unsigned int* r,
					 unsigned int* r_atual, int num_rainhas, int* max_rainhas, int lin) {
	// Não vale a pena continuar
	if (num_rainhas + (n - lin) < *max_rainhas) {
		return 0;
	}

	// Se todas as linhas foram processadas, atualiza a melhor solução se necessário
	if (lin == n) {
		atualiza_r(max_rainhas, num_rainhas, r, r_atual, n);
		if (*max_rainhas == n) {
			return 1;
		}
		return 0;
	}

	// Tenta colocar uma rainha na linha atual
	for (int col = 1; col <= n; col++) {
		if (nao_proibido(lin + 1, col, k, c) && nao_ataca_demais(lin + 1, col, n, r_atual)) {
			r_atual[lin] = col;

			if (solve_rainhas_bt(n, k, c, r, r_atual, num_rainhas + 1, max_rainhas, lin + 1)) {
				return 1;
			}

			r_atual[lin] = 0;  // Backtrack
		}
	}

	// Tenta resolver sem colocar uma rainha na linha atual
	return solve_rainhas_bt(n, k, c, r, r_atual, num_rainhas, max_rainhas, lin + 1);
}

//------------------------------------------------------------------------------
// Computa uma resposta para a instância (n, c) do problema das n rainhas
// com casas proibidas usando backtracking.
//
// n é o tamanho (número de linhas/colunas) do tabuleiro
//
// c é um vetor de k 'struct casa' indicando as casas proibidas
//
// r é um vetor de n posições (já alocado) a ser preenchido com a resposta:
//   r[i] = j > 0 indica que a rainha da linha i+1 fica na coluna j;
//   r[i] = 0     indica que não há rainha nenhuma na linha i+1
//
// devolve r
//
unsigned int* rainhas_bt(unsigned int n, unsigned int k, casa* c, unsigned int* r) {
	unsigned int* r_atual = calloc(n, sizeof(*r_atual));
	int max_rainhas = 0;

	solve_rainhas_bt(n, k, c, r, r_atual, 0, &max_rainhas, 0);

	free(r_atual);
	return r;
}

//------------------------------------------------------------------------------
// computa uma resposta para a instância (n,c) do problema das n
// rainhas com casas proibidas usando a modelagem do problema como
// conjunto independente de um grafo
//
// n, c e r são como em rainhas_bt()

unsigned int* rainhas_ci(unsigned int n, unsigned int k, casa* c, unsigned int* r) {
	n = n;
	k = k;
	c = c;

	return r;
}
