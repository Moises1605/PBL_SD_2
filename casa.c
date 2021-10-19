/*
5.1. O acionamento remoto deve ser feito por meio de uma página Web, que deve
apresentar no mínimo:
    5.1.1. Estado atual das lâmpadas da residência;
    5.1.2. Acionamento remoto da iluminação dos ambientes internos. Esse
    acionamento deve ser programável;
    5.1.3. Ativação e desativação remota do alarme;
    5.1.4. Estado atual do alarme e notificação de uma possível invasão;
    5.1.5. Histórico mensal do alarme, contendo a data, o horário e o tipo de evento
    ocorrido;
    5.1.6. Estado atual do ar-condicionado e a temperatura atual da sala;
    5.1.7. Interface para ajuste da faixa de operação do ar-condicionado.
    5.2. Usar recursos do ambiente Amazon Web Services (AWS).
*/
#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>
#include <wiringPi.h>
#include <lcd.h>
#include <string.h> 
#include "mqtt.h"
#include "credentials.h"
#include "MQTTClient.h"

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

MQTTClient client;

char* saida_iluminacao_interna;
char* saida_Saida_alarme;

int saida_iluminacao_internaAux = 0;
int saida_Saida_alarmeAux = 0;
int saida_ar_condicionadoAux = 0;
int horario = 12;

/*Lista de entradas do programa*/
int sensor_PJ = 0;
int sensor_presenca = 0;
int entrada_temperatura = 19;
char entrada_temperatura1[2] = "19";
char faixa_operacao_inferior[2];
char faixa_operacao_superior[2];
char* entrada_iluminacao_interna = "0";
int entrada_iluminacao_internaAux = 0;
int entrada_alarme;
int entrada_alarmeAux = 0;


char* saida_ar_condicionado;
int saida_iluminacao_jardim;
int saida_iluminacao_Garagem;

int faixa_operacao_S = 23;
int faixa_operacao_I = 17;
int temopar =1;
int timeout = 2;
char texto_lcd[100];



/* Subscribed MQTT topic listener function. */
int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    if(message) {
        printf("Message arrived\n");
        printf("  topic: %s\n", topicName);
        printf("  message: ");
        printf("%s\n", (char*)message->payload);
    }

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);

    return 1;
}

int on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message) {

    if(message) {
        printf("Mensagem chegou\n");
        printf("  No topico: %s\n", topicName);
        printf("  A mensagem: ");
        printf("%s\n", (char*)message->payload);

        char* payload = message->payload;

        if(strcmp(topicName,TOPIC_Alarme_P) == 0){
            entrada_alarme = atoi(payload);
        }else if(strcmp(topicName,TOPIC_faixaOPI_P) == 0){
            faixa_operacao_I = atoi(payload);
        }else if(strcmp(topicName,TOPIC_faixaOPS_P) == 0){
            faixa_operacao_S = atoi(payload);
        }else if(topicName == TOPIC_ILUMINACAO_INTERNA){ //Nome para printar
            saida_iluminacao_interna = payload;
        }/*else if(topicName == "sensorPJ"){
            sensor_PJ = message->payload - '0';
        }else if(topicName == "sensorPresenca"){
            sensor_presenca = message->payload - '0';
        }*/else if(strcmp(topicName,"moisesalmeida123_@hotmail.com/estIlumincaoInternaP") == 0){
            entrada_iluminacao_interna = message->payload;
            if(strcmp(entrada_iluminacao_interna,"1") == 0){
                entrada_iluminacao_internaAux = 1;
            }else{
                entrada_iluminacao_internaAux = 0;
            }
        }else if(strcmp(topicName,"moisesalmeida123_@hotmail.com/horario") == 0){
            horario = atoi(message->payload);
        }else if(strcmp(topicName,TOPIC_TEMPERATURA_P) == 0){
           entrada_temperatura = atoi(payload);
        } 
    }

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);

    return 1;
}

void connlost(void *context, char *cause)
{
    printf("Conexao perdida\n");
    if (cause)
        printf("Razao e : %s\n", cause);
    MQTTDisconnect();
    /* Force to reconnect! */
    MQTTBegin();
}

void MQTTSubscribe(const char* topic)
{
    printf("Subscribing no topico %s para o cliente %s usando QoS%d\n\n", 
        topic, CLIENTID, QOS);
    MQTTClient_subscribe(client, topic, QOS);
}

void MQTTPublish(const char* topic, char* message)
{
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;
    pubmsg.payload = message;
    pubmsg.payloadlen = (int)strlen(message);
    pubmsg.qos = QOS;
    pubmsg.retained = 1;
    MQTTClient_publishMessage(client, topic, &pubmsg, &token);
    int rc = MQTTClient_waitForCompletion(client, token, 1000);
}

void MQTTDisconnect()
{
    MQTTClient_disconnect(client, TIMEOUT);
    MQTTClient_destroy(&client);
}

void MQTTBegin()
{
    int rc = -1;
    printf("Iniciando conexao com o broker Maqiatto...\n");
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    conn_opts.keepAliveInterval = KEEP_ALIVE;
    conn_opts.cleansession = 1;
    conn_opts.username = USERNAME;
    conn_opts.password = PASSWORD;
    MQTTClient_create(&client, BROKER_ADDR, CLIENTID,
        MQTTCLIENT_PERSISTENCE_NONE, NULL);

    MQTTClient_setCallbacks(client, NULL, connlost, on_message, NULL);

    while ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        printf("Falha ao conectar, return code %d\n", rc);
        sleep(TIMEOUT / 1000); 
    }
}

int main(){ 
    
    MQTTBegin();

    MQTTSubscribe(TOPIC_ILUMINACAO_INTERNA);
    MQTTSubscribe(TOPIC_Alarme_P);
    MQTTSubscribe(TOPIC_faixaOPI_P);
    MQTTSubscribe(TOPIC_faixaOPS_P);
    MQTTSubscribe(TOPIC_ILUMINACAO_INTERNA);
    MQTTSubscribe(TOPIC_EST_ILUMINACAO_INTERNA);
    MQTTSubscribe(TOPIC_HORARIO);
    MQTTSubscribe(TOPIC_TEMPERATURA_P);
    
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
    
    //Para manter um loop infinito
    while(1){ 

        /*entrada_temperatura = digitalRead(POTEN_SDA) * 10;
        if(BUTTON_1 == LOW){
            entrada_temperatura = digitalRead(POTEN_SCL) * 10;
        }

        if(BUTTON_2 == LOW){
            entrada_temperatura = digitalRead(POTEN_SCL) * 10;
        }*/
        
        //fazer a conversão de tipos para esses valores
        sprintf(entrada_temperatura1, "%d", entrada_temperatura);
        MQTTPublish(TOPIC_TEMPERATURA, entrada_temperatura1);
        sprintf(faixa_operacao_inferior, "%d", faixa_operacao_I);
        MQTTPublish(TOPIC_OPERACAO_INFERIOR, faixa_operacao_inferior);
        sprintf(faixa_operacao_superior, "%d", faixa_operacao_S);
        MQTTPublish(TOPIC_OPERACAO_SUPERIOR, faixa_operacao_superior);


        if(digitalRead(DIP_1) == HIGH){
            MQTTPublish(TOPIC_ESTADO_ALARME, "ligado");
        }else{
            MQTTPublish(TOPIC_ESTADO_ALARME, "desligado");
        }

        //iluminacao_garagem
        if((digitalRead(DIP_2) == HIGH ) && !(horario > 6 && horario < 18)){
            saida_iluminacao_Garagem = 1;
            MQTTPublish(TOPIC_LUZ_GARAGEM, "ligado");
        }else{
            saida_iluminacao_Garagem = 0;
            MQTTPublish(TOPIC_LUZ_GARAGEM, "desligado");
        }

        //iluminacai_jardim
        if(horario >= 18 && horario <= 23){
            saida_iluminacao_jardim = 1;
            MQTTPublish(TOPIC_LUZ_JARDIM, "ligado");
        }else{
            saida_iluminacao_jardim = 0;
            MQTTPublish(TOPIC_LUZ_JARDIM, "desligado");
        }


        //iluminação interna
        if(digitalRead(DIP_2) == HIGH || entrada_iluminacao_internaAux == 1){
            saida_iluminacao_internaAux = 1;
            saida_iluminacao_interna = "Luz L";
            MQTTPublish(TOPIC_LUZ_INTERNA, "ligado");
        }else{
            saida_iluminacao_internaAux = 0;
            saida_iluminacao_interna = "Luz D";
            MQTTPublish(TOPIC_LUZ_INTERNA, "desligado");
        } 

        //alarme
        if(((digitalRead(DIP_1) == HIGH) || entrada_alarme == 1) && ((digitalRead(DIP_2) == HIGH) || (digitalRead(DIP_3) == HIGH))){
            saida_Saida_alarmeAux = 1;
            saida_Saida_alarme = "Alarme L";
            MQTTPublish(TOPIC_ALARME, "ligado");
        }else{
            saida_Saida_alarmeAux = 0;
            saida_Saida_alarme = "Alarme D";
            MQTTPublish(TOPIC_ALARME, "desligado");
        } 

        //ar condicionado
        //if(temopar){
            if(faixa_operacao_I >= faixa_operacao_S){
                saida_ar_condicionadoAux = 0;
                saida_ar_condicionado = "ar D";
                MQTTPublish(TOPIC_ARCONDICIONADO, "desligado");
            }else{
                if(digitalRead(DIP_2) == HIGH){
                    if(entrada_temperatura <= faixa_operacao_I){
                        saida_ar_condicionadoAux = 0;
                        saida_ar_condicionado = "ar D";
                        MQTTPublish(TOPIC_ARCONDICIONADO, "desligado");
                    }else if(entrada_temperatura >= faixa_operacao_S){
                        saida_ar_condicionadoAux = 1;
                        saida_ar_condicionado = "ar L";
                        MQTTPublish(TOPIC_ARCONDICIONADO, "ligado");
                    }
                }else{
                    saida_ar_condicionadoAux = 0;
                    saida_ar_condicionado = "ar D";
                    MQTTPublish(TOPIC_ARCONDICIONADO, "desligado");
                }
            }
            if(entrada_temperatura == 17){
                saida_ar_condicionado = "ar D";
                MQTTPublish(TOPIC_ARCONDICIONADO, "desligado");
                temopar = 0;
                
            }
        //}
        lcdClear(lcd);
        lcdPosition(lcd, 0, 0);
        if(saida_iluminacao_internaAux == 1){
            lcdPuts(lcd,"LUZ L");
        }else{
            lcdPuts(lcd,"LUZ D");
        }
        sleep(3000);
        lcdPosition(lcd, 5, 0);
        if(saida_Saida_alarmeAux == 1){
            lcdPuts(lcd,"Alarme L");
        }else{
            lcdPuts(lcd,"Alarme D");
        }
        sleep(3000);
        lcdPosition(lcd, 0, 1);
        if(saida_ar_condicionadoAux == 0){
            lcdPuts(lcd,"AR L");
        }else{
            lcdPuts(lcd,"AR D");
        }
        lcdPosition(lcd, 5, 1);
        if(saida_iluminacao_jardim == 1){
            lcdPuts(lcd,"LJ L");
        }else{
            lcdPuts(lcd,"LJ D");
        }
        sleep(3000);
        lcdPosition(lcd, 10, 0);
        if(saida_iluminacao_Garagem == 1){
            lcdPuts(lcd,"LG L");
        }else{
            lcdPuts(lcd,"LG D");
        }
        sleep(5000);
        
        //printar mensagens no lcd
        
        /*Por padrão, o texto é impresso na tela na linha superior, segunda coluna. Para alterar a posição, use 
        lcdPosition (lcd, COLUMN, ROW).Em um LCD 16 × 2, as linhas são numeradas de 0 a 1 e as colunas são numeradas de 0 a 15.*/
    }

    MQTTDisconnect();
    return 0;
} 

