#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
const char *ssid = "ICTS"; // Digite seu nome WiFi
const char *pass = "redeicts2014ICTS"; // Digite sua senha WiFi

WiFiClient client;

#define MQTT_SERV "io.adafruit.com"
#define MQTT_PORT 1883
#define MQTT_NAME "Lord_LT"
#define MQTT_PASS "aio_hrJl20ehE79qc3O2hMD3ntYh9NDl"
#define RELE_LIGADO LOW
#define RELE_DESLIGADO HIGH
#define LIGAR_SOLENOIDE digitalWrite(motorPin, RELE_DESLIGADO)

const int ledPin = D6;
const int ldrPin = D1;
const int umidadePin = A0;             // pino do sensor de umidade
const int motorPin = D0;
unsigned long interval = 10000;
unsigned long previousMillis = 0;
unsigned long interval1 = 1000;
unsigned long previousMillis1 = 0;
float porcentagemdeUmidade;              //leitura de umidade

//Configurar o feed que você está publicando em
Adafruit_MQTT_Client mqtt(&client, MQTT_SERV, MQTT_PORT, MQTT_NAME, MQTT_PASS);
Adafruit_MQTT_Publish DadosdeUmidade = Adafruit_MQTT_Publish(&mqtt, MQTT_NAME "/f/dados-de-umidade");

//Configurar o feed que você está assinando
Adafruit_MQTT_Subscribe LED = Adafruit_MQTT_Subscribe(&mqtt, MQTT_NAME "/f/led");
Adafruit_MQTT_Subscribe ValvulaSolenoide = Adafruit_MQTT_Subscribe(&mqtt, MQTT_NAME "/f/valvula-solenoide");

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(10);
  mqtt.subscribe(&LED);
  mqtt.subscribe(&ValvulaSolenoide);
  pinMode(motorPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(ldrPin, INPUT);
  digitalWrite(motorPin, LOW); // mantém o motor desligado inicialmente

  Serial.println("Conectando a ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("."); // imprimir ... até não conectado
  }
  Serial.println("");
  Serial.println("WiFi conectado");
while(1){
   digitalWrite(motorPin, 1);
   serial
  delay(1000);
   digitalWrite(motorPin, 0);
   delay(1000);
   
  }



  
}

void MQTT_connect(){

    int8_t ret;
    //Para if já estiver conectado
    if (mqtt.connected())
    {
      return;
    }
    uint8_t tentativas = 3;
    while ((ret = mqtt.connect()) != 0)//connect retornará 0 para connected
    {

      mqtt.disconnect();
      delay(5000);  // espera 5 segundos
      tentativas--;
      if (tentativas == 0)
      {
        // basicamente morre e espera que o WDT reinicie
        while (1);
      }
    }
  }

void loop(){
  
    MQTT_connect();
  
  
    int ldrStatus = analogRead(ldrPin);
  
    if (ldrStatus <= 200) {
  
      digitalWrite(ledPin, HIGH);
  
      Serial.print("Ligue o LED");
  
      Serial.println(ldrStatus);
  
    }
    else{
  
      digitalWrite(ledPin, LOW);
  
      Serial.print("Desligue o LED");
  
      Serial.println(ldrStatus);
  
    }
  
    porcentagemdeUmidade = ( 100.00 - ( (analogRead(umidadePin) / 1023.00) * 100.00 ) );
  
  
    Serial.print("A umidade do solo e  = ");
    Serial.print(porcentagemdeUmidade);
    Serial.println("%");
  
    if (porcentagemdeUmidade < 35) {
      digitalWrite(motorPin, RELE_LIGADO);         //  liga o motor
    }
    if (porcentagemdeUmidade > 35 && porcentagemdeUmidade < 37) {
      digitalWrite(motorPin, RELE_LIGADO);        //liga a bomba do motor
    }
    if (porcentagemdeUmidade > 38) {
      digitalWrite(motorPin, RELE_DESLIGADO);          // desliga o motor
    }
  
    if (! DadosdeUmidade.publish(porcentagemdeUmidade))
    {
      delay(5000);
    }
    Adafruit_MQTT_Subscribe * assinatura;
    while ((assinatura = mqtt.readSubscription(5000)))
    {
  
      if (assinatura == &LED)
      {
        //Imprime o novo valor no monitor serial
        Serial.println((char*) LED.lastread);
  
        if (!strcmp((char*) LED.lastread, "OFF"))
        {
          digitalWrite(ledPin, HIGH);
        }
        if (!strcmp((char*) LED.lastread, "ON"))
        {
          digitalWrite(ledPin, LOW);
        }
      }
  
      if (assinatura == &ValvulaSolenoide)
      {
        // //Imprime o novo valor no monitor serial
        Serial.println((char*) ValvulaSolenoide.lastread);
  
        if (!strcmp((char*) ValvulaSolenoide.lastread, "OFF"))
        {
          digitalWrite(motorPin, RELE_LIGADO);
        }
        if (!strcmp((char*) ValvulaSolenoide.lastread, "ON"))
        {
          LIGAR_SOLENOIDE;
        }
  
      }
  }
}
