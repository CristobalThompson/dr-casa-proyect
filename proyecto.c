#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "tdas/listasDobles.h"
#include "tdas/hashmap.h"
#include "tdas/extra.h"

// Structs


typedef struct{
    char nombre[100];
    List* sintomas;
    char cura[100];
    List* enfermedadesAdj;
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

void cargarMedicamentos(HashMap* medicamentos, char** campos, int debug){
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
        
        Medicamento* medicamento = malloc(sizeof(Medicamento)); if (medicamento == NULL) exit(1);

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
        enfermedad->enfermedadesAdj = create_List();
        push_Back(enfermedad->enfermedadesAdj, sano);

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

void cargar_CSVS(HashMap* enfermedades, HashMap* medicamentos, HashMap* pacientes, HashMap* sintomas, int debug){
    int contador = 1;
    char **campos;
    Pair* waza;
    if (debug) puts("-----------------------------------------");
    
    cargarMedicamentos(medicamentos, campos, debug);

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

void juntarListas(List* origen, List* agregar){
    for (Enfermedad* dato = first_List(agregar); dato != NULL;
        dato = next_List(agregar)){
            push_Back(origen, dato);
        }
}

void crearGrafo(HashMap* enfermedades, HashMap* mapaSintomas){
    Pair* par = firstMap(enfermedades);
    while(par != NULL){
        Enfermedad* nodo = par->value;
        List* sintomas = nodo->sintomas;
        char* sintoma = (char *) first_List(sintomas);
        while(sintoma != NULL){
            Pair* parDos = searchMap(mapaSintomas);
            if (parDos != NULL){
                List* AdjSintoma = parDos->value;
                juntarListas(nodo->enfermedadesAdj, adjSintoma);
            }
            sintoma = (char *) next_List(sintomas);
        }
        par = (Pair *) nextMap(enfermedades);
    }
    return;
}

int main(){

    HashMap* enfermedades = createMap(250);
    HashMap* medicamentos = createMap(250);
    HashMap* sintomas = createMap(250);
    HashMap* pacientes = createMap(1000);
    int debug = 1; //0 desactivado, 1 activado

    char buffer[10];
    char opcion;
    do{
        //mostrarMenu();

        fgets(buffer, sizeof(buffer), stdin);        // lee línea entera
        sscanf(buffer, " %c", &opcion);              // extrae primer caracter
        
        switch(opcion){

            case '1' :
                //cargar datos
                cargar_CSVS(enfermedades, medicamentos, pacientes, sintomas, debug);
                //crearGrafo(enfermedades, sintomas);
                printf("\nJuego Cargado correctamente!\n");
                break;
            case '2' :
                //???
                break;
            case '3' :
                //???
                break;
            case '4' :
                //???
                break;
            case '5' :
                //salir
                break;
            default:
                printf("Opción invalida!\n");
                break;
        }

        presioneTeclaParaContinuar();

    }while(opcion != '5');

    return 0;
}
