//////////////////////
// Executa Radio FM //
//////////////////////
void executaFM()
{
  unsigned long             _now           = millis();
  unsigned long             _nextFreqTime  = 1;
  int                       lastPosVol     = -1;
  int                       lastPosFrq     = -1;
  String                    BAND[]         = {"FM", "FM Mundo", "AM", "KW"};

  #define                   ROTARYSTEPS        1
  #define                   ROTARYMIN          0
  #define                   ROTARYMAX          15

  encoderVol.setPosition(FIX_VOLUME / ROTARYSTEPS);

  while (!digitalRead(btnModoPin))
  {
    _now = millis();
  
    btnModoState = digitalRead(btnModoPin);
   
    encoderVol.tick();                                        // Verifica o encoder do Volume

    int newPosVol = encoderVol.getPosition() * ROTARYSTEPS;   // captura a posição fisica atual e calcula a posição lógica

    if (newPosVol < ROTARYMIN) {
      encoderVol.setPosition(ROTARYMIN / ROTARYSTEPS);
      newPosVol = ROTARYMIN;
    } else if (newPosVol > ROTARYMAX) {
      encoderVol.setPosition(ROTARYMAX / ROTARYSTEPS);
      newPosVol = ROTARYMAX;
    } 

    if (lastPosVol != newPosVol) {
#ifdef DEBUG
      Serial.print("Rotary Volume ");
      Serial.println(newPosVol);
#endif    
      lastPosVol = newPosVol;
      radio.setVolume(lastPosVol);
      mostraVolume();    
    } 

    encoderFrq.tick();
    int newPosFrq = encoderFrq.getPosition() * radio.getFrequencyStep();

    if (newPosFrq < radio.getMinFrequency()) {
      encoderFrq.setPosition(radio.getMinFrequency() / radio.getFrequencyStep());
      newPosFrq = radio.getMinFrequency();
    } else if (newPosFrq > radio.getMaxFrequency()) {
      encoderFrq.setPosition(radio.getMaxFrequency() / radio.getFrequencyStep());
      newPosFrq = radio.getMaxFrequency();
    } 

    if (lastPosFrq != newPosFrq) {
#ifdef DEBUG
      Serial.print("Rotary Frequencia ");
      Serial.println(newPosFrq);
#endif
      lastPosFrq = newPosFrq;
      radio.setFrequency(lastPosFrq);
      mostraFrequencia(100); 
    } 

    btnMuteState = digitalRead(btnMutePin);                   // Faz a Leitura do Botões

    if (btnMuteState == LOW) {              
      radio.setMute(!radio.getMute());
      delay(500);
    }
//    else if (btnModoState == HIGH){
//      radio.setBassBoost(!radio.getBassBoost());
//    }
    else if (_now > _nextFreqTime )     // Atualiza tela do radio
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
  rds.processData(block1, block2, block3, block4);
}
////////////////////////////////////////////////////////////////
void mostraRDS() {
  radio.attachReceiveRDS(RDS_process);
  rds.attachServicenNameCallback(DisplayServiceName);
}
////////////////////////////////////////////////////////////////
/// Update the ServiceName text on the LCD display when in RDS mode.
void DisplayServiceName(char *name) {
#ifdef DEBUG
  Serial.print("RDS: "); Serial.println(name);
#endif
  monitor.setTextSize(1);
  imprimeTexto(name,"C",135);
} 
////////////////////////////////////////////////////////////////
void mostraFrequencia(int16_t _lin)
{
  int16_t vlinMax = monitor.height();
  char s[12];
  radio.formatFrequency(s, sizeof(s));               // Formata a frequencia sintonizada
  radio.clearRDS();

  monitor.setTextColor(WHITE,BLACK);  
  monitor.setTextSize(3);
  imprimeTexto(s,"C",_lin);                            // Mostra a frequencia sintonizada em numeros formatada
  if ( radioInfo.tuned )                              // Mostra simbolo de radio sintonizado - bolinha verde
     monitor.fillCircle(75, _lin, 5, GREEN);
     
  if (v_colAnte> 0) {
     monitor.drawFastVLine(v_colAnte  , vlinMax-83, 40, BLACK);
     monitor.drawFastVLine(v_colAnte+1, vlinMax-83, 40, BLACK);
  }
  
  int _x = 10;
  int _tam = 10;                                      // Mostra barra de frequencia
  for (int i=0; i<(_tam*21); i+=_tam)
  { 
    if (i%25 == 0) _x=20; else _x=10;
    monitor.drawFastVLine(80+i, vlinMax-53-_x, _x, WHITE);
  }

  // Calcular o movimento da frequencia o mostra barra da sintonia 
  float _colFreq = map(radio.getFrequency(), radio.getMinFrequency(), radio.getMaxFrequency(), 80, 280);
  monitor.drawFastVLine(_colFreq  , vlinMax-63-_x, _x+20, ORANGE);
  monitor.drawFastVLine(_colFreq+1, vlinMax-63-_x, _x+20, ORANGE);
  v_colAnte = _colFreq;
}
////////////////////////////////////////////////////////////////
void mostraVolume()
{
  int16_t vlinMax = monitor.height();
#ifdef DEBUG
  Serial.print(" Volume  : ");Serial.println(radio.getVolume());
#endif
  mostraTermometro("Vol", radio.getVolume(), radio.MAXVOLUME, (radio.MAXVOLUME-2), 10, vlinMax-43, 20 );
}
////////////////////////////////////////////////////////////////
void mostraSinal(int16_t _col, int16_t _lin, int16_t _tam, int16_t _forma)
{
#ifdef DEBUG
  Serial.print(" Sinal   : ");Serial.println(radioInfo.snr);
#endif

// Forma de Triangulo
if (_forma == 1){  
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

