#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
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

void cargarMedicamentos(HashMap* medicamentos, char** campos){
    FILE *archivoMedicamentos = fopen("data/medicamentos.csv", "r");
    if (archivoMedicamentos == NULL){
        perror("Error al abrir medicamentos");
        return;
    }
    campos = leer_linea_csv(archivoMedicamentos, ',');
    int contador = 1;
    while((campos = leer_linea_csv(archivoMedicamentos, ',')) != NULL){
        
        Medicamento* medicamento = malloc(sizeof(Medicamento)); if (medicamento == NULL) exit(1);

        //-------------------------------copiar cadenas-------------------------------------
        strncpy(medicamento->nombre, campos[0], sizeof(medicamento->nombre) - 1);
        medicamento->nombre[sizeof(medicamento->nombre) - 1] = '\0';

        strncpy(medicamento->descripcion, campos[1], sizeof(medicamento->descripcion) - 1);
        medicamento->descripcion[sizeof(medicamento->descripcion) - 1] = '\0';
        //-------------------------------------FIN------------------------------------------

        medicamento->sintomasCura = split_string(campos[2], ";");

        if (contador <= 5) {
            printf("%s\n%s\n\n", medicamento->nombre, medicamento->descripcion);
            for(char *sintoma = first_List(medicamento->sintomasCura); sintoma != NULL; 
                sintoma = next_List(medicamento->sintomasCura)){
                printf("sintoma: '%s'\n", sintoma);
          }
          printf("\n\n");
        }
        contador++;
        insertMap(medicamentos, strdup(medicamento->nombre), medicamento);
    }
    fclose(archivoMedicamentos);
}

void cargarEnfermedades(HashMap* enfermedades, char** campos){
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

        if (contador <= 5) {
            printf("%s\n%s\n\n", enfermedad->nombre, enfermedad->cura);
            for(char *sintoma = first_List(enfermedad->sintomas); sintoma != NULL; 
                sintoma = next_List(enfermedad->sintomas)){
                printf("sintoma: '%s'\n", sintoma);
          }
          printf("\n\n");
        }
        contador++;
        insertMap(enfermedades, strdup(enfermedad->nombre), enfermedad);
    }
    fclose(archivoEnfermedades);
}

void cargarPacientes(HashMap* pacientes, HashMap* enfermedades, char** campos){
    FILE *archivoPacientes = fopen("data/pacientes.csv", "r");
    if (archivoPacientes == NULL){
        perror("Error al abrir enfermedades");
        return;
    }

    campos = leer_linea_csv(archivoPacientes, ',');

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
        
        insertMap(pacientes, strdup(paciente->nombre), paciente);
    }
    fclose(archivoPacientes);
}

void cargar_CSVS(HashMap* enfermedades, HashMap* medicamentos, HashMap* pacientes, List* aux){
    int contador = 1;
    char **campos;

    cargarMedicamentos(medicamentos, campos);

    Pair* waza = (Pair*) firstMap(medicamentos);
    while(waza != NULL && contador <= 5){
        Medicamento* pos = waza->value;
        printf("Nodo del mapa medicamentos\n");
        printf("medicamento: %s\ndescripcion: %s\n\n", pos->nombre, pos->descripcion);
        for(char *sintoma = first_List(pos->sintomasCura); sintoma != NULL; 
                sintoma = next_List(pos->sintomasCura)){
                printf("sintoma: '%s'\n", sintoma);
          }
          printf("\n\n");
          waza = (Pair*) nextMap(medicamentos);
          ++contador;
    }

    cargarEnfermedades(enfermedades, campos);

    contador = 1;
    waza = (Pair*) firstMap(enfermedades);
    while(waza != NULL && contador <= 5){
        Enfermedad* value = waza->value;
        printf("Nodo del mapa enfermedades\n");
        printf("enfermedad: %s\ncura: %s\n\n", value->nombre, value->cura);
        for(char *sintoma = first_List(value->sintomas); sintoma != NULL; 
                sintoma = next_List(value->sintomas)){
                printf("sintoma: '%s'\n", sintoma);
          }
          printf("\n\n");
          waza = (Pair*) nextMap(enfermedades);
          ++contador;
    }

    cargarPacientes(pacientes, enfermedades, campos);

    contador = 1;
    waza = (Pair*) firstMap(pacientes);
    while(waza != NULL && contador <= 5){
        Paciente* value = waza->value;
        printf("Nodo del mapa pacientes\n");
        printf("nombre: %s\nEnfermedad: %s\n\n", value->nombre, value->enfermedad->nombre);
        waza = (Pair*) nextMap(pacientes);
        ++contador;
    }
}

int main(){

    HashMap* enfermedades = createMap(250);
    HashMap* medicamentos = createMap(250);
    HashMap* sintomas = createMap(250);
    HashMap* pacientes = createMap(1000);
    List* listaAux = create_List();

    char buffer[10];
    char opcion;
    do{
        //mostrarMenu();

        fgets(buffer, sizeof(buffer), stdin);        // lee línea entera
        sscanf(buffer, " %c", &opcion);              // extrae primer caracter
        
        switch(opcion){

            case '1' :
                //cargar datos
                cargar_CSVS(enfermedades, medicamentos, pacientes, listaAux);
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
