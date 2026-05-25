#ifndef PC_H_INCLUDED
#define PC_H_INCLUDED

#include <queue>
#include <vector>
//#include <mutex>
#include "job.h"
#include "semaforo.h"

// --- RECURSOS COMPARTIDOS (Se definen en pc.cpp) ---
extern std::priority_queue<Job> message_queue; // Buffer 1
extern std::vector<Job> pool_vram; // Buffer 2 (donde el maximo es de 5)

// --- MUTEX ---
extern std::mutex mtx_log; //Para escribir en sistema.log sin mezclar lineas
extern std::mutex mtx_vram; //protege el acceso al vector de VRAM
extern std::mutex mtx_queue; // Protege el acceso a la priority_queue
extern std::mutex mtx_id; // Protege el acceso al contador de IDs
// --- SEMAFOROS ---
extern Semaforo hay_jobs_cola; // Si hay algo en el primer Buffer
extern Semaforo hay_jobs_vram; // Si hay algo en el segundo Buffer
extern Semaforo slots_vram_libres; //Inicializado en 5 (capacidad VRAM)

// --- FUNCIONES DE LOS HILOS ---
void api_gateway(); // El productor: Genera Jobs y los mete a Buffer 1
void despachador(); // El INTERMEDIARIO: Pasa de Buffer 1 a Buffer 2 (Prioridad)
void worker();      // El Consumidor: Procesa Jobs del Buffer 2

#endif // PC_H_INCLUDED
