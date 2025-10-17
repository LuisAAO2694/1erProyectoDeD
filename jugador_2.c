#include <unistd.h>
#include <stdio.h>
#include <string.h>
//para el random  de las magias  y objetos ===>
#include <time.h>
//para el uso de malloc y free
#include <stdlib.h>

// aqui podemos definir que la cantidad de magias maximas es de 3
// # define MAX_MAGIAS 3;

#define MAX_NOMBRE 50
#define MAX_MAGIAS 3
#define MAX_OBJETOS 5

//Aqui hacemos una estructura tipo objeto
/*
    atributo es:
    0=HP
    1=AtaqueFisico
    2=AtaqueMagico
    3=DefensaFisica
    4=DefensaMagica
*/
typedef struct
{
    char nombre[MAX_NOMBRE];
    int atributo;
    int poder; //Num random entre 1 y 5
} Objeto;


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
    Objeto *inventario[MAX_OBJETOS]; // apuntador a los objetos 

    //btw buff = efecto posoitvo temporal que mejora algo "Solamente es un termino xd"
    //debuff = igual efecto temporal que decrementa algo

    //Contadores para los efectos temporales
    int turnosCongelado; //En caso de que (mayor que) > 0, el perosnaje pierde turnos
    int buffAtaqueFisico; //Este esun simple contador que cuenta los turnosrestantes para el buff de ataque
    int originalAtaqueFisico; //Valor original para restaurar cunado termine el buff
    int debuffDefensaFisica; //Contador de. turnos. rerstantes para el debuff de defensa
    int buffOriginalDefensaFisica; //El valor original para rrestaurarr cunado temirne el debuff
} Personaje;


//Estructura para las magias
//La necesitamos. porque las magias solo pueden recibir 1 o 2 void*
typedef struct 
{
    Personaje* atacante;   // Puntero al personaje que lanza la magia
    Personaje* objetivo;   // Puntero al personaje que recibe la magia
} DatosMagia;


//Creamos un arreglo de nombres para los objetos
const char *nombresBaseObj[] =
{
    "Espada", "Amuleto", "Anillo", "Escudo", "Armadura",
    "Poción", "Cristal", "Talisman", "Corona", "Guantelete",
    "Báculo", "Daga", "Arco", "Hacha", "Marte", "Anillo", "Medallon de Miquiztli"
};

//Aqui creamos otro arreglo de nombres pero de atributos del usuario
const char *nombresAtributos[] = 
{
    "HP", "Ataque Fisico", "Ataque Magico", "Defensa Fisica", "Defensa Magica"
};

//======================================================================================
//Aqui creamos una funcion que "crear" un objeto dinamico con atributos aleatorios
//Tenemos que retornar un puntero a objeto (debe de liberarse con free!!!) //No es void porque neceiistamos retornar un objeto
Objeto *crearObjetoAleatorio()
{
    //Objetos con memoria dinamica
    Objeto *nuevo = (Objeto*)malloc(sizeof(Objeto));
    //Esto nomas para manejo de errores, checar si hay memeoria dispnible para el objeto
    if(nuevo == NULL)
    {
        printf("==No se pudo asignar memoria para el objeto==\n");
        return NULL;
    }
    
    //Faltan cosas....
    //Generar indices aleatorios
    // FER : ya estan añadidas :)
    // con el random para cada generar el valor añadido a las estadisticas del jugador 
    // y para seleccionar a donde se van a modificar de la estructura

    int idxNombre = rand() % (sizeof(nombresBaseObj) / sizeof(nombresBaseObj[0]));
    int idxAtributo = rand() % 5;

    // Crear el nombre del objeto nuevo 
    snprintf(nuevo->nombre, MAX_NOMBRE, "%s de %s", nombresBaseObj[idxNombre], nombresAtributos[idxAtributo]);

    snprintf(nuevo->nombre, MAX_NOMBRE, "%s de %s", 
             nombresBaseObj[idxNombre], nombresAtributos[idxAtributo]);
    nuevo->atributo = idxAtributo;
    //se le añade el atributo de el objeto como 5 puntos de vida o asi 
    nuevo->poder = (rand() % 5) + 1;

    return nuevo;
}  

// esto es para aplicar cada objeto a la estructura del enemigo o jugador 
// esto con el random lo limitamos a numeros del 1 al 5 asi asignamos el valor por medio del nombre del array de atributos
void aplicarObjeto(Personaje *personaje, Objeto *objeto) {
    switch(objeto->atributo) {
        case 0: // se agrega vida a el jugador en cuestion
            personaje->HP += objeto->poder;
            break;
        case 1: // se le añade ataque fisico al jugador en cuestion 
            personaje->ataqueFisico += objeto->poder;
            break;
        case 2: // se le añade ataque magico al jugador en cuestion
            personaje->ataqueMagico += objeto->poder;
            break;
        case 3: // se le añade defensa fisica al jugador en cuestio
            personaje->defensaFisica += objeto->poder;
            break;
        case 4: // se le añade dafensa magica al jugador en cuestion
            personaje->defensaMagica += objeto->poder;
            break;
    }
}

// Fer: lo volvi a cambiar xd: es una funcion que inicializa los enemigos  
// creacion del enemigo facil
void enemigoFacil(Personaje * enemigo){
    // traje la misma o muy parecida estructura para crear el personaje dej usuario
    enemigo->HP = 20;
    enemigo->danio = 0;
    enemigo->ataqueFisico = 5;
    enemigo->ataqueMagico = 5;
    enemigo->defensaFisica = 5;
    enemigo->defensaMagica = 5;
    enemigo->turnosCongelado = 0;
    enemigo->buffAtaqueFisico = 0;
    enemigo->debuffDefensaFisica = 0;

}

// funcion para enemigo intermedio
void enemigoInterMedio(Personaje * enemigo){
    enemigo->HP = 35;
    enemigo->danio = 0;
    enemigo->ataqueFisico = 7;
    enemigo->ataqueMagico = 7;
    enemigo->defensaFisica = 7;
    enemigo->defensaMagica = 7;
    enemigo->turnosCongelado = 0;
    enemigo->buffAtaqueFisico = 0;
    enemigo->debuffDefensaFisica = 0;

    // aqui se le tiene que asignar de forma aleatoria las magias y los objetos
    // inicialisamos el inventario del enemigo en NULL 
    for (int i = 0; i < MAX_OBJETOS; i++)
    {
        *(enemigo->inventario + i) = NULL; 
    }

    // manda a llamar dos veces la funcion de crearObjetos y a la ves los aplica con la funcion de los switches 
    for (int i = 0; i < 2; i++){
        Objeto *obj = crearObjetoAleatorio();
        if(obj != NULL) {
            *(enemigo->inventario + i )= obj;
            aplicarObjeto(enemigo, obj);
        }
    }
}

// Funcion para eneimgo dificl
void enemigoDificil(Personaje * enemigo2){
    enemigo2->HP = 45;
    enemigo2->danio = 0;
    enemigo2->ataqueFisico = 10;
    enemigo2->ataqueMagico = 10;
    enemigo2->defensaFisica = 10;
    enemigo2->defensaMagica = 10;
    enemigo2->turnosCongelado = 0;
    enemigo2->buffAtaqueFisico = 0;
    enemigo2->debuffDefensaFisica = 0;

    for (int i = 0; i < MAX_OBJETOS; i++)
    {
        *(enemigo2->inventario + i) = NULL; 
    }

    for (int i = 0; i < 3; i++){
        Objeto *obj = crearObjetoAleatorio();
        if(obj != NULL) {
            *(enemigo2->inventario + i )= obj;
            aplicarObjeto(enemigo2, obj);
        }
    }
}

// Funcion para el jefe final
void jefeFinal(Personaje * enemigo3){
    enemigo3->HP = 60;
    enemigo3->danio = 0;
    enemigo3->ataqueFisico = 13;
    enemigo3->ataqueMagico = 13;
    enemigo3->defensaFisica = 10;
    enemigo3->defensaMagica = 10;
    enemigo3->turnosCongelado = 0;
    enemigo3->buffAtaqueFisico = 0;
    enemigo3->debuffDefensaFisica = 0;

    for (int i = 0; i < MAX_OBJETOS; i++)
    {
        *(enemigo3->inventario + i) = NULL; 
    }

    for (int i = 0; i < 5; i++){
        Objeto *obj = crearObjetoAleatorio();
        if(obj != NULL) {
            *(enemigo3->inventario + i )= obj;
            aplicarObjeto(enemigo3, obj);
        }
    }
}

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

    //Aqui guardo el valor original, para poder restauralo cunado termine el buff
    personajeObjetivo->originalAtaqueFisico = personajeObjetivo->ataqueFisico;
    
    //Creo una variable temporal que me sirve para definir cuanto aumenta el ataque
    //Para despues aplicarlo example(originalAtaqueFisico = 5 y aumento = 3)
    //ataqueFisico = 5 + 3 = 8
    int aumento = 3;
    personajeObjetivo->ataqueFisico = personajeObjetivo->originalAtaqueFisico + aumento;
    
    //Aqui establezco un contador para la duracion del buff
    // 3 = tres turnos
    personajeObjetivo->buffAtaqueFisico = 3;

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
    if (personaje->buffAtaqueFisico > 0) 
    {
        personaje->buffAtaqueFisico--;  //Reducir contador de buff
        if (personaje->buffAtaqueFisico == 0) 
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
    *(magiasDisponibles + 0) = bolaFuego;           //Magia 1: Daño directo
    *(magiasDisponibles + 1) = congelar;            //Magia 2: Pierde turno
    *(magiasDisponibles + 2) = bendicionFuerza;     //Magia 3: Buff temporal
    *(magiasDisponibles + 3) = maldicionDebilitadora; //Magia 4: Debuff temporal
    *(magiasDisponibles + 4) = sanacionDivina;      //Magia 5: Curación
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

//=== Crear Personaje ===
void crearPersonajeJugador(Personaje* jugador)
{
    printf("\n");
    printf("╔══════════════════════════════════════════════╗\n");
    printf("║           CREACIÓN DE PERSONAJE              ║\n");
    printf("╚══════════════════════════════════════════════╝\n\n");

    printf("Elige el nombre de tu personaje\n");
    printf("─────────────────────────────────────\n");
    printf("Anda sin miedo, solo quiero saber quien eres, dime tu nombre: ");

    //Aqui primero obtenemos el nombre del teclado con el fgets
    char nombreTemp[MAX_NOMBRE];
    //Aqui leo el teclado
    fgets(nombreTemp, sizeof(nombreTemp), stdin);

    //Despues podemos eliminar el salto de linea que el fgets crea
    //Investigamos y usamos strcspn que encuentra la posicion del \n y lo remplaza con \0
    //strcspn(void * str, void * search);
    nombreTemp[strcspn(nombreTemp, "\n")] = 0;

    //Aqui copiamos el nombre en limpio a la struct del personaje
    //strcpy(destino, origen)
    strcpy(jugador->nombre, nombreTemp);
    printf("✅ Nombre Guardado: %s\n\n", jugador->nombre);

    //===Inicializar los atributos ===
    printf("Inicializando tus atributos del persoanje...\n");
    printf("─────────────────────────────────────\n");
    sleep(5);

    //Aqui configuramos los atributos base del personaje
    jugador->HP = 20;
    jugador->danio = 0;
    jugador->ataqueFisico = 5;
    jugador->ataqueMagico = 5;
    jugador->defensaFisica = 5;
    jugador->defensaMagica = 5;

    //Aqui inicializo los contadores
    jugador->turnosCongelado = 0;
    jugador->buffAtaqueFisico = 0;
    jugador->debuffDefensaFisica = 0;

    //Inicializamos el inventario vacio=NULL
    for (int i = 0; i < MAX_OBJETOS; i++)
    {
        *(jugador->inventario + i) = NULL; 
    }

    printf("✅ Atributos base inicializados:\n");
    printf("   - Vida: %d HP\n", jugador->HP);
    printf("   - Ataque Físico: %d\n", jugador->ataqueFisico);
    printf("   - Ataque Mágico: %d\n", jugador->ataqueMagico);
    printf("   - Defensa Física: %d\n", jugador->defensaFisica);
    printf("   - Defensa Mágica: %d\n\n", jugador->defensaMagica);

    

    printf("Selección de Magias\n");
    printf("─────────────────────────────────────\n");
    printf("\nno es mucho pero ten te sera util para vivir\n");
    printf("Elige 3 magias de las 5 disponibles para tu aventura\n");
    printf("¡Elige sabiamente, no podrás cambiarlas después!\n\n");

    //Mandamos a llamar a la funcion mostrarMagiasDisponibles, para ver las que estan disponibles
    mostrarMagiasDisponibles();
    
    //Inicializamos un arreglo de punteros a funciones magicas
    void (*magiasDisponibles[5])(void*);
    //Le asignamos al arreglo un puntero a cada funcion magica
    inicializarMagiasDisponibles(magiasDisponibles);

    printf("\nSELECCIONA TUS 3 MAGIAS (ingresa los números 1-5):\n");
    printf("─────────────────────────────────────────────────────\n");

    //Creamos un contador de las magias elegidas
    int magiasSeleccionadas = 0;
    int selecciones[MAX_MAGIAS]; //Declaracion de las magias limite

    //Inicializamos el arreglo de selecciones con -1 (sin la seleccion)
    for(int i = 0; i < MAX_MAGIAS; i++)
    {
        *(selecciones + i) = -1; //-1 porque el usuario no. ha seleccionado nada, la seleccion empieza en 1-5
    }
    
    //Aqui con el while creo un ciclo para seleccionar las 3 magias 
    while (magiasSeleccionadas < MAX_MAGIAS)
    {
        printf("\n Selecciona magia %d/%d: ", magiasSeleccionadas + 1, MAX_MAGIAS);
        
        //Leo el numero que ingrese el usuario
        int seleccion;
        scanf("%d", &seleccion);

        //Valido que seleccione el rango correcto
        if(seleccion < 1 || seleccion > 5) 
        {
            printf("Error: Por favor ingresa un número entre 1 y 5\n");
            continue;  // Volver al inicio del ciclo
        }
        
        //Verificar si la magia ya fue seleccionada anteriormente
        //Creamos nuestra bandera
        int yaSeleccionada = 0;

        //Aqui creamos un ciclo para checar todas las magias que se han seleccionado
        for (int i = 0; i < magiasSeleccionadas; i++) 
        {
            //Checar si la magia que el usuario seleccioino ya existe en la lista de selecciones
            if (*(selecciones + i) == seleccion) 
            {
                //Si se encuentra un duplicado cae aqui
                yaSeleccionada = 1;
                break; //Salimos
            }
        }
        
        //Si la bandera esta activada 
        if (yaSeleccionada) 
        {
            //Entro y mando mensaje 
            printf("Error: Ya seleccionaste esa magia. Elige una diferente\n");
            continue; //Vuelve al while
        }

        //Si la seleccion es valida y sobre todo no se repite, la guardamos
        *(selecciones + magiasSeleccionadas) = seleccion;

        //Asignamos el puntero a funcion magica del personaje
        //Resto 1 porque los arrays empiezan en 0 pero en conosla se vera como 1 (meramente visual)
        *(jugador->magia + magiasSeleccionadas) = *(magiasDisponibles + (seleccion - 1));

        //Extra - Le mostramos al usuario su confirmacion
        printf("Magia %d asignada: ", seleccion);
        switch(seleccion) 
        {
            case 1: printf("Bola de Fuego\n"); break;
            case 2: printf("Congelar\n"); break;
            case 3: printf("Bendición de Fuerza\n"); break;
            case 4: printf("Maldición Debilitadora\n"); break;
            case 5: printf("Sanación Divina\n"); break;
        }
        
        magiasSeleccionadas++;  //Incrementar contador de magias seleccionadas
    }

    printf("\n");
    printf("╔══════════════════════════════════════════════╗\n");
    printf("║           PERSONAJE CREADO EXITOSAMENTE      ║\n");
    printf("╚══════════════════════════════════════════════╝\n\n");

    printf("👤 NOMBRE: %s\n", jugador->nombre);
    printf("❤️ VIDA: %d HP\n", jugador->HP);
    printf("⚔️ ATAQUE FÍSICO: %d\n", jugador->ataqueFisico);
    printf("🔮 ATAQUE MÁGICO: %d\n", jugador->ataqueMagico);
    printf("🛡️ DEFENSA FÍSICA: %d\n", jugador->defensaFisica);
    printf("✨ DEFENSA MÁGICA: %d\n", jugador->defensaMagica);
    
    printf("\n🔮 MAGIAS SELECCIONADAS:\n");
    printf("───────────────────────\n");

    //Mostramos las magias que selecciono el usuario
    for (int i = 0; i < MAX_MAGIAS; i++) 
    {
        //Muestro el numero de la magia
        printf("%d. ", i + 1);

        //En este switch determina que magia mostrar en funcion de lo que se guardo por el usuario
        switch(*(selecciones + i)) //Acedemos al arreglo
        {
            case 1: printf("Bola de Fuego\n"); break;
            case 2: printf("Congelar\n"); break;
            case 3: printf("Bendición de Fuerza\n"); break;
            case 4: printf("Maldición Debilitadora\n"); break;
            case 5: printf("Sanación Divina\n"); break;
        }
    }
    
    printf("\nMucha suerte muchacho y que nada te sea impedimento para sobrevivir\n");
}

void mostrarMenuPrincipal()
{
    printf("\n");
    printf("╔══════════════════════════════════════════════╗\n");
    printf("║                 RPG DE TEXTO                 ║\n");
    printf("║                                              ║\n");
    printf("║  Diseño y Estructura de Datos - Proyecto 1   ║\n");
    printf("╚══════════════════════════════════════════════╝\n\n");

    getchar();
    
    printf("!!! O NO !!!\n");
    getchar();
    printf("Para tu mala suerte has sido victima del sueño y la desesperación\n");
    getchar();
    printf("Has entrado a un mundo peligroso, aun no sabes si producto de tus sueños\n");
    getchar();
    printf("Pero no temas, no te dejaremos morir y que nadie recuerde tu historia \n");
    getchar();
    printf("Por favor dime tu nombre y te bendecire con lo que tenga a la mano\n");
    getchar();
    printf("...\n\n");
    sleep(5);
}

void estadoDeBatalla(Personaje *jugador, Personaje *enemigo){

    printf("\n╔══════════════════════════════════════════════╗\n");
    printf("║                 ESTADO COMBATE               ║\n");
    printf("╠══════════════════════════════════════════════╣\n");
    printf("║ %-20s VS %-20s ║\n", jugador->nombre, enemigo->nombre);
    printf("║                                              ║\n");

    int vidaJugador = jugador->HP - jugador->danio;
    if (vidaJugador < 0) vidaJugador = 0;
    int vidaEnemigo = enemigo->HP - enemigo->danio;
    if (vidaEnemigo < 0) vidaEnemigo = 0;
}

void calcularDañoFis(Personaje * atacante, Personaje * recibe){
    int fak = 0;
    int danoReduc = 0;
    if (atacante -> ataqueFisico > recibe -> defensaFisica){
        fak = atacante -> ataqueFisico - recibe -> defensaFisica;
        recibe -> HP = recibe -> HP - fak;
    }
    if (atacante-> ataqueFisico <= recibe ->defensaFisica){
        danoReduc = atacante->ataqueFisico / 2;
        recibe -> HP = recibe -> HP - danoReduc;
    }
}

void ataqueFisico(Personaje *atacante, Personaje *defensor){
    printf("\n⚔️  %s ataca a %s!\n", atacante->nombre, defensor->nombre);

}

int main(int argc, char const *argv[])
{
    int victorias = 0;
    // para que al derrotar a todos se acabe el juego 
    srand(time(NULL));
    //Fer: esto de aca arriba es para el uso del random, referenicias el codigo del profe 
    // aqui un if que tenga que si la vida que esta como hp llega a el mismo valor que daño entonces se pierde 
    // la cosa es que esto  es en cada pelea asi que va dentro de cada siclo por asi decirlo
    
    mostrarMenuPrincipal();

    //Declaramos el arreglo de personajes
    /*
        Estructura jugadores:
        jugadores[0] - Usuario
        jugadores[1-5] - Enemigos
    */
    Personaje jugadores[5];

    int turnos = 0 ;

    //Bien ahora tenemos que mandar a llamar a la funcion de crear personaje
    //Dato: Paso la direccion del primer elemento del arreglo (&jugadores[0])
    crearPersonajeJugador(jugadores);


}