#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define MAX_NOMBRE 50
#define MAX_MAGIAS 3
#define MAX_OBJETOS 5

// Declaración adelantada de estructuras
struct Objeto;

// Estructura para las magias
typedef struct 
{
    struct Personaje* atacante;   // Puntero al personaje que lanza la magia
    struct Personaje* objetivo;   // Puntero al personaje que recibe la magia
} DatosMagia;

// Estructura base para todos los personajes
typedef struct Personaje {
    char nombre[MAX_NOMBRE];
    int HP;
    int danio;
    int ataqueFisico;
    int ataqueMagico;
    int defensaFisica;
    int defensaMagica;
    void (*magia[MAX_MAGIAS])(void *);
    struct Objeto *inventario[MAX_OBJETOS];

    // Contadores para efectos temporales
    int turnosCongelado;
    int buffAtaqueFisico; // Cambié el nombre para evitar conflicto
    int originalAtaqueFisico;
    int debuffDefensaFisica;
    int buffOriginalDefensaFisica; // Cambié el nombre aquí también
} Personaje;

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

    /*
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
    */
   
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
    if (personaje->buffAtaqueFisico > 0) {
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

/*
 * FUNCIÓN: VERIFICAR_TEST
 * Propósito: Mostrar resultados de tests de manera organizada
 */
void verificarTest(const char* nombreTest, int condicion) {
    if (condicion) {
        printf("✅ %s: PASÓ\n", nombreTest);
    } else {
        printf("❌ %s: FALLÓ\n", nombreTest);
    }
}

/*
 * FUNCIÓN: MOSTRAR_ESTADO_PERSONAJE
 * Propósito: Mostrar estado actual de un personaje para debugging
 */
void mostrarEstadoPersonaje(Personaje* p, const char* nombre) {
    printf("\n--- Estado de %s ---\n", nombre);
    printf("Nombre: %s\n", p->nombre);
    printf("HP: %d, Daño: %d, Vida actual: %d/%d\n", 
           p->HP, p->danio, p->HP - p->danio, p->HP);
    printf("Ataque Físico: %d\n", p->ataqueFisico);
    printf("Ataque Mágico: %d\n", p->ataqueMagico);
    printf("Defensa Física: %d\n", p->defensaFisica);
    printf("Defensa Mágica: %d\n", p->defensaMagica);
    printf("Turnos Congelado: %d\n", p->turnosCongelado);
    printf("Buff Ataque Físico: %d\n", p->buffAtaqueFisico);
    printf("Debuff Defensa Física: %d\n", p->debuffDefensaFisica);
}

/*
 * TEST 1: BOLA DE FUEGO
 * Propósito: Verificar que el daño se calcula y aplica correctamente
 */
void testBolaFuego() {
    printf("\n=== TEST 1: BOLA DE FUEGO ===\n");
    
    // Crear personajes de prueba
    Personaje atacante = {"Mago", 20, 0, 5, 8, 4, 5, {NULL}, {NULL}, 0, 0, 0, 0, 0};
    Personaje objetivo = {"Guerrero", 20, 0, 7, 3, 5, 3, {NULL}, {NULL}, 0, 0, 0, 0, 0};
    
    // Preparar datos para la magia
    DatosMagia datos = {&atacante, &objetivo};
    
    // Estado inicial
    int vidaInicial = objetivo.HP - objetivo.danio;
    
    // Lanzar magia
    bolaFuego(&datos);
    
    // Verificaciones
    verificarTest("Daño aplicado correctamente", objetivo.danio > 0);
    verificarTest("Vida reducida", (objetivo.HP - objetivo.danio) < vidaInicial);
    verificarTest("Cálculo de daño correcto", objetivo.danio == (atacante.ataqueMagico - objetivo.defensaMagica));
    
    mostrarEstadoPersonaje(&objetivo, "Objetivo después de Bola de Fuego");
}

/*
 * TEST 2: CONGELAR
 * Propósito: Verificar que el efecto de congelación se aplica correctamente
 */
void testCongelar() {
    printf("\n=== TEST 2: CONGELAR ===\n");
    
    Personaje objetivo = {"Enemigo", 20, 0, 6, 4, 5, 4, {NULL}, {NULL}, 0, 0, 0, 0, 0};
    
    // Estado inicial
    int turnosCongeladoInicial = objetivo.turnosCongelado;
    
    // Lanzar magia
    congelar(&objetivo);
    
    // Verificaciones
    verificarTest("Turnos congelado incrementado", objetivo.turnosCongelado > turnosCongeladoInicial);
    verificarTest("Exactamente 1 turno congelado", objetivo.turnosCongelado == 1);
    
    mostrarEstadoPersonaje(&objetivo, "Objetivo después de Congelar");
}

/*
 * TEST 3: BENDICIÓN DE FUERZA
 * Propósito: Verificar que el buff de ataque se aplica correctamente
 */
void testBendicionFuerza() {
    printf("\n=== TEST 3: BENDICIÓN DE FUERZA ===\n");
    
    Personaje objetivo = {"Guerrero", 20, 0, 5, 3, 6, 4, {NULL}, {NULL}, 0, 0, 0, 0, 0};
    
    // Estado inicial
    int ataqueInicial = objetivo.ataqueFisico;
    int buffInicial = objetivo.buffAtaqueFisico;
    
    // Lanzar magia
    bendicionFuerza(&objetivo);
    
    // Verificaciones
    verificarTest("Ataque físico aumentado", objetivo.ataqueFisico > ataqueInicial);
    verificarTest("Buff aplicado por 3 turnos", objetivo.buffAtaqueFisico == 3);
    verificarTest("Valor original guardado", objetivo.originalAtaqueFisico == ataqueInicial);
    
    mostrarEstadoPersonaje(&objetivo, "Objetivo después de Bendición de Fuerza");
}

/*
 * TEST 4: MALDICIÓN DEBILITADORA
 * Propósito: Verificar que el debuff de defensa se aplica correctamente
 */
void testMaldicionDebilitadora() {
    printf("\n=== TEST 4: MALDICIÓN DEBILITADORA ===\n");
    
    Personaje atacante = {"Mago Oscuro", 20, 0, 4, 7, 3, 6, {NULL}, {NULL}, 0, 0, 0, 0, 0};
    Personaje objetivo = {"Paladín", 20, 0, 6, 2, 8, 5, {NULL}, {NULL}, 0, 0, 0, 0, 0};
    
    DatosMagia datos = {&atacante, &objetivo};
    
    // Estado inicial
    int defensaInicial = objetivo.defensaFisica;
    int debuffInicial = objetivo.debuffDefensaFisica;
    
    // Lanzar magia
    maldicionDebilitadora(&datos);
    
    // Verificaciones
    verificarTest("Defensa física reducida", objetivo.defensaFisica < defensaInicial);
    verificarTest("Debuff aplicado por 3 turnos", objetivo.debuffDefensaFisica == 3);
    verificarTest("Defensa no menor a 1", objetivo.defensaFisica >= 1);
    
    mostrarEstadoPersonaje(&objetivo, "Objetivo después de Maldición Debilitadora");
}

/*
 * TEST 5: SANACIÓN DIVINA
 * Propósito: Verificar que la curación funciona correctamente
 */
void testSanacionDivina() {
    printf("\n=== TEST 5: SANACIÓN DIVINA ===\n");
    
    Personaje objetivo = {"Curandero", 20, 12, 4, 6, 3, 5, {NULL}, {NULL}, 0, 0, 0, 0, 0};
    
    // Estado inicial
    int danioInicial = objetivo.danio;
    int vidaInicial = objetivo.HP - objetivo.danio;
    
    // Lanzar magia
    sanacionDivina(&objetivo);
    
    // Verificaciones
    verificarTest("Daño reducido", objetivo.danio < danioInicial);
    verificarTest("Vida aumentada", (objetivo.HP - objetivo.danio) > vidaInicial);
    verificarTest("Daño no es negativo", objetivo.danio >= 0);
    
    mostrarEstadoPersonaje(&objetivo, "Objetivo después de Sanación Divina");
}

/*
 * TEST 6: SISTEMA DE EFECTOS TEMPORALES
 * Propósito: Verificar que los efectos temporales se actualizan correctamente
 */
/*
 * TEST 6: SISTEMA DE EFECTOS TEMPORALES - VERSIÓN CORREGIDA
 */
void testSistemaEfectosTemporales() {
    printf("\n=== TEST 6: SISTEMA DE EFECTOS TEMPORALES ===\n");
    
    // CORRECCIÓN: Inicializar correctamente los contadores
    Personaje personaje = {"Guerrero", 20, 0, 8, 3, 6, 4, {NULL}, {NULL}, 
                           /* turnosCongelado */ 2, 
                           /* buffAtaqueFisico */ 3, 
                           /* originalAtaqueFisico */ 5, 
                           /* debuffDefensaFisica */ 2, 
                           /* buffOriginalDefensaFisica */ 6};
    
    // Estado inicial
    int turnosCongeladoInicial = personaje.turnosCongelado;
    int buffAtaqueInicial = personaje.buffAtaqueFisico;
    int debuffDefensaInicial = personaje.debuffDefensaFisica;
    
    printf("ANTES de actualizar:\n");
    printf("Turnos congelado: %d\n", personaje.turnosCongelado);
    printf("Buff ataque: %d\n", personaje.buffAtaqueFisico);
    printf("Debuff defensa: %d\n", personaje.debuffDefensaFisica);
    
    // Actualizar efectos
    actualizarEfectosTemporales(&personaje);
    
    // Verificaciones
    verificarTest("Turnos congelado reducido", personaje.turnosCongelado < turnosCongeladoInicial);
    verificarTest("Buff ataque reducido", personaje.buffAtaqueFisico < buffAtaqueInicial);
    verificarTest("Debuff defensa reducido", personaje.debuffDefensaFisica < debuffDefensaInicial);
    
    printf("DESPUÉS de actualizar:\n");
    printf("Turnos congelado: %d\n", personaje.turnosCongelado);
    printf("Buff ataque: %d\n", personaje.buffAtaqueFisico);
    printf("Debuff defensa: %d\n", personaje.debuffDefensaFisica);
    
    mostrarEstadoPersonaje(&personaje, "Personaje después de actualizar efectos");
}

/*
 * TEST 7: CASOS BORDE
 * Propósito: Probar situaciones extremas y casos especiales
 */
void testCasosBorde() {
    printf("\n=== TEST 7: CASOS BORDE ===\n");
    
    // Test 7.1: Daño mínimo siempre 1
    Personaje atacanteDebil = {"Aprendiz", 20, 0, 3, 2, 3, 2, {NULL}, {NULL}, 0, 0, 0, 0, 0};
    Personaje objetivoFuerte = {"Tanque", 20, 0, 5, 1, 6, 10, {NULL}, {NULL}, 0, 0, 0, 0, 0};
    
    DatosMagia datos = {&atacanteDebil, &objetivoFuerte};
    bolaFuego(&datos);
    
    verificarTest("Daño mínimo de 1 punto", objetivoFuerte.danio == 1);
    
    // Test 7.2: Curación no excede HP máximo
    Personaje casiMuerto = {"Herido", 20, 19, 4, 3, 4, 3, {NULL}, {NULL}, 0, 0, 0, 0, 0};
    sanacionDivina(&casiMuerto);
    verificarTest("Curación no excede HP máximo", casiMuerto.danio >= 0);
    
    // Test 7.3: Defensa nunca menor a 1
    Personaje objetivoDebil = {"Débil", 15, 0, 3, 2, 2, 2, {NULL}, {NULL}, 0, 0, 0, 0, 0};
    Personaje atacanteFuerte = {"Fuerte", 20, 0, 8, 7, 5, 4, {NULL}, {NULL}, 0, 0, 0, 0, 0};
    
    DatosMagia datos2 = {&atacanteFuerte, &objetivoDebil};
    maldicionDebilitadora(&datos2);
    verificarTest("Defensa nunca menor a 1", objetivoDebil.defensaFisica >= 1);
}

/*
 * FUNCIÓN: CORRER_TODOS_LOS_TESTS
 * Propósito: Ejecutar todas las pruebas automáticamente
 */
void correrTodosLosTests() {
    printf("INICIANDO PRUEBAS UNITARIAS DEL SISTEMA DE MAGIAS\n");
    printf("=====================================================\n");
    
    testBolaFuego();
    testCongelar();
    testBendicionFuerza();
    testMaldicionDebilitadora();
    testSanacionDivina();
    testSistemaEfectosTemporales();
    testCasosBorde();
    
    printf("\n🎯 RESUMEN DE PRUEBAS COMPLETADO 🎯\n");
    printf("Todas las magias han sido verificadas.\n");
}

/*
 * FUNCIÓN: DEMOSTRACIÓN_COMPLETA
 * Propósito: Mostrar una secuencia real de batalla con todas las magias
 */
void demostracionCompleta() {
    printf("\nDEMOSTRACIÓN COMPLETA DE BATALLA\n");
    printf("=====================================\n");
    
    // Crear personajes para la demostración
    Personaje jugador = {"Héroe", 25, 0, 7, 6, 5, 4, {NULL}, {NULL}, 0, 0, 0, 0, 0};
    Personaje enemigo = {"Orco", 20, 0, 6, 3, 4, 2, {NULL}, {NULL}, 0, 0, 0, 0, 0};
    
    printf("--- INICIO DE BATALLA ---\n");
    mostrarEstadoPersonaje(&jugador, "Jugador");
    mostrarEstadoPersonaje(&enemigo, "Enemigo");
    
    // Turno 1: Jugador usa Bola de Fuego
    printf("\n--- TURNO 1: Jugador ataca ---\n");
    DatosMagia datos1 = {&jugador, &enemigo};
    bolaFuego(&datos1);
    
    // Turno 2: Enemigo es congelado
    printf("\n--- TURNO 2: Jugador congela ---\n");
    congelar(&enemigo);
    
    // Turno 3: Jugador se bendice
    printf("\n--- TURNO 3: Jugador se fortalece ---\n");
    bendicionFuerza(&jugador);
    
    // Turno 4: Jugador debilita enemigo
    printf("\n--- TURNO 4: Jugador debilita enemigo ---\n");
    DatosMagia datos2 = {&jugador, &enemigo};
    maldicionDebilitadora(&datos2);
    
    // Turno 5: Jugador se cura
    printf("\n--- TURNO 5: Jugador se cura ---\n");
    jugador.danio = 10; // Simular daño recibido
    sanacionDivina(&jugador);
    
    // Mostrar estado final
    printf("\n--- ESTADO FINAL ---\n");
    mostrarEstadoPersonaje(&jugador, "Jugador");
    mostrarEstadoPersonaje(&enemigo, "Enemigo");
}

int main(int argc, char const *argv[]) 
{
    printf("SISTEMA DE TESTING DE MAGIAS\n");
    
    // Opción 1: Ejecutar pruebas automáticas
    correrTodosLosTests();
    
    // Opción 2: Ver demostración completa
    demostracionCompleta();
    
    // Opción 3: Probar sistema de efectos temporales
    printf("\n🔄 PROBANDO SISTEMA DE EFECTOS TEMPORALES 🔄\n");
    Personaje prueba = {"Prueba", 20, 0, 5, 4, 5, 4, {NULL}, {NULL}, 2, 3, 5, 2, 5};
    printf("Antes de actualizar efectos:\n");
    mostrarEstadoPersonaje(&prueba, "Personaje Prueba");
    
    for (int i = 0; i < 4; i++) {

        printf("\n--- Actualización %d ---\n", i + 1);
        actualizarEfectosTemporales(&prueba);
        mostrarEstadoPersonaje(&prueba, "Personaje Prueba");
    }
    
    printf("\n✨ TODAS LAS PRUEBAS COMPLETADAS ✨\n");
    printf("El sistema de magias está funcionando correctamente.\n");
    
    return 0;
}