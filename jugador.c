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
    int buffOriginalDefensaFisica; //El valor original para rrestaurarr cunado temirne el debuff
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
    Requerimientos: una magia debe causar que el oponente pierda el turno que va a ser esta
*/
void congelar(void *objetivo)
{
    printf("--- Lanzando Congelación ---");

    //Bien, aqui convertimos el punter void* a nuestro tipo (personaje*)
    //En esta magia nomas neceitamos afectar al objetivo, no vamos a neceistar al atacante
    Personaje* personajeObjeto = (Personaje*)objetivo;


    //Aqui esta el efecto de congelacion
    //turnosCongelado = 1 (necesito que pierda 1 turno)
    personajeObjeto->turnosCongelado = 1;

    printf("¡Hechizo Congelar lanzado!\n");
    printf("%s esta congelado y perdera su siguiente turno\n", personajeObjeto->nombre);
}

/*
    Magia 3: Bendicion de fuerza
    Proporsito: Se aumenta el ataque fisico de forma temporal (nomas 3 turnos)
    Mecanica que pensamos: Solo se aumentta el ataque fisico y ponemos un contador para lo de contar el tiempo
    Tipo de magia: buff de mejora
    Requerimientos: Mejorar uno de los atributos por 3 turnos
*/
void bendicionFuerza(void* objetivo)
{
    printf("--- Lanzando Bendición de Fuerza ---");

    //Convierto el puntero void a tipo personaje
    Personaje* personajeObjetivo = (Personaje*)objetivo;

    //Aqui guardo el valor original si es la primera vez que recibe el buff
    //Mas que todo en este if permito que se restaure el valor original cuando termine el efecto
    if(personajeObjetivo->ataqueFisico == 0)
    {
        personajeObjetivo->originalAtaqueFisico = personajeObjetivo->ataqueFisico;
    }
    
    //Aqui aplicamos el aumento temporal
    int aumento = 3;
    personajeObjetivo->ataqueFisico = personajeObjetivo->buffOriginalDefensaFisica + aumento;
    personajeObjetivo->ataqueFisico = 3; //Aqui le digo explicitamente que solo 3 turnos

    printf("¡%s recibe la Bendición de Fuerza!\n", personajeObjetivo->nombre);
    printf("Ataque físico aumentado en %d puntos por 3 turnos\n", aumento);
    printf("Nuevo ataque fisico: %d\n", personajeObjetivo->ataqueFisico);
}

/*
    Magia 4: Maldicion de debilidad
    Proporsito: Reduce la defensa fisica del oponente temporalmente (por 3 turnos)
    Mecanica que pensamos: Reduce la defensa fisica y le ponemos un contador
    Tipo de magia: debuff/debilita
    Requerimientos: Reduce uno de los atributos por 3 turnos
*/
void maldicionDebilitadora(void* datos)
{
    printf("--- Lanzando Maldición Debilitadora ---");

    //Aqui vuelvo a convertir mi puntero generico a tipo DatosMagia*
    DatosMagia* datosMagia = (DatosMagia*)datos;
    Personaje* atactante = datosMagia->atacante;
    Personaje* objetivo = datosMagia->objetivo;

    //Primero: Ahora guardo el valor original si es la primera vez que recibe el debuff
    if(objetivo->debuffDefensaFisica == 0)
    {
        objetivo->debuffDefensaFisica = objetivo->defensaFisica;
    }

    //Segundo: Aplicamos la reduccion temporal a la defensaFisica
    int reduccion = 2;
    objetivo->defensaFisica = objetivo->buffOriginalDefensaFisica - reduccion;

    //Tercero: Asegurar que la defensa no sea menor a 1
    if(objetivo->defensaFisica < 1)
    {
        objetivo->defensaFisica = 1;
    }

    //Cuarto: Ponemos la duracion del efecto
    objetivo->debuffDefensaFisica = 3;

    printf("%s lanza Maldicion Debilitadora sobre %s!\n", atactante->nombre, objetivo->nombre);
    printf("Defensa física reducida en %d puntos por 3 turnos\n", reduccion);
    printf("Nueva defensa fisica de %s: %d\n", objetivo->nombre, objetivo->defensaFisica);
}

/*
    Magia 5: Sanacion Divina
    Proporsito: Cura daño al personaje aliado
    Mecanica que pensamos: Restaura una cantidad fija de daños 
    Tipo de magia: De curacion
*/
void sanacionDivina(void* objetivo)
{
    printf("--- Lanzando Sanación Divina ---");

    //Convierto el puntero a tipo Personaje*
    Personaje* personajeObjetivo = (Personaje*)objetivo;

    //Puntos de daño que se curan
    int curacion = 8;

    //Aplico la curacion reduciiondo el daño acumulado
    personajeObjetivo->danio -= curacion;

    //El daño no puede ser negativo
    if(personajeObjetivo->danio < 0)
    {
        personajeObjetivo->danio = 0;
    }

    printf("¡%s recibe Sanación Divina!\n", personajeObjetivo->nombre);
    printf("Restaurados %d puntos de vida.\n", curacion);
    printf("Vida actual: %d/%d\n", personajeObjetivo->HP - personajeObjetivo->danio, personajeObjetivo->HP);
}

//Aqui creamos una funcion que lo que hace es actualizar los efectos temporales
//Reducimos los contadores de efectos temporales por cada turno
void actualizarEfectosTemporales(Personaje* personaje) {
    //1-Descongelación
    if (personaje->turnosCongelado > 0) 
    {
        personaje->turnosCongelado--;  //Reducimos el contador de congelacion
        if (personaje->turnosCongelado == 0) 
        {
            printf("¡%s se descongela!\n", personaje->nombre);
        }
    }
    
    //2-Fin de bendición de fuerza
    if (personaje->ataqueFisico > 0) {
        personaje->ataqueFisico--;  //Reducir contador de buff
        if (personaje->ataqueFisico == 0) 
        {
            //Restauro el valor original del ataque fisico
            personaje->ataqueFisico = personaje->originalAtaqueFisico;
            printf("¡El efecto de Bendición de Fuerza desaparece de %s!\n", personaje->nombre);
            printf("Ataque físico vuelve a: %d\n", personaje->ataqueFisico);
        }
    }
    
    //3-Fin de maldición debilitadora
    if (personaje->debuffDefensaFisica > 0) 
    {
        personaje->debuffDefensaFisica--;  //Reducir contador de debuff
        if (personaje->debuffDefensaFisica == 0) 
        {
            //Restaurar valor original de la defensa fisica
            personaje->defensaFisica = personaje->buffOriginalDefensaFisica; 
            printf("¡El efecto de Maldición Debilitadora desaparece de %s!\n", personaje->nombre);
            printf("Defensa física vuelve a: %d\n", personaje->defensaFisica);
        }
    }
}

//Creamos una funcion que inicializa las magias
//Esto mas que todo es para que el usuario pueda seleccionar 3
void inicializarMagiasDisponibles(void (**magiasDisponibles)(void*)) 
{
    magiasDisponibles[0] = bolaFuego;           //Magia 1: Daño directo
    magiasDisponibles[1] = congelar;            //Magia 2: Pierde turno
    magiasDisponibles[2] = bendicionFuerza;     //Magia 3: Buff temporal
    magiasDisponibles[3] = maldicionDebilitadora; //Magia 4: Debuff temporal
    magiasDisponibles[4] = sanacionDivina;      //Magia 5: Curación
}

void mostrarMagiasDisponibles() 
{
    printf("\n--- MAGIAS DISPONIBLES ---\n");
    printf("1. Bola de Fuego    - Daño mágico basado en ataque/defensa\n");
    printf("2. Congelar         - El oponente pierde su próximo turno\n");
    printf("3. Bendición Fuerza - Aumenta ataque físico por 3 turnos\n");
    printf("4. Maldición Debil. - Reduce defensa física por 3 turnos\n");
    printf("5. Sanación Divina  - Cura 8 puntos de daño\n");
}

int main(int argc, char const *argv[])
{
    // aqui un if que tenga que si la vida que esta como hp llega a el mismo valor que daño entonces se pierde 
    // la cosa es que esto  es en cada pelea asi que va dentro de cada siclo por asi decirlo 
}