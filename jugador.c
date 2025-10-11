#include <unistd.h>
#include <stdio.h>

# define MAX_NOMBRE 50;
// aqui podemos definir que la cantidad de magias maximas es de 3
// # define MAX_MAGIAS 3;

#include <stdio.h>

#define MAX_NOMBRE 50
#define MAX_MAGIAS 3
#define MAX_OBJETOS 5

// Declaración adelantada de estructuras
struct Objeto;

// Estructura base para todos los personajes (jugador y enemigos)
typedef struct {
    char nombre[MAX_NOMBRE];           // Nombre del personaje
    int HP;                            // Puntos de vida máxima
    int danio;                         // Daño recibido
    int ataqueFisico;                  // Ataque físico
    int ataqueMagico;                  // Ataque mágico
    int defensaFisica;                 // Defensa física
    int defensaMagica;                 // Defensa mágica
    void (*magia[MAX_MAGIAS])(void *); // apuntador a las magias 
    struct Objeto *inventario[MAX_OBJETOS]; // apuntador a los objetos 
} Personaje;

// Estructuras para los distintos jefes (heredan la base del personaje)
typedef struct {
    Personaje base;   // Enemigo fácil
} EnemigoFacil;

typedef struct {
    Personaje base;   // Enemigo intermedio
} EnemigoIntermedio;

typedef struct {
    Personaje base;   // Enemigo difícil
} EnemigoDificil;

typedef struct {
    Personaje base;   // Jefe final: Dragón de Obsidiana
} JefeFinal;


int main(int argc, char const *argv[])
{
    // aqui un if que tenga que si la vida que esta como hp llega a el mismo valor que daño entonces se pierde 
    // la cosa es que esto  es en cada pelea asi que va dentro de cada siclo por asi decirlo 
}