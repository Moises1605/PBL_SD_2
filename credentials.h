#ifndef LINUX_CREDENTIALS
#define LINUX_CREDENTIALS

/* Provide MQTT broker credentials as denoted in maqiatto.com. */
#define CLIENTID        "clientePlaca"
#define BROKER_ADDR     "maqiatto.com"
#define USERNAME        "moisesalmeida123_@hotmail.com"
#define PASSWORD        "pblsd"

/* Provide topic as it is denoted in your topic list. 
 * For example mine is : cadominna@gmail.com/topic1
 * To add topics, see https://www.maqiatto.com/configure
 */

//Tópicos utilizados pelo site
#define TOPICC                           "moisesalmeida123_@hotmail.com/opaa"
#define TOPIC                           "moisesalmeida123_@hotmail.com/teste"
#define TOPIC_TEMPERATURA               "moisesalmeida123_@hotmail.com/temperaturaS"
#define TOPIC_OPERACAO_INFERIOR         "moisesalmeida123_@hotmail.com/operacaoIS"
#define TOPIC_OPERACAO_SUPERIOR         "moisesalmeida123_@hotmail.com/operacaoSS"
#define TOPIC_LUZ_GARAGEM               "moisesalmeida123_@hotmail.com/luzGaragemS"
#define TOPIC_LUZ_JARDIM                "moisesalmeida123_@hotmail.com/luzJardimS"
#define TOPIC_LUZ_INTERNA               "moisesalmeida123_@hotmail.com/luzInternaS"
#define TOPIC_ALARME                    "moisesalmeida123_@hotmail.com/AlarmeS" // A Saida do alarme
#define TOPIC_ARCONDICIONADO            "moisesalmeida123_@hotmail.com/arcondicionadoS" 
#define TOPIC_ESTADO_ALARME             "moisesalmeida123_@hotmail.com/estadoAlarmeP"

//Tópicos utilizados pela placa
#define TOPIC_Alarme_P                  "moisesalmeida123_@hotmail.com/alarmeP"
#define TOPIC_faixaOPI_P                "moisesalmeida123_@hotmail.com/faixaOpIP"
#define TOPIC_faixaOPS_P                "moisesalmeida123_@hotmail.com/faixaOpSP"
#define TOPIC_ILUMINACAO_INTERNA        "moisesalmeida123_@hotmail.com/iluminacaoInternaP"
#define TOPIC_EST_ILUMINACAO_INTERNA    "moisesalmeida123_@hotmail.com/estIlumincaoInternaP"


#endif /* LINUX_CREDENTIALS */







