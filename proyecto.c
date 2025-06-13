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
    int id;
    int tiempoVida;
    Enfermedad* enfermedad;
}Paciente;

typedef struct{
    char nombre[100];
    char descripcion[300];
    List* sintomasCura;
}Medicamento;



//FUNCIONES

void cargar_CSVS(HashMap* enfermedades, HashMap* medicamentos, List* aux){
    return;
}

int main(){

    HashMap* enfermedades = createMap(250);
    HashMap* medicamentos = createMap(250);
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
                //cargar_CSVS(enfermedades, medicamentos, listaAux);
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
