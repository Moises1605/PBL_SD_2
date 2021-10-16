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


char saida_iluminacao_interna[];
char saida_Saida_alarme[];
char saida_ar_condicionado[];

/*Lista de entradas do programa*/
int sensor_PJ[];
int sensor_presenca;
float entrada_temperatura = 17;
float faixa_operacao_inferior = 17;
float faixa_operacao_superior = 23;
int entrada_iluminacao_interna;
int entrada_alarme;


char saida_ar_condicionado[] = "";
char saida_iluminacao_jardim[] = "";
char saida_iluminacao_Garagem[] = "";



int on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
    char* payload = message->payload;

    if(topicName == TOPIC_Alarme_P){
        saida_Saida_alarme = message->payload;
    }else if(topicName == TOPIC_faixaOPI_P){
        faixa_operacao_inferior =  atof(message->payload);
    }else if(topicName == TOPIC_faixaOPS_P){
        faixa_operacao_superior = atof(message->payload);
    }else if(topicName == TOPIC_ILUMINACAO_INTERNA){ //Nome para printar
        saida_iluminacao_interna = message->payload;
    }/*else if(topicName == "sensorPJ"){
        sensor_PJ = message->payload - '0';
    }else if(topicName == "sensorPresenca"){
        sensor_presenca = message->payload - '0';
    }*/else if(topicName == TOPIC_EST_ILUMINACAO_INTERNA){
        entrada_iluminacao_interna = message->payload - '0';
    }else if(topicName == TOPIC_ESTADO_ALARME){
        entrada_alarme = message->payload - '0';
    }
 
    /* Mostra a mensagem recebida */
    printf("Mensagem recebida! \n\rTopico: %s Mensagem: %s\n", topicName, payload);
 
    /* Faz echo da mensagem recebida */
    publish(client, MQTT_PUBLISH_TOPIC, payload);
 
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

int main(){ 
    
    MQTTBegin();

    MQTTSubscribe(TOPIC_ENTRADA_ILUMINACAO_I);
    MQTTSubscribe(TOPIC_ENTRADA_ALARME);
    MQTTSubscribe(TOPIC_ENTRADA_FAIXA_S);
    MQTTSubscribe(TOPIC_ENTRADA_FAIXA_I);
    

    int lcd;
    int horario;
    int faixa_operacao_S;
    int faixa_operacao_I;
    int temopar;
    int timeout;

    //Para receber as atualizações do site.
    MQTTClient_setCallbacks(client, NULL, NULL, on_message, NULL);
    //Para manter um loop infinito
    while(1){ 
        MQTTPublish(TOPIC_ARCONDICIONADO, "desligado");

        //printar mensagens no lcd
        /*Por padrão, o texto é impresso na tela na linha superior, segunda coluna. Para alterar a posição, use 
        lcdPosition (lcd, COLUMN, ROW).Em um LCD 16 × 2, as linhas são numeradas de 0 a 1 e as colunas são numeradas de 0 a 15.*/
    }

    MQTTDisconnect();
    return 0;
} 

