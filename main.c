#include "lib/include.h"

volatile uint16_t Result[2]; //1024 es 10 bits
uint16_t duty[2];
float valor1;
float valor2;

    /*Configurar el timer 0 y timer 1 en modo de 32 bits para hacer la siguiente rutina
    * una función que active el convertidor analógico digital y muestre dos señales de cierta
    * frecuencia y amplitud definida y los guarde en dos arreglos de tamaño 1024 y la
    * envíe por el protocolo de comunicación asíncrono (UART) para ser procesadas y regresadas al
    * microcontrolador en valores binarios las cuales modificaran el ancho de pulso y 
    * reconstruirán la señal enviada a un transistor o opam 
    * b) 2kHz, -5 a 5
    * 
    * 1. configurar timer 0 y 1 (el timer debe de activar al ADC)
    * 2. hacer la conversión (ADC)
    * 3. guardar dos señales en dos arreglos de 1024
    * 4. enviar las señales por el uart 
    * 5. regresar las señales como valores binarios
    */

void Hilo_1(void)
{
    GPIOB_AHB->DATA = (1<<0);
    ADC0_InSeq2(Result,duty);
    GPIOB_AHB->DATA = (0<<0);
}

int main(void)
{

    Configurar_PLL(_20MHZ);  //configuracion de velocidad de reloj
    Configurar_UART0();
    Configura_Reg_ADC0();
    Configura_Reg_PWM1(8);
    Configurar_Timer0A(&Hilo_1,10000); //periodo
   

    while (1)
    {
        //ADC0_InSeq2(Result,duty); //llamada a la conversion por procesador
        //PWM0->_0_CMPB = duty[0];
        //PWM0->_1_CMPA = duty[1];
        //PWM0->_2_CMPA = duty[2];

        valor1=(char)(((Result[0]))*3.3)/4096;
        valor2=(char)(((Result[1]))*3.3)/4096;

        printChar(valor1);
        printChar(valor2);
    }
    
}

