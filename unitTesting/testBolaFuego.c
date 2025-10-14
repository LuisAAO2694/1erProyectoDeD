#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * ARCHIVO DE PRUEBA UNITARIA PARA BOLA DE FUEGO
 * Propósito: Probar solo la magia Bola de Fuego de forma aislada
 * Compilación: gcc -o test_bola_fuego test_bola_fuego.c
 * Ejecución: ./test_bola_fuego
 */

#define MAX_NOMBRE 50
#define MAX_MAGIAS 3
#define MAX_OBJETOS 5

// Declaración adelantada
struct Objeto;

/*
 * ESTRUCTURA PERSONAJE (solo lo necesario para la prueba)
 * Incluimos solo los campos que usa Bola de Fuego
 */
typedef struct {
    char nombre[MAX_NOMBRE];
    int HP;
    int danio;
    int ataqueMagico;
    int defensaMagica;
    // Solo incluimos los campos que vamos a usar en la prueba
} Personaje;

/*
 * ESTRUCTURA DATOSMAGIA
 * Para pasar ambos personajes a la función mágica
 */
typedef struct {
    Personaje* atacante;
    Personaje* objetivo;
} DatosMagia;

/*
 * MAGIA 1: BOLA DE FUEGO - FUNCIÓN A PROBAR
 * Esta es exactamente tu función, copiada aquí para las pruebas
 */
void bolaFuego(void* datos)
{
    printf("--- Lanzando Bola De Fuego ---\n");

    // Primero convierto el puntero (void*) al tipo correcto
    // En este caso lo convierto a un DatosMagia* para poder acceder a los campos que definimos
    DatosMagia* datosMagia = (DatosMagia*)datos;

    // Ahora extraigo los personajes de la estructura
    Personaje* atacante = datosMagia->atacante;
    Personaje* objetivo = datosMagia->objetivo;

    // Bien ahora pues tenemos que hacer daño
    // Formula: Ataque del lanzador - Defensa mágica del objetivo
    int dano = atacante->ataqueMagico - objetivo->defensaMagica;

    // Ahora aseguramos que el daño mínimo sea de 1
    // Btw evitamos que el daño sea 0 o negativo
    if(dano < 1)
    {
        dano = 1;
    }

    // Aplico el daño
    // Se incrementa el daño acumulado al personaje objetivo (sumo el daño y lo registro)
    objetivo->danio += dano;

    printf("!%s lanza una Bola de Fuego!!\n",atacante->nombre);
    printf("Ataque Mágico (%s): %d vs Defensa Mágica (%s): %d\n", 
           atacante->nombre, atacante->ataqueMagico, 
           objetivo->nombre, objetivo->defensaMagica);  // ⚠️ CORREGIDO: defensaMagica
    
    printf("!%s recibe %d puntos de daño!!\n", objetivo->nombre, dano);
    printf("Vida restante de %s: %d/%d\n", 
           objetivo->nombre, objetivo->HP - objetivo->danio, objetivo->HP);
}

/*
 * FUNCIÓN: CREAR_PERSONAJE_PRUEBA
 * Propósito: Crear personajes de prueba para los tests
 */
Personaje crearPersonajePrueba(char* nombre, int HP, int ataqueMagico, int defensaMagica) {
    Personaje p;
    strcpy(p.nombre, nombre);
    p.HP = HP;
    p.danio = 0;  // Siempre empezamos con 0 de daño
    p.ataqueMagico = ataqueMagico;
    p.defensaMagica = defensaMagica;
    return p;
}

/*
 * FUNCIÓN: MOSTRAR_ESTADO_PERSONAJE
 * Propósito: Mostrar el estado actual de un personaje
 */
void mostrarEstadoPersonaje(Personaje* p) {
    printf("[%s] HP: %d/%d, AtaqueM: %d, DefensaM: %d, Daño acumulado: %d\n",
           p->nombre, p->HP - p->danio, p->HP, p->ataqueMagico, p->defensaMagica, p->danio);
}

/*
 * TEST 1: ATAQUE EXITOSO
 * Escenario: Atacante fuerte vs Defensa débil
 * Resultado esperado: Daño significativo
 */
void testAtaqueExitoso() {
    printf("\n=== TEST 1: ATAQUE EXITOSO ===\n");
    
    // Crear personajes de prueba
    Personaje mago = crearPersonajePrueba("Mago", 20, 10, 5);
    Personaje orco = crearPersonajePrueba("Orco", 20, 3, 3);
    
    // Mostrar estado inicial
    printf("ESTADO INICIAL:\n");
    mostrarEstadoPersonaje(&mago);
    mostrarEstadoPersonaje(&orco);
    
    // Preparar datos para la magia
    DatosMagia datos;
    datos.atacante = &mago;
    datos.objetivo = &orco;
    
    // Lanzar Bola de Fuego
    bolaFuego(&datos);
    
    // Mostrar estado final
    printf("\nESTADO FINAL:\n");
    mostrarEstadoPersonaje(&orco);
    
    // Verificar resultado
    int danoEsperado = 10 - 3; // 7 puntos de daño
    if (orco.danio == danoEsperado) {
        printf("✅ TEST 1 PASADO: Daño correcto (%d)\n", danoEsperado);
    } else {
        printf("❌ TEST 1 FALLADO: Esperado %d, Obtenido %d\n", danoEsperado, orco.danio);
    }
}

/*
 * TEST 2: DEFENSA FUERTE
 * Escenario: Atacante débil vs Defensa fuerte
 * Resultado esperado: Daño mínimo (1 punto)
 */
void testDefensaFuerte() {
    printf("\n=== TEST 2: DEFENSA FUERTE ===\n");
    
    Personaje aprendiz = crearPersonajePrueba("Aprendiz", 20, 4, 8);
    Personaje guerrero = crearPersonajePrueba("Guerrero", 20, 8, 10);
    
    printf("ESTADO INICIAL:\n");
    mostrarEstadoPersonaje(&aprendiz);
    mostrarEstadoPersonaje(&guerrero);
    
    DatosMagia datos;
    datos.atacante = &aprendiz;
    datos.objetivo = &guerrero;
    
    bolaFuego(&datos);
    
    printf("\nESTADO FINAL:\n");
    mostrarEstadoPersonaje(&guerrero);
    
    // Ataque (4) - Defensa (10) = -6 → debería ser 1 (mínimo)
    if (guerrero.danio == 1) {
        printf("✅ TEST 2 PASADO: Daño mínimo aplicado correctamente\n");
    } else {
        printf("❌ TEST 2 FALLADO: Esperado 1, Obtenido %d\n", guerrero.danio);
    }
}

/*
 * TEST 3: DAÑO ACUMULATIVO
 * Escenario: Múltiples ataques al mismo objetivo
 * Resultado esperado: Daño se acumula correctamente
 */
void testDanioAcumulativo() {
    printf("\n=== TEST 3: DAÑO ACUMULATIVO ===\n");
    
    Personaje mago1 = crearPersonajePrueba("Mago1", 20, 6, 4);
    Personaje mago2 = crearPersonajePrueba("Mago2", 20, 6, 4);
    Personaje objetivo = crearPersonajePrueba("Objetivo", 20, 2, 3);
    
    printf("ESTADO INICIAL:\n");
    mostrarEstadoPersonaje(&objetivo);
    
    DatosMagia datos1, datos2;
    datos1.atacante = &mago1;
    datos1.objetivo = &objetivo;
    datos2.atacante = &mago2;
    datos2.objetivo = &objetivo;
    
    // Primer ataque
    bolaFuego(&datos1);
    int danoPrimerAtaque = objetivo.danio;
    
    // Segundo ataque
    bolaFuego(&datos2);
    int danoTotal = objetivo.danio;
    
    printf("\nESTADO FINAL:\n");
    mostrarEstadoPersonaje(&objetivo);
    
    // Verificar que el daño se acumula: (6-3)=3 por cada ataque = 6 total
    if (danoTotal == 6) {
        printf("✅ TEST 3 PASADO: Daño acumulativo correcto (%d)\n", danoTotal);
    } else {
        printf("❌ TEST 3 FALLADO: Esperado 6, Obtenido %d\n", danoTotal);
    }
}

/*
 * TEST 4: PERSONAJE MUERTO
 * Escenario: Ataque que debería derrotar al personaje
 * Resultado esperado: Daño >= HP (personaje derrotado)
 */
void testPersonajeMuerto() {
    printf("\n=== TEST 4: PERSONAJE A PUNTO DE MORIR ===\n");
    
    Personaje magoPoderoso = crearPersonajePrueba("MagoPoderoso", 20, 15, 5);
    Personaje enemigoDebil = crearPersonajePrueba("EnemigoDebil", 10, 2, 2);
    
    // Simular que el enemigo ya tiene daño previo
    enemigoDebil.danio = 5;  // Ya tiene 5 de daño de antes
    
    printf("ESTADO INICIAL (enemigo ya herido):\n");
    mostrarEstadoPersonaje(&enemigoDebil);
    
    DatosMagia datos;
    datos.atacante = &magoPoderoso;
    datos.objetivo = &enemigoDebil;
    
    bolaFuego(&datos);
    
    printf("\nESTADO FINAL:\n");
    mostrarEstadoPersonaje(&enemigoDebil);
    
    // Verificar condición de derrota
    if (enemigoDebil.danio >= enemigoDebil.HP) {
        printf("✅ TEST 4 PASADO: Personaje debería estar derrotado\n");
        printf("   Daño (%d) >= HP (%d)\n", enemigoDebil.danio, enemigoDebil.HP);
    } else {
        printf("⚠️  TEST 4: Personaje aún con vida\n");
    }
}

/*
 * FUNCIÓN PRINCIPAL DE PRUEBAS
 * Ejecuta todos los tests unitarios
 */
int main() {
    printf("========================================\n");
    printf("UNIT TESTING: MAGIA BOLA DE FUEGO\n");
    printf("========================================\n");
    
    // Ejecutar todos los tests
    testAtaqueExitoso();
    testDefensaFuerte();
    testDanioAcumulativo();
    testPersonajeMuerto();
    
    printf("\n========================================\n");
    printf("PRUEBAS COMPLETADAS\n");
    printf("========================================\n");
    
    return 0;
}