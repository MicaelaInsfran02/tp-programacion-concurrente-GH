#ifndef JOB_H_INCLUDED
#define JOB_H_INCLUDED

//#include <string>

struct Job{

    int id;
    int prioridad; // 1: significa que es premium, 0: significa free

    // Para que el Premium (1) salga antes que el Free (0),
    //definimos que un Job es "menor" si su prioridad es menor.
    bool operator<(const Job& otro) const {
    return prioridad < otro.prioridad;
    }

};

#endif // JOB_H_INCLUDED
