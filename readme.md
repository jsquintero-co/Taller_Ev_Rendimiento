#  Evaluación de Rendimiento: Multiplicación de Matrices

Este proyecto realiza un análisis de rendimiento sobre la multiplicación de matrices cuadradas ($O(N^3)$), comparando el comportamiento de **hilos POSIX (`pthread`)** frente a **procesos (`fork`)**, bajo dos patrones de acceso a memoria (**Fila x Columna** y **Fila x Transpuesta**).

##  Mecánica del Proyecto

El rendimiento de un software no solo depende de las operaciones matemáticas, sino de **cómo el código interactúa con la arquitectura del hardware**.

### 1. Algoritmos y Jerarquía de Memoria

* **FxC (Fila por Columna - Tradicional):** Recorre la matriz $A$ de forma horizontal y la matriz $B$ de forma vertical. En el lenguaje C (que utiliza ordenamiento *Row-Major*), los elementos de las columnas no están contiguos en RAM. Leer una columna implica saltos de $8N$ bytes en memoria, provocando fallos de caché (**Cache Misses**) y forzando al procesador a esperar respuestas desde la memoria principal.

* **FxT (Fila por Transpuesta - Optimizado):** Transpone la matriz $B$ previo al cálculo. Al multiplicar, ambas matrices se leen de forma horizontal (secuencial). Esto permite que el *prefetcher* del procesador cargue las líneas de caché (**L1/L2/L3**) con anticipación, aprovechando la localidad espacial y reduciendo drásticamente el tiempo de ejecución.

### 2. Mecanismos de Concurrencia

* **Hilos POSIX (`pthread`):** Comparten el mismo espacio de direcciones del proceso padre. Su sobrecosto (*overhead*) de creación es mínimo, resultando ideales para dividir tareas en bloques paralelos.

* **Procesos (`fork`):** Crean duplicados independientes del espacio de memoria mediante el mecanismo *Copy-on-Write*. En cálculos muy breves, el costo del kernel para gestionar el nuevo proceso supera la ganancia del procesamiento paralelo.

## Diseño Experimental

El experimento sigue un diseño factorial completo con **224 configuraciones distintas por plataforma** y **30 repeticiones** por caso para garantizar significancia estadística.

| Factor | Niveles Evaluados | Propósito del Diseño |
| ----- | ----- | ----- |
| **Tamaño de Matriz (**$N$**)** | `96`, `240`, `480`, `720`, `960`, `1440`, `1920`, `2400` | Recorrer los niveles de la jerarquía (L2 $\rightarrow$ L3 $\rightarrow$ RAM). |
| **Nivel de Concurrencia (**$p$**)** | `1`, `2`, `4`, `6`, `8`, `12`, `16` | Evaluar rendimiento mononúcleo, multinúcleo y sobresuscripción. |
| **Mecanismo Concurrente** | POSIX Threads (`pthread`) vs Procesos (`fork`) | Cuantificar el sobrecosto de administración del Sistema Operativo. |
| **Patrón de Acceso** | Fila por Columna (FxC) vs Fila por Transpuesta (FxT) | Cuantificar la eficiencia del uso de la memoria caché. |

##  Detalles del Procedimiento

1. **Compilación Modular:** Automatización de compilación con optimizaciones de código de alto nivel (`-O3`) e inclusión de librerías POSIX para gestionar los hilos.
2. **Control de Entorno de Ejecución:** Para minimizar el ruido del sistema, las pruebas se realizan deshabilitando servicios secundarios y configurando el escalado de frecuencia del procesador en modo de máximo rendimiento (*performance governor*).
3. **Calibración y Captura Automatizada:** Un script de control ejecuta de manera iterativa cada combinación de variables, registrando el tiempo exacto de CPU en microsegundos y almacenando los datos brutos por separado para su posterior análisis estadístico (cálculo de promedios, desviación estándar y coeficiente de variación).

##  Estructura del Repositorio

```
.
├── src/                   # Código fuente C y encabezados de la aplicación
│   ├── moduloMM.c         # Funciones de asignación de memoria y multiplicación
│   ├── moduloMM.h         # Prototipos de funciones y definición de estructuras
│   ├── mxmPosixFxC.c      # Implementación POSIX Fila x Columna
│   ├── mxmPosixFxT.c      # Implementación POSIX Fila x Transpuesta
│   ├── mxmForkFxC.c       # Implementación Fork Fila x Columna
│   └── mxmForkFxT.c       # Implementación Fork Fila x Transpuesta
├── lanzador.pl            # Script de automatización de pruebas
└── Makefile               # Script de compilación optimizado
```

##  Integrantes del Equipo

* **Santiago Arias Tovar**
* **Juan Sebastián Quintero Gil**
* **Jesús Daniel Duque Aparicio**
* **Pablo Samuel León Hernández**
* **Katheryn Camacho González**

**Asignatura:** Sistemas Operativos  
**Institución:** Pontificia Universidad Javeriana  
**Año:** 2026
