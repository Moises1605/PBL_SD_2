#include <stdio.h>
#include <lcd.h>
#include <wiringPi.h>

//Saidas LCD
#define LCD_RS  25 
#define LCD_E   1  
#define LCD_D4  12 
#define LCD_D5  16 
#define LCD_D6  20
#define LCD_D7  21

//Entradas DIP SWICTH
#define DIP_1 4
#define DIP_2 17
#define DIP_3 27
#define DIP_4 22

//Entradas Botões
#define BUTTON_1 5
#define BUTTON_2 19
#define BUTTON_3 26 

//Potenciomentto
#define POTEN_SDA 2
#define POTEN_SCL 3
 
int main()
{

    wiringPiSetup();    
    pinMode(DIP_1,INPUT); // alarme dip switch
    
    int lcd = lcdInit(2,16,4,LCD_RS,LCD_E,LCD_D4,LCD_D5,LCD_D6,LCD_D7,0,0,0,0); 
    double entrada_temperatura;
    char entrada_temperaturaAux[10]; 
    double faixa_superior;
    char faixa_superiorAux[10];
    double faixa_inferior;
    char faixa_inferiorAux[10];

    while(1)
    {   

        if(BUTTON_3 == HIGH){
            entrada_temperatura = digitalRead(POTEN_SDA) /** 10*/;
            lcdPuts(lcd,digitalRead(POTEN_SDA));
        }
        if(BUTTON_1 == HIGH){
            faixa_inferior = digitalRead(POTEN_SCL) /** 10*/;
            lcdPuts(lcd,digitalRead(POTEN_SCL));
        }
        if(BUTTON_2){
            faixa_superior = digitalRead(POTEN_SCL)/* * 10*/;
            lcdPuts(lcd,digitalRead(POTEN_SCL));
        }

        /*if(BUTTON_3 == HIGH){
            entrada_temperatura = digitalRead(POTEN_SDA)* 10;
            sprintf(entrada_temperaturaAux,"%f",entrada_temperatura)
            lcdPuts(lcd,entrada_temperaturaAux);
        }
        if(BUTTON_1 == HIGH){
            faixa_inferior = digitalRead(POTEN_SCL)* 10;
            sprintf(faixa_inferiorAux,"%f",faixa_inferior)
            lcdPuts(lcd,faixa_inferiorAux);
        }
        if(BUTTON_2){
            faixa_superior = digitalRead(POTEN_SCL)* 10;
            sprintf(faixa_superiorAux,"%f",faixa_superior)
            lcdPuts(lcd,faixa_superiorAux);
        }*/

        lcdClear(lcd);
        lcdPosition(lcd, 0, 0);
        lcdPuts(lcd,"Ar D");
        lcdPosition(lcd, 5, 0);
        lcdPuts(lcd,"Luz L");
        lcdPosition(lcd, 0, 1);
        lcdPuts(lcd,"Alarme L"); //delay
        lcdPosition(lcd, 0, 0); //protocolo i2c //nilton braga // 0 -- 3000 e pouco 14bits
    }
    return 0;
}