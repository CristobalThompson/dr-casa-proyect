#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "tdas/listasDobles.h"
#include "tdas/hashmap.h"
#include "tdas/extra.h"

// Structs

typedef struct{
    int id;
    int tiempoVida;
    char enfermedad[300];
    List* sintomas;
}paciente;

typedef struct{
    char nombre[100];
    char descripcion[300];
    List* sintomasCura;
    List* sintomasSecundarios;
}medicamento;


typedef struct{
    char nombre[100];
    List* sintomas;
    char cura[100];
}enfermedad;



//FUNCIONES


int main(){

    char buffer[10];
    char opcion;
    do{
        //mostrarMenu();

        fgets(buffer, sizeof(buffer), stdin);        // lee línea entera
        sscanf(buffer, " %c", &opcion);              // extrae primer caracter
        
        switch(opcion){

            case '1' :
                //cargar datos
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
