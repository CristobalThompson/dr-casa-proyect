#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "tdas/listasDobles.h"
#include "tdas/hashmap.h"
#include "tdas/extra.h"
#include <windows.h>

// Structs


typedef struct{
    char nombre[100];
    List* sintomas;
    char cura[100];
    HashMap* enfermedadesAdj;
}Enfermedad;

typedef struct{
    char nombre[100];
    int id;
    int tiempoVida;
    Enfermedad* enfermedad;
}Paciente;

typedef struct{
    char nombre[100];
    char descripcion[350];
    List* sintomasCura;
}Medicamento;



//FUNCIONES

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

        // Copiar nombre y descripción
        strncpy(medicamento->nombre, campos[0], sizeof(medicamento->nombre) - 1);
        medicamento->nombre[sizeof(medicamento->nombre) - 1] = '\0';

        strncpy(medicamento->descripcion, campos[1], sizeof(medicamento->descripcion) - 1);
        medicamento->descripcion[sizeof(medicamento->descripcion) - 1] = '\0';

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

        // --- NUEVO BLOQUE PARA LLENAR mapaMedicamentoSintomas ---
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
        // ---------------------------------------------------------
    }

    fclose(archivoMedicamentos);
}

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


void generar_nueva_enfermedad(Paciente* paciente, int debug){
    Enfermedad* actual = paciente->enfermedad;
    long pos = rand();
    Enfermedad* nueva = posMap(actual->enfermedadesAdj, pos, "sano");
    
    if (nueva != NULL) paciente->enfermedad = nueva;

    if (debug){
        printf("\n[DEBUG] Cambiando de enfermedad a una adyacente\n");
        printf("[DEBUG] enfermedad actual '%s'\n", actual->nombre);
        printf("[DEBUG] se genero el numero %d\n", pos);
        if (nueva == NULL) printf("[DEBUG] NO EXISTE NUEVA ENFERMEDAD\n");
        else printf("[DEBUG] nueva enfermedad '%s'\n", nueva->nombre);
    }
}

void NuevoPaciente(HashMap* pacientes, List* pacientesActivos, int debug) {
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

    static int semillaInicializada = 0;
    if (!semillaInicializada) {
        srand(time(NULL));
        semillaInicializada = 1;
    }

    long posicionAleatoria = rand() % totalPacientes;
    Pair* parActual = firstMap(pacientes);
    for (long i = 0; i < posicionAleatoria && parActual != NULL; i++) {
        parActual = nextMap(pacientes);
    }

    if (parActual == NULL || parActual->value == NULL) {
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
    pacienteSeleccionado->tiempoVida = generarDias();

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



void mostrarPacientesActivos(List* pacientesActivos) {
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

void imprimirCinematica(const char* texto, unsigned int delayMilisegundos) {
    for (int i = 0; texto[i] != '\0'; i++) {
        printf("%c", texto[i]);
        fflush(stdout);
        Sleep(delayMilisegundos);  // Sleep usa milisegundos en Windows
    }
}

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


void mostrarPreMenu(){
    printf("\n        ¡Bienvenido a Side Effect!\n");
    printf("\nSeleccione su opción:\n");
    printf("1) Cargar datos.\n");
    printf("2) Jugar\n");
    printf("3) Lore\n");
    printf("4) Tutorial\n");
    printf("5) Información del juego\n");
}

void mostrarMenu(){
    printf("\n    Menu de opciones del jugador.\n");
    printf("1) Nuevo paciente.\n");
    printf("2) Mostrar lista de pacientes.\n");
    printf("3) Revisar paciente.\n");
    printf("4) Terminar partida.\n");
    printf("5) Salir del juego.\n");
}

int main(){

    HashMap* enfermedades = createMap(250);
    HashMap* medicamentos = createMap(250);
    HashMap* sintomas = createMap(250);
    HashMap* pacientes = createMap(1000);
    HashMap* mapaMedicamentoSintomas = createMap(250);
    List* pacientesActivos = create_List(); // Crear lista de pacientes activos
    int debug = 0; //0 desactivado, 1 activado

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
            //información del juego
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
            NuevoPaciente(pacientes, pacientesActivos, debug);
            break;
            case '2' :
                mostrarPacientesActivos(pacientesActivos);
                break;
            case '3' :
                //atender paciente
                break;
            case '4' :
                //Terminar partida
                break;
            case '5' :
                //salir
                break;
            default:
                printf("\n  Opción invalida!\n");
                break;
        }

        presioneTeclaParaContinuar();

    }while(opcion != '4');

    return 0;
}

