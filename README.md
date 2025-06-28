## SIDE EFFECT - JUEGO DE DIAGNÓSTICO MÉDICO

## DESCRIPCIÓN
Side Effect es un juego de simulación médica en un mundo caótico. El jugador asume el rol de un doctor encargado de diagnosticar y curar pacientes que presentan síntomas aleatorios y enfermedades impredecibles.  
El objetivo es utilizar el medicamento correcto para salvar a tantos pacientes como sea posible antes de que mueran.
Cada paciente tiene un tiempo limitado de vida, una enfermedad específica, y síntomas que pueden coincidir con los de otras enfermedades. El juego se basa en una red de enfermedades conectadas por síntomas comunes, generando decisiones impredecibles y estrategias creativas. No sigue una lógica médica real: todo está hecho para poner a prueba tu intuición.

## CÓMO COMPILAR Y EJECUTAR
Este sistema ha sido desarrollado en lenguaje C. Para ejecutarlo correctamente, sigue estos pasos:

## REQUISITOS PREVIOS
- Tener instalado **Visual Studio Code**
- Instalar la extensión **C/C++** (de Microsoft)
- Tener instalado un compilador de C como **gcc** (en Windows se recomienda MinGW o usar el entorno WSL)

## PASOS PARA COMPILAR Y EJECUTAR
1. **Descargar y descomprimir el archivo .zip en una carpeta de tu elección.**

2. **Abre el proyecto en Visual Studio Code**
    - Inicia Visual Studio Code.
    - Selecciona Archivo > Abrir carpeta... y elige la carpeta donde descomprimiste el proyecto.

3. **Compila el código**
    - Abre el archivo principal (por ejemplo, main.c).
    - Abre la terminal integrada (Terminal > Nueva terminal).
    - Selecciones la terminal Git Bash
    - En la terminal, compila el programa con el siguiente comando:
    gcc tdas/*.c proyecto.c -Wno-unused-result -o se

4. **Ejecuta el programa**
    - Una vez compilado, puedes ejecutar la aplicación con:
    ./se

## FUNCIONALIDADES

## MENÚ PRINCIPAL
1. Cargar datos desde archivos CSV
Carga las enfermedades, medicamentos y pacientes desde los archivos CSV.

2. Jugar  
Comienza la partida, donde podrás gestionar pacientes, diagnosticar enfermedades y aplicar tratamientos.

3. Lore
Muestra una introducción con estilo cinemático sobre el universo del juego.

4. Tutorial 
(Próximamente) Explica las mecánicas paso a paso.

5. Información del juego 
Explica el funcionamiento del juego, su enfoque intuitivo y el uso de síntomas impredecibles.

### ACCIONES DURANTE EL JUEGO
1. Nuevo Paciente
Añade un nuevo paciente aleatorio de la base de datos (máximo 10 activos).

2. Mostrar Pacientes Activos 
Lista todos los pacientes en espera con su nombre, ID, síntomas y días restantes de vida.

3. Revisar Paciente 
(Próximamente) Permite diagnosticar y aplicar medicamentos al paciente.

4. Terminar Partida
Finaliza la sesión de juego.

5. Salir del juego
Cierra el programa.

## PROBLEMAS CONOCIDOS