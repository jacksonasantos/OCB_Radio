/////////////////////
// Inicializa FM   //
/////////////////////
bool iniciaFM()
{
#ifdef DEBUG
    Serial.print("Inicializando Radio... ");
#endif  
  if(!radio.init()) {                                       // Inicializa o Radio 
#ifdef DEBUG
    Serial.println("Falhou!");
#endif
    return -1;
  }  
  else {
#ifdef DEBUG
    Serial.println("OK!");
    radio.debugEnable();                                  // Habilita informacoes de debug dos comandos do radio para a porta Serial
#endif 
    radio.setBandFrequency(FIX_BAND, FIX_STATION);         // Define configuracoes de Banda e Frequencia inicial do radio
    radio.setVolume(FIX_VOLUME);                           // Define configuracoes de Volume inicial do radio
    radio.setMono(false);
    radio.setMute(false);
    radio.setSoftMute(false);
    radio.attachReceiveRDS(RDS_process);                   // setup the information chain for RDS data.
    rds.attachServicenNameCallback(DisplayServiceName);
    encoderFrq.setPosition(FIX_STATION / radio.getFrequencyStep());  // Posiciona o botão da Frequencia na Frequencia Default
#ifdef DEBUG
    Serial.print("Debug Radio... ");
    radio.debugRadioInfo();
    Serial.print("Debug Audio... ");
    radio.debugAudioInfo();
    //radio.debugStatus();
#endif
    return 1;
  }
}

//////////////////////
// Executa Radio FM //
//////////////////////
void executaFM()
{ 
  unsigned long             _now           = millis();
  unsigned long             _nextFreqTime  = 1;
  int                       _lastPosVol    = -1;
  int                       _lastPosFrq    = -1;
  
  String                    BAND[]         = {"FM", "FM Mundo", "AM", "KW"};

  #define                   ROTARYSTEPS        1
  #define                   ROTARYMIN          0
  #define                   ROTARYMAX          15

  encoderVol.setPosition(FIX_VOLUME / ROTARYSTEPS);

  while (!digitalRead(btnModoPin))
  {
    _now = millis();
  
    encoderVol.tick();                                         // Verifica o encoder do Volume

    int _newPosVol = encoderVol.getPosition() * ROTARYSTEPS;   // captura a posição fisica atual e calcula a posição lógica

    if (_newPosVol < ROTARYMIN) {
      encoderVol.setPosition(ROTARYMIN / ROTARYSTEPS);
      _newPosVol = ROTARYMIN;
    } else if (_newPosVol > ROTARYMAX) {
      encoderVol.setPosition(ROTARYMAX / ROTARYSTEPS);
      _newPosVol = ROTARYMAX;
    } 

    if (_lastPosVol != _newPosVol) {
#ifdef DEBUG
    Serial.print("Rotary Volume ");  Serial.println(_newPosVol);
#endif    
      _lastPosVol = _newPosVol;
      radio.setVolume(_lastPosVol);
      mostraVolume();    
    } 

    encoderFrq.tick();
    int _newPosFrq = encoderFrq.getPosition() * radio.getFrequencyStep();

    if (_newPosFrq < radio.getMinFrequency()) {
      encoderFrq.setPosition(radio.getMinFrequency() / radio.getFrequencyStep());
      _newPosFrq = radio.getMinFrequency();
    } else if (_newPosFrq > radio.getMaxFrequency()) {
      encoderFrq.setPosition(radio.getMaxFrequency() / radio.getFrequencyStep());
      _newPosFrq = radio.getMaxFrequency();
    } 

    if (_lastPosFrq != _newPosFrq) {
#ifdef DEBUG
    Serial.print("Rotary Frequencia ");
    Serial.println(_newPosFrq);
#endif
      _lastPosFrq = _newPosFrq;
      radio.setFrequency(_lastPosFrq);
      mostraFrequencia(100); 
    } 

    btnMuteState = digitalRead(btnMutePin);                   // Faz a Leitura do Botões
    btnPrevState = digitalRead(btnPrevPin);
    btnNextState = digitalRead(btnNextPin);
    
    if (btnMuteState == LOW) {                                // Verifica Botão de Mude      
      radio.setMute(!radio.getMute());
      delay(500);
    }
    else if (btnPrevState == HIGH){                           // Verifica Seek Avançar
      radio.seekDown(false);
      delay(10);
      encoderFrq.setPosition(radio.getFrequency() / radio.getFrequencyStep());  // Posiciona o botão da Frequencia na Frequencia Escolhida
    }
    else if (btnNextState == HIGH){                          // Verifica Seek Retroceder
      radio.seekUp(false);
      delay(10);
      encoderFrq.setPosition(radio.getFrequency() / radio.getFrequencyStep());  // Posiciona o botão da Frequencia na Frequencia Escolhida
    }
    else if (_now > _nextFreqTime )                          // Verifica Atualização da tela do radio
    { 
      mostra_relogio();
      radio.checkRDS();
      radio.getRadioInfo(&radioInfo);
      radio.getAudioInfo(&audioInfo);

      monitor.setTextColor(WHITE,BLACK);  
      imprimeTexto(BAND[radio.getBand()-1],"C",45);
      monitor.setTextSize(1);
      imprimeTexto((audioInfo.mute      ? "MUTE  " : "      "),"D",40);
      imprimeTexto((audioInfo.softmute  ? "SMUTE " : "      "),"D",55);
      imprimeTexto((audioInfo.bassBoost ? "BASS  " : "      "),"D",70);
      imprimeTexto((radioInfo.tuned     ? "TUNED " : "      "),"D",85);
      imprimeTexto((radioInfo.stereo    ? "STEREO" : "MONO  "),"D",100);
      monitor.setTextSize(2);
      mostraSinal( 60, 120, 40, 2);     // usar 3o parametro com valor PAR | 4o param (1)=Triangulo (2)=Radar
      mostraFrequencia(100);
      mostraRDS(); 
      mostraVolume();    
      _nextFreqTime = _now + 1000;
    }
  }
}
////////////////////////////////////////////////////////////////
void RDS_process(uint16_t block1, uint16_t block2, uint16_t block3, uint16_t block4) {
  g_block1 = block1;
  g_block2 = block2;
  g_block3 = block3;
  g_block4 = block4;
  rds.processData(block1, block2, block3, block4);
}
////////////////////////////////////////////////////////////////
void mostraRDS() {
  if ( g_block1 ) {
#ifdef DEBUG
    Serial.print('RDS [');  Serial.print(g_block1, HEX); Serial.print(g_block2, HEX); Serial.print(g_block3, HEX); Serial.print(g_block4, HEX); Serial.println(']');
#endif          
  } 
}
////////////////////////////////////////////////////////////////
/// Update the ServiceName text on the LCD display when in RDS mode.
void DisplayServiceName(char *name) 
{
  bool found = false;

  for (uint8_t n = 0; n < 8; n++)
    if (name[n] != ' ') found = true;

  if (found) {
#ifdef DEBUG
    Serial.print("RDS:");
    Serial.print(name);
    Serial.println('.');
#endif
    monitor.setTextSize(1);
    imprimeTexto(name,"C",135);
  }
} 
////////////////////////////////////////////////////////////////
void mostraFrequencia(int16_t _lin)
{
  int16_t _linMax    = monitor.height();
  char    s[12];
  
  radio.formatFrequency(s, sizeof(s));                 // Formata a frequencia sintonizada
  radio.clearRDS();

  monitor.setTextColor(WHITE,BLACK);  
  monitor.setTextSize(3);
  imprimeTexto(s,"C",_lin);                            // Mostra a frequencia sintonizada em numeros formatada
  if ( radioInfo.tuned )                               // Mostra simbolo de radio sintonizado - bolinha verde
     monitor.fillCircle(75, _lin, 5, GREEN);
     
  if (_colAnte > 0) {
     monitor.drawFastVLine(_colAnte  , _linMax-83, 40, BLACK);
     monitor.drawFastVLine(_colAnte+1, _linMax-83, 40, BLACK);
  }
  
  int _x   = 10;
  int _tam = 10;                                       // Mostra barra de frequencia
  for (int i=0; i<(_tam*21); i+=_tam)
  { 
    if (i%25 == 0) _x=20; else _x=10;
    monitor.drawFastVLine(80+i, _linMax-53-_x, _x, WHITE);
  }

  // Calcular o movimento da frequencia o mostra barra da sintonia 
  float _colFreq = map(radio.getFrequency(), radio.getMinFrequency(), radio.getMaxFrequency(), 80, 280);
  monitor.drawFastVLine(_colFreq  , _linMax-63-_x, _x+20, ORANGE);
  monitor.drawFastVLine(_colFreq+1, _linMax-63-_x, _x+20, ORANGE);
  _colAnte = _colFreq;
}
////////////////////////////////////////////////////////////////
void mostraVolume()
{
  int16_t _linMax = monitor.height();
#ifdef DEBUG
  Serial.print(" Volume  : ");Serial.println(radio.getVolume());
#endif
  mostraTermometro("Vol", radio.getVolume(), radio.MAXVOLUME, (radio.MAXVOLUME-2), 10, _linMax-43, 20 );
}
////////////////////////////////////////////////////////////////
void mostraSinal(int16_t _col, int16_t _lin, int16_t _tam, int16_t _forma)
{
#ifdef DEBUG
  Serial.print(" Sinal   : ");Serial.println(radioInfo.snr);
#endif

if (radioInfo.snr > 0) {
  // Forma de Triangulo
  if (_forma == 1) {  
    monitor.fillTriangle(_col+_tam+2,_lin-(_tam/2)-2,_col+_tam+1,_lin+1,_col-3,_lin+1,WHITE); 
    monitor.fillTriangle(_col+_tam  ,_lin-(_tam/2)  ,_col+_tam  ,_lin  ,_col  ,_lin  ,BLACK); 
  }
  // Forma de Radar
  else {  
    monitor.drawCircle(_col, _lin, 5, GREEN);
  }
  
    int _sinal = map(radioInfo.snr,0,127,0,5);
    if ( _sinal > (_tam/2) ) {_sinal = (_tam/2);}
       
    int y = 0;
    for (int x = 1; x <= _sinal; x++) { 
      if (_forma == 1){  
        monitor.drawFastVLine(_col+(2*x), _lin-x, x, WHITE);
      }
      else {
        if (x==1)
          monitor.fillCircle(_col, _lin, 5, GREEN);
        else {
          monitor.drawCircleHelper(_col , _lin - (x+y), 10+y, 3, GREEN);     
          y = y+3;
        }
      }
    }
  }
}

