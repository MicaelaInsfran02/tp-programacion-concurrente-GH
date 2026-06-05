#include <iostream>
#include <thread>
#include <vector>
#include <ctime>
#include "pc.h"
#include "semaforo.h"

// Variables externas definidas en pc.cpp
extern Semaforo hay_jobs_cola;
extern Semaforo hay_jobs_vram;
extern Semaforo slots_vram_libres;


int main() {
    srand(time(NULL));
    init(hay_jobs_cola, 0);
    init(hay_jobs_vram, 0);
    init(slots_vram_libres, 5);


    // =========================================================================
    // ESCENARIO A: 1 Productor / 2 Consumidores
    // =========================================================================
    /*std::cout << ">>> Ejecutando ESCENARIO A" << std::endl;
    std::thread p1(api_gateway, 20);
    std::thread d(despachador);
    std::thread w1(worker);
    std::thread w2(worker);

    p1.join();
    d.join();
    w1.join();
    w2.join();*/

    // =========================================================================
    // ESCENARIO B: 3 Productores / 1 Consumidor (COMENTADO)
    // =========================================================================

    /*std::cout << ">>> Ejecutando ESCENARIO B" << std::endl;
    std::thread p1(api_gateway, 5);
    std::thread p2(api_gateway, 5);
    std::thread p3(api_gateway, 5);
    std::thread d(despachador);
    std::thread w1(worker);

    p1.join(); p2.join(); p3.join();
    d.join();
    w1.join();*/


    // =========================================================================
    // ESCENARIO C: 3 Productores / 3 Consumidores (COMENTADO)
    // =========================================================================

    std::cout << ">>> Ejecutando ESCENARIO C" << std::endl;

    //------> PARA LA PRUEBA DE CARGA MASIVA
    /*std::thread p1(api_gateway, 500);
    std::thread p2(api_gateway, 500);
    std::thread p3(api_gateway, 500);*/

    std::thread p1(api_gateway, 5);
    std::thread p2(api_gateway, 5);
    std::thread p3(api_gateway, 5);
    std::thread d(despachador);
    std::thread w1(worker);
    std::thread w2(worker);
    std::thread w3(worker);

    p1.join(); p2.join(); p3.join();
    d.join();
    w1.join(); w2.join(); w3.join();

    std::cout << "===============================================" << std::endl;
    std::cout << "   PROCESAMIENTO FINALIZADO CON EXITO" << std::endl;
    std::cout << "===============================================" << std::endl;

    return 0;
}
