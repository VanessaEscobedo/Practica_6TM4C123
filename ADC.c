#include "lib/include.h"

//modulo 0->sec2
//1->PE2, 2->PE1, 8->PE5 (3 canales para los 3 potenciómetros)

extern void Configura_Reg_ADC0(void)
{
    SYSCTL->RCGCADC = (1<<0); //inicializar el modulo 0 de reloj del adc (RCGCADC) p. 352 
    //                     F     E      D       C      B     A      
    SYSCTL->RCGCGPIO |= (1<<5)|(1<<4)|(1<<3)|(0<<2)|(0<<1)|(1<<0); //habilitar reloj puertos A, D, E y F p. 340, 801
    //ADC->puerto E
    
    //p. 663 (GPIODIR) habilita los pines como I/O 
    GPIOE_AHB->DIR = (0<<2) | (0<<1) | (0<<5); //PE2, PE5 y PE1
    //1->salida
    //0->entrada
    
    //habilitar funciones alternativas, para que el modulo analógico tenga control de esos pines (GPIOAFSEL) p. 672 
    GPIOE_AHB->AFSEL = (1<<2) | (1<<1) | (1<<5); 
    
    GPIOE_AHB->DEN = (0<<2) | (0<<1) | (0<<5);  //deshabilita el modo digital (GPIODEN) p. 683
    
    //registro combinado con el GPIOAFSEL y la tabla p. 1351, p. 688 (GPIOPCTL) 
    GPIOE_AHB->PCTL = GPIOE_AHB->PCTL & (0xFF0FF00F); //la posición indica el pin, el 0 que es ADC
    
    GPIOE_AHB->AMSEL = (1<<2) | (1<<1) | (1<<5); //habilitar analogico (GPIOAMSEL) p. 687 
    
    //el registro (ADCPC) establece la velocidad de conversión por segundo p. 891
    ADC0->PC = (0<<2)|(1<<1)|(1<<0); //para config. a 250 ksps (0x3)
    
    //este registro (ADCSSPRI) configura la prioridad de los secuenciadores p. 841
    ADC0->SSPRI = 0x3021; //mayor prioridad sec2
    
    //(ADCACTSS) este registro controla la activación de los secuenciadores p. 821
    ADC0->ACTSS  =   (0<<3) | (0<<2) | (0<<1) | (0<<0); //primero se desactivan para config.
    
    //este registro (ADCEMUX) selecciona el evento que activa la conversión (trigger) p. 834
    ADC0->EMUX  = (0x0000); //se configura por procesador
    
    //este registro (ADCSSMUX2) define las entradas analógicas con el canal y secuenciador seleccionado p. 867
    ADC0->SSMUX1 = 0x0128; //canal 1, 2 y 8 para modulo 0
    
    //este registro (ADCSSCTL2), configura el bit de control de muestreo y la interrupción p. 868
    ADC0->SSCTL1 = (1<<6) | (1<<5) |  (1<<2) | (1<<1) | (1<<10) | (1<<9);
    
    //*enable ADC Interrupt interrumpe la conversión p. 825
    ADC0->IM |= (1<<2); //para indicar que manda la señal
    //NVIC_PRI4_R = (NVIC_PRI4_R & 0xFFFFFF00) | 0x00000020;
    //NVIC_EN0_R = 0x00010000;
    
    //p. 821 (ADCACTSS) Este registro controla la activación de los secuenciadores
    ADC0->ACTSS = (0<<3) | (1<<2) | (0<<1) | (0<<0); //sec2

    ADC0->PSSI |= (1<<2); //se habilita el registro cuando se configura por procesador 
}


extern void ADC0_InSeq2(uint16_t *Result,uint16_t *duty)
{
       //ADC Processor Sample Sequence Initiate (ADCPSSI)
       ADC0->PSSI = 0x00000004; //por procesador
       while((ADC0->RIS&0x04)==0){}; 
       Result[2] = ADC0->SSFIFO2&0xFFF; 
       Result[1] = ADC0->SSFIFO2&0xFFF;
       Result[0] = ADC0->SSFIFO2&0xFFF;
    
       duty[0] = 50000-(Result[0]*5000)/4096;
       duty[1] = 50000-(Result[1]*5000)/4096;
       duty[2] = 50000-(Result[2]*5000)/4096;
       
       ADC0->ISC = 0x0004;  //conversion finalizada
}


