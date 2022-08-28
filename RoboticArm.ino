#include <ESP8266WiFi.h> // Importa a Biblioteca ESP8266WiFi
#include <PubSubClient.h> // Importa a Biblioteca PubSubClient
#include <Wire.h>  
#include <Servo.h>

Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;

long previousMillis = 0;
long interval = 30000;

//defines:
//defines de id mqtt e tópicos para publicação e subscribe
#define TOPICO_SERVO1   "faeterj/roboticarm/servo1"     //tópico MQTT de recebimento de informações do Broker
#define TOPICO_SERVO2   "faeterj/roboticarm/servo2"     //tópico MQTT de recebimento de informações do Broker
#define TOPICO_SERVO3   "faeterj/roboticarm/servo3"     //tópico MQTT de recebimento de informações do Broker
#define TOPICO_SERVO4   "faeterj/roboticarm/servo4"     //tópico MQTT de recebimento de informações do Broker
#define TOPICO_COMANDO  "faeterj/roboticarm/comando"    //tópico MQTT de recebimento de informações do Broker
#define TOPICO_STATUS   "faeterj/roboticarm/status"     //tópico MQTT de envio de informações para o Broker
#define ID_MQTT         "FAETERJ_MQTT1234"              //id mqtt (para identificação de sessão)
 
//defines - mapeamento de pinos do NodeMCU
#define D0    16
#define D1    5
#define D2    4
#define D3    0
#define D4    2
#define D5    14
#define D6    12
#define D7    13
#define D8    15
#define D9    3
#define D10   1

#define GPIO2 2

#define RELAY1 D5
#define RELAY2 D6

// WIFI
const char* SSID = "Warlock"; // SSID / nome da rede WI-FI que deseja se conectar
const char* PASSWORD = "bc25fcb38b"; // Senha da rede WI-FI que deseja se conectar
  
// MQTT
const char* BROKER_MQTT = "test.mosquitto.org"; //URL do broker MQTT que se deseja utilizar
int BROKER_PORT = 1883; // Porta do Broker MQTT
 
//Variáveis e objetos globais
WiFiClient espClient; // Cria o objeto espClient
PubSubClient MQTT(espClient); // Instancia o Cliente MQTT passando o objeto espClient


//Prototypes
bool initWiFi();
void initMQTT();
bool reconnectWiFi(); 
void mqtt_callback(char* topic, byte* payload, unsigned int length);
bool VerificaConexoesWiFIEMQTT(void);
 
/* 
 *  Implementações das funções
 */
void setup() 
{
    servo1.attach(D4, 500, 2400);
    servo1.write(90);
    servo2.attach(D5, 500, 2400);
    servo2.write(0);
    servo3.attach(D6, 500, 2400);
    servo3.write(0);
    servo4.attach(D7, 500, 2400);
    servo4.write(0);
    
    Serial.begin(9600);

    delay(3000);

    Serial.println();Serial.println();
    Serial.println("Exemplo MQTT");

    delay(1000);

    //inicializações:
    if (initWiFi())
    {
        initMQTT();
    }
}
  
bool initWiFi() 
{
    delay(5000);
    Serial.println("------Conexao WI-FI------");
    Serial.print("Conectando-se na rede: ");
    Serial.println(SSID);
    Serial.println("Aguarde");
     
    return reconnectWiFi();
}
  
void initMQTT() 
{
    MQTT.setServer(BROKER_MQTT, BROKER_PORT);   //informa qual broker e porta deve ser conectado
    MQTT.setCallback(mqtt_callback);            //atribui função de callback (função chamada quando qualquer informação de um dos tópicos subescritos chega)
}
  
//Função: função de callback 
//        esta função é chamada toda vez que uma informação de 
//        um dos tópicos subscritos chega)
void mqtt_callback(char* topic, byte* payload, unsigned int length) 
{
    String msg;
 
    //obtem a string da payload recebida
    for(int i = 0; i < length; i++) 
    {
      char c = (char)payload[i];
      msg += c;
    }

    if (strcmp(topic, TOPICO_SERVO1)==0)
    {
      servo1.write(msg.toInt());
    }

    if (strcmp(topic, TOPICO_SERVO2)==0)
    {
      servo2.write(msg.toInt());
    }

    if (strcmp(topic, TOPICO_SERVO3)==0)
    {
      servo3.write(msg.toInt());
    }

    if (strcmp(topic, TOPICO_SERVO4)==0)
    {
      servo4.write(msg.toInt());
    }

    if (msg.equals("acenderLED"))
    {
      digitalWrite(LED_BUILTIN, LOW);
    }

    if (msg.equals("apagarLED"))
    {
      digitalWrite(LED_BUILTIN, LOW);
    }

    if (msg.startsWith("getStatus"))
    {
      if ((WiFi.status() == WL_CONNECTED) && (MQTT.connected()))
      {
        char dh[30];
        String sdh = getTime();
  
        sdh.toCharArray(dh, 30);
        MQTT.publish(TOPICO_STATUS, dh);
        Serial.print(TOPICO_STATUS);
        Serial.print(" -> ");
        Serial.println(dh);
      }
    }

    int pausa = 750;

    if (msg.equals("reset"))
    {
      servo1.write(90);
      delay(pausa);
      servo2.write(0);
      delay(pausa);
      servo3.write(0);
      delay(pausa);
      servo4.write(0);
    }
    if (msg.equals("show"))
    {     
      servo1.write(34);
//      delay(pausa);
      servo2.write(58);
      delay(pausa);
      servo3.write(180);
//      delay(pausa);
      servo4.write(30);
      delay(pausa);
      servo2.write(102);
      delay(pausa);
      servo1.write(62);
//      delay(pausa);
      servo2.write(39);
      delay(pausa);
      servo4.write(100);
      delay(pausa);
      servo3.write(95);
      delay(pausa);
      servo3.write(180);
//      delay(pausa);
      servo4.write(0);
      delay(pausa);
      servo4.write(85);
      delay(pausa);
      servo4.write(19);
//      delay(pausa);
      servo1.write(0);
      delay(pausa);
      servo1.write(180);
      delay(pausa);
      servo1.write(80);
      delay(pausa);
    }
    
    Serial.print(topic);
    Serial.print(" -> ");
    Serial.println(msg);     
}
  
bool reconnectMQTT() 
{
    bool ret = false;

    if (MQTT.connected())
    {
      ret = true;
    }
    else
    {
      if (MQTT.connect(ID_MQTT)) 
      {
          Serial.println("Conectado com sucesso ao broker MQTT!");
          MQTT.subscribe(TOPICO_COMANDO);
          MQTT.subscribe(TOPICO_SERVO1);
          MQTT.subscribe(TOPICO_SERVO2);
          MQTT.subscribe(TOPICO_SERVO3);
          MQTT.subscribe(TOPICO_SERVO4);
          ret = true;
      } 
      else
      {
          Serial.println("Falha ao reconectar no broker.");
      }
    }
      
    return ret;
}
  
bool reconnectWiFi() 
{
    bool ret = false;
    int attemptstimeout = 25;
    int attempts = 0;
    
    //se já está conectado a rede WI-FI, nada é feito. 
    //Caso contrário, são efetuadas tentativas de conexão
    if (WiFi.status() == WL_CONNECTED)
    {
        return true;
    }
         
    WiFi.begin(SSID, PASSWORD); // Conecta na rede WI-FI

    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(500);
        Serial.print(".");
        yield();

        attempts++;

        if (attempts>attemptstimeout)
        {
          ESP.restart();
          break;
        }
    }
    
    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println();
        Serial.print("Conectado com sucesso na rede ");
        Serial.print(SSID);
        Serial.println("IP obtido: ");
        Serial.print("<");
        Serial.print(WiFi.localIP());
        Serial.println(">");
        ret = true;      
    }

    return ret;
}
 
bool VerificaConexoesWiFIEMQTT(void)
{
    bool ret = false;
    
    if (reconnectWiFi())
    {
        if (!MQTT.connected())
        {
            ret = reconnectMQTT(); //se não há conexão com o Broker, a conexão é refeita
        }
        else
        {
            ret = true;
        }
    }

    return ret;
}
 
 
//programa principal
void loop() 
{
  /*
    for (pos = 0; pos <= 180; pos += 1) { // movimento de 0 a 180º
      // crescente
      servo1.write(pos);              // escreve posicao em meuservo
      delay(35);                       // aguarda 35ms
    }
    for (pos = 180; pos >= 0; pos -= 1) { // movimento de 180º a 0
      // descrescente
      servo1.write(pos);              // escreve posicao em meuservo
      delay(35);                       // aguarda 35ms
    }
 */
    //garante funcionamento das conexões WiFi e ao broker MQTT
    unsigned long currentMillis = millis();
    
    if ((previousMillis==0) || (currentMillis - previousMillis > interval))
    {
      previousMillis = currentMillis;
//      Serial.println(currentMillis);
      if (VerificaConexoesWiFIEMQTT())
      {
        //pass
      }    
    }
    
    //keep-alive da comunicação com broker MQTT
    MQTT.loop();
}

String getTime() {
    WiFiClient client;
    int attempt = 0;
    String theDate = "#datetime#";
  
    while (!!!client.connect("google.com.br", 80)) {
      Serial.println("connection failed, retrying...");
  
      if (attempt==10)
        break;
    }
  
    if (attempt<10)
    {
      client.print("HEAD / HTTP/1.1\r\n\r\n");
     
      while(!!!client.available()) {
         yield();
      }
    
      while(client.available()){
        if (client.read() == '\n') {    
          if (client.read() == 'D') {    
            if (client.read() == 'a') {    
              if (client.read() == 't') {    
                if (client.read() == 'e') {    
                  if (client.read() == ':') {    
                    client.read();
                    theDate = client.readStringUntil('\r');
                    client.stop();
                    break;
                  }
                }
              }
            }
          }
        }
      }
    }
  
    return theDate;
}
