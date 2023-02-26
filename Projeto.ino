
//----------------------------------------Biblioteca do ESP8266----------------------------------------------------------------------------------------------------------------//
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
//----------------------------------------Biblioteca do MFRC522----------------------------------------------------------------------------------------------------------------//
#include <SPI.h>
#include <MFRC522.h>

#include "PageIndex.h" //--> Pagina Web

#define SS_PIN D2  //--> SDA / SS Conectado no D2 do ESP
#define RST_PIN D1  //--> RST Conectado no  D1 do ESP
MFRC522 mfrc522(SS_PIN, RST_PIN);  //--> Criada a instancia do MFRC522.

#define ON_Board_LED 2  //--> Definine um LED no ESP, indica durante o processo de conexão a um roteador wi-fi

//----------------------------------------SSID e senha do WiFi-----------------------------------------------------------------------------------------------------------------//
const char* ssid = "Barbosa Reis";  //--> SSID do Wifi
const char* password = "989302478";  //--> Senha do Wifi


ESP8266WebServer server(80);  //--> Server na porta 80

int readsuccess;
byte readcard[4];
char str[32] = "";
String StrUID;

//-----------------------------Esta rotina é executada quando você abre o endereço IP do NodeMCU ESP8266 no navegador----------------------------------------------------------//
void handleRoot() {
  String s = MAIN_page; //--> Read HTML contents
  server.send(200, "text/html", s); //--> Send web page
}

//--------------------------------Procidure para enviar os resultados da leitura do UID para a página da web-------------------------------------------------------------------//
void handleUIDResult() {
  readsuccess = getid();
 
  if(readsuccess) {
    digitalWrite(ON_Board_LED, LOW); //--> Led na placa é ligado como um indicador de leitura UID bem-sucedida
    server.send(200, "text/plane", StrUID); //--> Enviar valor apenas para solicitação ajax do cliente
    
    Serial.println(StrUID);
    Serial.println(""); 
    Serial.println("Por favor insira o cartão para verificar o UID!");
    Serial.println("");  
    digitalWrite(ON_Board_LED, HIGH); //--> Led desligado como um indicador de que o UID foi impresso no terminal
  }
}


//----------------------------------------Procedure para ler e obter um UID de um RFID-----------------------------------------------------------------------------------------//
int getid() {  
  if(!mfrc522.PICC_IsNewCardPresent()) {
    return 0;
  }
  if(!mfrc522.PICC_ReadCardSerial()) {
    return 0;
  }
 
  
  Serial.println("O UID DO CARTÃO DIGITALIZADO É: ");
  
  for(int i=0;i<4;i++){
    readcard[i]=mfrc522.uid.uidByte[i]; //--> Armazenando o UID do tag no readcard
    array_to_string(readcard, 4, str);
    StrUID = str;
  }
  mfrc522.PICC_HaltA();
  return 1;
}

//------------------------------------Procedurepara alterar o resultado da leitura de um array UID em uma string---------------------------------------------------------------//
void array_to_string(byte array[], unsigned int len, char buffer[]) {
    for (unsigned int i = 0; i < len; i++)
    {
        byte nib1 = (array[i] >> 4) & 0x0F;
        byte nib2 = (array[i] >> 0) & 0x0F;
        buffer[i*2+0] = nib1  < 0xA ? '0' + nib1  : 'A' + nib1  - 0xA;
        buffer[i*2+1] = nib2  < 0xA ? '0' + nib2  : 'A' + nib2  - 0xA;
    }
    buffer[len*2] = '\0';
}


//------------------------------------------------------------------------------SETUP------------------------------------------------------------------------------------------//
void setup() {
  Serial.begin(115200); //--> Inicializa a comunicação serial com o PC
  SPI.begin();      //--> Inicialização do barramento SPI
  mfrc522.PCD_Init(); //--> Inicializa o MFRC522

  delay(500);

  WiFi.begin(ssid, password); //--> Aqui ele faz o procedimento de conexão no Wifi
  Serial.println("");
    
  pinMode(ON_Board_LED,OUTPUT); 
  digitalWrite(ON_Board_LED, HIGH); //--> Desliga o Led 

  //------------------------------------------------------------Processo de conexão--------------------------------------------------------------------------------------------//
  Serial.print("Conectando");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    //------------------------------------------> Faz o LED OnBoard piscar no processo de conexão com o wi-fi.
    digitalWrite(ON_Board_LED, LOW);
    delay(250);
    digitalWrite(ON_Board_LED, HIGH);
    delay(250);
  }
  digitalWrite(ON_Board_LED, HIGH); //--> Desliga o LED OnBoard quando estiver conectado ao wi-fi.

  //----------------------------------------Se conectado com sucesso ao wi-fi, o endereço IP é exibido no monitor serial-------------------------------------------------------//
  Serial.println("");
  Serial.print("Conexão bem sucedida a : ");
  Serial.println(ssid);
  Serial.print("Endereço do IP : ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot); //--> Rotina para exibir a página da web.
  server.on("/UIDResult", handleUIDResult);  //--> Rotina para manipulação de chamadas de procedimento handleUIDResult

  server.begin(); //--> Start server
  Serial.println("HTTP server started");
  Serial.println("");

  Serial.println("Por favor insira o cartão para verificar o UID !");
  Serial.println("");
}

//-----------------------------------------------------------------------------------------------LOOP--------------------------------------------------------------------------//
void loop() {
  server.handleClient();  //--> Solicitaçoes dos clientes
}
