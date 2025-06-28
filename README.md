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
- La lógica de búsqueda de pacientes o medicamentos puede fallar si hay diferencias menores en mayúsculas/minúsculas.
- La partida no puede guardarse ni reanudarse: es completamente en tiempo real.

## A MEJORAR
- Implementar una interfaz de usuario más amigable.
- Agregar un modo multijugador competitivo.

## EJEMPLO DE USO
**Paso 1: Cargar datos**
Seleccionando la opción 1) Cargar datos desde el menú inicial, el juego lee la información desde los archivos CSV:
- data/enfermedades.csv
- data/medicamentos.csv
- data/pacientes.csv
Se generarán conexiones entre enfermedades según síntomas compartidos, y se mostrará el mensaje:
"Juego Cargado correctamente!"

**Paso 2: Jugar**
Seleccionando la opción 2) Jugar, comienza la partida principal.
Se mostrará el menú con las siguientes acciones disponibles:

1) Nuevo paciente:
Se añade un nuevo paciente aleatorio a la lista de atención, con síntomas y una enfermedad. Si ya hay 10 pacientes activos, no se podrán agregar más.

2) Tomar medicamento:
Permite buscar un medicamento por síntoma. Se muestran los medicamentos disponibles y se puede agregar uno al inventario.

3) Revisar paciente:
Muestra los pacientes activos, sus síntomas y días restantes. Desde ahí se puede administrar un medicamento del inventario. Si la cura es correcta, el paciente se sana; si es incorrecta, pierde un día de vida y puede desarrollar una nueva enfermedad.

4) Terminar partida:
Finaliza la partida actual. Si un paciente muere, también se termina automáticamente.

5) Salir del juego:
Cierra el programa.

**Paso 3: Lore e información**
En el menú inicial también puedes acceder a:

3) Lore: muestra una introducción narrativa del mundo caótico en el que se desarrolla el juego.

5) Información del juego: explica las mecánicas básicas del sistema y el objetivo general.

**Paso 4: Manejo de errores**
Si el usuario selecciona una opción inválida en cualquier menú, se mostrará:
"Opcion invalida!"
Y se volverá automáticamente al menú anterior sin afectar el flujo del programa.

## CONTRIBUCIONES
**Juan Atuesta:**
- 1. Definió la función NuevoPaciente.

- 2. Definió la función mostrarLore.

- 3. Definió la función tutorial.

**Francisca Jorquera:**
- 1. Definió las funciones mostrarPreMenu, mostrarMenu y menuPaciente.

- 2. Definió la función estadoPaciente.

- 3. Definió la función administrar.

- 4. Definió la función tomar.

**Millaray Rebolledo:**
- 1. Definió la función mostrarInfo.

- 2. Creó el README.

- 3. Definió la función atender.

**Cristóbal Thompson**
- 1. Definió la función cargar_CSVS.

- 2. Definió la función crearGrafo.

- 3. Definió la función generar_nueva_enfermedad.

- 4. Añadió los comentarios.
