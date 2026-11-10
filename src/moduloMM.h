/*#######################################################################################
 #* Autores del taller:       Juan Sebastián Quintero Gil
 #*                           Santiago Arias Tovar
 #*                           Katheryn Camacho Gonzalez
 #*                           Pablo Samuel León Hernandez
 #*                           Jesús Daniel Duque Aparicio
 #* Profesor y autor del código: J. Corredor, PhD
 #* Fecha: 03 de Noviembre 2026
 #* Modulo: moduloMM.h
 #       - Interfaz (prototipos) de las funciones comunes a los cuatro programas de
 #         multiplicación de matrices: inicialización, impresión, transposición,
 #         los dos algoritmos de multiplicación (FxC y FxT) y la medición de tiempo.
 #* Versión:
 #*	 	Concurrencia de Tareas: Paralelismo sobre Multiplicación de Matrices
 #* Descripción:
 #       - Las matrices son cuadradas (D x D), de tipo double, y se guardan en un
 #         vector de D*D posiciones ordenado por filas (row-major): el elemento
 #         [i][j] está en la posición i*D + j.
######################################################################################*/

#ifndef __MODULOMM_H__
#define __MODULOMM_H__

/* Llena m1 y m2 (D x D) con números aleatorios: m1 entre 0 y 4, m2 entre 0 y 7. */
void iniMatrix(double *m1, double *m2, int D);

/* Inicia el cronómetro: guarda la hora actual (gettimeofday) en una variable global. */
void InicioMuestra();

/* Detiene el cronómetro e imprime el tiempo transcurrido en microsegundos. */
void FinMuestra();

/* Imprime la matriz solo si es pequeña (D < 13), para poder verificar resultados a mano. */
void impMatrix(double *matrix, int D);

/* Calcula la transpuesta de mB (N x N) y la guarda en mT: mT[i][j] = mB[j][i]. */
void matrixTRP(int N, double *mB, double *mT);

/* Algoritmo FxC (fila x columna): calcula las filas [filaI, filaF) de mC = mA x mB,
 * recorriendo una fila de mA y una COLUMNA de mB (acceso a saltos de D posiciones). */
void mxmForkFxC(double *mA, double *mB, double *mC, int D, int filaI, int filaF);

/* Algoritmo FxT (fila x transpuesta): calcula las filas [filaI, filaF) de mC = mA x mB
 * usando mT = transpuesta de mB, recorriendo una fila de mA y una FILA de mT
 * (acceso contiguo en memoria, aprovecha mejor la caché). */
void mxmForkFxT(double *mA, double *mT, double *mC, int D, int filaI, int filaF);

#endif
