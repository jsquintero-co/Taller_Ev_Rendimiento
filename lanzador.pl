#!/usr/bin/perl
#**************************************************************
#         		Pontificia Universidad Javeriana
#     Autores del taller:	Juan Sebastián Quintero Gil
#				Santiago Arias Tovar
#				Katheryn Camacho Gonzalez
#				Pablo Samuel León Hernandez 
#				Jesús Daniel Duque Aparicio
#     Profesor y autor del código: J. Corredor
#     Fecha: 03 de Noviembre 2026
#     Materia: Sistemas Operativos
#     Tema: Taller de Evaluación de Rendimiento
#     Fichero: script automatización ejecución por lotes
#
#     Descripción:
#       Ejecuta la batería de experimentos de forma automática: para cada
#       programa, tamaño de matriz y número de hilos, corre el programa
#       $Repeticiones veces y agrega cada tiempo (en microsegundos) al final
#       de un archivo .dat dentro de la carpeta $Resultados.
#       Nombre de cada archivo: <programa>-<N>-Hilos-<hilos>.dat
#       Con estos valores se generan 4 x 8 x 7 = 224 archivos de 30 líneas.
#
#     Cambios respecto al original:
#       - Listas de tamaños e hilos ampliadas y 30 repeticiones (antes 4).
#       - Se activó la línea system(...), que venía comentada.
#       - Rutas entre comillas en system(...) para soportar carpetas con espacios.
#       - Se quitó el printf de cada comando y se crea la carpeta de resultados.
#****************************************************************/

# Carpeta actual, desde donde se ejecuta el script (se le quita el salto de línea).
$Path = `pwd`;
chomp($Path);

# Batería de experimentación: todos los tamaños son múltiplos de 48,
# divisibles exactamente entre cada valor de @Num_Hilos
@Nombre_Ejecutable = ("mxmPosixFxC","mxmPosixFxT","mxmForkFxC","mxmForkFxT");   # programas a evaluar
@Size_Matriz  = ("96","240","480","720","960","1440","1920","2400");         # tamaños N
@Num_Hilos    = (1,2,4,6,8,12,16);                                          # hilos o procesos
$Repeticiones = 30;                 # ejecuciones por configuración
$Resultados	  = "Soluciones";       # carpeta donde se guardan los .dat
$Binarios	  = "bin";              # carpeta con los ejecutables compilados

# Crea la carpeta de resultados si no existe
mkdir("$Path/$Resultados") unless -d "$Path/$Resultados";

# Recorre todas las combinaciones programa x tamaño x hilos.
foreach $nombre (@Nombre_Ejecutable){
	foreach $size (@Size_Matriz){
		foreach $hilo (@Num_Hilos) {
			# Archivo de resultados de esta configuración
			$file = "$Path/$Resultados/$nombre-".$size."-Hilos-".$hilo.".dat";
			printf("$file \n");     # muestra el avance en el log
			for ($i=0; $i<$Repeticiones; $i++) {
				# Ejecuta el programa y agrega (>>) su salida, el tiempo en µs, al .dat
				system("\"$Path/$Binarios/$nombre\" $size $hilo  >> \"$file\"");
			}
		}
	}
}
