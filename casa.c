#include <stdio.h>
#include <stdlib.h> 
#include <wiringPi.h>
#include <mosquitto.h>
#include <string.h> 

#define HOST "localhost"
#define PORT  1883
#define KEEP_ALIVE 60
#define MSG_MAX_SIZE  512
#define TOPIC_NUM 3

bool session = true;

const static char* topic[TOPIC_NUM] =
{
    "Gai Ye:",
    "ycy ",
    "CCYY "
}; 

void my_message_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message)
{
    
    if(message->payloadlen){
        printf("%s %s", message->topic, (char *)message->payload);
    }else{
        printf("%s (null)\n", message->topic);
    }
    fflush(stdout);
}

void my_connect_callback(struct mosquitto *mosq, void *userdata, int result)
{
    int i;
    if(!result){
        /* Subscribe to broker information topics on successful connect. */
        mosquitto_subscribe(mosq, NULL, "CCYY ", 2);
    }else{
        fprintf(stderr, "Connect failed\n");
    }
}

void my_subscribe_callback(struct mosquitto *mosq, void *userdata, int mid, int qos_count, const int *granted_qos)
{
    int i;
    printf("Subscribed (mid: %d): %d", mid, granted_qos[0]);
    for(i=1; i<qos_count; i++){
        printf(", %d", granted_qos[i]);
    }
    printf("\n");
}

void my_log_callback(struct mosquitto *mosq, void *userdata, int level, const char *str)
{
    /* Pring all log messages regardless of level. */
    printf("%s\n", str);
}


int main(){ 

    struct mosquitto *mosq = NULL;
    char buff[MSG_MAX_SIZE];
    
    //Libmosquito library initialization
    mosquitto_lib_init();
    //Creating a mosquitto client
    mosq = mosquitto_new(NULL,session,NULL);
    if(!mosq){
        printf("create client failed..\n");
        mosquitto_lib_cleanup();
        return 1;
    }
    //Set callback function, use when necessary
    mosquitto_log_callback_set(mosq, my_log_callback);
    mosquitto_connect_callback_set(mosq, my_connect_callback);
    mosquitto_message_callback_set(mosq, my_message_callback);
    mosquitto_subscribe_callback_set(mosq, my_subscribe_callback);
    

    //Connect to server
    if(mosquitto_connect(mosq, HOST, PORT, KEEP_ALIVE)){
        fprintf(stderr, "Unable to connect.\n");
        return 1;
    }
    //Start a thread, and call mosquitto? Loop() continuously in the thread to process network information
    int loop = mosquitto_loop_start(mosq); 
    if(loop != MOSQ_ERR_SUCCESS)
    {
        printf("mosquitto loop error\n");
        return 1;
    }


    while(fgets(buff, MSG_MAX_SIZE, stdin) != NULL)
    {
        /*Publish news*/
        mosquitto_publish(mosq,NULL,"ycy ",strlen(buff)+1,buff,0,0);
        memset(buff,0,sizeof(buff));
    }

    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();

    /*Lista de entradas do programa*/ 
    pinMode(4,INPUT); // alarme dip switch
    pinMode(27,INPUT); //sensor presença
    pinMode(22,INPUT); //sensor de portas e janelas
    pinMode(,INPUT); //temperatura pontenciometro 



    pinMode(,OUTPUT); // Saida geral, o que muda é a mensagem apresentada

    /*int entrada_alarme;
    int entrada_sensor_presenca;
    int entrada_sensot_PJ;
    int entrada_temperatura;*/
    
    int horario;
    int faixa_operacao_S;
    int faixa_operacao_I;
    int temopar

    /*Lista de entradas do programa*/
    int saida_iluminacao_interna;
    int saida_Saida_alarme;
    int saida_sensot_PJ;
    int saida_ar_condicionado;
    int saida_iluminacao_jardim;
    int saida_iluminacao_Garagem;


    //Para manter um loop infinito
    while(1){ 

        //iluminacao_garagem
        if(digitalRead(27) == HIGH && !(horario > 6 && horario < 18)){
            saida_iluminacao_Garagem = 1;
        }else{
            saida_iluminacao_Garagem = 0;
        }

        //iluminacai_jardim
        if(horario >= 18 && horario <= 23){
            saida_iluminacao_jardim = 1;
        }else{
            saida_iluminacao_jardim = 0;
        }


        //iluminação interna
        if(digitalRead(27) == HIGH){
            saida_iluminacao_interna = 1;
        }else{
            saida_iluminacao_interna = 0;
        } 

        //alarme
        if((digitalRead(22) == HIGH)  || (digitalRead(27) == HIGH) && (digitalRead(4) == HIGH)){
            saida_Saida_alarme = 1;
        }else{
            saida_Saida_alarme = 0;
        } 

        //ar condicionado
        if(temopar){
            if(faixa_operacao_I >= faixa_operacao_S){
                saida_ar_condicionado = 0;
            }else{
                if(digitalRead(27) == HIGH){
                    if(entrada_temperatura <= faixa_operacao_I){
                        saida_ar_condicionado = 0;
                    }else if(entrada_temperatura >= faixa_operacao_S){
                        saida_ar_condicionado = 1;
                    }
                }else{
                    saida_ar_condicionado = 0;
                }
            }
            if(entrada_temperatura == 17){
                saida_ar_condicionado = 0;
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
    }

    return 0;
} 

