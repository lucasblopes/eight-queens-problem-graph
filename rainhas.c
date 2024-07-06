#include "rainhas.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void mostra_resposta(unsigned int n, unsigned int *r) {
	for (unsigned int i = 0; i < n; i++) {
		printf("%u ", r[i]);
	}
	printf("\n");
}

// Verifica se a casa (lin, col) não está nas casas proibidas
int nao_proibido(unsigned int lin, unsigned int col, unsigned int k, casa *c) {
	for (unsigned int i = 0; i < k; i++) {
		if (c[i].linha == lin && c[i].coluna == col) {
			return 0;
		}
	}
	return 1;
}

// Verifica se uma rainha na posição (lin, col) não ataca nenhuma das demais em r
int nao_ataca_demais(unsigned int lin, unsigned int col, unsigned int n, unsigned int *r) {
	unsigned int lin1;
	unsigned int col1;

	for (unsigned int i = 0; i < n; i++) {
		lin1 = i + 1;
		col1 = r[i];
		// Não estão na mesma coluna
		if (col1 == col) {
			return 0;
		}
		// Não estão na mesma diagonal
		if (col1 && abs((int)(col1 - col)) == abs((int)(lin1 - lin))) {
			return 0;
		}
	}
	return 1;
}

// Atualiza a melhor solução encontrada até agora
void atualiza_r(int *max_rainhas, int num_rainhas, unsigned int *r, unsigned int *r_atual, int n) {
	if (num_rainhas > *max_rainhas) {
		*max_rainhas = num_rainhas;
		memcpy(r, r_atual, n * sizeof(*r));
	}
}

// Função principal de resolução usando backtracking
int solve_rainhas_bt(unsigned int n, unsigned int k, casa *c, unsigned int *r,
					 unsigned int *r_atual, int num_rainhas, int *max_rainhas, int lin) {
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
unsigned int *rainhas_bt(unsigned int n, unsigned int k, casa *c, unsigned int *r) {
	unsigned int *r_atual = calloc(n, sizeof(*r_atual));
	int max_rainhas = 0;

	solve_rainhas_bt(n, k, c, r, r_atual, 0, &max_rainhas, 0);

	free(r_atual);
	return r;
}

//------------------------------------------------------------------------------
// Computa uma resposta para a instância (n, c) do problema das n
// rainhas com casas proibidas usando a modelagem do problema como
// conjunto independente de um grafo.
//
// n, c e r são como em rainhas_bt()

grafo monta_grafo_restricoes(unsigned int n, unsigned int k, casa *c) {
	grafo g;
	g.tam = n * n;
	g.tamAtivo = g.tam - k;
	g.matriz = calloc(g.tam, sizeof(int *));
	for (int i = 0; i < g.tam; i++) {
		g.matriz[i] = calloc(g.tam, sizeof(int));
	}

	for (int i = 0; i < k; i++) {
		unsigned int indexVertice = (n * (c[i].linha - 1)) + (c[i].coluna - 1);
		for (int j = 0; j < g.tam; j++) {
			g.matriz[indexVertice][j] = -1;
			g.matriz[j][indexVertice] = -1;
		}
	}

	for (int i = 0; i < g.tam; i++) {
		if (g.matriz[i][i] == -1) continue;

		for (int j = i + 1; j % n != 0; j++) {
			if (g.matriz[i][j] == -1) continue;
			g.matriz[i][j] = 1;
			g.matriz[j][i] = 1;
		}

		for (int j = i + n; j < g.tam; j += n) {
			if (g.matriz[i][j] == -1) continue;
			g.matriz[i][j] = 1;
			g.matriz[j][i] = 1;
		}

		for (int j = (i + n + 1); j < g.tam; j += (n + 1)) {
			if (g.matriz[i][j] == -1) continue;
			g.matriz[i][j] = 1;
			g.matriz[j][i] = 1;
		}

		for (int j = (i + n - 1); (j % n) != (n - 1) && j < g.tam; j += (n - 1)) {
			if (g.matriz[i][j] == -1) continue;
			g.matriz[i][j] = 1;
			g.matriz[j][i] = 1;
		}
	}

	return g;
}

void printa_restricoes(grafo G) {
	printf("\n==== RESTRICOES DO GRAFO ====\n");
	for (int i = 0; i < G.tam; i++) {
		printf("%d ", i);
		if (G.matriz[i][i] == -1) {
			printf("Probido \n");
			continue;
		}
		printf("Ataca: ");
		for (int j = 0; j < G.tam; j++) {
			if (G.matriz[i][j] == 1) printf("%d ", j);
		}
		printf("\n");
	}
	printf("===\n");
}

void copia_solucao(unsigned int *destino, unsigned int *origem, unsigned int tamanho) {
	for (unsigned int i = 0; i < tamanho; i++) {
		destino[i] = origem[i];
	}
}

unsigned int *ConjIndep(grafo G, unsigned int n, unsigned int *I, unsigned int *C,
						unsigned int tamI, unsigned int tamC, unsigned int *melhorSolucao,
						unsigned int *tamMelhorSolucao) {
	if (tamI > *tamMelhorSolucao) {
		copia_solucao(melhorSolucao, I, tamI);
		*tamMelhorSolucao = tamI;
	}

	if (tamI == n) {
		return I;
	}

	if (tamI + tamC <= *tamMelhorSolucao) {
		return NULL;
	}

	unsigned int v = C[tamC - 1];
	unsigned int *C2 = malloc(tamC * sizeof(unsigned int));
	unsigned int tamC2 = 0;
	for (unsigned int i = 0; i < tamC; i++) {
		if (i != tamC - 1 && G.matriz[v][C[i]] != 1) {
			C2[tamC2++] = C[i];
		}
	}

	I[tamI] = v;
	unsigned int *R = ConjIndep(G, n, I, C2, tamI + 1, tamC2, melhorSolucao, tamMelhorSolucao);
	if (R != NULL) {
		free(C2);
		return R;
	}

	R = ConjIndep(G, n, I, C, tamI, tamC - 1, melhorSolucao, tamMelhorSolucao);
	free(C2);
	return R;
}

unsigned int *rainhas_ci(unsigned int n, unsigned int k, casa *c, unsigned int *r) {
	memset(r, 0, n * sizeof *r);
	grafo G = monta_grafo_restricoes(n, k, c);

	unsigned int *I = malloc(n * sizeof(unsigned int));
	unsigned int *C = malloc(G.tamAtivo * sizeof(unsigned int));
	unsigned int tamI = 0;
	unsigned int tamC = 0;

	for (unsigned int i = 0; i < G.tam; i++) {
		if (G.matriz[i][i] != -1) {
			C[tamC++] = i;
		}
	}

	unsigned int *melhorSolucao = malloc(n * sizeof(unsigned int));
	unsigned int tamMelhorSolucao = 0;

	ConjIndep(G, n, I, C, tamI, tamC, melhorSolucao, &tamMelhorSolucao);

	free(I);
	free(C);

	for (unsigned int i = 0; i < tamMelhorSolucao; i++) {
		unsigned int linha = melhorSolucao[i] / n + 1;
		unsigned int coluna = melhorSolucao[i] % n + 1;
		r[linha - 1] = coluna;
	}

	free(melhorSolucao);
	return r;
}
