#**************************************************************
#                       Pontificia Universidad Javeriana
#     Autores del taller:       Juan Sebastián Quintero Gil
#                               Santiago Arias Tovar
#                               Katheryn Camacho Gonzalez
#                               Pablo Samuel León Hernandez
#                               Jesús Daniel Duque Aparicio
#     Profesor y autor del código: J. Corredor
#     Fecha: 03 de Noviembre 2026
#     Materia: Sistemas Operativos
#     Tema: Taller de Evaluación de Rendimiento
#     Fichero: script automatización para la compilación
#
#     Descripción: Automatizar y optimizar el proceso de
#	           compilación.
#
#****************************************************************/

# Variables globales de configuración del compilador y directorios
GCC = gcc
CFLAGS = -lm -O3          
FOPENMP = -fopenmp        
POSIX = -lpthread         # Librería de hilos POSIX requerida para mxmPosix
BIN = bin                 # Directorio de salida para los binarios ejecutables
SRC = src                 # Directorio de código fuente

modulo = moduloMM         # Módulo base compartido con la lógica de matrices

# Lista de los 4 ejecutables principales evaluados en el taller
PROGRAMAS = mxmPosixFxC mxmPosixFxT mxmForkFxC mxmForkFxT 

# Compila todos los programas definidos en la lista
All: $(PROGRAMAS)

# Compilar versiones con Hilos POSIX y algoritmo Fila por Columna
mxmPosixFxC:
	$(GCC) $(CFLAGS) $(SRC)/$(modulo).c $(SRC)/$@.c -o $(BIN)/$@ $(POSIX)

# Compilar versiones con Procesos Fork y algoritmo Fila por Columna
mxmForkFxC:
	$(GCC) $(CFLAGS) $(SRC)/$(modulo).c $(SRC)/$@.c -o $(BIN)/$@ 

# Compilar versiones con Hilos POSIX y algoritmo Fila por Transpuesta
mxmPosixFxT:
	$(GCC) $(CFLAGS) $(SRC)/$(modulo).c $(SRC)/$@.c -o $(BIN)/$@ $(POSIX)

# Compilar versiones con Procesos Fork y algoritmo Fila por Transpuesta
mxmForkFxT:
	$(GCC) $(CFLAGS) $(SRC)/$(modulo).c $(SRC)/$@.c -o $(BIN)/$@ 

# Elimina todos los binarios generados en la carpeta bin/
clean:
	$(RM) $(BIN)/*