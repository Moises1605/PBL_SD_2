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
//#include <mosquitto.h>
#include <string.h> 

#include "mqtt.h"
#include "credentials.h" 
#include "MQTTClient.h"

MQTTClient client;

char* saida_iluminacao_interna;
char* saida_Saida_alarme;
char saida_ar_condicionado[];

/*Lista de entradas do programa*/
int sensor_PJ = 0;
int sensor_presenca;
float entrada_temperatura = 17;
float faixa_operacao_inferior = 17;
float faixa_operacao_superior = 23;
char* entrada_iluminacao_interna;
char* entrada_alarme;


char saida_ar_condicionado[];
char saida_iluminacao_jardim[];
char saida_iluminacao_Garagem[];

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

        if(topicName == TOPIC_Alarme_P){
            saida_Saida_alarme = payload;
        }else if(topicName == TOPIC_faixaOPI_P){
            faixa_operacao_inferior =  atof((char*)message->payload);
        }else if(topicName == TOPIC_faixaOPS_P){
            faixa_operacao_superior = atof((char*)message->payload);
        }else if(topicName == TOPIC_ILUMINACAO_INTERNA){ //Nome para printar
            saida_iluminacao_interna = (char*)message->payload;
        }/*else if(topicName == "sensorPJ"){
            sensor_PJ = message->payload - '0';
        }else if(topicName == "sensorPresenca"){
            sensor_presenca = message->payload - '0';
        }*/else if(topicName == TOPIC_EST_ILUMINACAO_INTERNA){
            entrada_iluminacao_interna = (char*)message->payload - '0';
        }else if(topicName == TOPIC_ESTADO_ALARME){
            entrada_alarme = (char*)message->payload - '0';
        } 
    }

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);

    return 1;
}

void connlost(void *context, char *cause)
{
    printf("Connection lost\n");
    if (cause)
        printf("Reason is : %s\n", cause);
    MQTTDisconnect();
    /* Force to reconnect! */
    MQTTBegin();
}

void MQTTSubscribe(const char* topic)
{
    printf("Subscribing to topic %s for client %s using QoS%d\n\n", 
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
    /*printf("Waiting for publication of message: %s\n"
            "topic: %s\n client: %s\n",
            message, TOPIC, CLIENTID);*/
    int rc = MQTTClient_waitForCompletion(client, token, 1000);
    /*printf("Message with delivery token %d delivered\n", token);*/
}

void MQTTDisconnect()
{
    MQTTClient_disconnect(client, TIMEOUT);
    MQTTClient_destroy(&client);
}

void MQTTBegin()
{
    int rc = -1;
    printf("Initializing MQTT...\n");
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    conn_opts.keepAliveInterval = KEEP_ALIVE;
    conn_opts.cleansession = 1;
    conn_opts.username = USERNAME;
    conn_opts.password = PASSWORD;
    MQTTClient_create(&client, BROKER_ADDR, CLIENTID,
        MQTTCLIENT_PERSISTENCE_NONE, NULL);

    /* Set connection, subscribe and publish callbacks. */
    MQTTClient_setCallbacks(client, NULL, connlost, on_message, NULL);

    while ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        sleep(TIMEOUT / 1000); 
    }
}

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

/*const static char* topic[TOPIC_NUM] = {
    "arCondicionado",
    "alarme ",
    "sensorPJ ",
    "faixaOpI",
    "faixaOpS",
    "iluminacaoInterna",
    "sensorPresenca",
    "temperatura",
    "iluminacaoGaragem",
    "iluminacaoJardim"
};*/




int main(){ 
    
    MQTTBegin();

    //MQTTSubscribe(TOPIC_LUZ_INTERNA);
    //MQTTSubscribe(TOPIC_ALARME);
    //MQTTSubscribe(TOPIC_OPERACAO_SUPERIOR);
    //MQTTSubscribe(TOPIC_OPERACAO_INFERIOR);
        MQTTSubscribe(TOPICC);

    int lcd;
    int horario;
    int faixa_operacao_S;
    int faixa_operacao_I;
    int temopar;
    int timeout;

    //Para receber as atualizações do site.
        //Para manter um loop infinito
    while(1){ 
        MQTTPublish(TOPICC, "desligado");
        sleep(5000 / 1000);
        //printar mensagens no lcd
        /*Por padrão, o texto é impresso na tela na linha superior, segunda coluna. Para alterar a posição, use 
        lcdPosition (lcd, COLUMN, ROW).Em um LCD 16 × 2, as linhas são numeradas de 0 a 1 e as colunas são numeradas de 0 a 15.*/
    }

    MQTTDisconnect();
    return 0;
} 

