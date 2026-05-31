#include <iostream>
#include <thread>
#include <vector>
#include <ctime>
#include "pc.h"
#include "semaforo.h"

int main() {

    srand(time(NULL)); // Inicializar la semilla para los números aleatorios de prioridad

    std::cout << "===============================================" << std::endl;
    std::cout << "   SISTEMA DE RENDERIZADO INICIADO  " << std::endl;
    std::cout << "===============================================" << std::endl;

    // Inicializar semáforos
    init(hay_jobs_cola, 0);         // Empieza en 0 porque la cola está vacía
    init(hay_jobs_vram, 0);         // Empiexa en 0 porque el pool está vacío
    init(slots_vram_libres, 5);     // Empieza en 5 (hay 5 lugares libres)

    //std::thread hilo_productor(api_gateway);
    //std::thread hilo_despachador(despachador);
    //hilo_productor.join();
    //hilo_despachador.join();

 //ESCENARIO A
/*
     std::thread p1(api_gateway);

     std::thread d(despachador);

     std::thread w1(worker);
     std::thread w2(worker);

     p1.join();
     d.join();
     w1.join();
     w2.join();

*/


//ESCENARIO B

    std::thread p1(api_gateway, 7);
     std::thread p2(api_gateway, 7);
     std::thread p3(api_gateway, 6);

     std::thread d(despachador);

     std::thread w1(worker);

     p1.join();
     p2.join();
     p3.join();
     d.join();
     w1.join();

//ESCENARIO C
/*
     std::thread p1(api_gateway, 500);// productor 1 genera 7 jobs
     std::thread p2(api_gateway, 500) ; // productor 2 genera 7 jobs
     std::thread p3(api_gateway, 500); // productor 3 genera 6 jobs

     std::thread d(despachador);

     std::thread w1(worker);
     std::thread w2(worker);
     std::thread w3(worker);

     p1.join();
     p2.join();
     p3.join();
     d.join();
     w1.join();
     w2.join();
     w3.join();


*/

    return 0;
}