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
    char descripcion[350];
    List* sintomasCura;
}Medicamento;



//FUNCIONES

void cargar_CSVS(HashMap* enfermedades, HashMap* medicamentos, List* aux){
    FILE *archivoMedicamentos = fopen("data/medicamentos.csv", "r");
    if (archivoMedicamentos == NULL){
        perror("Error al abrir medicamentos");
        return;
    }

    char **campos;

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
    }
    fclose(archivoMedicamentos);


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
                cargar_CSVS(enfermedades, medicamentos, listaAux);
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
