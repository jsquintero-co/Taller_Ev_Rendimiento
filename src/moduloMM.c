/*#######################################################################################
 #*     Autores del taller:       Juan Sebastián Quintero Gil
 #*                               Santiago Arias Tovar
 #*                               Katheryn Camacho Gonzalez
 #*                               Pablo Samuel León Hernandez
 #*                               Jesús Daniel Duque Aparicio
 #*    Profesor y autor del código: J. Corredor, PhD
 #*    Fecha: 03 de Noviembre 2026
 #* Modulo: moduloMM.c
 #       - Implementación de las funciones comunes a los cuatro programas:
 #         iniMatrix, impMatrix, matrixTRP, mxmForkFxC, mxmForkFxT,
 #         InicioMuestra y FinMuestra.
 #* Versión:
 #*	 	Concurrencia de Tareas: Paralelismo sobre Multiplicación de Matrices
 #* Descripción:
 #       - Las matrices son vectores de D*D doubles guardados por filas (row-major):
 #         el elemento [i][j] está en la posición i*D + j.
 #       - Las funciones de multiplicación reciben un rango de filas [filaI, filaF)
 #         para que cada hilo o proceso calcule solo su parte de la matriz C.
 #       - Observación: este archivo define __MODULOMM_H__ antes de incluir
 #         moduloMM.h, por lo que el contenido del .h se omite aquí. No causa
 #         errores porque cada función se define antes de usarse.
######################################################################################*/

#ifndef __MODULOMM_H__
#define __MODULOMM_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <sys/time.h>
#include "moduloMM.h"

/* Instantes de inicio y fin de la medición. Son globales porque InicioMuestra()
 * y FinMuestra() se llaman por separado y deben compartirlos. */
struct timeval inicio, fin;

/* impMatrix: imprime una matriz D x D, una fila por línea, con 2 decimales.
 * Solo imprime si D < 13: con matrices grandes la salida sería ilegible y
 * además quedaría mezclada con los tiempos que recoge lanzador.pl. */
void impMatrix(double *matrix, int D){
	if(D < 13){
		printf("\n");
		for(int i=0; i<D*D; i++){
			if(i%D==0) printf("\n");          /* salto de línea al empezar cada fila */
			printf("%.2f ", matrix[i]);
		}
		printf("\n**-----------------------------**\n");
	}
}

/* matrixTRP: calcula la transpuesta de mB y la guarda en mT (mT[i][j] = mB[j][i]).
 * La usa el algoritmo FxT para que las columnas de B queden como filas
 * contiguas en memoria. Costo: N*N operaciones. */
void matrixTRP(int N, double *mB, double *mT){
	for(int i=0; i<N; i++)
		for(int j=0; j<N; j++)
			mT[i*N+j] = mB[j*N+i];
	impMatrix(mT, N);                         /* muestra la transpuesta si N < 13 */
}

/* mxmForkFxT: multiplicación fila x transpuesta.
 * Calcula las filas filaI..filaF-1 de mC = mA x B, donde mT es la transpuesta de B.
 * Cada C[i][j] es el producto punto de la fila i de mA con la fila j de mT.
 * Ambos punteros avanzan de a 1 posición: acceso contiguo, amigable con la caché. */
void mxmForkFxT(double *mA, double *mT, double *mC, int D, int filaI, int filaF){
    for (int i = filaI; i < filaF; i++)
        for (int j = 0; j < D; j++) {
			double *pA, *pB, Suma = 0.0;
			pA = mA+i*D;                      /* inicio de la fila i de A */
			pB = mT+j*D;                      /* inicio de la fila j de T (= columna j de B) */
            for (int k = 0; k < D; k++, pA++, pB++)
				Suma += *pA * *pB;
			mC[i*D+j] = Suma;
        }
}

/* mxmForkFxC: multiplicación clásica fila x columna.
 * Calcula las filas filaI..filaF-1 de mC = mA x mB.
 * Cada C[i][j] es el producto punto de la fila i de mA con la columna j de mB.
 * pB avanza D posiciones en cada paso (salta una fila entera de B), por lo que
 * cada lectura cae en una zona distinta de memoria: con D grande es lento. */
void mxmForkFxC(double *mA, double *mB, double *mC, int D, int filaI, int filaF){
    for (int i = filaI; i < filaF; i++)
        for (int j = 0; j < D; j++) {
			double *pA, *pB, Suma = 0.0;
			pA = mA+i*D;                      /* inicio de la fila i de A */
			pB = mB+j;                        /* primer elemento de la columna j de B */
            for (int k = 0; k < D; k++, pA++, pB+=D)
				Suma += *pA * *pB;
			mC[i*D+j] = Suma;
        }
}

/* InicioMuestra: arranca el cronómetro guardando la hora actual del sistema
 * (precisión de microsegundos). */
void InicioMuestra(){
	gettimeofday(&inicio, (void *)0);
}

/* FinMuestra: detiene el cronómetro, calcula el tiempo transcurrido desde
 * InicioMuestra() y lo imprime en microsegundos. Ese número es el que
 * lanzador.pl guarda en cada archivo .dat. Es tiempo real (de pared), no de CPU. */
void FinMuestra(){
	gettimeofday(&fin, (void *)0);
	fin.tv_usec -= inicio.tv_usec;
	fin.tv_sec  -= inicio.tv_sec;
	double tiempo = (double) (fin.tv_sec*1000000 + fin.tv_usec);
	printf("%9.0f \n", tiempo);
}

/* iniMatrix: llena m1 y m2 (D x D) con números aleatorios.
 * m1 queda con valores entre 0 y 4, y m2 entre 0 y 7.
 * La semilla es la hora actual, así que cada ejecución usa matrices distintas. */
void iniMatrix(double *m1, double *m2, int D){
	srand(time(NULL));
	for(int i=0; i<D*D; i++, m1++, m2++){
		*m1 = (double)rand()/RAND_MAX*(5.0-1.0);
		*m2 = (double)rand()/RAND_MAX*(9.0-2.0);
	}
}

#endif
