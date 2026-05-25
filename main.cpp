#include <iostream>
#include <thread>
#include <vector>
#include <ctime>
#include "pc.h"
#include "semaforo.h"

using namespace std;

int main()
{
    cout << "------------------------" << endl;
    cout << "SISTEMA DE RENDERIZADO INICIADO" << endl;
    cout << "------------------------" << endl;

    // Inicializar semáforos
    init(hay_jobs_cola, 0); // No hay jobs al inicio
    init(hay_jobs_vram, 0); // No hay jobs en VRAM al
    init(slots_vram_libres, 5); // VRAM tiene 5 slots libres
    // Crear hilos
    thread hilo_productor(api_gateway);
    hilo_productor.join();
    return 0;
}
