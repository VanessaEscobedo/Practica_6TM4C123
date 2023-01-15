#include "lib/include.h"

//timer 0 32 bits

void (*PeriodicTask)(void); 
extern void Configurar_Timer0A(void(*task)(void), unsigned long int periodo)
{
    SYSCTL->RCGCTIMER |= (1<<0); //habilitar timer 0 (RCGCTIMER) p. 338
    PeriodicTask = task; //asignat dirección de memoria

    TIMER0->CTL = (0<<0); //deshabilitar el timer antes de configurarlo (CTL) p. 722

    TIMER0->CFG = 0x00000000; //selección modo de operación (32 bits) p. 728
    TIMER0->TAMR = (0x2<<0); //configurar el modo del timer (Periodic Timer Mode) p. 732
    //timer A
 
    TIMER0->TAILR = periodo - 1; //carga
    TIMER0->ICR = 1<<0; //limpiar
    TIMER0->IMR = 1<<0; //habilitar la mascara de la interupción

    NVIC->IP[4] = (NVIC->IP[4]&0x00FFFFFF) | 0x20000000; //**
    //[n]->numero de prioridad 
    //00 en la posición 3 porqu es 4n+3

    NVIC->ISER[0] = (1UL << 19); //tabla interrupciones p. 104 
    // numero de interrupcion TIMER0A = 19 
    // n=4 ----> [4n+3] [4n+2] [4n+1] [4n] ---> [4n+3]
    
    TIMER0->CTL = (1<<0);
}

extern void Timer0A_Handler(void)
{
  TIMER0->ICR = 1<<0;// acknowledge TIMER0A timeout
  (*PeriodicTask)();           
}


