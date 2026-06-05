#ifndef JOB_H_INCLUDED
#define JOB_H_INCLUDED


struct Job{

    int id;
    int prioridad; //1: Premium, 0: Free
    int edad = 0;  // Contador de postergaciones para Aging

    // La prioridad efectiva es la suma de su estatus y su edad
    int prioridad_total() const{
        return prioridad + edad;
    }

    // El operador < define el orden en la priority_queue
    // (C++ saca primero al que devuelve "falso" en la comparación de menor)
    bool operator<(const Job& otro) const {
        return prioridad_total() < otro.prioridad_total();
    }

};

#endif // JOB_H_INCLUDED
