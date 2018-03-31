/* Biblioteca relacionada para poder usar o ESP8266 */
#include <SoftwareSerial.h>

/* Biblioteca relacionada para de poder user o DHT11 */
#include <DHT.h>

/* DHT11 na entrada analógica e modelo */
#define DHTPIN A1
#define DHTTYPE DHT11

/* Define relacionado para poder enviar comandos ao ESP8266*/
#define DEBUG true

/* Pinos de Rx e Tx, sempre devem ser colocado invertidos no arduino para criar um loop */
#define pinRx 7
#define pinTx 6

/* Inicializa os pinos rx e tx */
SoftwareSerial esp8266(pinRx, pinTx);

/* Inicializa o DHT11 */
DHT dht(DHTPIN, DHTTYPE);

/* id = esp_123 e pwd = 1234test */
void setup(){
  /* Inicializa a serial, o ESP8266 e o DHT11 */
  Serial.begin(9600);
  esp8266.begin(9600);
  dht.begin();

  /* Reseta o módulo */
  sendData("AT+RST\r\n",2000,DEBUG); // reset module
  
  /* Configura como ponto de acesso */
  sendData("AT+CWMODE=2\r\n",1000,DEBUG);
  
  /* Exibe o endereço IP */
  sendData("AT+CIFSR\r\n",1000,DEBUG);

  /* Configura múltiplas conexões */
  sendData("AT+CIPMUX=1\r\n",1000,DEBUG);

  /* Ligar o servidor na porta 80 */
  sendData("AT+CIPSERVER=1,80\r\n",1000,DEBUG);
}
     
void loop(){
  /* Verifica se o ESP8266 tá disponível */
  if(esp8266.available()){
      Serial.println("dispon");
      /* Se acho o ID da pessoa que conectou */
      if(esp8266.find("+IPD,")){
        
        Serial.println("acho id");
        
        /* Espera para ler o buffer do serial */
        delay(1000);
        
        /* Pega o ID de quem conectou */
        int connectionId = esp8266.read() - 48; // subtract 48 because the read() function returns 
                                               // the ASCII decimal value and 0 (the first decimal number) starts at 48

        /* Monta o conteúdo do json */
        String json;// = "{\"message\": \"Hello world! Your JustAPIs instance is running correctly.\"}";

        float humidity = dht.readHumidity();
        float temperature = dht.readTemperature();
        /* Verifica se o valor de retorno é válido */
        if (isnan(temperature) || isnan(humidity)) {
          json += "{\"temperature\": \"-1\", ";
          json += "\"humidity\": \"-1\"}";
        } else {
          json += "{\"temperature\": \"" + String(temperature) + "\", ";
          json += "\"humidity\": \"" + String(humidity) + "\"}";
        }
        
        /* Monta o comando o AT para enviar os dados */
        String sendCommand  = "AT+CIPSEND=";
        sendCommand += connectionId;
        sendCommand += ",";
        sendCommand += json.length();
        sendCommand += "\r\n";

        /* Envia o comando AT e depois envia o json */
        sendData(sendCommand, 1000, DEBUG);
        sendData(json, 1000, DEBUG);
 
        /* Monta o comando o AT para fecha a conexão */
        String closeCommand = "AT+CIPCLOSE="; 
        closeCommand+=connectionId;
        closeCommand+="\r\n";

         /* Envia o comando AT para fechar a conexão */
        sendData(closeCommand,1000,DEBUG);
      }
  }
}
     
/*
* Name: sendData
* Description: Function used to send data to ESP8266.
* Params: command - the data/command to send; timeout - the time to wait for a response; debug - print to Serial window?(true = yes, false = no)
* Returns: The response from the esp8266 (if there is a reponse)
*/
String sendData(String command, const int timeout, boolean debug){
  String response = "";
  esp8266.print(command); // send the read character to the esp8266
  long int time = millis();
  
  while( (time+timeout) > millis()){
    while(esp8266.available()){
      // The esp has data so display its output to the serial window 
      char c = esp8266.read(); // read the next character.
      response+=c;
    }  
  }
        
  if(debug){
    Serial.print(response);
  }
  
  return response;
}
