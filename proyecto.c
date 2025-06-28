#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "tdas/listasDobles.h"
#include "tdas/hashmap.h"
#include "tdas/extra.h"

// - - - - - - - - ESTRUCTURAS - - - - - - - - - -

/*================================================================================
Struct utilizada para almacenar la información de un nodo enfermedad. La estructura
contiene información relevante de una enfermedad y ademas un hashMap para las 
enfermedades adyacentes para el grafo de enfermedades.
================================================================================*/
typedef struct{
    char nombre[100];           //nombre de la enfermedad
    List* sintomas;             //lista de sintomas de la enfermedad
    char cura[100];             //cadena que indica el nombre del medicamento el cual cura la enfermedad
    HashMap* enfermedadesAdj;   //enfermedades adyacentes a la enfermedad actual
}Enfermedad;

/*================================================================================
Struct utilizada para almacenar la información del paciente, asi como su nombre,
tiempo de vida restante, su enfermedad y mas.
================================================================================*/
typedef struct{
    char nombre[100];           //nombre del paciente
    int id;                     //ID del paciente
    int tiempoVida;             //dias restantes de vida del paciente
    Enfermedad* enfermedad;     //enfermedad que presenta el paciente
}Paciente;

/*================================================================================
Struct utilizada para almacenar la información de cada medicamento, contiene el
nombre del medicamento, una breve descripción y una lista de sintomas que cura 
dicho medicamento.
================================================================================*/
typedef struct{
    char nombre[100];           //nombre del medicamento
    char descripcion[350];      //descripción del medicamento
    List* sintomasCura;         //lista de sintomas el cual cura el medicamento
}Medicamento;



//FUNCIONES



/*================================================================================
-----------------------------------GENERAR DIAS-----------------------------------
FUNCIONALIDAD: Función la cual genera aleatoriamente entre los parametros
asignados entre minimo y maximo posible, unn numero el cual indicara la cantidad
de dias de vida que le quedan al paciente

PARÁMETROS: Ninguno.

COMPLEJIDAD: o(1)
================================================================================*/
int generarDias(){
    int minDias = 2;
    int maxDias = 10;

    // Asegurar que la semilla se inicialice solo una vez
    static int semillaInicializada = 0;
    if (!semillaInicializada) {
        srand(time(NULL));
        semillaInicializada = 1;
    }

    return rand() % (maxDias - minDias + 1) + minDias;
}


/*================================================================================
-------------------------------CARGAR MEDICAMENTOS--------------------------------
FUNCIONALIDAD: Función dedicada a cargar todos los medicamentos del csv, 
almacenando en cada struct de medicamento y posteriormente almacenarlos en ambos
mapas de medicamentos.

PARÁMETROS: 2 mapas Hash de medicamentos, un puntero doble de char y un valor
entero el cual permite mostrar mensajes si es que esta activo.

COMPLEJIDAD: o(n)
            -n : cantidad de datos del csv
================================================================================*/
void cargarMedicamentos(HashMap* medicamentos, HashMap* mapaMedicamentoSintomas, char** campos, int debug){
    FILE *archivoMedicamentos = fopen("data/medicamentos.csv", "r");
    if (archivoMedicamentos == NULL){
        perror("Error al abrir medicamentos");
        return;
    }

    campos = leer_linea_csv(archivoMedicamentos, ',');
    int contador = 1;

    if (debug) {
        puts("[DEBUG] Mostrando los primeros 5 medicamentos del archivo");
        printf("\n");
    }

    while((campos = leer_linea_csv(archivoMedicamentos, ',')) != NULL){
        Medicamento* medicamento = malloc(sizeof(Medicamento)); 
        if (medicamento == NULL) exit(1);

        //-------------------------------copiar cadenas-------------------------------------
        strncpy(medicamento->nombre, campos[0], sizeof(medicamento->nombre) - 1);
        medicamento->nombre[sizeof(medicamento->nombre) - 1] = '\0';

        strncpy(medicamento->descripcion, campos[1], sizeof(medicamento->descripcion) - 1);
        medicamento->descripcion[sizeof(medicamento->descripcion) - 1] = '\0';
        //-------------------------------------FIN------------------------------------------

        medicamento->sintomasCura = split_string(campos[2], ";");

        if (debug && contador <= 5){
            printf("[DEBUG] %s\n[DEBUG] %s\n\n", medicamento->nombre, medicamento->descripcion);
            for(char *sintoma = first_List(medicamento->sintomasCura); sintoma != NULL; 
                sintoma = next_List(medicamento->sintomasCura)){
                printf("[DEBUG] sintoma: '%s'\n", sintoma);
            }
            printf("\n\n");
            contador++;
        }

        insertMap(medicamentos, strdup(medicamento->nombre), medicamento);

        for (char* sintoma = first_List(medicamento->sintomasCura); sintoma != NULL; sintoma = next_List(medicamento->sintomasCura)) {
            Pair* par = searchMap(mapaMedicamentoSintomas, sintoma);
            if (par == NULL) {
                List* lista = create_List();
                push_Back(lista, medicamento);
                insertMap(mapaMedicamentoSintomas, strdup(sintoma), lista);
            } else {
                List* lista = (List*) par->value;
                push_Back(lista, medicamento);
            }
        }
    }

    fclose(archivoMedicamentos);
}


/*================================================================================
-------------------------------CARGAR ENFERMEDADES--------------------------------
FUNCIONALIDAD: Función dedicada a cargar todas las enfermedades del csv,
almacenando en cada struct de enfermedad y posteriormente en ambos mapas
ingresados.

PARÁMETROS: 2 mapas Hash uno de enfermedades y el otro de sintomas, un puntero
doble de char y un valor entero el cual permite mostrar mensajes si es que esta
activo.

COMPLEJIDAD: o(n)
            -n : cantidad de datos del csv
================================================================================*/
void cargarEnfermedades(HashMap* enfermedades, HashMap* sintomas, char** campos, int debug){
    FILE *archivoEnfermedades = fopen("data/enfermedades.csv", "r");
    if (archivoEnfermedades == NULL){
        perror("Error al abrir enfermedades");
        return;
    }
    int contador = 1;
    campos = leer_linea_csv(archivoEnfermedades, ',');

    Enfermedad* sano = malloc(sizeof(Enfermedad)); if (sano == NULL) exit(1);
    //-------------------------------copiar cadenas-------------------------------------
    strncpy(sano->nombre, "sano", sizeof(sano->nombre) - 1);
    sano->nombre[sizeof(sano->nombre) - 1] = '\0';
    
    strncpy(sano->cura, "Se encuentra sano!", sizeof(sano->cura) - 1);
    sano->cura[sizeof(sano->cura) - 1] = '\0';
    //-------------------------------------FIN------------------------------------------

    insertMap(enfermedades, strdup("sano"), sano);
    if (debug) {
        puts("[DEBUG] Mostrando las primeras 5 enfermedades del archivo");
        printf("\n");
    }
    while((campos = leer_linea_csv(archivoEnfermedades, ',')) != NULL){
        Enfermedad* enfermedad = malloc(sizeof(Enfermedad)); if (enfermedad == NULL) exit(1);
        enfermedad->enfermedadesAdj = createMap(300);
        insertMap(enfermedad->enfermedadesAdj, strdup("sano"), sano);

        //-------------------------------copiar cadenas-------------------------------------
        strncpy(enfermedad->nombre, campos[0], sizeof(enfermedad->nombre) - 1);
        enfermedad->nombre[sizeof(enfermedad->nombre) - 1] = '\0';

        strncpy(enfermedad->cura, campos[2], sizeof(enfermedad->cura) - 1);
        enfermedad->cura[sizeof(enfermedad->cura) - 1] = '\0';
        //-------------------------------------FIN------------------------------------------

        enfermedad->sintomas = split_string(campos[1], ";");

        for(char *sintoma = first_List(enfermedad->sintomas); sintoma != NULL; 
                sintoma = next_List(enfermedad->sintomas)){
                    Pair* nodo = searchMap(sintomas, strdup(sintoma));
                    if (nodo == NULL){
                        List* lista = create_List();
                        push_Back(lista, enfermedad);
                        insertMap(sintomas, strdup(sintoma), lista);
                    }
                    else push_Back(nodo->value, enfermedad);
                }

        if (debug && contador <= 5){
            printf("[DEBUG] %s\n[DEBUG] %s\n\n", enfermedad->nombre, enfermedad->cura);
            for(char *sintoma = first_List(enfermedad->sintomas); sintoma != NULL; 
                sintoma = next_List(enfermedad->sintomas)){
                printf("[DEBUG] sintoma: '%s'\n", sintoma);
            }
            printf("\n\n");
            contador++;
        }
        insertMap(enfermedades, strdup(enfermedad->nombre), enfermedad);
    }
    fclose(archivoEnfermedades);
}


/*================================================================================
---------------------------------CARGAR PACIENTES---------------------------------
FUNCIONALIDAD: Función dedicada a cargar todos los pacientes del csv, almacenando
en cada struct de paciente y posteriormente almacenandolo en el mapa de pacientes.

PARÁMETROS: 2 mapas Hash uno de pacientes y el otro el de enfermedades, un puntero
doble de char y un valor entero el cual permite mostrar mensajes si es que esta
activo.

COMPLEJIDAD: o(n)
            -n : cantidad de datos del csv
================================================================================*/
void cargarPacientes(HashMap* pacientes, HashMap* enfermedades, char** campos, int debug){
    FILE *archivoPacientes = fopen("data/pacientes.csv", "r");
    if (archivoPacientes == NULL){
        perror("Error al abrir enfermedades");
        return;
    }
    int contador = 1;
    campos = leer_linea_csv(archivoPacientes, ',');

    if (debug) {
        puts("[DEBUG] Mostrando los primeros 5 pacientes del archivo");
        printf("\n");
    }

    while((campos = leer_linea_csv(archivoPacientes, ',')) != NULL){
        
        Paciente* paciente = malloc(sizeof(Paciente)); if (paciente == NULL) exit(1);
        //-------------------------------copiar cadenas-------------------------------------
        strncpy(paciente->nombre, campos[1], sizeof(paciente->nombre) - 1);
        paciente->nombre[sizeof(paciente->nombre) - 1] = '\0';
        //-------------------------------------FIN------------------------------------------

        paciente->id  = atoi(campos[0]);
        paciente->enfermedad = NULL;
        Pair* par = searchMap(enfermedades, strdup(campos[2]));
        if (par != NULL) paciente->enfermedad = (Enfermedad *) par->value;
        
        //Agregar dias restantes con random
        paciente->tiempoVida = generarDias();


        if (debug && contador <= 5){
            printf("[DEBUG] %d\n[DEBUG] %d\n", paciente->id, paciente->tiempoVida);
            printf("[DEBUG] %s\n[DEBUG] %s\n\n", paciente->nombre, paciente->enfermedad->nombre);
            contador++; 
        }

        insertMap(pacientes, strdup(paciente->nombre), paciente);
    }
    fclose(archivoPacientes);
}


/*================================================================================
---------------------------------CARGAR CSVS--------------------------------------
FUNCIONALIDAD: Función la cual llama a las 3 funciones de carga y si el valor
debug esta activo muestra los primeros 5 nodos de cada mapa.

PARÁMETROS: mapas Hash de enfermedades, medicamentos, pacientes, sintomas y 
medicamentos por sintomas ademas, un valor entero el cual permite mostrar mensajes 
si es que esta activo.

COMPLEJIDAD: o(n * m * p)
            -n : cantidad de datos del csv enfermedades.
            -m : cantidad de datos del csv medicamentos.
            -p : cantidad de datos del csv pacientes.
================================================================================*/
void cargar_CSVS(HashMap* enfermedades, HashMap* medicamentos, HashMap* pacientes, HashMap* sintomas, HashMap* mapaMedicamentoSintomas, int debug){
    int contador = 1;
    char **campos;
    Pair* waza;
    if (debug) puts("-----------------------------------------");

    cargarMedicamentos(medicamentos, mapaMedicamentoSintomas, campos, debug);
    if (debug) {
        puts("-----------------------------------------");
        printf("[DEBUG] 5 primeros Nodos del mapa medicamentos\n\n");
        waza = (Pair*) firstMap(medicamentos);
        while(waza != NULL && contador <= 5){
            Medicamento* pos = waza->value;
            printf("[DEBUG] medicamento: %s\n[DEBUG] descripcion: %s\n\n", pos->nombre, pos->descripcion);
            for(char *sintoma = first_List(pos->sintomasCura); sintoma != NULL; 
                sintoma = next_List(pos->sintomasCura)){
                printf("[DEBUG] sintoma: '%s'\n", sintoma);
                }
            printf("\n\n");
            waza = (Pair*) nextMap(medicamentos);
            ++contador;
        }
        puts("-----------------------------------------");
    }
    
    cargarEnfermedades(enfermedades, sintomas, campos, debug);

    if (debug) {
        puts("-----------------------------------------");
        contador = 1;
        waza = (Pair*) firstMap(enfermedades);
        printf("[DEBUG] 5 primeros Nodos del mapa enfermedades\n\n");
        while(waza != NULL && contador <= 5){
            Enfermedad* value = waza->value;
            printf("[DEBUG] enfermedad: %s\n[DEBUG] cura: %s\n\n", value->nombre, value->cura);
            for(char *sintoma = first_List(value->sintomas); sintoma != NULL; 
                sintoma = next_List(value->sintomas)){
                printf("[DEBUG] sintoma: '%s'\n", sintoma);
            }
            printf("\n\n");
            waza = (Pair*) nextMap(enfermedades);
            ++contador;
            }
        puts("-----------------------------------------");
    }
    
    cargarPacientes(pacientes, enfermedades, campos, debug);

    if (debug) {
        puts("-----------------------------------------");
        contador = 1;
        waza = (Pair*) firstMap(pacientes);
        printf("[DEBUG] 5 primeros Nodos del mapa pacientes\n\n");
        while(waza != NULL && contador <= 5){
            Paciente* value = waza->value;
            printf("[DEBUG] %d\n", value->id);
            printf("[DEBUG] nombre: %s\n[DEBUG] Enfermedad: %s\n\n", value->nombre, value->enfermedad->nombre);
            waza = (Pair*) nextMap(pacientes);
            ++contador;
        }
        puts("-----------------------------------------");

        contador = 1;
        waza = (Pair*) firstMap(sintomas);
        printf("[DEBUG] 5 primeros Nodos del mapa sintomas\n\n");
        while(waza != NULL && contador <= 5){
            int contador2 = 1;
            Enfermedad* primer = first_List(waza->value);
            printf("\n[DEBUG] Sintoma: %s\n\n", waza->key);
            while(primer != NULL && contador2 <= 5){
                printf("[DEBUG] enfermedad: %s\n[DEBUG] cura: %s\n\n", primer->nombre, primer->cura);
                ++contador2;
                primer = next_List(waza->value);
            }
            waza = (Pair*) nextMap(sintomas);
            ++contador;
        }
        puts("-----------------------------------------");
    }
}


/*================================================================================
---------------------------------JUNTAR MAPS--------------------------------------
FUNCIONALIDAD: Función la cual recorre toda la lista ingresada y cada elemento
lo inserta en el mapa Hash ingresado.

PARÁMETROS: Un mapa Hash,, una lista enlazada, un puntero a char y 2 valores
enteros, uno de contador y otro del debug.

COMPLEJIDAD: o(n)
            -n : cantidad de datos del csv
================================================================================*/
void juntarMaps(HashMap* origen, List* agregar, char* original ,int debug, int cont){
    if (debug && cont <= 5) puts("[DEBUG] mostrando primeros 5 vecinos");
    int contador = 1;
    for (Enfermedad* dato = first_List(agregar); dato != NULL;
        dato = next_List(agregar)){
            if (strcmp(dato->nombre, original) == 0){
                if (debug && contador <= 5 && cont <= 5) printf("[DEBUG] no se puede insertar el nodo original! '%s'\n", original);
                continue;
            }

            if (debug && contador <= 5 && cont <= 5){
                printf("[DEBUG] vecino: %s\n", dato->nombre);
            }
            Pair* aux = searchMap(origen, strdup(dato->nombre));

            if (aux == NULL) insertMap(origen, strdup(dato->nombre), dato);
            else{
                if (debug && contador <= 5 && cont <= 5){
                    printf("[DEBUG] un nodo ya se encontraba dentro! '%s'\n", dato->nombre);
                }
            }
            ++contador;
        }
    if (debug && cont <= 5) printf("\n");
}


/*================================================================================
----------------------------------CREAR GRAFO-------------------------------------
FUNCIONALIDAD: Genera el grafo de enfermedas, recorriendo cada enfermedad,
conectandolo con enfermedades que compartan sintomas en comun.

PARÁMETROS: Un mapa Hash de enfermedades, un mapa hash de enfermedades por sintomas
y un valor entero el cual permite mostrar mensajes si es que esta activo.

COMPLEJIDAD: o(e * s * a)
            -e : cantidad de enfermedades
            -s : cantidad de sintomas por enfermedad
            -a : cantidad de datos de cada lista de sintomas
================================================================================*/
void crearGrafo(HashMap* enfermedades, HashMap* mapaSintomas, int debug){
    Pair* par = firstMap(enfermedades);
    int contador = 1;
    if (debug) puts("mostrando primeras 5 enfermedades conectadas con sus vecinos");
    while(par != NULL){
        Enfermedad* nodo = par->value;

        if (nodo == NULL || nodo->sintomas == NULL) {
            if (debug) printf("[DEBUG] nodo: %s\n", par->key); //nodo sano es que se filtra aqui
            par = nextMap(enfermedades);
            continue;
        }
        List* sintomas = nodo->sintomas;
        int largo = size_List(sintomas);
        char* sintoma = (char *) first_List(sintomas);

        if (debug && contador <= 5) {
            printf("[DEBUG] Enfermedad actual actual: '%s'\n", par->key);
            printf("[DEBUG] la enfermedad actual presenta '%d' sintomas\n\n", largo);
        }

        while(sintoma != NULL){
            if (debug && contador <= 5) printf("[DEBUG] se buscara enfermedades con sintoma '%s'\n", sintoma);
            Pair* parDos = searchMap(mapaSintomas, strdup(sintoma));
            if (parDos != NULL){
                List* adjSintoma = parDos->value;
                juntarMaps(nodo->enfermedadesAdj, adjSintoma, par->key,  debug, contador);
            }
            sintoma = (char *) next_List(sintomas);

        }
        long totalMap = size_Map(nodo->enfermedadesAdj);
        if (debug && contador <= 5) printf("[DEBUG] '%s' posee '%ld' vecinos\n", par->key, totalMap);
        if (debug && contador <= 5) printf("-----------------------------------------\n");
        par = (Pair *) nextMap(enfermedades);
        ++contador;
    }
    return;
}


/*================================================================================
-----------------------------GENERAR NUEVA ENFERMEDAD-----------------------------
FUNCIONALIDAD: Funcion la cual toma al azar una enfermedad vecina de la enfermedad
actual del paciente y actualiza la enfermedad del paciente a la nueva enfermedad
si es posible.

PARÁMETROS: un puntero a paciente y un valor entero el cual permite mostrar 
mensajes si es que esta activo.

COMPLEJIDAD: o(1)
================================================================================*/
void generar_nueva_enfermedad(Paciente* paciente, int debug){
    Enfermedad* actual = paciente->enfermedad;
    long pos = rand();
    Pair* par = (posMap(actual->enfermedadesAdj, pos, "sano"));
    if (par != NULL) {
        Enfermedad* nueva = (Enfermedad *) par->value;
        paciente->enfermedad = nueva;
        if (debug){
            printf("\n[DEBUG] Cambiando de enfermedad a una adyacente\n");
            printf("[DEBUG] enfermedad actual '%s'\n", actual->nombre);
            printf("[DEBUG] se genero el numero %ld\n", pos);
            if (par == NULL) printf("[DEBUG] NO EXISTE NUEVA ENFERMEDAD\n");
            else printf("[DEBUG] nueva enfermedad '%s'\n", nueva->nombre);
        }
    }
}


/*================================================================================
--------------------------------NUEVO PACIENTE------------------------------------
FUNCIONALIDAD: Agrega un nuevo paciente al azar a la lista de pacientes activos.

PARÁMETROS: Un mapa Hash de pacientes, una lista enlazada de pacientes activos y
un valor entero el cual permite mostrar mensajes si es que esta activo.

COMPLEJIDAD: o(1)
================================================================================*/
void NuevoPaciente(HashMap* pacientes, List* pacientesActivos, int debug, int esFinal) {
    if (esFinal){
        printf("\nLa partida ya termino.\nRevise sus estadisticas.\n\n");
        return;
    }
    if (pacientes == NULL || pacientesActivos == NULL) return;

    if (size_List(pacientesActivos) >= 10) {
        printf("\n¡Has alcanzado el limite de pacientes activos!\n");
        printf("Los pasillos del hospital estan llenos... ¡Atiende a alguien antes de recibir mas!\n");
        return;
    }
    
    long totalPacientes = size_Map(pacientes);
    if (totalPacientes == 0) {
        printf("No hay pacientes disponibles para agregar.\n");
        return;
    }

    long posicionAleatoria = rand() % totalPacientes;
    Pair* parActual = posMap(pacientes, posicionAleatoria, "a");
    /*for (long i = 0; i < posicionAleatoria && parActual != NULL; i++) {
        parActual = nextMap(pacientes);
    }*/



    if (parActual == NULL && parActual->value == NULL) {
        printf("Error al seleccionar paciente aleatorio.\n");
        return;
    }

    Paciente* pacienteSeleccionado = (Paciente*) parActual->value;

    Paciente* actual = first_List(pacientesActivos);
    while (actual != NULL) {
        if (actual->id == pacienteSeleccionado->id) {
            printf("El paciente %s ya esta siendo atendido.\n", pacienteSeleccionado->nombre);
            return;
        }
        actual = next_List(pacientesActivos);
    }

    push_Front(pacientesActivos, pacienteSeleccionado);

    printf("\n==========================================\n");
    printf("       NUEVO PACIENTE AGREGADO\n");
    printf("==========================================\n");
    printf("ID del paciente: %d\n", pacienteSeleccionado->id);
    printf("Nombre: %s\n", pacienteSeleccionado->nombre);

    if (pacienteSeleccionado->enfermedad && pacienteSeleccionado->enfermedad->sintomas) {
        printf("Sintomas presentados:\n");
        int contadorSintomas = 1;
        for (char* sintoma = first_List(pacienteSeleccionado->enfermedad->sintomas); 
             sintoma != NULL; 
             sintoma = next_List(pacienteSeleccionado->enfermedad->sintomas)) {
            printf("  %d. %s\n", contadorSintomas, sintoma);
            contadorSintomas++;
        }
    }

    printf("Dias restantes de vida: %d\n", pacienteSeleccionado->tiempoVida);
    printf("==========================================\n");
}


/*================================================================================
----------------------------MOSTRAR PACIENTES ACTIVOS-----------------------------
FUNCIONALIDAD: Muestra todo los pacientes de la lista de pacientes.

PARÁMETROS: Una lista enlazada de pacientes.

COMPLEJIDAD: o(n * s)
            -n : cantidad de datos de la lista
            -s : cantidad de sintomas
================================================================================*/
void mostrarPacientesActivos(List* pacientesActivos,int esFinal) {
    if (esFinal == 1)return;
    if (pacientesActivos == NULL || size_List(pacientesActivos) == 0) {
        printf("No hay pacientes activos.\n");
        return;
    }
    
    printf("\n=== PACIENTES ACTIVOS ===\n");
    int contador = 1;
    
    for (Paciente* paciente = first_List(pacientesActivos); paciente != NULL; 
         paciente = next_List(pacientesActivos)) {
        printf("%d. %s (ID: %d)\n", contador, paciente->nombre, paciente->id);
        
        if (paciente->enfermedad && paciente->enfermedad->sintomas) {
            printf("   Sintomas:\n");
            int num = 1;
            for (char* sintoma = first_List(paciente->enfermedad->sintomas); 
                 sintoma != NULL; 
                 sintoma = next_List(paciente->enfermedad->sintomas)) {
                printf("     %d. %s\n", num++, sintoma);
            }
        } else {
            printf("   Sintomas: Ninguno\n");
        }

        printf("   Dias restantes: %d\n\n", paciente->tiempoVida);
        contador++;
    }

    printf("Total de pacientes activos: %d\n", size_List(pacientesActivos));
}


/*================================================================================
-----------------------------------ESPERAR MS-------------------------------------
FUNCIONALIDAD: Función para esperar una cantidad de milisegundos 
(portátil a todos los SO)

PARÁMETROS: un entero sin signo que representa la cantidad de milisegundos para 
el printf.

COMPLEJIDAD: o(1)
================================================================================*/
void esperar_ms(unsigned int milisegundos) {
    clock_t inicio = clock(); // Captura el tiempo inicial
    // Espera activa hasta que haya pasado el tiempo indicado
    while ((clock() - inicio) * 1000 / CLOCKS_PER_SEC < milisegundos);
}


/*================================================================================
--------------------------------LEER ESCENARIOS-----------------------------------
FUNCIONALIDAD: Función que imprime texto como una cinemática, letra por letra.

PARÁMETROS: un puntero a char del texto, y un valor entero que indica la demora
entre caracter y caracter.

COMPLEJIDAD: o(1)
================================================================================*/
void imprimirCinematica(const char* texto, unsigned int delayMilisegundos) {
    for (int i = 0; texto[i] != '\0'; i++) {
        printf("%c", texto[i]);      // Muestra un carácter
        fflush(stdout);              // Asegura que se imprima inmediatamente
        esperar_ms(delayMilisegundos); // Espera antes de mostrar el siguiente
    }
}


/*================================================================================
----------------------------------MOSTRAR LORE------------------------------------
FUNCIONALIDAD: Muestra el lore de nuestro programa.

PARÁMETROS: Ninguno.

COMPLEJIDAD: o(1)
================================================================================*/
void mostrarLore() {
    const char* lore =
        "\n"
        "Anio 2077...\n"
        "La humanidad sobrevive, pero la medicina ya no es la misma.\n"
        "Enfermedades mutantes cambian de forma, se ocultan y enganian.\n"
        "Los sintomas ya no siguen reglas.\n"
        "Cada paciente es una combinacion unica, impredecible...\n\n"
        "SIDE EFFECT es la ultima esperanza.\n"
        "Una clinica experimental donde cada diagnostico es una batalla\n"
        "y cada tratamiento puede salvar una vida... o terminarla.\n\n"
        "Explora sintomas. Conecta enfermedades. Encuentra la cura.\n"
        "Pero recuerda...\n"
        "No todos los efectos estan previstos.\n\n"
        "          Bienvenido a SIDE EFFECT.\n\n";

    system("cls");  // Limpiar pantalla en Windows
    imprimirCinematica(lore, 28);  // Delay de 28 ms entre caracteres

    presioneTeclaParaContinuar();
}


/*================================================================================
----------------------------------MOSTRAR INFO------------------------------------
FUNCIONALIDAD: Muestra la información del juego.

PARÁMETROS: Ninguno.

COMPLEJIDAD: o(1)
================================================================================*/
void mostrarInfo() {
    const char* info =
        "\n"
        "Side Effect es un minijuego donde asumes el rol de un doctor\n"
        "en un mundo caotico, tratando de diagnosticar y curar pacientes\n"
        "con enfermedades impredecibles.\n\n"
        "Cada paciente tiene una enfermedad, sintomas aleatorios y pocos\n"
        "dias de vida. Tu objetivo es aplicar el medicamento correcto antes\n"
        "de que sea demasiado tarde.\n\n"
        "El juego mezcla estrategia, humor y decisiones rapidas. No sigue\n"
        "una logica medica real, sino que usa una red de enfermedades y\n"
        "efectos secundarios inesperados para poner a prueba tu intuicion.\n\n"
        "¿Podras curar a mas pacientes que tus amigos antes de que todo colapse?\n\n";

    system("cls");
    imprimirCinematica(info, 25);
    presioneTeclaParaContinuar();
}


/*================================================================================
---------------------------------MOSTRAR PRE MENU---------------------------------
FUNCIONALIDAD: Muestra las opciones disponibles del pre menu.

PARÁMETROS: Ninguno.

COMPLEJIDAD: o(1)
================================================================================*/
void mostrarPreMenu(){
    printf("\n        ¡Bienvenido a Side Effect!\n");
    printf("\nSeleccione su opcion:\n");
    printf("1) Cargar datos.\n");
    printf("2) Jugar\n");
    printf("3) Lore\n");
    printf("4) Tutorial\n");
    printf("5) Informacion del juego\n");
}


/*================================================================================
----------------------------------MOSTRAR MENU------------------------------------
FUNCIONALIDAD: Muestra las opciones disponibles del menu principal.

PARÁMETROS: Ninguno.

COMPLEJIDAD: o(1)
================================================================================*/
void mostrarMenu(){
    printf("\n==========================================\n");
    printf("      Menu de opciones del jugador.\n");
    printf("==========================================\n");
    printf("1) Nuevo paciente.\n");
    printf("2) Tomar medicamento.\n");
    printf("3) Revisar paciente.\n");
    printf("4) Terminar partida.\n");
    printf("5) Salir del juego.\n");
}


/*================================================================================
-----------------------------------MENU PACIENTE----------------------------------
FUNCIONALIDAD: Muestra las opciones disponibles del menu del paciente.

PARÁMETROS: Ninguno.

COMPLEJIDAD: o(1)
================================================================================*/
void menuPaciente(){
    printf("\n==========================================\n");
    printf("            Menu paciente\n");
    printf("==========================================\n");
    printf("1) Ver estado actual\n");
    printf("2) Administrar medicamento\n");
    printf("3) Atras\n");
}


/*================================================================================
----------------------------------ESTADO PACIENTE---------------------------------
FUNCIONALIDAD: Muestra el estado del paciente actual, su nombre, sus sintomas y
su tiempo de vida restante.

PARÁMETROS: puntero de tipo paciente.

COMPLEJIDAD: o(1)
================================================================================*/
void estadoPaciente(Paciente* paciente){
    char* sintomas;
    printf("\n- - - Estado del paciente - - -\n");
    printf("Nombre: %s\n",paciente->nombre);
    printf("Sintomas: \n");
    sintomas = first_List(paciente->enfermedad->sintomas);
    while (sintomas != NULL){
        printf("  %s\n",sintomas);
        sintomas = next_List(paciente->enfermedad->sintomas);
    }
    printf("Tiempo de vida: %i dias.\n",paciente->tiempoVida);
    printf("- - - - - - - - - - - - - - - - - - -\n");
}


/*================================================================================
------------------------------MOSTRAR MEDICAMENTO---------------------------------
FUNCIONALIDAD: Muestra el medicamento ingresado, su nombre, su descripcion y sus
sintomas que cura.

PARÁMETROS: un puntero de tipo medicamento.

COMPLEJIDAD: o(1)
================================================================================*/
void mostrarMedicamento(Medicamento* medicina){
    char* sintoma = first_List(medicina->sintomasCura);
    printf("Nombre: %s\n",medicina->nombre);
    printf("Descripcion: %s\n", medicina->descripcion);
    printf("Sintomas que cura:\n");
    while(sintoma != NULL){
        printf("    %s\n",sintoma);
        sintoma = next_List(medicina->sintomasCura);
    }
}


/*================================================================================
------------------------------SELECCIONAR MEDICAMENTO-----------------------------
FUNCIONALIDAD: Seleccionar un medicamento de la lista de medicamentos.

PARÁMETROS: Un puntero de tipo paciente, una lista enlazada y una lista de
medicamentos.

COMPLEJIDAD: o(1)
================================================================================*/
void seleccionarMedicamento(Paciente* paciente,List* inventario,Medicamento** medicini){
    char opcion;
    char buffer[10];

    Medicamento* medicina = (Medicamento*)first_List(inventario);
    if (medicina == NULL){
        printf("No tienes medicamentos en su inventario!\n");
        *medicini = NULL;
        return;
    }
    unsigned int talla = size_List(inventario);
    unsigned int contador = 0;
    printf("\n    Seleccione el medicamento:\n");

    do{
        mostrarMedicamento(medicina);
        
        if (contador > 0) printf("a) Anterior.\n");
        if (contador < talla - 1) printf("d) Siguiente.\n");
        printf("w) Seleccionar.\n");
        printf("s) Volver.\n");

        fgets(buffer, sizeof(buffer), stdin);
        sscanf(buffer, " %c", &opcion);

        switch (opcion)
        {
        case 'a':
            if (contador > 0) {
                medicina = prev_List(inventario);
                contador--;
            }
            else printf("Opcion no valida!\n");
            break;
        case 'd':
            if (contador < talla - 1){
                medicina = next_List(inventario);
                contador++;
            }
            else printf("Opcion no valida!\n");
            break;
        case 'w':
            *medicini = medicina;
            return;
        case 's':
            //
            *medicini = NULL;
            return;
        default:
            printf("Opcion no valida!\n");
            break;
        }
    }while(opcion != 's');
}


/*================================================================================
-------------------------------------ADMINISTRAR----------------------------------
FUNCIONALIDAD: Administra el medicamento al paciente, en caso de que sea el
correcto el paciente se cura, en caso contrario cambia su enfermedad y le resta
1 al contador de dias de vida restantes.

PARÁMETROS: puntero de tipo paciente, un mapa Hash, un puntero de tipo medicamento
y un valor entero el cual permite mostrar mensajes 
si es que esta activo.

COMPLEJIDAD: o(1)
================================================================================*/
void administrar(Paciente* paciente,HashMap* enfermedades,Medicamento* medicina,int debug){
    if (strcmp(paciente->enfermedad->cura,medicina->nombre)== 0){//si es cura
        paciente->enfermedad = (searchMap(enfermedades,"sano"))->value; //ahora es el nodo sano
        printf("\nEl paciente ha sido curado!\n");
    }
    else{
        paciente->tiempoVida--;
        generar_nueva_enfermedad(paciente,debug);
        printf("\nEl paciente ha desarrollado una nueva enfermedad!\n");
    }
}


/*================================================================================
------------------------------------ATENDER---------------------------------------
FUNCIONALIDAD: Es la funcion que muestra el menú para atender al paciente y contiene
las funciones para realizar dichas acciones del menú.

PARÁMETROS: Mapa con los medicamentos, mapa con las enfermedades, lista con los
pacientes para atender, lista con los medicamentos (inventario), valor entero 
que permite mostrar mensajes si es que está activo, entero que indica si la 
partida ya ha terminado.

COMPLEJIDAD: o(1)
================================================================================*/
void atender(HashMap* medicamentos,HashMap*enfermedades,List* pacientesActivos, List* inventario,int debug,int *esFinal){
    if (*esFinal == 1){
        printf("\nLa partida ya termino.\nRevise sus estadisticas\n");
        return;
    }
    Paciente* paciente;
    Medicamento* medicina;
    paciente = (Paciente*)(first_List(pacientesActivos));
    if (paciente == NULL){
        printf("No tiene pacientes en su lista!\n");
        return;
    }
    
    Enfermedad* nodoSano = (searchMap(enfermedades,"sano"))->value;

    char opcion;
    char buffer[10];
    do{
        menuPaciente();
        fgets(buffer, sizeof(buffer), stdin);
        sscanf(buffer, " %c", &opcion);

        switch (opcion)
        {
        case '1':
            estadoPaciente(paciente);
            break;
        case '2':
            seleccionarMedicamento(paciente,inventario,&medicina);
            if (medicina == NULL){
                printf("No ha seleccionado una medicina!\n");
            } 
            else{
                administrar(paciente,enfermedades,medicina,debug);
                //si está sano se saca de la lista.
                if (paciente->enfermedad == nodoSano){
                    pop_Front(pacientesActivos);
                    paciente = first_List(pacientesActivos);
                }
                //si la vida es 0 termina el juego.
                else{
                    if (paciente->tiempoVida < 1){
                        printf("\nEl paciente ha muerto...\n");
                        printf("Fin de la partida.\n");
                        *esFinal = 1;
                        return;
                    }
                }
                //sino se continúa.
            } 
            //Si el paciente está sano, siguiente.
            break;
        case '3':
            //atrás
            return;
        default:
            printf("¡Opcion no valida!");
            break;
        }
    }while(opcion != '3');
}


/*================================================================================
----------------------------------- TOMAR ----------------------------------------
FUNCIONALIDAD: Función encargada de agregar medicamentos al inventario del jugador.
Solicita el sintoma para buscar y mostrar los medicamentos que curan ese sintoma.
Luego solicita el nombre del medicamento que desea agregar al inventario y los
añade.

PARÁMETROS: inventario del jugador, el mapa que contiene los medicamentos guardados
según los sintomas y el mapa que contiene todos los medicamentos ordenado por 
los nombres, (esfinal) entero que indica si la partida ya ha terminado.

COMPLEJIDAD: o(1)
================================================================================*/
void tomar(List* inventario, HashMap* mapaMedicamentoSintomas,HashMap* medicamentos,int esFinal){
    if (esFinal == 1){
        printf("\nLa partida ya termino.\nRevise sus estadisticas.\n\n");
        return;
    }

    //      buscar por sintoma
    char qSintoma[60];
    char buffer[70];

    printf("\nIngrese el sintoma que desea curar.\n");
    fgets(qSintoma, sizeof(qSintoma), stdin);
    
    qSintoma[strcspn(qSintoma, "\n")] = 0;  // elimina el \n

    Pair* par = searchMap(mapaMedicamentoSintomas,qSintoma);
    if (par == NULL){
        printf("Error en la busqueda. Quizas escribiste mal!\n");
        return;
    }
    List* lista = par->value;
    //      mostrar medicamentos que traen esos sintomas
    printf("\n=============================\n");
    printf("  Medicamentos disponibles:\n");
    printf("=============================\n");
    Medicamento* medicina = first_List(lista);
    while (medicina != NULL){
        printf("Nombre: %s\n",medicina->nombre);
        printf("Descripcion: %s\n",medicina->descripcion);
        printf("Sintomas:\n");
        char* sintoma = first_List(medicina->sintomasCura);
        while (sintoma != NULL){
            printf("- %s\n",sintoma);
            sintoma = next_List(medicina->sintomasCura);
        }
        printf("------------------------------------\n");
        medicina = next_List(lista);
    }
    //      ingresar el nombre del medicamento a añadir
    printf("\nIngrese el nombre del medicamento que desea agregar al inventario.\n");
    //fgets(buffer, sizeof(buffer), stdin);
    fgets(qSintoma, sizeof(qSintoma), stdin);
    //sscanf(buffer, " %59s", &qSintoma);
    qSintoma[strcspn(qSintoma, "\n")] = 0;  // elimina el \n
    
    Pair* otroPair = searchMap(medicamentos,qSintoma);
    if (otroPair == NULL){
        printf("Error en la busqueda. Quizas escribiste mal!\n");
        return;
    }
    Medicamento* laMedicina = otroPair->value;
    //      Se agrega el medicamento al inventario.

    push_Front(inventario,laMedicina);
    printf("\nMedicamento agregado!\n");
}


/*================================================================================
-------------------------------------MAIN-----------------------------------------
FUNCIONALIDAD: Main del programa.

PARÁMETROS: Ninguno.
================================================================================*/
int main(){
    int esFinal = 0;
    HashMap* enfermedades = createMap(250);
    HashMap* medicamentos = createMap(250);
    HashMap* sintomas = createMap(250);
    HashMap* pacientes = createMap(1000);
    HashMap* mapaMedicamentoSintomas = createMap(250);
    
    List* pacientesActivos = create_List(); // Crear lista de pacientes activos
    int debug = 0; //0 desactivado, 1 activado
    List* inventario = create_List();

    char buffer[10];
    char opcion;
    // - - - - - - - - - - PRE MENU - - - - - - - - - 
    do{
        mostrarPreMenu();
        
        fgets(buffer, sizeof(buffer), stdin);
        sscanf(buffer, " %c", &opcion);

        switch (opcion)
        {
        case '1':
            //cargar datos
            cargar_CSVS(enfermedades, medicamentos, pacientes, sintomas, mapaMedicamentoSintomas, debug);
            crearGrafo(enfermedades, sintomas, debug);
            printf("\nJuego Cargado correctamente!\n");
            break;
        case '2':
            break;
        case '3':
            mostrarLore();
            opcion = 0;
            break;
        case '4':
            //funcion comenzar tutorial
            break;
        case '5':
            mostrarInfo();
            opcion = 0;
            break;
        default:
            printf("\n  Opcion invalida!\n");
            break;
        }
    } while (opcion != '2');
    // - - - - - - - Menú - - - - - - - - - 
    do{
        mostrarMenu();

        fgets(buffer, sizeof(buffer), stdin);        // lee línea entera
        sscanf(buffer, " %c", &opcion);              // extrae primer caracter
        
        switch(opcion){

            case '1' :
                // nuevo paciente
                NuevoPaciente(pacientes, pacientesActivos, debug, esFinal);
                break;
            case '2' ://tomar medicamento
                tomar(inventario,mapaMedicamentoSintomas,medicamentos,esFinal);
                break;
            case '3' :
                //atender paciente
                mostrarPacientesActivos(pacientesActivos,esFinal);
                atender(medicamentos,enfermedades,pacientesActivos,inventario,debug,&esFinal);
                break;
            case '4' :
                //Terminar partida
                esFinal = 1;
                break;
            case '5' :
                //salir
                break;
            default:
                printf("\n  Opcion invalida!\n");
                break;
        }

        presioneTeclaParaContinuar();

    }while(opcion != '5' && esFinal  != 1);

    return 0;
}

