/*
 * ESP8266 SPIFFS HTML Web Page with JPEG, PNG Image 
 * https://circuits4you.com
 */

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>   //Include File System Headers
#define xD0 16
#define xD1 5
#define xD2 4
#define xD3 0
#define xD4 2
#define xD5 14
#define xD6 12
#define xD7 13
#define xD8 15

const char* htmlfile = "/index.html";

//WiFi Connection configuration
const char* ssid = "Livebox-5454";           // Identifiant WiFi
/*
const char* ssid = "atelier_info";           // Identifiant WiFi
const char* password = "6i3ap4v7y5";  // Mot de passe WiFi*/


#define LED LED_BUILTIN
//déclaration des Variables
#define MotorGEnA  xD8 
#define MotorGIn1  xD7   
#define MotorGIn2  xD6   

#define MotorDEnB  xD8 
#define MotorDIn3  xD3   
#define MotorDIn4  xD2 

int gVitesse=50;
int gDirection=0;
ESP8266WebServer server(80);


void handleMotor(){
  String Val = server.arg("val");
  int p = Val.toInt();

  gDirection=p;
  
  //analogWrite(MotorGEnA,p);

// Gauche [-100   -75   -50   -25  centre   +25   +50   +75    +100] Droit //
// 
  if(p <-40) { 
            Serial.println("Motor G/D= Gauche :"); Serial.println(p);
            digitalWrite(MotorGIn1,HIGH); digitalWrite(MotorGIn2,LOW);
            digitalWrite(MotorDIn3,LOW); digitalWrite(MotorDIn4,LOW);
      } 
  else if (p > 40) { 
            Serial.println("Motor G/D= Droite :"); Serial.println(p);
            digitalWrite(MotorGIn1,LOW); digitalWrite(MotorGIn2,LOW);
            digitalWrite(MotorDIn3,HIGH); digitalWrite(MotorDIn4,LOW);
      }
 else { 
            Serial.println("Motor G/D === tout droit =>"); Serial.println(p);
            digitalWrite(MotorGIn1,HIGH); digitalWrite(MotorGIn2,LOW);
            digitalWrite(MotorDIn3,HIGH); digitalWrite(MotorDIn4,LOW);
      }

  server.send(200, "text/plane","");
}
void handleVitesse(){
  String Val = server.arg("val");
  int p = Val.toInt()*10;
  gVitesse=p;
  
  analogWrite(MotorGEnA,p);

  server.send(200, "text/plane","");
}

void handleRoot(){
  server.sendHeader("Location", "/index.html",true);   //Redirect to our html web page
  server.send(302, "text/plane","");
}

void handleWebRequests(){
  if(loadFromSpiffs(server.uri())) return;
  String message = "File Not Detected\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " NAME:"+server.argName(i) + "\n VALUE:" + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  Serial.println(message);
}

void setup() { 
  Serial.begin(115200);
  Serial.println("Start Setup");

  pinMode(MotorGEnA, OUTPUT);
  pinMode(MotorGIn1, OUTPUT);
  pinMode(MotorGIn2, OUTPUT);

 //pinMode(MotorDEnB, OUTPUT);
 pinMode(MotorDIn3, OUTPUT);
  pinMode(MotorDIn4, OUTPUT);
  
  pinMode(LED_BUILTIN, OUTPUT);    //met la led du Wemos en sortie
  
  //Initialize File System
  SPIFFS.begin();
  Serial.println("File System Initialized");

  //Connect to wifi Network
  WiFi.begin(ssid, password);     //Connect to your WiFi router
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP

  //Initialize Webserver
  server.on("/",handleRoot);
  server.on("/setMotor",handleMotor); //Reads ADC function is called from out index.html
  server.on("/setVitesse",handleVitesse); //Reads ADC function is called from out index.html
  
  server.onNotFound(handleWebRequests); //Set setver all paths are not found so we can handle as per URI
  server.begin();  
}

int cycleBlink=0;
void loop() {
 cycleBlink++;
 //if ( cycleBlink < 50000 ) { digitalWrite(LED_BUILTIN,HIGH); }
 //else if ( cycleBlink < 100000 ) { digitalWrite(LED_BUILTIN,LOW); }
 //else { cycleBlink=0;
 //   Serial.print("Vitesse=");Serial.print(gVitesse);
 //   Serial.print("  Direction=");Serial.println(gDirection);
 //}
 //analogWrite(MotorGEnA,gVitesse);
 
 server.handleClient();
}

bool loadFromSpiffs(String path){
  String dataType = "text/plain";
  if(path.endsWith("/")) path += "index.htm";

  if(path.endsWith(".src")) path = path.substring(0, path.lastIndexOf("."));
  else if(path.endsWith(".html")) dataType = "text/html";
  else if(path.endsWith(".htm")) dataType = "text/html";
  else if(path.endsWith(".css")) dataType = "text/css";
  else if(path.endsWith(".js")) dataType = "application/javascript";
  else if(path.endsWith(".png")) dataType = "image/png";
  else if(path.endsWith(".gif")) dataType = "image/gif";
  else if(path.endsWith(".jpg")) dataType = "image/jpeg";
  else if(path.endsWith(".ico")) dataType = "image/x-icon";
  else if(path.endsWith(".xml")) dataType = "text/xml";
  else if(path.endsWith(".pdf")) dataType = "application/pdf";
  else if(path.endsWith(".zip")) dataType = "application/zip";
  File dataFile = SPIFFS.open(path.c_str(), "r");
  if (server.hasArg("download")) dataType = "application/octet-stream";
  if (server.streamFile(dataFile, dataType) != dataFile.size()) {
  }

  dataFile.close();
  return true;
}
