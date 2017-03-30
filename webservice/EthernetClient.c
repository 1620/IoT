#include <DHT.h>

//#include <Ethernet_v2.h> //para segunda geracao do shield ethernet
#include <Ethernet.h> //para primeira geracao do shield ethernet
#include <SD.h>
#include <SPI.h>

#include <RCSwitch.h>

#define DHTPIN 7
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

RCSwitch mySwitchTX = RCSwitch();
RCSwitch mySwitchRX = RCSwitch();

byte mac[] = { 0x90, 0xA2, 0xDA, 0x10, 0x02, 0x5E };
IPAddress ip(192, 168, 25, 11);
EthernetServer server(80);

File webFile;

#define REQ_BUF_SZ    40
char HTTP_req[REQ_BUF_SZ] = { 0 };
char req_index = 0;


const int botao = 3;
const int carga1 = 43;
const int carga2 = 47;
int flag1 = 0;
int flag2 = 0;

void setup() {
  pinMode(carga1, OUTPUT);
  pinMode(carga2, OUTPUT);
  pinMode(botao, INPUT);
  analogReference(INTERNAL1V1);

  dht.begin(); //inicia DHT22

  mySwitchTX.enableTransmit(36);   //pino TX
  mySwitchRX.enableReceive(0);    //pino RX - 0 = pin2

  mySwitchTX.send(2222, 24);  
  
  Ethernet.begin(mac, ip);
  server.begin();

  Serial.begin(9600);

  Serial.println("Inicializando cartao MicroSD...");
  if (!SD.begin(4)) {
    Serial.println("ERRO - iniciallizacao do cartao falhou!");
    return;
  }
  Serial.println("SUCESSO - cartao MicroSD inicializado.");

  if (!SD.exists("index.htm")) {
    Serial.println("ERRO - index.htm nao foi encontrado!");
    return;
  }
  Serial.println("SUCESSO - Encontrado arquivo index.htm.");

}

void loop()
{
  
  if (digitalRead(botao) == 1)
  {
    digitalWrite(carga1, !digitalRead(carga1));
    flag1 = !flag1;
    delay(500);
  }

  if (mySwitchRX.available()) 
  {
    
    int value = mySwitchRX.getReceivedValue();
    
    if (value == 1122)
    {
      flag2 = 1;  
      Serial.println("recebido flag 1");    
    }
    if (value == 2211) 
    {
      flag2 = 0;
      Serial.println("recebido flag 0");
    }
    if (value == 3333) 
    {
      flag2 = !flag2;
      Serial.println("recebido inverte flag");
    }
    
        
    mySwitchRX.resetAvailable();
  }
  
  
  EthernetClient client = server.available();

  if (client) {
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {

        char c = client.read();

        if (req_index < (REQ_BUF_SZ - 1)) {
          HTTP_req[req_index] = c;
          req_index++;
        }

        if (c == '\n' && currentLineIsBlank) {
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          client.println();

          if (StrContains(HTTP_req, "ajax_LerDados")) {
            LerDados(client);
          }

          if (StrContains(HTTP_req, "ajax_carga1")){
            SetCarga1();
          }

          if (StrContains(HTTP_req, "ajax_carga2")){
            SetCarga2();
          }

          else {

            webFile = SD.open("index.htm");
            if (webFile) {
              while (webFile.available()) {
                client.write(webFile.read());
              }
              webFile.close();
            }
          }
          Serial.println(HTTP_req);
          req_index = 0;
          StrClear(HTTP_req, REQ_BUF_SZ);
          break;
        }

        if (c == '\n') {
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          currentLineIsBlank = false;
        }
      }
    }

    delay(1);
    client.stop();

  }

}

void LerDados(EthernetClient novoCliente) 
{
  novoCliente.print(dht.readHumidity(), 1);
  novoCliente.println("%");

  novoCliente.print("|");

  novoCliente.print(dht.readTemperature(), 1);
  novoCliente.println("*C");

  novoCliente.print("|");

  novoCliente.print(flag1);
  
  novoCliente.print("|");

  novoCliente.print(flag2);
  
  novoCliente.print("|");
  
  //espero receber algo como 90%|25*C|0|
}

void StrClear(char *str, char length) {
  for (int i = 0; i < length; i++) {
    str[i] = 0;
  }
}


char StrContains(char *str, char *sfind)
{
  char found = 0;
  char index = 0;
  char len;

  len = strlen(str);

  if (strlen(sfind) > len) {
    return 0;
  }

  while (index < len) {
    if (str[index] == sfind[found]) {
      found++;
      if (strlen(sfind) == found) {
        return 1;
      }
    }
    else {
      found = 0;
    }
    index++;
  }
  return 0;
}


void SetCarga1()
{
  if(flag1 == 0)
  {
    digitalWrite(carga1, HIGH);
    flag1 = 1;
  }
  else
  {
    digitalWrite(carga1, LOW);
    flag1 = 0;
  }
}

void SetCarga2()
{
  if(flag2 == 0)
  {
    mySwitchTX.send(1111, 24);
    Serial.println("envia sinal 1111");
  }
  else
  {    
    mySwitchTX.send(2222, 24);
    Serial.println("envia sinal 2222");
  }
}
