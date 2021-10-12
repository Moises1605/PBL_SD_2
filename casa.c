#include <stdio.h>
#include <stdlib.h> 
#include <wiringPi.h>
#include <lcd.h>
#include <mosquitto.h>
#include <string.h> 

#define HOST "localhost"
#define PORT  1883
#define KEEP_ALIVE 60
#define MSG_MAX_SIZE  512
#define TOPIC_NUM 3 

//Entradas DIP SWICTH
#define DIP_1 4
#define DIP_2 17
#define DIP_3 27
#define DIP_4 22

//Entradas Botões
#define BUTTON_1 5
#define BUTTON_2 19
#define BUTTON_3 26


//Entradas Potenciomentto
#define POTEN_SDA 2
#define POTEN_SCL 3

//Saidas LCD
#define LCD_RS  25 
#define LCD_E   1  
#define LCD_D4  12 
#define LCD_D5  16 
#define LCD_D6  20
#define LCD_D7  21

bool session = true;

const static char* topic[TOPIC_NUM] = {
    "arCondicionado",
    "alarme ",
    "sensorPJ ",
    "faixaOpI",
    "faixaOpS",
    "iluminacao1",
    "iluminacao2",
    "iluminacao3",

}; 

int on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
    char* payload = message->payload;
 
    /* Mostra a mensagem recebida */
    printf("Mensagem recebida! \n\rTopico: %s Mensagem: %s\n", topicName, payload);
 
    /* Faz echo da mensagem recebida */
    publish(client, MQTT_PUBLISH_TOPIC, payload);
 
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

int main(){ 
    

    int lcd;
    wiringPiSetupGpio();
    /*Lista de entradas do programa*/ 
    pinMode(DIP_1,INPUT); // alarme
    pinMode(DIP_2,INPUT); // sensor presença
    pinMode(DIP_3,INPUT); // sensor de portas e janelas
    pinMode(DIP_4,INPUT); 

    pinMode(BUTTON_1,INPUT); 
    pinMode(BUTTON_2,INPUT); 
    pinMode(BUTTON_3,INPUT); 

    pinMode(POTEN_SDA,INPUT);
    pinMode(POTEN_SCL,INPUT);

    int lcd = lcdInit(2,16,4,LCD_RS,LCD_E,LCD_D4,LCD_D5,LCD_D6,LCD_D7,0,0,0,0); 
    //sleep(2);

    int horario;
    int faixa_operacao_S;
    int faixa_operacao_I;
    int temopar

    /*Lista de entradas do programa*/
    char saida_iluminacao_interna[];
    char saida_Saida_alarme[];
    char saida_ar_condicionado[];

    char saida_sensot_PJ[] = "";
    char saida_ar_condicionado[] = "";
    char saida_iluminacao_jardim[] = "";
    char saida_iluminacao_Garagem[] = "";

    //Para manter um loop infinito
    MQTTClient_setCallbacks(client, NULL, NULL, on_message, NULL);
    while(1){ 

        //iluminacao_garagem
        if(digitalRead(27) == HIGH && !(horario > 6 && horario < 18)){
            saida_iluminacao_Garagem = 1;
            MQTTPublish(TOPIC_LUZ_Garagem, "test maqiatto.com!");
        }else{
            saida_iluminacao_Garagem = 0;
            MQTTPublish(TOPIC_LUZ_Jardim, "test maqiatto.com!");
        }

        //iluminacai_jardim
        if(horario >= 18 && horario <= 23){
            saida_iluminacao_jardim = 1;
            MQTTPublish(TOPIC, "test maqiatto.com!");
        }else{
            saida_iluminacao_jardim = 0;
            MQTTPublish(TOPIC, "test maqiatto.com!");
        }


        //iluminação interna
        if(digitalRead(27) == HIGH){
            saida_iluminacao_interna = 1;
            saida_iluminacao_interna = "Luz L"
            MQTTPublish(TOPIC, "test maqiatto.com!");
        }else{
            saida_iluminacao_interna = 0;
            saida_iluminacao_interna = "Luz D"
            MQTTPublish(TOPIC, "test maqiatto.com!");
        } 

        //alarme
        if((digitalRead(22) == HIGH)  || (digitalRead(27) == HIGH) && (digitalRead(4) == HIGH)){
            saida_Saida_alarme = "Alarme L";
            MQTTPublish(TOPIC, "test maqiatto.com!");
        }else{
            saida_Saida_alarme = "Alarme D";
            MQTTPublish(TOPIC, "test maqiatto.com!");
        } 

        //ar condicionado
        if(temopar){
            if(faixa_operacao_I >= faixa_operacao_S){
                saida_ar_condicionado = "ar D";
                MQTTPublish(TOPIC, "test maqiatto.com!");
            }else{
                if(digitalRead(27) == HIGH){
                    if(entrada_temperatura <= faixa_operacao_I){
                        saida_ar_condicionado = "ar D";
                        MQTTPublish(TOPIC, "test maqiatto.com!");
                    }else if(entrada_temperatura >= faixa_operacao_S){
                        saida_ar_condicionado = "ar L";
                        MQTTPublish(TOPIC, "test maqiatto.com!");
                    }
                }else{
                    saida_ar_condicionado = "ar D";
                    MQTTPublish(TOPIC, "test maqiatto.com!");
                }
            }
            if(entrada_temperatura == 17){
                saida_ar_condicionado = "ar D";
                MQTTPublish(TOPIC, "test maqiatto.com!");
                temopar = 0;
                
            }
        }

        if(timeout){
            temopar = 1;
        }

        //quebra do loop
        if(0){
            break;
        } 

        //printar mensagens no lcd
        lcdClear(lcd);
        lcdPosition(lcd, 0, 0);
        lcdPuts(lcd,saida_ar_condicionado);
        lcdPosition(lcd, 5, 0);
        lcdPuts(lcd,saida_iluminacao_interna);
        lcdPosition(lcd, 0, 1);
        lcdPuts(lcd,saida_Saida_alarme);
        /*Por padrão, o texto é impresso na tela na linha superior, segunda coluna. Para alterar a posição, use 
        lcdPosition (lcd, COLUMN, ROW).Em um LCD 16 × 2, as linhas são numeradas de 0 a 1 e as colunas são numeradas de 0 a 15.*/
    }

    return 0;
} 

