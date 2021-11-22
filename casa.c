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

//Declaração da variavel para configuração do mqtt
MQTTClient client;

//Variáveis utilizadas para gerenciamento do programa
char* saida_iluminacao_interna;
char* saida_Saida_alarme;

int saida_iluminacao_internaAux = 0;
int saida_Saida_alarmeAux = 0;
int saida_ar_condicionadoAux = 0;
int horario = 12;

int sensor_PJ = 0;
int sensor_presenca = 0;
int entrada_temperatura = 19;
char entrada_temperatura1[2] = "19";
char faixa_operacao_inferior[2];
char faixa_operacao_superior[2];
char* entrada_iluminacao_interna = "0";
int entrada_iluminacao_internaAux = 0;
int entrada_alarme;
char* entrada_alarmeAux;


char* saida_ar_condicionado;
int saida_iluminacao_jardimAux; 
char* saida_iluminacao_jardim;
int saida_iluminacao_GaragemAux;
char* saida_iluminacao_Garagem;

int faixa_operacao_S = 23;
int faixa_operacao_I = 17;
int temopar =1;
int timeout = 2;
char texto_lcd[100];

//Variaveis para manipulação dos arquivos de texto.
FILE *arquivo_log;
FILE *arquivo_log_auxiliar;



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

//Procedimento utilizado caso o dispositivo perca a conexão com o serviço mqtt
void connlost(void *context, char *cause)
{
    printf("Conexao perdida\n");
    if (cause)
        printf("Razao e : %s\n", cause);
    MQTTDisconnect();
    /* Force to reconnect! */
    MQTTBegin();
}

//Procedimento utilizado para dar subscribe no tópico especificado
void MQTTSubscribe(const char* topic)
{
    printf("Subscribing no topico %s para o cliente %s usando QoS%d\n\n", 
        topic, CLIENTID, QOS);
    MQTTClient_subscribe(client, topic, QOS);
}

//Procedimento utilizado para dar publish no tópico especificado
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

//Prodecimento responsável por desconectar o dispositivo do serviço mqtt
void MQTTDisconnect()
{
    MQTTClient_disconnect(client, TIMEOUT);
    MQTTClient_destroy(&client);
}

//Inicia a conexão com o broker
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

//Função utilizada quando o dispositivo recebe alguma mensagem em um dos tópicos escritos
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
        }else if(strcmp(topicName,TOPIC_LOG_P) == 0){
            log_dispositivo(payload);
        }else if(vstrcmp(topicName,TOPIC_deletar_LOG_P) == 0){
            deletarLog(atoi(payload));
        } 
    }

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);

    return 1;
}

//Escreve o log dos dispositivos no arquivo
void log_dispositivo(char* conteudo){
    arquivo_log = fopen("log.txt", "a");
    fprintf(arquivo_log, "%s", conteudo);
    fclose(arquivo_log);
    /*fprintf(arquivo_log, "%s", dispositivo);
    fprintf(arquivo_log, "%s", ": ");
    fprintf(arquivo_log, "%s", conteudo);
    fprintf(arquivo_log, "%s", "\n");*/
}


//Apaga os registros até a linha indicada.
void deletarLog(int num_linha){
    char* conteudo_linha;
    arquivo_log = fopen("log.txt", "a");
    arquivo_log_auxiliar = fopen("log_auxiliar.txt", "a");

    for(int i =0; i <= num_linha; i++){
        fgets (conteudo_linha, 100,arquivo_log);    
    }

    while((fgets(conteudo_linha, 100,arquivo_log)) != EOF){
        fprintf(arquivo_log_auxiliar, "%s", conteudo_linha);
    }

    fclose(arquivo_log);
    remove("log.txt");
    arquivo_log = fopen("log.txt", "a");
    while((fgets(conteudo_linha, 100,arquivo_log)) != EOF){
        fprintf(arquivo_log, "%s", conteudo_linha);
    }

    fclose(arquivo_log_auxiliar);
    fclose(arquivo_log);
    remove("log_auxiliar.txt");

}

int main(){ 
    
    MQTTBegin();//Inicia a conexão com o broker

    //Dar subscribe nos tópicos necessários
    MQTTSubscribe(TOPIC_ILUMINACAO_INTERNA);
    MQTTSubscribe(TOPIC_Alarme_P);
    MQTTSubscribe(TOPIC_faixaOPI_P);
    MQTTSubscribe(TOPIC_faixaOPS_P);
    MQTTSubscribe(TOPIC_ILUMINACAO_INTERNA);
    MQTTSubscribe(TOPIC_EST_ILUMINACAO_INTERNA);
    MQTTSubscribe(TOPIC_HORARIO);
    MQTTSubscribe(TOPIC_TEMPERATURA_P);
    
    //inicia a biblioteca de mapeamento das pinagens da placa
    wiringPiSetupGpio();

    /*Esquema de penagem dos perifericos da placa*/ 
    pinMode(DIP_1,INPUT); // representa o alarme
    pinMode(DIP_2,INPUT); // representa o sensor presença
    pinMode(DIP_3,INPUT); // representa o sensor de portas e janelas
    pinMode(DIP_4,INPUT); 

    pinMode(BUTTON_1,INPUT); 
    pinMode(BUTTON_2,INPUT); 
    pinMode(BUTTON_3,INPUT); 

    pinMode(POTEN_SDA,INPUT);
    pinMode(POTEN_SCL,INPUT);

    int lcd = lcdInit(2,16,4,LCD_RS,LCD_E,LCD_D4,LCD_D5,LCD_D6,LCD_D7,0,0,0,0); 
    //Indica que o dispositivo foi ligado
    MQTTPublish(TOPIC_ESTADO_DISPOSITIVO, "ligado");
    //Para manter um loop infinito
    while(1){ 
        //log_dispositivo("ligado","Dispositivo");
        
        //Publica alguns dados iniciais em alguns tópicos
        sprintf(entrada_temperatura1, "%d", entrada_temperatura);
        MQTTPublish(TOPIC_TEMPERATURA, entrada_temperatura1);
        sprintf(faixa_operacao_inferior, "%d", faixa_operacao_I);
        MQTTPublish(TOPIC_OPERACAO_INFERIOR, faixa_operacao_inferior);
        sprintf(faixa_operacao_superior, "%d", faixa_operacao_S);
        MQTTPublish(TOPIC_OPERACAO_SUPERIOR, faixa_operacao_superior);


        //Veririca a situação do alarme e publica a mesma
        if(digitalRead(DIP_1) == HIGH){
            entrada_alarmeAux = "Alarme ligado";
            MQTTPublish(TOPIC_ESTADO_ALARME, "ligado");
        }else{
            entrada_alarmeAux = "Alarme desligado";
            MQTTPublish(TOPIC_ESTADO_ALARME, "desligado");
        }
        //log_dispositivo(saida_iluminacao_interna,"Alarme");


        // Veririca a situação da iluminacao da garagem e publica a mesma,  verifica se está dentro do horário de funcionamento e se o sensor de presença está ligado
        if((digitalRead(DIP_2) == HIGH ) && !(horario > 6 && horario < 18)){
            saida_iluminacao_GaragemAux = 1;
            saida_iluminacao_Garagem = "ligado";
            MQTTPublish(TOPIC_LUZ_GARAGEM, "ligado");
        }else{
            saida_iluminacao_GaragemAux = 0;
            saida_iluminacao_Garagem = "desligado";
            MQTTPublish(TOPIC_LUZ_GARAGEM, "desligado");
        }
        //log_dispositivo(saida_iluminacao_Garagem,"Iluminação garagem");

        // Veririca a situação da iluminacao do jardim e publica a mesma, verifica se está dentro do horário de funcionamento
        if(horario >= 18 && horario <= 23){
            saida_iluminacao_jardimAux = 1;
            saida_iluminacao_jardim = "ligado";
            MQTTPublish(TOPIC_LUZ_JARDIM, "ligado");
        }else{
            saida_iluminacao_jardimAux = 0;
            saida_iluminacao_jardim = "desligado"
            MQTTPublish(TOPIC_LUZ_JARDIM, "desligado");
        }
        //log_dispositivo(saida_iluminacao_jardim,"Iluminação jardim");

        // Veririca a situação da iluminação interna e publica a mesma, verifica se o sensor de presença está ligado
        if(digitalRead(DIP_2) == HIGH || entrada_iluminacao_internaAux == 1){
            saida_iluminacao_internaAux = 1;
            saida_iluminacao_interna = "Luz Ligada";
            MQTTPublish(TOPIC_LUZ_INTERNA, "ligado");
        }else{
            saida_iluminacao_internaAux = 0;
            saida_iluminacao_interna = "Luz Desligada";
            MQTTPublish(TOPIC_LUZ_INTERNA, "desligado");
        } 
        //log_dispositivo(saida_iluminacao_interna,"Iluminação interna");

        //Veririca a situação da saida do alarme e publica a mesma, se o alarme está ativado, se o sensor de presença ou o sensor de porta e janela está ligado.
        if(((digitalRead(DIP_1) == HIGH) || entrada_alarme == 1) && ((digitalRead(DIP_2) == HIGH) || (digitalRead(DIP_3) == HIGH))){
            saida_Saida_alarmeAux = 1;
            saida_Saida_alarme = "Alarme Ligado";
            MQTTPublish(TOPIC_ALARME, "ligado");
        }else{
            saida_Saida_alarmeAux = 0;
            saida_Saida_alarme = "Alarme Desligado";
            MQTTPublish(TOPIC_ALARME, "desligado");
        } 
        //log_dispositivo(saida_Saida_alarme,"Saida Alarme");

        // Verifica a situação do ar condicionado e publica a mesma

        //Caso a faixa de operação superior seja menor que a inferior o ar condicionado é desligado
        if(faixa_operacao_I >= faixa_operacao_S){
            saida_ar_condicionadoAux = 0;
            saida_ar_condicionado = "ar Desligado";
            MQTTPublish(TOPIC_ARCONDICIONADO, "desligado");
        }else{
            // Se o sensor de presença estiver desligado o ar condicionado é desligado
            if(digitalRead(DIP_2) == HIGH){
                // verifica se a temperatura indicada está dentro da faixa de operação
                if(entrada_temperatura <= faixa_operacao_I){
                    saida_ar_condicionadoAux = 0;
                    saida_ar_condicionado = "ar Desligado";
                    MQTTPublish(TOPIC_ARCONDICIONADO, "desligado");
                }else if(entrada_temperatura >= faixa_operacao_S){
                    saida_ar_condicionadoAux = 1;
                    saida_ar_condicionado = "ar Ligado";
                    MQTTPublish(TOPIC_ARCONDICIONADO, "ligado");
                }
            }else{
                saida_ar_condicionadoAux = 0;
                saida_ar_condicionado = "ar Desligado";
                MQTTPublish(TOPIC_ARCONDICIONADO, "desligado");
            }
        }
        if(entrada_temperatura == 17){
            saida_ar_condicionado = "ar Desligado";
            MQTTPublish(TOPIC_ARCONDICIONADO, "desligado");
            temopar = 0;
            
        }
        //log_dispositivo(saida_ar_condicionado,"Ar condicionado");

       
        //printar mensagens no lcd
        
        /*Por padrão, o texto é impresso na tela na linha superior, segunda coluna. Para alterar a posição, use 
        lcdPosition (lcd, COLUMN, ROW).Em um LCD 16 × 2, as linhas são numeradas de 0 a 1 e as colunas são numeradas de 0 a 15.*/
    }
    MQTTPublish(TOPIC_ESTADO_DISPOSITIVO, "desligado");
    MQTTDisconnect();
    return 0;
} 

/*
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
        
*/