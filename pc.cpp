#include <iostream>
#include <vector>
#include <queue>
#include <mutex>
#include <thread>
#include <chrono>
#include "pc.h"
#include "semaforo.h"
#include "job.h"


// --- RECURSOS COMPARTIDOS (Se definen en pc.cpp) ---
std::priority_queue<Job> message_queue; // Buffer 1
std::vector<Job> pool_vram; // Buffer 2 (maximo 5)

// --- MUTEX ---
std::mutex mtx_log; //Para escribir en sistema.log sin mezclar líneas std::mutex mtx_vram; //protege el acceso al vector de VRAM
std::mutex mtx_queue; // Protege el acceso a la priority_queue
std::mutex mtx_vram;
std::mutex mtx_id;
std::mutex mtx_finalizados; //Para cuando finaliza el consumidor

// --- SEMÁFOROS ---
Semaforo hay_jobs_cola; // Si hay algo en el primer Buffer
Semaforo hay_jobs_vram; // Si hay algo en el segundo Buffer
Semaforo slots_vram_libres; //Inicializado en 5 (capacidad VRAM)

// -- Variables globales
int siguiente_id = 1; //Para asignar IDs únicos a los jobs

int jobs_finalizados = 0;
//const int TOTAL_JOBS = 20;

//Configuracion A,B,C, Prueba 1
//Prueba de Carga Masiva (1500 jobs)
//const int TOTAL_JOBS = 1500;

//Configuracion A,B,C Prueba 2
//Prueba de Vacuidad (0 jobs)
//const int TOTAL_JOBS = 0;

//Configuracion A,B,C Prueba 3
//Prueba de Saturacion (8 jobs premium)
//const int TOTAL_JOBS = 8;

//Configuracion A,B,C Prueba 4
//Prueba de Equidad (Anti-Starvation)10 prem, 10 free
    const int TOTAL_JOBS = 20;

// --- FUNCIONES ---
//  void api_gateway(int limite){
void api_gateway(int limite){

    int jobsCreados = 0;        // contador local para este productor

        //while(jobsCreados < 20){
         while(jobsCreados < limite){// cada productor genera hasta su límite
             jobsCreados++;

        //CONFIGURACION PRUEBA 4
        //10 PREMIUM PRIMERO
        //auto inicio = std::chrono::steady_clock::now();
        //while ((std::chrono::steady_clock::now() - inicio < std::chrono::milliseconds(5000))
         //  && jobsCreados < limite/2) {   // mitad de los jobs como premium
          //  jobsCreados++;

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
        //-------------------------------------------
        //prioridad Premium Configuracion A,B,C PRUEBA 3
        //nuevo_job.prioridad = 1;
        //Prioridad Premium Configuracion A,B,C PRUEBA 4
        //nuevo_job.prioridad = 1; // Premium


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
        std:: cout <<"job "<<id<<" con prioridad "<<nuevo_job.prioridad<<" creado y puesto en la cola."<<std::endl;
        mtx_log.unlock();
    }

    //---- Bloque FREE ( Prioridad 0 despues de los 5000ms)----
//    while (jobsCreados < limite){
//        jobsCreados++;
//
//        mtx_id.lock();
//        int id = siguiente_id;
//        siguiente_id++;
//        mtx_id.unlock();
//
//        Job nuevo_job;
//        nuevo_job.id = id;
//        // Prioridad Free Configuración A Prueba 4
//        nuevo_job.prioridad = 0; // Free
//
//        std::this_thread::sleep_for(std::chrono::milliseconds(100));
//        mtx_queue.lock();
//        message_queue.push(nuevo_job);
//        mtx_queue.unlock();
//
//        signal(hay_jobs_cola);
//
//        mtx_log.lock();
//        std::cout << "job " << id << " con prioridad " << nuevo_job.prioridad
//                  << " creado y puesto en la cola." << std::endl;
//        mtx_log.unlock();
//  }
 }

void despachador(){
    int procesados = 0;
    //while(procesados < 20){
    while(procesados < TOTAL_JOBS){
        procesados++;
        //esperar a que haya jobs en la cola
        wait(hay_jobs_cola);
        //esperar un slot libre en VRAM
        wait(slots_vram_libres);

        Job job_a_vram;

        //Sacar de la message queue
        mtx_queue.lock();
        job_a_vram = message_queue.top(); // obtener el job con mayor prioridad (premium antes que free)
        message_queue.pop();
        mtx_queue.unlock();

        // meter a VRAM
        mtx_vram.lock();
        pool_vram.push_back(job_a_vram);
        mtx_vram.unlock();


        //avisar que hay un nuevo job en VRAM
        signal(hay_jobs_vram);
        //mostrar
        mtx_log.lock();
        std::cout << "job "<<job_a_vram.id<<" con prioridad " <<job_a_vram.prioridad<<" asignado a VRAM." << std::endl;
        mtx_log.unlock();
        //"Debe existir un retardo simulado de 450ms entre asignaciones exitosas"
        std::this_thread::sleep_for(std::chrono::milliseconds(450));




    }

};

void worker() {

    while(true){

        wait(hay_jobs_vram);

        Job trabajo;
        mtx_vram.lock();

        trabajo = pool_vram.front();
        pool_vram.erase(pool_vram.begin());

        mtx_vram.unlock();

        std::this_thread::sleep_for(
            std::chrono::milliseconds(600)
        );

        signal(slots_vram_libres);

        mtx_finalizados.lock();

        jobs_finalizados++;

        int actual = jobs_finalizados;

        mtx_finalizados.unlock();

        mtx_log.lock();

        std::cout
            << "[WORKER "
            << std::this_thread::get_id()
            << "] Job "
            << trabajo.id
            << " finalizado."
            << std::endl;

        mtx_log.unlock();

        if(actual >= TOTAL_JOBS)
            break;
    }
}
