#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_NOMBRE 50

// Estructura de los obetos 
typedef struct {
    char nombre[MAX_NOMBRE]; 
    char atributo[20];      
    int poder;               
} Objeto;

const char *nombresBase[] = {
    "Espada", "Amuleto", "Anillo", "Escudo", "Armadura",
    "Poción", "Cristal", "Talisman", "Corona", "Guantelete"
};

const char *efectosAtributos[] = {
    "HP", "Ataque Fisico", "Ataque Magico", "Defensa Fisica", "Defensa Magica"
};

Objeto *crearObjetoAleatorio() {
    Objeto *nuevo = (Objeto *)malloc(sizeof(Objeto));
    if (nuevo == NULL) {
        printf("Error: No se pudo asignar memoria para el objeto.\n");
        return NULL;
    }

    // aqui si establece el objeto de os aleatorios previos y tambien sus atrivutos 
    int idxNombre = rand() % (sizeof(nombresBase) / sizeof(nombresBase[0]));
    int idxAtributo = rand() % (sizeof(efectosAtributos) / sizeof(efectosAtributos[0]));

    // combina los nombres 
    snprintf(nuevo->nombre, MAX_NOMBRE, "%s de %s", nombresBase[idxNombre], efectosAtributos[idxAtributo]);

    strcpy(nuevo->atributo, efectosAtributos[idxAtributo]);

    nuevo->poder = (rand() % 5) + 1;

    return nuevo;
}

//  para liberar la memoria 
void liberarObjeto(Objeto *obj) {
    if (obj != NULL) {
        printf("Liberando objeto: %s\n", obj->nombre);
        free(obj);
    }
}

// Eje
int main() {
    srand(time(NULL));

    Objeto *o1 = crearObjetoAleatorio();
    Objeto *o2 = crearObjetoAleatorio();
    Objeto *o3 = crearObjetoAleatorio();

    printf("Objeto 1: %s | Atributo: %s | Poder: %d\n", o1->nombre, o1->atributo, o1->poder);
    printf("Objeto 2: %s | Atributo: %s | Poder: %d\n", o2->nombre, o2->atributo, o2->poder);
    printf("Objeto 3: %s | Atributo: %s | Poder: %d\n", o3->nombre, o3->atributo, o3->poder);

    liberarObjeto(o1);
    liberarObjeto(o2);
    liberarObjeto(o3);

    return 0;
}
// con este codigo aora solo falta pues con esos atributos modificar tanto a el jugador como a los enemigos 
