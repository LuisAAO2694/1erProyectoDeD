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

//Estructura para las magias
//La necesitamos. porque las magias solo pueden recibir 1 o 2 void*
typedef struct 
{
    Personaje* atacante;   // Puntero al personaje que lanza la magia
    Personaje* objetivo;   // Puntero al personaje que recibe la magia
} DatosMagia;


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

    //btw buff = efecto posoitvo temporal que mejora algo "Solamente es un termino xd"
    //debuff = igual efecto temporal que decrementa algo

    //Contadores para los efectos temporales
    int turnosCongelado; //En caso de que (mayor que) > 0, el perosnaje pierde turnos
    int ataqueFisico; //Este esun simple contador que cuenta los turnosrestantes para el buff de ataque
    int originalAtaqueFisico; //Valor original para restaurar cunado termine el buff
    int debuffDefensaFisica; //Contador de. turnos. rerstantes para el debuff de defensa
    int buffOrginialDefensaFisica; //El valor original para rrestaurarr cunado temirne el debuff
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


/*
    Magia 1: Bola de fuego
    Proporsito: Causar daño mágico al oponente
    Mecanica que pensamos: Daño = ataque magico(atacante) - defensa magica(objetivo)
    Tipo de magia: Ofensiva directa
*/
void bolaFuego(void* datos)
{
    printf("--- Lanzando Bola De Fuego ---\n");

    //Primero convierto. el puntero (void*) al tipo que correcto
    //En este caso loc onvierto a un DatosMagia* para poder acceder a los camposque definimos
    DatosMagia* datosMagia = (DatosMagia*)datos;

    //Ahora extraigo los personajes de la estructura
    Personaje* atacante = datosMagia->atacante;
    Personaje* objetivo = datosMagia->objetivo;

    //Bien ahora pues tenemos que hacer daño
    //Formula: Ataque del lanzador - Defensa magica del objetivo
    int dano = atacante->ataqueMagico - objetivo->defensaMagica;

    //Ahora aseguramos que el daño minimo sea de 1
    //Btw evitamos que el daño sea 0 o negativo
    if(dano < 1)
    {
        dano = 1;
    }

    //Aplico el daño
    //Se incrementa el daño acumulado al personaje objetivo (sumo el daño y lo registro)
    objetivo->danio += dano;

    printf("!%s lanza una Bola de Fuego!!\n",atacante->nombre);
    printf("Ataque Magicco (%s): %d vs Defensa Magica (%s): %d\n", atacante->nombre, atacante->ataqueMagico, 
        objetivo->nombre, objetivo->defensaMagica);
    
    printf("!%s recie %d puntos de daño!!\n", objetivo->nombre, dano);
    printf("Vida restante de %s: %d/%d\n", objetivo->nombre, objetivo->HP - objetivo->danio, objetivo->HP);
}

/*
    Magia 2: Congelar
    Proporsito: Hacer que el oponente pierda su siguientte turno
    Mecanica que pensamos: Ponemos un contador que limite al objeto actuar el siguiente turno
    Tipo de magia: De control/estado
    Requerimientos: una magia debe causar que el oponente pierda el turno
*/
//En CONSTRUCCION....

int main(int argc, char const *argv[])
{
    // aqui un if que tenga que si la vida que esta como hp llega a el mismo valor que daño entonces se pierde 
    // la cosa es que esto  es en cada pelea asi que va dentro de cada siclo por asi decirlo 
}