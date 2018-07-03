#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// Valor uniformemente distribuído em [0,1)
#define RAND ((double)(rand() >> 1)/((RAND_MAX >> 1) + 1))

// Intervalo do processo de Poisson
#define T 4000.0

// Incremento para o custo do assegurado
#define DC 0.0271828183

// Quantidade máxima para calcular esperança
#define MAX_EP 1000

// Variável aleatória exponencial
float expon(float lambda){
	return log(1.0-RAND)/(-lambda);
}

// Processo de Poisson
void pp(const float lambda, int *N, float **S){
	float instante = 0;
	for (;;){
		instante += expon(lambda);
		if (instante > T) break;
		(*N)++;
		*S = realloc(*S, *N*sizeof(float));
		(*S)[*N-1] = instante;
	}
}

// Proporção do tempo até quebrar
void phi(const float u0, const float c, const int N, float *S, float *T_R, float *p){
	// Soma dos gastos com acidentes
	float xs = 0;
	for (int i = 0; i < N; i++){
		xs += expon(1);
		if ( u0 + c*S[i] - xs < 0 ){
			// Tempo até a quebra
			*T_R = S[i];
			// Proporção de tempo sem quebrar
			*p = S[i] / T;
			return;
		}
	}
}

int main(int argc, char **argv){
	// Semente aleatória
	srand((unsigned int)time(NULL));

	// Reserva da asseguradora no instante t = 0
	float u0 = atof(argv[1]);

	// Taxa do processo de Poisson
	int lambda_pp = atoi(argv[2]);

	// Instantes dos eventos do processo de Poisson
	float *S = NULL;
	int N = 0;
	// Gerar processo de Poisson
	pp(lambda_pp, &N, &S);
	// Custo para o assegurado
	float c = 0;
	// Tempo até a quebra
	float T_R = T;
	// Esperança do tempo até a quebra
	float ET_R = 0;
	// Proporção de tempo até quebrar
	float p = 1;
	// Esperança da proporção para um certo c
	float ep = 0;

	while ( ep < 0.9 ){
		// Calcular esperança para o custo c escolhido
		for (int j = 0; j < MAX_EP; j++){
			T_R = T;
			p = 1;
			phi(u0, c, N, S, &T_R, &p);
			ep += p;
			ET_R += T_R;
		}
		ep /= (float)MAX_EP;
		ET_R /= (float)MAX_EP;

		// Coluna 0: Custo para o assegurado
		// Coluna 1: Tempo até a quebra
		// Coluna 2: Proporção de tempo até quebrar
		printf("%.6f\t%.6f\t%.6f\n", c, ET_R, ep);
		fflush(stdout);

		c += DC;
	}
	free(S);

	return 0;
}
