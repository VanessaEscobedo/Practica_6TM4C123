#include "lib/include.h"

//para el ADC (se puede usar cualquier modulo, canal y sec)
//modulo 0->sec1
//canal 2->PE1 (se necesitan dos canales para las dos señales)
//canal 1->PE2

extern void Configura_Reg_ADC0(void)
{
    SYSCTL->RCGCADC = (1<<0); //inicializar el modulo 0 de reloj del adc (RCGCADC) p. 352 
    //                     F     E      D       C      B     A      
    SYSCTL->RCGCGPIO |= (1<<5)|(1<<4)|(1<<3)|(0<<2)|(0<<1)|(1<<0); //habilitar reloj puertos A, D, E y F p. 340, 801
    //ADC->puerto E
    
    //p. 663 (GPIODIR) habilita los pines como I/O 
    GPIOE_AHB->DIR = (0<<2) | (0<<1); //PE
    //1->salida
    //0->entrada
    
    GPIOE_AHB->AFSEL = (1<<2) | (1<<1); //habilitar funciones alternativas, para que el modulo analógico tenga control de esos pines (GPIOAFSEL) p. 672 
    
    GPIOE_AHB->DEN = (0<<2) | (0<<1);  //deshabilita el modo digital (GPIODEN) p. 683
    
    //registro combinado con el GPIOAFSEL y la tabla p. 1351, p. 688 (GPIOPCTL) 
    GPIOE_AHB->PCTL = GPIOE_AHB->PCTL & (0xFFFFF00F); //la posición indica el pin, el 0 que es ADC
    
    GPIOE_AHB->AMSEL = (1<<2) | (1<<1); //habilitar analogico (GPIOAMSEL) p. 687 
    
    //el registro (ADCPC) establece la velocidad de conversión por segundo p. 891
    ADC0->PC = (0<<2)|(1<<1)|(1<<0); //para config. a 250 ksps (0x3)
    
    //este registro (ADCSSPRI) configura la prioridad de los secuenciadores p. 841
    ADC0->SSPRI = 0x3201; //mayor prioridad sec1
    
    //(ADCACTSS) este registro controla la activación de los secuenciadores p. 821
    ADC0->ACTSS  =   (0<<3) | (0<<2) | (0<<1) | (0<<0); //primero se desactivan para config.
    
    //este registro (ADCEMUX) selecciona el evento que activa la conversión (trigger) p. 834
    ADC0->EMUX  = (0x0050); //se configura por timer
    //0x5->timer, el trigger debe ser establecido con el bit tnOTE en el registro GPTMCTL (p. 737)
    //se pone un 5 en la posición 1 porque cada posición corresponde a un EMUX (0-3) y estoy trabajando son sec1
    
    //este registro (ADCSSMUXn) define las entradas analógicas con el canal y secuenciador seleccionado p. 867
    ADC0->SSMUX1 = 0x0021; //canal 1 y 2
    
    //este registro (ADCSSCTLn), configura el bit de control de muestreo y la interrupción p. 868
    //se debe configurar para cada muestra (canal)
    ADC0->SSCTL1 = (1<<6) | (1<<5) | (1<<2) | (1<<1); //para 2 canales

    //*enable ADC Interrupt interrumpe la conversión p. 825
    //ADC0->IM |= (1<<2); //para indicar que manda la señal
    //NVIC_PRI4_R = (NVIC_PRI4_R & 0xFFFFFF00) | 0x00000020;
    //NVIC_EN0_R = 0x00010000;
    
    //p. 821 (ADCACTSS) Este registro controla la activación de los secuenciadores
    ADC0->ACTSS = (0<<3) | (0<<2) | (1<<1) | (0<<0); //se activa el sec1

    //ADC0->PSSI |= (1<<2); //se habilita el registro cuando se configura por procesador 
}


extern void ADC0_InSeq2(uint16_t *Result,uint16_t *duty)
{
       ADC0->PSSI = 0x00000002; //para iniciar el muestreo en los secuenciadores (ADCPSSI) p. 845
       //se pone un 2 porque se pone un 1 en el bit 1 (0<<3) | (0<<2) | (1<<1) |(0<<0)
       //1->comienza el muestreo en el sec
       //0->sin efecto
       while((ADC0->RIS&0x02)==0){}; //esperar hasta que termine la conversión
       Result[0] = ADC0->SSFIFO2&0xFFF; //leer el valor del adc
       Result[1] = ADC0->SSFIFO2&0xFFF;

       /*duty[0] = 50000-(Result[0]*5000)/4096; //hace la conversión
       duty[1] = 50000-(Result[1]*5000)/4096;*/
       
       ADC0->ISC = 0x0002;  //conversion finalizada
}


