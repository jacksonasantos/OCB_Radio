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
#include <RDSParser.h>                                          // Controlar o conteudo do RDS
#include <RotaryEncoder.h>                                      // Controlar os Encoder´s
#include <DFRobotDFPlayerMini.h>                                // Controlar Mini Player MP3 - SD

uint16_t                  vg_identifier  = 0x7575;              // Variavel de identificacao do tft LCD 2.4
float                     v_colAnte      = 0;           
int                       btnPrevState   = 0;
int                       btnNextState   = 0;
int                       btnMuteState   = 0;
int                       btnModoState   = 2;                   // Variavel Global de modo de operacao (1-Setup / 2-Radio FM / 3-SD / 4-USB)

                                                             // Configuracao das Portas 
                                                                // Definicao Variaveis
const int                 vg_PlayPin     = 3;                      // 3              - PWM         - Controle de status do Play do MP3 (BUSY)
const int                 vg_Rele1Pin    = 4;                      // 4              - PWM         - Controle de status do Rele1 para som do Radio
const int                 vg_Rele2Pin    = 5;                      // 5              - PWM         - Controle de status do Rele2 para som do MP3
const int                 btnPrevPin     = 6;                      // 6              - PWM         - Botão Prev
const int                 btnNextPin     = 7;                      // 7              - PWM         - Botão Next
const int                 btnMutePin     = 8;                      // 8              - PWM         - Botão Mute
const int                 btnModoPin     = 9;                      // 9              - PWM         - Botão Modo
                                                                // Definicao Objetos
RTC_DS3231                relogio;                                 // I2C(SCL1/SDA1) - Digital     - Modulo RTC          ligado as portas I2C                   
                                                                   // 19-18          - Digital     - Modulo MP3          ligado as portas RX1, TX1 na Serial1
SI4703                    radio;                                   // 20-21          - Digital     - Modulo SI4703       ligado as portas I2C (SCL/SDA) do Mega 
                                                                   // 22-29          - Digital     - Modulo TFT LCD - D0-D7
Adafruit_TFTLCD           monitor(40, 38, 39, 42, 41);             // 38-42          - Digital     - Modulo LCD - Controle
RotaryEncoder             encoderVol(A6, A7);                      // A6-A7          - Analogica   - Rotary-Encoder Volume
RotaryEncoder             encoderVolMP3(A6, A7);                   //                - copia       - Rotary-Encoder Volume
RotaryEncoder             encoderFrq(A2, A3);                      // A2-A3          - Analogica   - Rotary-Encoder Frequencia
RotaryEncoder             encoderVlr(A2, A3);                      //                - copia       - Rotary-Encoder usando no Setup

DFRobotDFPlayerMini       mp3player;                         // Objeto de controle do Mp3
RDSParser                 rds;                               // Objeto de controle do Radio - Informacoes RDS
AUDIO_INFO                audioInfo;                         // Objeto de controle do Radio - Informacoes Audio
RADIO_INFO                radioInfo;                         // Objeto de controle do Radio - Informacoes Radio

#define                   FIX_BAND           RADIO_BAND_FM   // Define Banda inicial de sintonia do radio como FM
#define                   FIX_STATION        10230           // Define Estacao preferencia de FM
#define                   FIX_VOLUME         6               // Define Volume inicial


// Define nomes legiveis das Cores dos valores de 16-bit - https://stackoverflow.com/questions/13720937/c-defined-16bit-high-color
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
  Serial.begin(115200);                                    // Inicializa Serial
#endif

  pinMode(btnPrevPin,INPUT);                             // Configura os Botoes de Controle do Radio
  pinMode(btnNextPin,INPUT);                             
  pinMode(btnMutePin,INPUT);                             
  pinMode(btnModoPin,INPUT);
  pinMode(vg_Rele1Pin,OUTPUT);
  pinMode(vg_Rele2Pin,OUTPUT);

  iniciaTFT();                                           // Inicializa o Monitor

//  telaIntroducao();                                    // Apresenta Abertura
  preparaTFT();
  limpaArea();

#ifdef DEBUG
  Serial.print("Inicializando Relógio... ");             
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

#ifdef DEBUG
  Serial.print("Inicializando MP3 Player... ");
#endif  
  if (!iniciaMP3(DFPLAYER_DEVICE_SD))                    // Inicializa o MP3 Player (Mini SD)
#ifdef DEBUG
     Serial.println("Falhou!");
#endif  
  else
#ifdef DEBUG
     Serial.println("OK!");
#endif  
 
#ifdef DEBUG
  Serial.println("Inicializando Radio... ");
#endif  
  radio.init();                                          // Inicializa o Radio 
#ifdef DEBUG
  radio.debugEnable();                                   // Habilita informacoes de debug do radio para a porta Serial
#endif  
  radio.setBand(FIX_BAND);                               // Define configuracoes de Banda inicial do radio
  radio.setFrequency(FIX_STATION);                       // Define configuracoes de Frequencia inicial do radio
  radio.setVolume(FIX_VOLUME);                           // Define configuracoes de Volume inicial do radio
  radio.setBassBoost(false);
  radio.setMono(false);
  radio.setMute(false);
  radio.setSoftMute(false);
  radio.attachReceiveRDS(RDS_process);                   // setup the information chain for RDS data.
  rds.attachServicenNameCallback(DisplayServiceName);
#ifdef DEBUG
  radio.debugRadioInfo();
  radio.debugAudioInfo();
  //radio.debugStatus();
#endif
#ifdef DEBUG
  Serial.println("Radio OK!");
#endif  

  encoderFrq.setPosition(FIX_STATION / radio.getFrequencyStep());  // Posiciona o botão da Frequencia na Frequencia Default
} 

//////////
// Loop //
//////////
void loop() 
{ 
  if (digitalRead(btnModoPin) ) {
    btnModoState++;
    if (btnModoState > 4) {btnModoState = 1;}
    delay(50);
#ifdef DEBUG
    Serial.print("MODO: ");   Serial.println(btnModoState);
#endif  
  }      

if( btnModoState == 1 || btnModoState == 2 ) {
  digitalWrite(vg_Rele1Pin, LOW); 
  digitalWrite(vg_Rele2Pin, LOW); 
}
else {  
  digitalWrite(vg_Rele1Pin, HIGH); 
  digitalWrite(vg_Rele2Pin, HIGH); 
}

  monitor.setTextSize(2);
  monitor.setTextColor(WHITE, BLUE);
  if (btnModoState == 1)                              // Executa o modo Setup
     { imprimeTexto("Setup","E",5);
       executaSetup(); }                         
  else if(btnModoState == 2)                          // Executa o modo Radio FM
     { radio.setMute(0);
       imprimeTexto("Radio","E",5);
       executaFM(); }
  else if(btnModoState == 3)                          // Executa o modo MP3 no SD
     { radio.setMute(1);
       imprimeTexto("SD   ","E",5);
       executaMp3(DFPLAYER_DEVICE_SD); }
  else if(btnModoState == 4)                          // Executa o modo MP3 no USB
     { radio.setMute(1);
       imprimeTexto("USB  ","E",5);
       executaMp3(DFPLAYER_DEVICE_U_DISK); }
  
  limpaArea();

} 

