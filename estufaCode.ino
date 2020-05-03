/*
 * Este código foi criado por: Ângelo Augusto
 * V3.0
 * 03/05/2020
 */
#include <LiquidCrystal.h>
#include <dht.h>
#include <Servo.h>
#include <IRremote.h>

#define DHT11_PIN 7       //Define a Porta de DHT11
#define SERVO 6           //Define a Porta para o Servo

IRrecv receptor(13);      // Recetor de sinal
dht DHT;                  //Variavel do Sensor de Temperatura e Humidade
Servo servo;              // Variável Servo
decode_results resultado; //variavel do resultado descodificado do

int codigo;
int pos;               // Posição Servo
int porta_rele1 = 8;   //Porta ligada ao pino IN1 do modulo    LED
int porta_rele2 = 10;  //Porta ligada ao pino IN2 do modulo   Bomba de Agua
int pin_vent = 9;      //Ventoinha
int ldrPin = A0;       //Sensor Luz LDR no pino analógico 0
int ldrValor = 0;      //Valor lido do LDR
int segundosServo = 0; //Segundos do servo
int segundosAgua = 0;  //Segundos da agua
int segundosLuz = 0;   //Segundos da luz
int segundosLuzComando=0;
int segundosServoComando=0;
int segundosAguaComando=0;

String led;

boolean verificaServo = false; //Diz se a janela está aberta ou fechada
boolean verificaAgua = false;  //Diz se a bomba de água está ligada ou desligada
boolean verificaLuz = false;   //Diz se a luz está acesa ou apagada
boolean verificaLuz2=false;
boolean verificaAgua2=false;

LiquidCrystal lcd(12, 11, 5, 4, 3, 2); //Define os pinos que serão utilizados para ligação ao display

void setup() {
  receptor.enableIRIn();
  servo.attach(SERVO);

  pinMode(porta_rele1, OUTPUT); //Porta 1 do rele
  pinMode(porta_rele2, OUTPUT); //Porta 2 do rele
  pinMode(pin_vent, OUTPUT);    //Pino da ventoinha

  servo.write(15);    //Servo abre até 15º
  Serial.begin(9600); //Abre a porta 9600 do monitor do Arduino
  lcd.begin(16, 2);   //Define quantas linhas e colunas tem o lcd

}
void servoMexerAbrir(){
  for (pos = 15; pos < 110; pos++){
    servo.write(pos); //Guarda o ângulo atual do Servo
    delay(10); //Espera 0.01 segundos
  }
}
void servoMexerFechar(){
  for (pos = 110; pos >= 15; pos--){
    servo.write(pos); //Guarda o ângulo atual do Servo
    delay(10); //Espera 0.01 segundos
  }
}

void lcdAmostraValores() //Função para escrever no LCD
{
  if (ldrValor <= 800){
    led = "ON";
  }else{
    led = "OFF";
  }
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temp:");
    lcd.print(DHT.temperature,1);
    lcd.print("C ");
    lcd.print("LED");
    lcd.setCursor(0, 1);
    lcd.print("Humi:");
    lcd.print(DHT.humidity,1);
    lcd.print("% ");
    lcd.print(led);
  
}
void comando(){
  codigo = resultado.value;
  switch (codigo)
  {
  case (0xFFA25D): //1
    servoMexerAbrir();
    break;
  case (0xFF629D): //2
    servoMexerFechar();
    break;
  case (0xFFE21D): //3
    if(verificaLuz2==false){
      verificaLuz=true;
    }
    break;
  case (0xFF22DD): //4
    digitalWrite(porta_rele1, LOW);
    segundosLuzComando=0;
    verificaLuz=false;
    break;
  case (0xFF02FD): //5
    if(verificaAgua2==false){
      verificaAgua=true;
    }
    break;
  case (0xFFC23D): //6
    digitalWrite(porta_rele2, LOW);
    segundosAguaComando=0;
    verificaAgua=false;
    break;
  }
  receptor.resume();
  delay(100);
}
void loop() {
  
  
  ldrValor = analogRead(ldrPin);
  DHT.read11(DHT11_PIN);

  if (receptor.decode(&resultado)){
    comando();
  }
  Serial.println(segundosLuz);
  delay(2000);
  lcdAmostraValores();
  
  segundosLuz=segundosLuz+2;
  segundosServo=segundosServo+2;
  segundosAgua=segundosAgua+2;
  
  if(verificaLuz==false){
    luz(segundosLuz);
  }else{
    segundosLuzComando=segundosLuzComando+2;
    luzCom(segundosLuzComando);
  }
  if(verificaAgua==false){
    agua(segundosAgua);
  }else{
    segundosAguaComando=segundosAguaComando+2;
    aguaCom(segundosAguaComando);
  }
  servoMove(segundosServo);
  
}

void servoMove(int tempo){
  if (tempo <= 600 && verificaServo==false){//10 min
    digitalWrite(pin_vent, HIGH); //Acelera Ventoinha
    verificaServo=true;
    servoMexerAbrir();
  }else if(tempo>=600 && tempo<3600 && verificaServo==true){ //10 min e 1 hora
    digitalWrite(pin_vent, LOW); //Desacelera Ventoinha
      servoMexerFechar();
      verificaServo=false;
  }else if(tempo>3600){ //passar para 1 hora
    segundosServo = 0;
  }
}

void luz(int tempo){
  if (tempo <= 28800){//8 horas
    digitalWrite(porta_rele1, HIGH);
    verificaLuz2=true;
  }else if(tempo>=28800 && tempo<57600){ //8 horas e 16 horas
    digitalWrite(porta_rele1, LOW);
    verificaLuz2=false;
  }else if(tempo>57600){ //16 horas
    Serial.println("Fim");
    segundosLuz = 0;
  }
}
void luzCom(int tempo){
  if (tempo <= 120){//2 minutos
    digitalWrite(porta_rele1, HIGH);
  }else{
    verificaLuz=false;
    segundosLuzComando=0;
  }
}

void agua(int tempo){
  if (tempo <= 900){//15 min
    digitalWrite(porta_rele2, HIGH);
    verificaAgua2=true;
  }else if(tempo>=900 && tempo<2100){ //15 min e 35 min
    digitalWrite(porta_rele2, LOW);
    verificaAgua2=false;
  }else if(tempo>2100){ //35min
    Serial.println("Fim");
    segundosAgua = 0;
  }
}
void aguaCom(int tempo){
  if (tempo <= 120){//2 minutos
    digitalWrite(porta_rele2, HIGH);
  }else{
    verificaAgua=false;
    segundosAguaComando=0;
  }
}
