#include <WiFi.h>
 #define SSID "ESP32Server"
#define PASSWORD "87654321"
#define SERVER_PORT 33333
 

int pinoVermelho = 16; //pino que ligamos o LED vermelho
int pinoVerde = 17; //pino que ligamos o LED verde
int pinoTouchOn = 4; //pino com sensor touch (pode-se usar a constante nativa T0)
int pinoTouchBlink = 13; //pino com sensor touch (pode-se usar a constante nativa T4)
int capacitanciaMaxima = 20; //valor que nos da a certeza de toque (ache esse valor através da calibragem)

 //Arquivo principal. Neste arquivo vão os 'includes' e as configurações principais
//que são compartilhadas entre os outros arquivos .ino

//Protocolo que o Server e o Client utilizarão para se comunicar
enum Protocol{
    PIN, //Pino que se deseja alterar o estado
    VALUE, //Estado para qual o pino deve ir (HIGH = 1 ou LOW = 0)
    BUFFER_SIZE //O tamanho do nosso protocolo. IMPORTANTE: deixar sempre como último do enum
};
 
//Diretiva de compilação que informará qual arquivo que queremos que seja compilado
//Caso queira que o arquivo Client.ino seja compilado, remova ou comente a linha do '#define'
//abaixo
//Caso queira que o arquivo Server.ino seja compilado, deixe o '#define IS_SERVER' abaixo descomentado


WiFiServer server(SERVER_PORT);

void setup()
{
   Serial.begin(115200);
   delay(1000);
   pinMode(pinoVermelho, OUTPUT);
   pinMode(pinoVerde, OUTPUT);

   //Coloca este ESP como Access Point
    WiFi.mode(WIFI_AP);
    //SSID e Senha para se conectarem a este ESP
    WiFi.softAP(SSID, PASSWORD);
    //Inicia o server
    server.begin();
    
}

void loop()
{

  //Verifica se tem algum cliente se conectando
    WiFiClient client = server.available();
    if (client)
    {     
        //Se o cliente tem dados que deseja nos enviar
        if (client.available())
        {//Criamos um buffer para colocar os dados
           uint8_t buffer[Protocol::BUFFER_SIZE];
            //Colocamos os dados enviados pelo cliente no buffer
            int len = client.read(buffer, Protocol::BUFFER_SIZE);
            //Verificamos qual o pino que o cliente enviou
            int pinNumber = buffer[Protocol::PIN];
            //Verificamos qual o valor deste pino
            int value = buffer[Protocol::VALUE];
            //Colocamos o pino em modo de saída
            pinMode(pinNumber, OUTPUT);
            //Alteramos o estado do pino para o valor passado
            digitalWrite(pinNumber, value);
        }
 
        //Fecha a conexão com o cliente
        client.stop();
    }
    
  Serial.print(touchRead(pinoTouchOn));
  Serial.print("-");
  Serial.print("-");

  Serial.println(touchRead(pinoTouchBlink));
  
  Serial.print(WiFi.gatewayIP());
  int mediaT0 = 0;
  int mediaT4 = 0;
  //faz 100 leituras de cada sensor touch e calcula a média do valor lido
  for(int i=0; i< 100; i++)
  {
    mediaT0 += touchRead(pinoTouchOn);
    mediaT4 += touchRead(pinoTouchBlink);
  }
   
  mediaT0 = mediaT0 / 100;
  mediaT4 = mediaT4 / 100;
 
  //verifica se o valor médio lido no pinoTouchOn é menor que a capacitância máxima definida
  //se verdadeiro, isso caracteriza um toque
  // os LEDs vemelho e verde ficarão acesos
  if(mediaT0 < capacitanciaMaxima)
  {
    digitalWrite(pinoVermelho, HIGH);
    digitalWrite(pinoVerde, HIGH);
  }
  //verifica se o valor médio lido no pinoTouchBlink é menor que a capacitância máxima definida
  //se verdadeiro, isso caracteriza um toque
  // os LEDs vemelho e verde ficarão piscando com um intervalo de 500 ms
  else if(mediaT4 < capacitanciaMaxima)
  {
    digitalWrite(pinoVermelho, !digitalRead(pinoVermelho)); //inverte o estado atual do LED (HIGH/LOW)
    digitalWrite(pinoVerde, !digitalRead(pinoVerde)); //inverte o estado atual do LED (HIGH/LOW)
    //delay(500);                   
  }
  //se nenhum dos pinos touch estão sendo tocados, os LEDS ficam apagados
  else{
    digitalWrite(pinoVermelho, LOW);
    digitalWrite(pinoVerde, LOW);
  } 
  //delay(10);
}