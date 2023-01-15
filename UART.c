#include "lib/include.h"

//configuración UART0, Baud-rate 9600
//PA0->Rx
//PA1->Tx
extern void Configurar_UART0(void)
{
    SYSCTL->RCGCUART  = (1<<0); //habilitar e inicializar el UART0 (RCGCUART) p. 344 
    //0->deshabilitado
    //1->habilitar
    SYSCTL->RCGCGPIO |= (1<<0); //habilitar reloj del puerto A (RCGCGPIO) p. 340 
    //UART 0-> PA0 y PA1 p. 1351
    
    GPIOA->AFSEL = (1<<1) | (1<<0); //habilitar funciones alternativas en los pines A1 y A0 p. 672
    //0->GPIO
    //1->función alternativa
    GPIOA->PCTL = (GPIOA->PCTL&0xFFFFFF00) | 0x00000011; // p. 688
    //se pone un 1 en el bit 0 y 4 (p. 688)
    GPIOA->DEN = (1<<0) | (1<<1); //habilitar pines como digitales (DEN) p. 682
    
    UART0->CTL = (0<<9) | (0<<8) | (0<<0); //deshabilitar UART, transmisión y recepción para configurarlo p. 918

    // UART Integer Baud-Rate Divisor (UARTIBRD) pag.914
    /*
    BRD = fclk/(16*baud-rate) = 20,000,000 / (16*9600) = 130.20
    UARTFBRD[DIVFRAC] = integer(.20 * 64 + 0.5) = 13.3
    */
    UART0->IBRD = 130; 
    UART0->FBRD = 14;  // UART Fractional Baud-Rate Divisor (UARTFBRD) p. 915
    
    UART0->LCRH = (0x3<<5)|(1<<4); //  UART Line Control (UARTLCRH) p. 916
    //0x3->longitud de 8 bits
    //4->se habilita el fifo
    UART0->CC =(0<<0); //  UART Clock Configuration (UARTCC) p .939
    //se pone 0 para especificar que se trabaja con el reloj del sistema

    UART0->CTL = (1<<0) | (1<<8) | (1<<9); //habilitar
}

extern char readChar(void)
{
    //UART FR flag pag 911
    //UART DR data 906
    int v;
    char c;
    while((UART0->FR & (1<<4)) != 0 );
    v = UART0->DR & 0xFF;
    c = v;
    return c;
}
extern void printChar(char c)
{
    while((UART0->FR & (1<<5)) != 0 );
    UART0->DR = c;
}
extern void printString(char* string)
{
    while(*string)
    {
        printChar(*(string++));
    }
}

extern char * readString(char delimitador)
{

   int i=0;
   char *string = (char *)calloc(10,sizeof(char));
   char c = readChar();
   while(c != delimitador)
   {
       *(string+i) = c;
       i++;
       if(i%10==0)
       {
           string = realloc(string,(i+10)*sizeof(char));
       }
       c = readChar();
   }

   return string;

}

