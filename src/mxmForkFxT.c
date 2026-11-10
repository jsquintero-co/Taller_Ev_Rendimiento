/*#######################################################################################
#* Autores del taller:       Juan Sebastián Quintero Gil
#*                           Santiago Arias Tovar
#*                           Katheryn Camacho Gonzalez
#*                           Pablo Samuel León Hernandez
#*                           Jesús Daniel Duque Aparicio
#* Profesor y autor del código: J. Corredor, PhD
#* Fecha: 03 de Noviembre 2026
#* Programa: mxmForkFxT.c
#*      Multiplicación de Matrices algoritmo fila x transpuesta (FxT)
#* Versión:
#*      Paralelismo con Procesos Fork
#* Descripción:
#*      - El padre calcula T = transpuesta de B y luego crea numProcesos hijos con
#*        fork(). Cada hijo calcula un bloque de filas de C con mxmForkFxT
#*        (fila de A x fila de T, acceso contiguo en memoria) y termina; el padre
#*        espera a todos con wait() e imprime el tiempo.
#*      - La transposición está dentro del tiempo medido. Los hijos heredan T ya
#*        calculada porque fork() copia la memoria del padre.
#*      - Observación: matC no es memoria compartida (ver mxmForkFxC.c); el padre
#*        no recibe el resultado, pero el tiempo medido es válido.
######################################################################################*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "moduloMM.h"

/* main: lee los argumentos, prepara las matrices, transpone B, crea los procesos
 * hijos, espera a que terminen e imprime el tiempo empleado. */
int main(int argc, char *argv[]) {
	if (argc < 3){
		printf("Ingreso de argumentos \n $./ejecutable tamMatriz numHilos\n");
		exit(0);
	}

	int N		= (int) atoi(argv[1]);      /* dimensión de la matriz */
	int num_P  	= (int) atoi(argv[2]);      /* número de procesos hijos */
    int filasxP	= N/num_P;                  /* filas que calcula cada hijo */

	/* Reserva de memoria (calloc la deja en cero); matT guarda la transpuesta de B. */
	double *matA = (double *) calloc(N*N, sizeof(double));
	double *matB = (double *) calloc(N*N, sizeof(double));
	double *matC = (double *) calloc(N*N, sizeof(double));
	double *matT = (double *) calloc(N*N, sizeof(double));

	/* Datos de entrada; se imprimen solo si N < 13. No entra en el tiempo medido. */
    iniMatrix(matA, matB, N);
    impMatrix(matA, N);
    impMatrix(matB, N);

	InicioMuestra();                        /* ---- inicio de la medición ---- */

	matrixTRP(N, matB, matT);               /* T = transpuesta de B (la hace el padre) */

	for (int i = 0; i < num_P; i++) {
		pid_t pid = fork();                 /* crea un proceso hijo, copia del padre */

        if (pid == 0) {                     /* ---- código que ejecuta el hijo ---- */
            int filaI = i*filasxP;
            /* el último hijo llega hasta N para cubrir las filas sobrantes */
            int filaF = (i == num_P - 1) ? N : filaI + filasxP;

			mxmForkFxT(matA, matT, matC, N, filaI, filaF);

			/* Si la matriz es pequeña, el hijo imprime las filas que calculó
			 * (es la única forma de ver el resultado, porque matC no es compartida). */
			if(N<11){
           		printf("\nChild PID %d calculated rows %d to %d:\n", getpid(), filaI, filaF-1);
            	for (int f = filaI; f < filaF; f++) {
                	for (int c = 0; c < N; c++) {
                    	printf(" %.2f ", matC[N*f+c]);
                	}
                	printf("\n");
            	}
			}
            exit(0);                        /* el hijo termina aquí */
        } else if (pid < 0) {               /* fork falló */
            perror("fork failed");
            exit(1);
        }
        /* el padre sigue el ciclo y crea el siguiente hijo */
    }

    /* El padre espera a que terminen todos los hijos. */
    for (int i = 0; i < num_P; i++) {
        wait(NULL);
    }

	FinMuestra();                           /* ---- fin de la medición: imprime µs ---- */


	/* Liberación de memoria */
	free(matA);
	free(matB);
	free(matC);
	free(matT);

    return 0;
}
