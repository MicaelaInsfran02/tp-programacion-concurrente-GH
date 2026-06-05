#include <iostream>
#include <vector>
#include <queue>
#include <mutex>
#include <thread>
#include <chrono>
#include "semaforo.h"
#include "job.h"
#include "pc.h"

// --- PUNTO DE INICIO DE TIEMPO GLOBAL ---
std::chrono::steady_clock::time_point inicio_sistema = std::chrono::steady_clock::now();

// --- RECURSOS COMPARTIDOS ---
std::priority_queue<Job> message_queue; // BUFFER 1
std::vector<Job> pool_vram;             // BUFFER 2
int jobs_finalizados = 0;
int siguiente_id = 1;
// =============== CONFIGURACIÓN DE CARGA ==============
const int TOTAL_JOBS = 20; // para prueba equidad
//const int TOTAL_JOBS = 1500; // para prueba de Carga Masiva
//const int TOTAL_JOBS = 0; // para prueba de Vacuidad
//const int TOTAL_JOBS = 10; // para prueba de Saturación de Recursos
// =====================================================




// --- MUTEX ---
std::mutex mtx_queue; // Protege Buffer 1
std::mutex mtx_vram;  // Protege Buffer 2
std::mutex mtx_log;
std::mutex mtx_id;
std::mutex mtx_finalizados;

// --- SEMÁFOROS ---
Semaforo hay_jobs_cola;  // Controla Buffer 1
Semaforo hay_jobs_vram;  // Controla Buffer 2
Semaforo slots_vram_libres; // Capacidad del Buffer 2 (inicia en 5)

void api_gateway(int limite) {
    for (int i = 0; i < limite; i++) {
        mtx_id.lock();
        int id = siguiente_id++;
        mtx_id.unlock();

        Job nuevo;
        nuevo.id = id;
        nuevo.prioridad = rand() % 2; // Formato estándar aleatorio pedido
        nuevo.edad = 0;

        /*// --- LÓGICA ESTRATÉGICA PARA FORZAR SATURACIÓN AL INICIO ---
        if (i < 8) {
            nuevo.prioridad = 1; // Los primeros 8 Jobs SIEMPRE serán Premium
        } else {
            nuevo.prioridad = rand() % 2; // El resto de la carga se distribuye al azar
        }*/

        // Retardo de ingreso a Message Queue (100ms)
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        mtx_queue.lock();
        message_queue.push(nuevo);
        mtx_queue.unlock();

        signal(hay_jobs_cola);

        // --- REGISTRO CON TIEMPO CORREGIDO ---
        auto ahora = std::chrono::steady_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(ahora - inicio_sistema).count();

        mtx_log.lock();
        std::cout << "[" << ms << "ms]  Job " << nuevo.id << " - Prioridad " << nuevo.prioridad << "[CREADO]" << std::endl;
        std::cout << "[" << ms << "ms]  Job " << nuevo.id << " - Prioridad " << nuevo.prioridad << "[EN COLA]" << std::endl;
        mtx_log.unlock();
    }
}

void despachador() {
    int procesados = 0;
    while (procesados < TOTAL_JOBS) {
        wait(hay_jobs_cola);
        wait(slots_vram_libres);

        mtx_queue.lock();
        Job elegido = message_queue.top();
        message_queue.pop();

        // Aplicar AGING: Envejecer a los que quedaron esperando en Buffer 1
        std::vector<Job> temporal;
        while (!message_queue.empty()) {
            Job j = message_queue.top();
            message_queue.pop();
            j.edad++;
            temporal.push_back(j);
        }
        for (const auto& j : temporal) message_queue.push(j);
        mtx_queue.unlock();

        // Pasar al Pool de VRAM (BUFFER 2)
        mtx_vram.lock();
        pool_vram.push_back(elegido);
        mtx_vram.unlock();

        signal(hay_jobs_vram);

        // --- REGISTRO CON TIEMPO CORREGIDO ---
        auto ahora = std::chrono::steady_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(ahora - inicio_sistema).count();

        mtx_log.lock();
        std::cout << "[" << ms << "ms]  Job " << elegido.id << " - Prioridad " << elegido.prioridad << " [ASIGNADO_VRAM]" << std::endl;
        mtx_log.unlock();

        // Retardo simulado entre asignaciones (450ms)
        std::this_thread::sleep_for(std::chrono::milliseconds(450));
        procesados++;
    }

    // Señal de finalización para los workers
    for (int i = 0; i < 10; i++) signal(hay_jobs_vram);
}

void worker() {
    while (true) {
        wait(hay_jobs_vram);

        mtx_finalizados.lock();
        if (jobs_finalizados >= TOTAL_JOBS) {
            mtx_finalizados.unlock();
            break;
        }
        mtx_finalizados.unlock();

        // Renderizado (mínimo 600ms en VRAM)
        std::this_thread::sleep_for(std::chrono::milliseconds(600));

        mtx_vram.lock();
        if (!pool_vram.empty()) {
            Job trabajo = pool_vram.front();
            pool_vram.erase(pool_vram.begin());

            // Retardo de liberación de slot (250ms)
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
            mtx_vram.unlock();

            signal(slots_vram_libres);

            mtx_finalizados.lock();
            jobs_finalizados++;
            mtx_finalizados.unlock();

            // --- REGISTRO CON TIEMPO CORREGIDO ---
            auto ahora = std::chrono::steady_clock::now();
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(ahora - inicio_sistema).count();

            mtx_log.lock();
            std::cout << "[" << ms << "ms]  Job " << trabajo.id << " - Prioridad " << trabajo.prioridad << " [FINALIZADO]" << std::endl;
            mtx_log.unlock();
        } else {
            mtx_vram.unlock();
        }
    }
}
