/*Inicio*/
#include <stdio.h>
#include <stdlib.h> 

int main(){
    /*Lista de entradas do programa*/
    int entrada_alarme;
    int entrada_sensor_presenca;
    int entrada_sensot_PJ;
    int entrada_temperatura;
    
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

        //quebra do loop
        if(0){
            break;
        }
    }

    //iluminacao_garagem
    if(entrada_sensor_presenca && !(horario > 6 && horario < 18)){
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
    if(entrada_sensor_presenca){
        saida_iluminacao_interna = 1;
    }else{
        saida_iluminacao_interna = 0;
    } 

    //alarme
    if((entrada_sensot_PJ || entrada_sensor_presenca) && entrada_alarme){
        saida_Saida_alarme = 1;
    }else{
        saida_Saida_alarme = 0;
    } 

    //ar condicionado
    if(temopar){
        if(faixa_operacao_I >= faixa_operacao_S){
            saida_ar_condicionado = 0;
        }else{
            if(entrada_sensor_presenca){
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

    return 0;
}
