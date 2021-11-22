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
#include <string.h> 


FILE *arquivo_log;
FILE *arquivo_log_auxiliar;

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

    for(int i =0; i < num_linha; i++){
        fgets (conteudo_linha, 100,arquivo_log);    
    }

    while((fgets(conteudo_linha, 100,arquivo_log)) != EOF){
        fprintf(arquivo_log_auxiliar, "%s", conteudo_linha);
    }

    fclose(arquivo_log);
    remove("log_auxiliar.txt");
    arquivo_log = fopen("log.txt", "a");
    while((fgets(conteudo_linha, 100,arquivo_log)) != EOF){
        fprintf(arquivo_log, "%s", conteudo_linha);
    }
    
    fclose(arquivo_log_auxiliar);
    fclose(arquivo_log);
}

int main(){ 
    char* conteudo = "Teste de escrita em arquivo"
    log_dispositivo(conteudo);
    return 0;
} 