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

void my_message_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message){
    
    if(message->payloadlen){
        printf("%s %s", message->topic, (char *)message->payload);
    }else{
        printf("%s (null)\n", message->topic);
    }
    fflush(stdout);
}

void my_connect_callback(struct mosquitto *mosq, void *userdata, int result){
    int i;
    if(!result){
        /* Subscribe to broker information topics on successful connect. */
        mosquitto_subscribe(mosq, NULL, "arCondicionado ", 2);
        mosquitto_subscribe(mosq, NULL, "alarme ", 2);
        mosquitto_subscribe(mosq, NULL, "sensorPJ ", 2);
        mosquitto_subscribe(mosq, NULL, "faixaOpI ", 2);
        mosquitto_subscribe(mosq, NULL, "faixaOpS ", 2);
        mosquitto_subscribe(mosq, NULL, "iluminacao1 ", 2);
        mosquitto_subscribe(mosq, NULL, "iluminacao2 ", 2);
        mosquitto_subscribe(mosq, NULL, "iluminacao3 ", 2);
    }else{
        fprintf(stderr, "Connect failed\n");
    }
}

void my_subscribe_callback(struct mosquitto *mosq, void *userdata, int mid, int qos_count, const int *granted_qos){
    int i;
    printf("Subscribed (mid: %d): %d", mid, granted_qos[0]);
    for(i=1; i<qos_count; i++){
        printf(", %d", granted_qos[i]);
    }
    printf("\n");
}

void my_log_callback(struct mosquitto *mosq, void *userdata, int level, const char *str){
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
            saida_iluminacao_interna = "Luz L"
        }else{
            saida_iluminacao_interna = 0;
            saida_iluminacao_interna = "Luz D"
        } 

        //alarme
        if((digitalRead(22) == HIGH)  || (digitalRead(27) == HIGH) && (digitalRead(4) == HIGH)){
            saida_Saida_alarme = "Alarme L";
        }else{
            saida_Saida_alarme = "Alarme D";
        } 

        //ar condicionado
        if(temopar){
            if(faixa_operacao_I >= faixa_operacao_S){
                saida_ar_condicionado = "ar D";
            }else{
                if(digitalRead(27) == HIGH){
                    if(entrada_temperatura <= faixa_operacao_I){
                        saida_ar_condicionado = "ar D";
                    }else if(entrada_temperatura >= faixa_operacao_S){
                        saida_ar_condicionado = "ar L";
                    }
                }else{
                    saida_ar_condicionado = "ar D";
                }
            }
            if(entrada_temperatura == 17){
                saida_ar_condicionado = "ar D";
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

