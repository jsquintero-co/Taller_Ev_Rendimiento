/*#######################################################################################
#* Autores del taller:       Juan Sebastián Quintero Gil
#*                           Santiago Arias Tovar
#*                           Katheryn Camacho Gonzalez
#*                           Pablo Samuel León Hernandez
#*                           Jesús Daniel Duque Aparicio
#* Profesor y autor del código: J. Corredor, PhD
#* Fecha: 03 de Noviembre 2026
#* Autor: J. Corredor, PhD
#* Programa: mxmPosixFxT.c
#*      Multiplicación de Matrices algoritmo fila x transpuesta (FxT)
#* Versión:
#*      Paralelismo con Hilos Pthreads "Posix"
#* Descripción:
#*      - Igual que mxmPosixFxC, pero primero calcula T = transpuesta de B y luego
#*        multiplica cada fila de A por una FILA de T. Las operaciones son las
#*        mismas, pero los datos se leen seguidos en memoria y se aprovecha la caché.
#*      - La transposición está dentro del tiempo medido.
#*      - N debe ser divisible entre numHilos: si no, las últimas N % numHilos
#*        filas no se calculan (ver filaF en mxmPosixFxT).
######################################################################################*/

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "moduloMM.h"

/* Mutex del programa. Se bloquea y libera al final de cada hilo sin proteger
 * ninguna sección crítica: cada hilo escribe filas distintas de C. */
pthread_mutex_t MM_mutex;

/* Matrices globales compartidas por todos los hilos; matrixT es la transpuesta de B. */
double *matrixA, *matrixB, *matrixT, *matrixC;

/* Parámetros que recibe cada hilo. */
struct parametros{
        int nH;     /* número total de hilos */
        int idH;    /* identificador de este hilo (0 .. nH-1) */
        int N;      /* dimensión de la matriz */
};

/* mxmPosixFxT: función que ejecuta cada hilo.
 * Calcula sus filas de C: desde (D/nH)*idH hasta (D/nH)*(idH+1) - 1.
 * Cada C[i][j] es el producto punto de la fila i de A con la fila j de T;
 * los dos punteros avanzan de a 1 posición (acceso contiguo). */
void *mxmPosixFxT(void *variables){
	struct parametros *data = (struct parametros *)variables;
	int idH		= data->idH;
	int nH		= data->nH;
	int D		= data->N;
	int filaI	= (D/nH)*idH;           /* primera fila de este hilo */
	int filaF	= (D/nH)*(idH+1);       /* fila siguiente a la última (no incluida) */

    for (int i = filaI; i < filaF; i++){
        for (int j = 0; j < D; j++){
			double *pA, *pB, Suma = 0.0;
			pA = matrixA + i*D;         /* inicio de la fila i de A */
			pB = matrixT + j*D;         /* inicio de la fila j de T (= columna j de B) */
            for (int k = 0; k < D; k++, pA++, pB++){
				Suma += *pA * *pB;
			}
			matrixC[i*D+j] = Suma;
		}
	}

	pthread_mutex_lock (&MM_mutex);
	pthread_mutex_unlock (&MM_mutex);
	pthread_exit(NULL);
}

/* main: lee los argumentos, prepara las matrices, transpone B, crea los hilos,
 * espera a que terminen e imprime el tiempo empleado. */
int main(int argc, char *argv[]){
	if (argc < 3){
		printf("Ingreso de argumentos \n $./ejecutable tamMatriz numHilos\n");
		exit(0);
	}
    int N 		= (int) atoi(argv[1]);      /* dimensión de la matriz */
    int num_Th	= (int) atoi(argv[2]);      /* número de hilos */

    pthread_t p[num_Th];                    /* identificadores de los hilos */
    pthread_attr_t atrMM;                   /* atributos de creación de los hilos */

	/* Reserva de memoria (calloc la deja en cero). */
	matrixA  = (double *)calloc(N*N, sizeof(double));
	matrixB  = (double *)calloc(N*N, sizeof(double));
	matrixC  = (double *)calloc(N*N, sizeof(double));
	matrixT  = (double *)calloc(N*N, sizeof(double));

	/* Datos de entrada; se imprimen solo si N < 13. No entra en el tiempo medido. */
	iniMatrix(matrixA, matrixB, N);
	impMatrix(matrixA, N);
	impMatrix(matrixB, N);

	InicioMuestra();                        /* ---- inicio de la medición ---- */

	matrixTRP(N, matrixB, matrixT);         /* T = transpuesta de B (secuencial, dentro del tiempo) */

	pthread_mutex_init(&MM_mutex, NULL);
	pthread_attr_init(&atrMM);
	pthread_attr_setdetachstate(&atrMM, PTHREAD_CREATE_JOINABLE);   /* para poder hacer join */

	/* Crea num_Th hilos; cada uno recibe su propia estructura de parámetros.
	 * Observación: la memoria de "datos" nunca se libera (pequeña fuga de memoria). */
    for (int j=0; j<num_Th; j++){
		struct parametros *datos = (struct parametros *) malloc(sizeof(struct parametros));
		datos->idH = j;
		datos->nH  = num_Th;
		datos->N   = N;

        pthread_create(&p[j],&atrMM,mxmPosixFxT,(void *)datos);
	}

	/* Espera a que todos los hilos terminen. */
    for (int j=0; j<num_Th; j++)
        pthread_join(p[j],NULL);

	FinMuestra();                           /* ---- fin de la medición: imprime µs ---- */

	impMatrix(matrixC, N);                  /* resultado, solo si N < 13 */

	/*Liberación de Memoria*/
	free(matrixA);
	free(matrixB);
	free(matrixC);
	free(matrixT);

	pthread_attr_destroy(&atrMM);
	pthread_mutex_destroy(&MM_mutex);
	pthread_exit (NULL);

	return 0;
}

