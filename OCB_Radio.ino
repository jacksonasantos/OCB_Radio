/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
// Programa : Rádio
// Autor    : Jackson Alessandro dos Santos
// Data     : 15/09/2017
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 

#define DEBUG                                                // Flag de debug (Se habilitado. Deixa o sistemas mais lento)

                                                             // Inclusao de Biblioteca utilizadas
#include <Arduino.h>                                            // Biblioteca Padrao Arduino
#include <RTClib.h>                                             // Driver Relogio - Tiny RTC I2C
#include <Adafruit_GFX.h>                                       // Biblioteca Grafica Principal
#include <Adafruit_TFTLCD.h>                                    // Biblioteca Configuração Display LCD TFT
#include <SPI.h>                                                // Habilita a comunicação com devices que usam o barramento SPI - Serial Peripheral Interface
#include <radio.h>                                              // Radio FM
#include <si4703.h>                                             // Controlador Radio
#include <RDSParser.h>                                          // Controlar o conteudo do RDS do radio
#include <RotaryEncoder.h>                                      // Controlar os Encoder´s
#include <DFRobotDFPlayerMini.h>                                // Controlar Mini Player MP3 - SD
#include <SoftwareSerial.h>                                     // Controlar a Interface Serial via Software
#include <SD.h>                                                 // Controlar o leitor de SD do TFT

uint16_t                  vg_identifier  = 0x7575;              // Variavel de identificacao do tft LCD 2.4
int                       btnPrevState   = 0;                   // Variavel de controle do Retrocesso; no Seek no Radio e Anterior no MP3 | USB
int                       btnNextState   = 0;                   // Variavel de controle do Avanço; no Seek no Radio e Proximo no MP3 | USB
int                       btnMuteState   = 0;                   // Variavel de controle do Mude no Radio | MP3 | USB
int                       btnModoState   = 2;                   // Variavel de controle de modo de operacao (1-Setup / 2-Radio FM / 3-SD / 4-USB)
float                     _colAnte       = 0;                   // Variavel de controle da coluna anterior utilizada no display da Frequencia no Radio

                                                             // Configuracao das Portas 
                                                                // Definicao Variaveis                                                               
const int                 vg_PlayPin     = 3;                      // 3              - PWM         - Controle de status do Play do MP3 (BUSY)
const int                 vg_Rele1Pin    = 4;                      // 4              - PWM         - Controle de status do Rele1 para som do Radio
const int                 vg_Rele2Pin    = 5;                      // 5              - PWM         - Controle de status do Rele2 para som do MP3
const int                 btnPrevPin     = 6;                      // 6              - PWM         - Botão Prev
const int                 btnNextPin     = 7;                      // 7              - PWM         - Botão Next
const int                 btnMutePin     = 8;                      // 8              - PWM         - Botão Mute
const int                 btnModoPin     = 9;                      // 9              - PWM         - Botão Modo
const int                 vg_chipSD      = 53;                     // 53             - Digital     - Controle Pino Iniciação do SD

                                                                // Definicao Objetos
RTC_DS3231                relogio;                                 // I2C(SCL1/SDA1) - Digital     - Modulo RTC          ligado as portas I2C                   
RotaryEncoder             encoderVol(10, 11);                      // 10-11          - PWM         - Rotary-Encoder Volume
RotaryEncoder             encoderVolMP3(10, 11);                   //                - copia       - Rotary-Encoder Volume
RotaryEncoder             encoderFrq(12, 13);                      // 12-13          - PWM         - Rotary-Encoder Frequencia
RotaryEncoder             encoderVlr(12, 13);                      //                - copia       - Rotary-Encoder usando no Setup
SoftwareSerial            mySoftwareSerial(17, 16);                // 17-16          - Digital     - Modulo MP3          ligado as portas RX2, TX2 na SoftSerial2
SI4703                    radio;                                   // 20-21          - Digital     - Modulo SI4703       ligado as portas I2C (SCL/SDA) do Mega 
                                                                   // 22-29          - Digital     - Modulo TFT LCD      ligado as portas D0-D7
Adafruit_TFTLCD           monitor(40, 38, 39, 42, 41);             // 38-42          - Digital     - Modulo LCD          ligado as portas de controle RD,WS,RS,CS,RST

DFRobotDFPlayerMini       mp3player;                         // Objeto de controle do Mp3
RDSParser                 rds;                               // Objeto de controle do Radio - Informacoes RDS
AUDIO_INFO                audioInfo;                         // Objeto de controle do Radio - Informacoes Audio
RADIO_INFO                radioInfo;                         // Objeto de controle do Radio - Informacoes Radio

#define                   FIX_BAND           RADIO_BAND_FM   // Definição Banda inicial de sintonia do radio como FM
#define                   FIX_STATION        10230           // Definição Estacao de preferencia de FM
#define                   FIX_VOLUME         6               // Definição Volume inicial


// Definição de nomes legiveis das Cores para valores de 16-bit
#define BLACK            0x0000      /*   0,   0,   0 */
#define NAVY             0x000F      /*   0,   0, 128 */
#define DARKGREEN        0x03E0      /*   0, 128,   0 */
#define DARKCYAN         0x03EF      /*   0, 128, 128 */
#define MAROON           0x7800      /* 128,   0,   0 */
#define PURPLE           0x780F      /* 128,   0, 128 */
#define OLIVE            0x7BE0      /* 128, 128,   0 */
#define LIGHTGREY        0xC618      /* 192, 192, 192 */
#define DARKGREY         0x7BEF      /* 128, 128, 128 */
#define BLUE             0x001F      /*   0,   0, 255 */
#define GREEN            0x07E0      /*   0, 255,   0 */
#define CYAN             0x07FF      /*   0, 255, 255 */
#define RED              0xF800      /* 255,   0,   0 */
#define MAGENTA          0xF81F      /* 255,   0, 255 */
#define YELLOW           0xFFE0      /* 255, 255,   0 */
#define WHITE            0xFFFF      /* 255, 255, 255 */
#define ORANGE           0xFD20      /* 255, 165,   0 */
#define GREENYELLOW      0xAFE5      /* 173, 255,  47 */ 

///////////
// Setup //
///////////
void setup() 
{  
#ifdef DEBUG  
  Serial.begin(115200);                                  // Inicializa Serial caso seja configurado para DEBUG
#endif

  pinMode(btnPrevPin,INPUT);                             // Configura os Botoes de Controle
  pinMode(btnNextPin,INPUT);                             
  pinMode(btnMutePin,INPUT);                             
  pinMode(btnModoPin,INPUT);
  pinMode(vg_Rele1Pin,OUTPUT);
  pinMode(vg_Rele2Pin,OUTPUT);
  pinMode(vg_PlayPin,INPUT_PULLUP); 
  pinMode(SS, OUTPUT);
  
  digitalWrite(vg_PlayPin,HIGH); 
  digitalWrite(10,HIGH); 
  digitalWrite(11,HIGH); 

//------------------------------------------------
#ifdef DEBUG
  Serial.print("Inicializando Chip SD..... ");             
#endif  
  if (!SD.begin(vg_chipSD)) {                            // Inicializa o Chip SD do TFT
#ifdef DEBUG 
    Serial.println("Falhou!");
#endif    
    return;
  }
  else {
#ifdef DEBUG
  Serial.println("OK!");
#endif  
  }
//------------------------------------------------
  iniciaTFT();                                           // Inicializa o Monitor
  telaIntroducao();                                      // Apresenta Abertura
//------------------------------------------------
#ifdef DEBUG
  Serial.print("Inicializando Relógio..... ");             
#endif  
  if (!relogio.begin()) {                                // Inicializa o Relogio
#ifdef DEBUG 
    Serial.println("Falhou!");
#endif    
    return;
  }
  else {
#ifdef DEBUG
  Serial.println("OK!");
#endif  
  }
//------------------------------------------------
#ifdef DEBUG
  Serial.print("Inicializando MP3 Player.. ");
#endif  
  if (!iniciaMP3(DFPLAYER_DEVICE_SD)){                   // Inicializa o MP3 Player (Mini SD)
#ifdef DEBUG
    Serial.println("Falhou!");
#endif  
    return;
  }
  else {
#ifdef DEBUG
     Serial.println("OK!");
#endif  
  }
//------------------------------------------------
  iniciaFM();                                            // Inicializa o Radio FM

  preparaTFT();                                          // Prepara o Monitor com a configuração inicial
  limpaArea();                                           // Limpa a Área util de deixa fundo preto

} 

//////////
// Loop //
//////////
void loop() 
{ 
  if (digitalRead(btnModoPin) ) {                        // Verifica se o botão dp Modo de Execução foi acionado
    btnModoState++;
    if (btnModoState > 4) {btnModoState = 1;}
    delay(50);
#ifdef DEBUG
    Serial.print("MODO: ");   Serial.println(btnModoState);
#endif  
  }      

  if( btnModoState == 1 || btnModoState == 2 ) {         // Configura os Rele para direcionarmento da saida dos alto falantes (1 e 2 modos SETUP e FM)
    digitalWrite(vg_Rele1Pin, LOW); 
    digitalWrite(vg_Rele2Pin, LOW);   
  }
  else {  
    digitalWrite(vg_Rele1Pin, HIGH); 
    digitalWrite(vg_Rele2Pin, HIGH); 
  }

  monitor.setTextSize(2);
  monitor.setTextColor(WHITE, BLUE);
  if (btnModoState == 1)                                 // Executa o modo Setup
     { imprimeTexto("Setup","E",5);
       executaSetup(); }                         
  else if(btnModoState == 2)                             // Executa o modo Radio FM
     { radio.setMute(0);
       imprimeTexto("Radio","E",5);
       executaFM(); }
  else if(btnModoState == 3)                             // Executa o modo MP3 no SD
     { radio.setMute(1);
       imprimeTexto("SD   ","E",5);
       executaMp3(DFPLAYER_DEVICE_SD); } 
  else if(btnModoState == 4)                             // Executa o modo MP3 no USB
     { radio.setMute(1);
       imprimeTexto("USB  ","E",5);
       executaMp3(DFPLAYER_DEVICE_U_DISK); }
  
  limpaArea();

} 

