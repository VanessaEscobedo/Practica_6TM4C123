#include "lib/include.h"

//modulo 0, generador 0 y 1
//M0PWM1->PB7 (generador 0)
//M0PWM2->PB4 (generador 1)
extern void Configura_Reg_PWM1(uint16_t freq)
{
    SYSCTL->RCGCPWM |= (1<<0); //habilitar reloj de modulo PWM0 (RCGCPWM) p. 354
    SYSCTL->RCGCGPIO |= (1<<1); //habilitar reloj de GPIO Puerto B (RCGCGPIO) p. 340
    //PB5 y PB4 trabajan con el módulo 0 y el gen 1
    
    SYSCTL->RCC &= ~(1<<20); //deshabilitar divisor (RCC) p. 254
    //# cuentas = 50,000,000/10,000 = 5000 
    //el divisor se habilita si el numero de cuentas es mayor a 65 mil (16 bits)

    GPIOB->AFSEL |= (1<<4) | (1<<7); //funciones alternativas (AFSEL) p. 672
    //usamos PB4 (PWM2)
    //1->función alternativa
    //0->GPIO (I/O)
    GPIOB->DIR = (1<<4) | (1<<7); //configura el pin correspondiente como I/O p. 663
    //el pin B4 es una salida 
    GPIOB->PCTL |= (GPIOB->PCTL&0x0FF0FFFF) | 0x40040000; //indicar funcion alternativa (PWM) (PCTL) p. 689, 1351
    //el 4 indica que es pwm, la posición que es el pin PB4
    GPIOB->DEN |= (1<<4) | (1<<7); //pin digital p. 682
    
    PWM0->_0_CTL = (0<<0);
    PWM0->_1_CTL = (0<<0); //deshabilitar generador 1, M0PWM2 (CTL) p. 1266
    //1->se habilita todo el bloque PWM
    //0->se deshabilita todo el bloque PWM
    
    PWM0->_0_GENB = 0x0000080C;
    PWM0->_1_GENA = 0x0000008C; //registro de las acciones del pwm p. 1285
    //0x0000008C para modo countdown
    //MnPWM2 trabaja con pwmA' p. 1248
    
    PWM0->_0_LOAD = 5000;
    PWM0->_1_LOAD = 5000; //carga (cuentas = fclk/fpwm) 
    PWM0->_0_CMPB = 2500;
    PWM0->_1_CMPA = 2500; //ciclo de trabajo 50%
    
    PWM0->_0_CTL = (1<<0);
    PWM0->_1_CTL = (1<<0);//activar el generador 1
    
    PWM0->ENABLE = (1<<2) | (1<<1); //habilitar el bloque p. 1248 
}

