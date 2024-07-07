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

/* ----- grafos ------ */

grafo monta_grafo_restricoes(unsigned int n, unsigned int k, casa *c) {
	// criando grafo
	struct grafo g;
	g.tam = n * n;
	g.tamAtivo = g.tam - k;
	g.matriz = calloc(g.tam, sizeof(int *));
	for (int i = 0; i < g.tam; i++) {
		g.matriz[i] = calloc(g.tam, sizeof(int));
	}

	// removendo casas proibidas
	for (int i = 0; i < k; i++) {
		unsigned int indexVertice = (n * (c[i].linha - 1)) + (c[i].coluna - 1);
		for (int j = 0; j < g.tam; j++) {
			g.matriz[indexVertice][j] = -1;
			g.matriz[j][indexVertice] = -1;
		}
	}

	// adicionando restricoes
	for (int i = 0; i < g.tam; i++) {
		if (g.matriz[i][i] == -1) continue;

		// mesma linha (para a direita)
		for (int j = i + 1; j % n != 0; j++) {
			if (g.matriz[i][j] == -1) continue;
			g.matriz[i][j] = 1;
			g.matriz[j][i] = 1;
		}

		// mesma coluna (para baixo)
		for (int j = i + n; j < g.tam; j += n) {
			if (g.matriz[i][j] == -1) continue;
			g.matriz[i][j] = 1;
			g.matriz[j][i] = 1;
		}

		// mesma diagonal principal (para baixo)
		for (int j = (i + n + 1); j < g.tam; j += (n + 1)) {
			if (g.matriz[i][j] == -1) continue;
			g.matriz[i][j] = 1;
			g.matriz[j][i] = 1;
		}

		// mesma diagonal secundaria (para baixo)
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
		printf("%d ", i + 1);
		if (G.matriz[i][i] == -1) {
			printf("Proibido \n");
			continue;
		}
		printf("Ataca: ");
		for (int j = 0; j < G.tam; j++) {
			if (G.matriz[i][j] == 1) printf("%d ", j + 1);
		}
		printf("\n");
	}
	printf("===\n");
}

//------------------------------------------------------------------------------
// computa uma resposta para a instância (n,c) do problema das n
// rainhas com casas proibidas usando a modelagem do problema como
// conjunto independente de um grafo
//
// n, c e r são como em rainhas_bt()

unsigned int *ConjIndep(grafo G, unsigned int n, unsigned int *I, unsigned int tamI,
						unsigned int *C, unsigned int tamC) {
	// Caso base: Se o conjunto independente atual tem o tamanho desejado, retorne-o.
	if (tamI == n) {
		unsigned int *R = (unsigned int *)malloc(n * sizeof(unsigned int));
		memcpy(R, I, n * sizeof(unsigned int));
		return R;
	}

	// Se o tamanho do conjunto independente atual mais o conjunto de candidatos é menor que n, não
	// há solução.
	if (tamI + tamC < n) {
		return NULL;
	}

	// Remova um vértice de C
	unsigned int v = C[--tamC];

	// Crie um novo conjunto C sem os vizinhos de v (vértices adjacentes a v)
	unsigned int *newC = (unsigned int *)malloc(tamC * sizeof(unsigned int));
	unsigned int tamNewC = 0;
	for (unsigned int i = 0; i < tamC; i++) {
		if (G.matriz[v][C[i]] != 1) {
			newC[tamNewC++] = C[i];
		}
	}

	// Crie um novo conjunto I incluindo v
	unsigned int *newI = (unsigned int *)malloc((tamI + 1) * sizeof(unsigned int));
	memcpy(newI, I, tamI * sizeof(unsigned int));
	newI[tamI] = v;

	// Chame recursivamente a função com o novo conjunto I e C
	unsigned int *R = ConjIndep(G, n, newI, tamI + 1, newC, tamNewC);

	// Libere a memória alocada para newI e newC
	free(newI);
	free(newC);

	// Se R não é NULL, significa que encontramos um conjunto independente de tamanho n, então
	// retorne R
	if (R != NULL) {
		return R;
	}

	// Caso contrário, chame recursivamente a função sem incluir v em I
	return ConjIndep(G, n, I, tamI, C, tamC);
}

unsigned int *rainhas_ci(unsigned int n, unsigned int k, casa *c, unsigned int *r) {
	memset(r, 0, n * sizeof *r);
	grafo G = monta_grafo_restricoes(n, k, c);
	/* printa_restricoes(G); */

	for (int i = n; i > 0; i--) {
		unsigned int *I = (unsigned int *)malloc(0);
		unsigned int *C = (unsigned int *)malloc(G.tamAtivo * sizeof(unsigned int));
		unsigned int tamC = 0;
		for (unsigned int j = 0; j < G.tam; j++) {
			if (G.matriz[j][j] != -1) {
				C[tamC++] = j;
			}
		}

		unsigned int *result = ConjIndep(G, i, I, 0, C, tamC);

		free(I);
		free(C);

		/* cada posicao de r eh a coluna da rainha em cada linha */
		if (result != NULL) {
			for (unsigned int j = 0; j < i; j++) {
				r[result[j] / n] = (result[j] % n) + 1;
			}
			free(result);
			return r;
		}
	}

	return r;
}
