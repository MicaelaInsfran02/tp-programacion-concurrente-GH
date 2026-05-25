#include <iostream>
#include <queue>
#include <vector>
#include <mutex>
#include <thread>
#include <chrono>
#include <fstream> // Para el sistema.log
#include "pc.h"
#include "semaforo.h"
#include "job.h"

//recursos compartidos
std::priority_queue<Job> message_queue;
std::vector<Job> pool_vram;

//--- mutex ---
std::mutex mtx_queue;
std::mutex mtx_vram;
std::mutex mtx_log;
std::mutex mtx_id;

//--- semaforos ---
Semaforo hay_jobs_cola;
Semaforo hay_jobs_vram;
Semaforo slots_vram_libres;

// --- contador de IDs para los Jobs ---
int siguiente_id = 1;

// --- FUNCIONES ---
void api_gateway(){

    while(true){
        //crear el id del job
        mtx_id.lock();
        int id=siguiente_id;
        siguiente_id++;
        mtx_id.unlock();
        //crear el job
        Job nuevo_job;
        nuevo_job.id=id;
        //prioridad aleatorio
        nuevo_job.prioridad=rand()%2; // 0-> free o 1-> premium
        //retardo de produccion
        std::this_thread::sleep_for(std::chrono::milliseconds(100 )); 
        //meter el job a la cola
        mtx_queue.lock();
        message_queue.push(nuevo_job);
        mtx_queue.unlock();
        //avisar que hay un nuevo job en la cola
        signal(hay_jobs_cola);
        //mostrar 
        mtx_log.lock();
        std:: cout <<"job"<<id<<" con prioridad "<<nuevo_job.prioridad<<" creado y puesto en la cola."<<std::endl;
        mtx_log.unlock();
    }
};
void despachador();
void worker();
