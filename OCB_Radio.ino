/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
// Programa : Rádio
// Autor    : Jackson Alessandro dos Santos
// Data     : 15/09/2017
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 

#define DEBUG                                                // Flag de debug (Se habilitado. Deixa o sistemas mais lento)
#define DEBUG_DTL                                            // Flag de detalhamento do debug de alguns componentes

                                                             // Inclusao de Biblioteca utilizadas
#include <Arduino.h>                                            // Biblioteca Padrao Arduino
#include <RTClib.h>                                             // Bibliotera Relogio - Tiny RTC I2C
#include <Adafruit_GFX.h>                                       // Biblioteca Grafica Principal
#include <Adafruit_TFTLCD.h>                                    // Biblioteca Configuração Display LCD TFT
#include <SPI.h>                                                // Biblioteca que habilita a comunicação com devices que usam o barramento SPI - Serial Peripheral Interface
#include <radio.h>                                              // Biblioteca Customizada p/ Radio FM
#include <si4703.h>                                             // Biblioteca Controlador Radio
#include <RDSParser.h>                                          // Biblioteca Controlar o conteudo do RDS do radio
#include <RotaryEncoder.h>                                      // Biblioteca Encoder´s
#include <DFRobotDFPlayerMini.h>                                // Biblioteca Mini Player MP3 - SD
#include <SD.h>                                                 // Biblioteca leitor de SD do TFT
#include <Wire.h>                                               // Biblioteca conexão I2C para PT2314 - Processador de Audio
#include <pt2314.h>                                             // Biblioteca Processador de Audio IC com: 4 entradas, 1 Saída Estéreo, Volume, Bass, Treble, e Balance
#include <EEPROM.h>             // Load and Save settings

uint16_t                  vg_identifier  = 0x7575;              // Variavel de identificacao do tft LCD 2.4
int                       vg_Volume      = 6;
int                       btnMuteState   = 0;                   // Variavel de controle do Mude no Radio | MP3 | USB
int                       btnPrevState   = 0;                   // Variavel de controle do Retrocesso; no Seek no Radio e Anterior no MP3 | USB
int                       btnNextState   = 0;                   // Variavel de controle do Avanço; no Seek no Radio e Proximo no MP3 | USB
int                       btnOkState     = 0;                   // Variavel de controle do OK
int                       btnSetupState  = 0;                   // Variavel de controle do Setup
int                       btnPreset1State= 0;                   // Variavel de controle do Preset 1
int                       btnPreset2State= 0;                   // Variavel de controle do Preset 2
int                       btnPreset3State= 0;                   // Variavel de controle do Preset 3
float                     _colAnte       = 0;                   // Variavel de controle da coluna anterior utilizada no display da Frequencia no Radio
char                      *modos[3]      = {"Radio", "SD   ", "USB  "};

                                                             // Configuracao das Portas 
                                                                // Definicao Variaveis                                                               
const int                 vg_PlayPin     = 4;                      // 4              - PWM         - Controle de status do Play do MP3 (BUSY)
const int                 btnModoPin     = 5;                      // 5              - PWM         - Botão Modo
const int                 btnPrevPin     = 6;                      // 6              - PWM         - Botão Prev
const int                 btnNextPin     = 7;                      // 7              - PWM         - Botão Next
const int                 btnMutePin     = 8;                      // 8              - PWM         - Botão Mute
const int                 btnOkPin       = 11;                     // 11             - PWM         - Botão OK
const int                 btnSetupPin    = 30;                     // 30             - Digital     - Botão Setup o OCB
const int                 btnPreset1     = 31;                     // 31             - Digital     - Botão Preset1
const int                 btnPreset2     = 32;                     // 32             - Digital     - Botão Preset2
const int                 btnPreset3     = 33;                     // 33             - Digital     - Botão Preset3
const int                 vg_chipSD      = 53;                     // 53             - Digital     - Controle Pino Iniciação do SD

                                                                // Definicao Objetos
RTC_DS3231                relogio;                                 // I2C(SCL1/SDA1) - Digital     - Modulo RTC          ligado as portas I2C                   
RotaryEncoder             encoderVol(9, 10);                       // 9-10           - PWM         - Rotary-Encoder Volume
RotaryEncoder             encoderVolMP3(9, 10);                    //                - copia       - Rotary-Encoder Volume do MP3
RotaryEncoder             encoderFrq(12, 13);                      // 12-13          - PWM         - Rotary-Encoder Frequencia
RotaryEncoder             encoderVlr(12, 13);                      //                - copia       - Rotary-Encoder usando no Setup
//                        Serial2(17, 16);                         // 17-16          - Digital     - Modulo MP3          ligado as portas RX2, TX2 do Serial2
SI4703                    radio;                                   // 20-21          - Digital     - Modulo SI4703       ligado as portas I2C (SCL/SDA) do Mega 
                                                                   // 22-29          - Digital     - Modulo TFT LCD      ligado as portas D0-D7
Adafruit_TFTLCD           monitor(40, 38, 39, 42, 41);             // 38-42          - Digital     - Modulo LCD          ligado as portas de controle RD,WS,RS,CS,RST

DFRobotDFPlayerMini       mp3player;                         // Objeto de controle do Mp3
RDSParser                 rds;                               // Objeto de controle do Radio - Informacoes RDS
AUDIO_INFO                audioInfo;                         // Objeto de controle do Radio - Informacoes Audio
RADIO_INFO                radioInfo;                         // Objeto de controle do Radio - Informacoes Radio
PT2314                    audioProcessor;                    // Objeto de controle do Processador de Audio

#define                   FIX_BAND           RADIO_BAND_FM   // Definição Banda inicial de sintonia do radio como FM
#define                   FIX_STATION        10230           // Definição Estacao de preferencia de FM
#define                   FIX_VOLUME         6               // Definição Volume inicial
#define                   MIN_VOLUME_ENC     0               // Displayed / encoder stops
#define                   MAX_VOLUME_ENC     15              // Displayed / encoder stops
#define                   MIN_VOLUME_PT2314  24              // Sent to the PT2314 (0..63)
#define                   MAX_VOLUME_PT2314  63              // Sent to the PT2314 (0..63)

#define                   PRESETS            3               // Definição da quantidade de Faixas gravadas
#define                   SETTINGS_ADDRESS   64              // EEPROM Addresses
#define                   SETTINGS_ID_STRING "OCB 1.0"       // Identificação do Produto/Versão

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

////////////////
// Structures //
////////////////
typedef struct SettingsType {
    char        id[8];
    byte        source;
    signed char currentPreset;
    int         presetFrequency[PRESETS]; 
    int         currentFrequency;
    int         currentVolume;
    int         bass;
    int         treble;
    int         balance;
};
SettingsType              settings;                          // Objetp de controle das variaveis a sere armazenadas na EEPROM

/**************************
 * Prototipos das Funcoes *
 **************************/
// OCB FM
bool iniciaFM         ();
void executaFM        ();
void RDS_process      (uint16_t block1, uint16_t block2, uint16_t block3, uint16_t block4) ;
void mostraFrequencia (int16_t _lin);
void mostraVolume     ();
void mostraSinal      (int16_t _col, int16_t _lin, int16_t _tam, int16_t _forma);

// OCB MP3
bool iniciaMP3        ();
void executaMp3       (uint8_t device);
void mostraEQMP3      ();
void mostraVolumeMP3  ();
void printDetail      (uint8_t type, int value, int p_posmsg);

// OCB Setup
void executaSetup     ();
void saveSettings     ();
void loadSettings     ();

// OCB Utilitarios
void iniciaTFT        (); 
void telaIntroducao   (); 
void preparaTFT       ();
void imprimeTexto     (String pTexto, String pAlinhamento, int pLinha);
void limpaArea        ();
void mostraTermometro (String p_titulo, int16_t p_valor, int16_t p_tamanho, int16_t p_referencia, int16_t p_col_baixo, int16_t p_lin_baixo, int16_t p_largura );
void mostra_relogio   ();
void bmpDraw          (char *filename, int x, int y);
uint16_t read16       (File f);
uint32_t read32       (File f);


///////////
// Setup //
///////////
void setup() 
{  
#ifdef DEBUG  
  Serial.begin(115200);                                  // Inicializa Serial caso seja configurado para DEBUG
#endif

  loadSettings();                                        // Carrega as configurações iniciais
  settings.source = 0;                                   // Define Radio como inicial
  saveSettings();

  pinMode(btnModoPin ,INPUT);
  pinMode(btnPrevPin ,INPUT);                            // Configura os Botoes de Controle
  pinMode(btnNextPin ,INPUT);                             
  pinMode(btnMutePin ,INPUT_PULLUP);                             
  pinMode(btnOkPin   ,INPUT_PULLUP);                             
  pinMode(btnSetupPin,INPUT);    
  pinMode(btnPreset1 ,INPUT);    
  pinMode(btnPreset2 ,INPUT);    
  pinMode(btnPreset3 ,INPUT);    
  pinMode(vg_PlayPin ,INPUT_PULLUP); 
  pinMode(vg_chipSD  ,OUTPUT);
  
  digitalWrite(vg_PlayPin,HIGH); 

//------------------------------------------------
// Inicializa o Chip SD
#ifdef DEBUG
  Serial.print("Inicializando Chip SD..... ");             
#endif  
  if (!SD.begin(vg_chipSD)) {                            
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
// Inicializa o Monitor
  iniciaTFT();                                           
  telaIntroducao();                                      // Apresenta Abertura
//------------------------------------------------
 // Inicializa o Relogio
#ifdef DEBUG
  Serial.print("Inicializando Relógio..... ");             
#endif  
  if (!relogio.begin()) {                               
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
// Inicializa o MP3 Player (Mini SD)
#ifdef DEBUG
  Serial.print("Inicializando MP3 Player.. ");
#endif  
  if (!iniciaMP3()){                   
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
// Inicializa o Radio FM
  iniciaFM();                                            

//------------------------------------------------
// Inicializa o Processador de Audio
#ifdef DEBUG
  Serial.print("Inicializando Processador de Audio.. ");
#endif  
  audioProcessor.setVolume(map(settings.currentVolume, MIN_VOLUME_ENC, MAX_VOLUME_ENC, MIN_VOLUME_PT2314, MAX_VOLUME_PT2314));
  audioProcessor.setBass(settings.bass);
  audioProcessor.setTreble(settings.treble);
  audioProcessor.setSource(0, false, 3);
  audioProcessor.setBalance(settings.balance);
#ifdef DEBUG
     Serial.println("OK!");
#endif  
    
  preparaTFT();                                          // Prepara o Monitor com a configuração inicial
  limpaArea();                                           // Limpa a Área util de deixa fundo preto

} 

//////////
// Loop //
//////////
void loop() 
{ 
  if (digitalRead(btnModoPin) ) {                        // Verifica se o botão dp Modo de Execução foi acionado
    settings.source++;
    if (settings.source > 2) {settings.source = 0;}
    saveSettings();
    delay(50);
#ifdef DEBUG
    Serial.print("MODO: ");   Serial.println(settings.source);
#endif  
  }      

  monitor.setTextSize(2);
  monitor.setTextColor(WHITE, BLUE);
  imprimeTexto(modos[settings.source],"E",5);
  
  if(settings.source == 0)                                  // Executa o modo Radio FM
     { radio.setMute(0);
       audioProcessor.setSource(0, false, 3);
       executaFM(); }
  else if(settings.source == 1)                             // Executa o modo MP3 no SD
     { radio.setMute(1);
       audioProcessor.setSource(1, false, 3);
       executaMp3(DFPLAYER_DEVICE_SD); } 
  else if(settings.source == 2)                             // Executa o modo MP3 no USB
     { radio.setMute(1);
       audioProcessor.setSource(1, false, 3);
       executaMp3(DFPLAYER_DEVICE_U_DISK); }
  
  limpaArea();

} 

