#include <Arduino.h>
#include "BluetoothSerial.h"
#include <Wire.h>
#include "rgb_lcd.h"
#include <ESP32Encoder.h>

BluetoothSerial SerialBT;
ESP32Encoder encoder;
rgb_lcd lcd;

// declaration du PWM
const int onMoteur = 25;
int PWM = 27;
int phase = 26;

// declaration ecran lcd
int Red_value = 0;
int Green_value = 0;
int Blue_value = 0;

// declaration de la vitesse de rotation du moteur
int PWM_valeur;
int vit;

// capteur 
int val_capteur; // valeur lue dans setup ou loop

// encoder variables
int32_t val_encoder; 
int val_gaug; // position modulo

char BluetoothData;

void setup()
{
  Serial.begin(115200);
  pinMode(onMoteur, OUTPUT);
  pinMode(PWM, OUTPUT);
  pinMode(phase, OUTPUT);

  // démarrage du Bluetooth
  if (!SerialBT.begin("ESP32_BT_Tayyip_Jules"))
  {
    Serial.println("Erreur initialisation Bluetooth Tayyip_Jules");
    while (1)
      ;
  }
  Serial.println("Bluetooth Tayyip_Jules actif. En attente de connexion...");

  // initialisation de l'écran LCD RGB
  Wire1.setPins(15, 5);
  lcd.begin(16, 2, LCD_5x8DOTS, Wire1);
  lcd.setRGB(Red_value, Green_value, Blue_value);

  // encoder hardware setup
  encoder.attachFullQuad(23, 19);
  encoder.setCount(0);

  // lecture initiale du capteur
  val_capteur = analogRead(36);
}

void loop()
{
  
  if (SerialBT.available())
  {
    char c = SerialBT.read();
    Serial.print("Reçu : ");
    Serial.println(c);
    if (c == 'G')
    {
      digitalWrite(onMoteur, HIGH);
      analogWrite(PWM, vit);   
      digitalWrite(phase, LOW); // Configuration du signe (direction)
      SerialBT.println("Moteur ON");
    }
    if (c == 'D')
    {
      digitalWrite(onMoteur, HIGH);
      analogWrite(PWM, vit);     
      digitalWrite(phase, HIGH); // Configuration du signe (direction)
      SerialBT.println("Moteur ON");
    }
    if (c == 'F')
    {
      digitalWrite(onMoteur, LOW);
      analogWrite(PWM, 0); // Arrêt du PWM
      SerialBT.println("Moteur OFF");
    }
    if (c == 'O')
    {
      digitalWrite(onMoteur, HIGH);
      analogWrite(PWM, vit);    
      digitalWrite(phase, LOW); // Configuration du signe (direction)
      SerialBT.println("Moteur ON");
    }

    // écran lcd 
    if (c == 'R')
      Red_value = SerialBT.parseInt(); // Read Red value
    if (c == 'V')
      Green_value = SerialBT.parseInt(); // Read Green Value
    if (c == 'B')
      Blue_value = SerialBT.parseInt(); // Read Blue Value

    lcd.setRGB(Red_value, Green_value, Blue_value);
    
    // viteese de rotation du moteur
    if (c == 'P') {
      PWM_valeur = SerialBT.parseInt();
      vit = map(PWM_valeur, 0, 255, 0, 255); 
      analogWrite(PWM, vit);
    }
    
    val_encoder = encoder.getCount();

    if (val_capteur > 2000)
    {
      SerialBT.print ("*LR0G255B0");
      encoder.setCount(0);
    }
    else
    {
      SerialBT.print ("*LR0G0B0");
    }

    val_gaug = val_encoder % 800;
    if (val_gaug < 0)
    {
      val_gaug += 800;
    }
    
    SerialBT.print("*T");
    SerialBT.print(val_gaug);

  }
}