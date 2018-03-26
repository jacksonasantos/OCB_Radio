/*
 * OCB MP3
 *
bool iniciaMP3        ()
void executaMp3       (uint8_t device)
void mostraEQMP3      ()
void mostraVolumeMP3  ()
void printDetail      (uint8_t type, int value, int p_posmsg)
*/
//////////////////////
// Inicializa MP3   //
/////////////////////
bool iniciaMP3()
{
  Serial1.begin(9600);
  if (!mp3player.begin(Serial1, false)) {                 // Usa softwareSerial para comunicacar com player - false para desabilitar ACK 
    return -1;
  }
  else {
    mp3player.setTimeOut(50);                          	// Define o time out (500ms) da comunicacao serial
    mp3player.outputSetting(false, 15);                 	// output setting, habilita saida das mensagens e configura o valor do "ganho"
    mp3player.enableDAC();                                // Enable On-chip DAC
    mp3player.volume(30);                         // Define o volume inicial
    mp3player.EQ(DFPLAYER_EQ_NORMAL);                     // Define a equalizacao do som
#ifdef DEBUG_DTL  
    Serial.println(" ");
    Serial.print  ("    Estado             "); Serial.println(mp3player.readState());                     // read mp3 state
    Serial.print  ("    Volume             "); Serial.println(mp3player.readVolume());                    // read current volume
    Serial.print  ("    Equalizador        "); Serial.println(mp3player.readEQ());                        // read EQ setting
#endif
  return 1;
  }
}

///////////////////
// Executa MP3   //
///////////////////
void executaMp3(uint8_t device)
{
  mp3player.outputDevice(device);                       // Configura o tipo de device a usar
  
  unsigned long             _now               = millis();
  
  int                       _lastPosVol        = -1;
  int                       _vEQ               = mp3player.readEQ();
  
  #define                   ROTARYvolSTEPS     1
  #define                   ROTARYvolMIN       0
  #define                   ROTARYvolMAX       30
  
  encoderVolMP3.setPosition(vg_Volume / ROTARYvolSTEPS);

/*  
  //----Mp3 control----
  mp3player.sleep();     //sleep
  mp3player.volumeUp(); //Volume Up
  mp3player.volumeDown(); //Volume Down  
  
  //----Mp3 play----
  mp3player.play(1);  //Play the first mp3
  mp3player.loop(1);  //Loop the first mp3
  mp3player.pause();  //pause the mp3
  mp3player.start();  //start the mp3 from the pause
  mp3player.playFolder(15, 4);  //play specific mp3 in SD:/15/004.mp3; Folder Name(1~99); File Name(1~255)
 // mp3player.enableLoopAll(); //loop all mp3 files.
  mp3player.disableLoopAll(); //stop loop all mp3 files.
  mp3player.playMp3Folder(4); //play specific mp3 in SD:/MP3/0004.mp3; File Name(0~65535)
  mp3player.advertise(3); //advertise specific mp3 in SD:/ADVERT/0003.mp3; File Name(0~65535)
  mp3player.stopAdvertise(); //stop advertise
  mp3player.playLargeFolder(2, 999); //play specific mp3 in SD:/02/004.mp3; Folder Name(1~10); File Name(1~1000)
  mp3player.loopFolder(5); //loop all mp3 files in folder SD:/05.
  mp3player.randomAll(); //Random play all the mp3.
  mp3player.enableLoop(); //enable loop.
  mp3player.disableLoop(); //disable loop.
*/
  monitor.setTextColor(WHITE,BLACK);  
  monitor.setTextSize(2);

mp3player.enableLoopAll();

  while ( !digitalRead(btnModoPin) )
  {
    if (digitalRead(btnSetupPin)){                                    // Executa o modo Setup
      executaSetup(); 
      return;
    }                         
    
    if (( mp3player.readState() == 256 ) || 
        ( mp3player.readState() == 512 )){                            // Verifia o Estado igual a parado e inicia automaticamento
      mp3player.start();
    }
    
    if (mp3player.readType() == DFPlayerPlayFinished) {               // Passa automaticamente para proxima musica
      mp3player.next();  
    }  
    imprimeTexto( String(mp3player.readState(),DEC),"C", 150);
    imprimeTexto( String(mp3player.readType(),DEC),"C", 180);
    
    _now = millis();

    encoderVolMP3.tick();                                             // Verifica o encoder do Volume
    
    int newPosVol = encoderVolMP3.getPosition() * ROTARYvolSTEPS;     // captura a posicao fisica atual e calcula a posicao logica

    if (newPosVol < ROTARYvolMIN) {
      encoderVolMP3.setPosition(ROTARYvolMIN / ROTARYvolSTEPS);
      newPosVol = ROTARYvolMIN;
    } else if (newPosVol > ROTARYvolMAX) {
      encoderVolMP3.setPosition(ROTARYvolMAX / ROTARYvolSTEPS);
      newPosVol = ROTARYvolMAX;
    } 
    if (_lastPosVol != newPosVol) {
#ifdef DEBUG
    Serial.print("Rotary Volume : ");Serial.println(newPosVol);
#endif    
      _lastPosVol = newPosVol;
      vg_Volume = newPosVol;
//      mp3player.volume(_lastPosVol);
      audioProcessor.setVolume(map(vg_Volume, MIN_VOLUME_ENC, MAX_VOLUME_ENC, MIN_VOLUME_PT2314, MAX_VOLUME_PT2314));
      delay(500);
      mostraVolumeMP3();    
      settings.currentVolume = vg_Volume;
      saveSettings();  
  } 

    btnMuteState    = digitalRead(btnMutePin);
    btnPrevState    = digitalRead(btnPrevPin);
    btnNextState    = digitalRead(btnNextPin);
    btnEQState      = digitalRead(btnEQPin);
    
    if (btnMuteState == LOW) {                              // Faz a Leitura do Botão MUTE        
      if ( mp3player.readState() == 513 || mp3player.readState() == 257 ) {                 // Estado 513 - Tocando Musica
        mp3player.pause();
        monitor.setTextColor(YELLOW,BLACK);  
        monitor.setTextSize(2);
        imprimeTexto("PAUSE","C",50);
        monitor.setTextColor(WHITE,BLACK);  
        delay(5);
#ifdef DEBUG
    Serial.println("PAUSE");
#endif
      }
      else {
        mp3player.start();
        monitor.setTextColor(WHITE,BLACK);  
        monitor.setTextSize(2);
        imprimeTexto("     ","C",50);
        delay(5);          
#ifdef DEBUG
    Serial.println("START");
#endif
      }
    }
    else if (btnPrevState == HIGH){
      mp3player.previous();
      delay(300);
    }
    else if (btnNextState == HIGH){
      mp3player.next();
      delay(300);
    }
    else if (btnEQState == HIGH){
      _vEQ++;
      if (_vEQ == 5) {_vEQ = 0;}
#ifdef DEBUG
  Serial.print("Equalização: ");
  Serial.print(_vEQ);
#endif
      mp3player.EQ(_vEQ);
      delay(300);
#ifdef DEBUG
  Serial.print(" - ");
  Serial.println(mp3player.readEQ());
#endif
    }

    monitor.setTextColor(WHITE,BLACK);  
    monitor.setTextSize(2);
    String _qtdeArquivo = String(mp3player.readFileCounts(device));
    String _currArquivo = String(mp3player.readCurrentFileNumber(device));
    imprimeTexto( _qtdeArquivo, "C", 70);
    imprimeTexto( _currArquivo, "C", 90);
    
    mostraEQMP3();                                                  // Mostra a equalizacao do som
   
    if (mp3player.available()) {
      mostra_relogio();
      printDetail(mp3player.readType(), mp3player.read(),monitor.height()-20);            // Imprime a mensagem detalhada do DFPlayer para verificar com diferentes erros e/ou estados.
    }
    
  }
  mp3player.stop();    						                                    // Desliga o modulo
}
////////////////////////////////////////////////////////////////
void mostraEQMP3()
{
  String       DESC_EQ[]         = {"Normal ", "Pop    ", "Rock   ", "Jazz   ", "Classic", "Bass   "};
  monitor.setTextColor(WHITE,BLACK);  
  monitor.setTextSize(1);
  imprimeTexto(DESC_EQ[mp3player.readEQ()],"D",60);
  monitor.setTextSize(2);
}
////////////////////////////////////////////////////////////////
void mostraVolumeMP3()
{
  int16_t _linMax = monitor.height();
#ifdef DEBUG_DTL
    Serial.print(" Volume  : ");Serial.println(vg_Volume);
#endif
  mostraTermometro("Vol", vg_Volume, ROTARYvolMAX, (ROTARYvolMAX-2), 10, _linMax-43, 20 );
}
//////////////////////////////////
// Apresenta detalhes do Player //
//////////////////////////////////
void printDetail(uint8_t type, int value, int p_posmsg )
{
  monitor.setTextColor(WHITE,BLUE);  
  monitor.setTextSize(1);
#ifdef DEBUG
    Serial.print(F("Detalhe Tipo: "));
    Serial.print(type,DEC);
    Serial.print(F("  Valor: "));
    Serial.println(value,DEC);
#endif
  switch (type) {
    case TimeOut:
      imprimeTexto("Tempo Esgotado!","C",p_posmsg);
      break;
    case WrongStack:
      imprimeTexto("Erro de Empilhamento!","C",p_posmsg);
      break;
    case DFPlayerCardInserted:
      imprimeTexto("Cartão Inserido!","C",p_posmsg);
      break;
    case DFPlayerCardRemoved:
      imprimeTexto("Cartao Removido!","C",p_posmsg);
      break;
    case DFPlayerCardOnline:
      imprimeTexto("Cartao OnLine!","C",p_posmsg);
      break;
    case DFPlayerPlayFinished:
      break;
    case DFPlayerError:
      switch (value) {
        case Busy:
          imprimeTexto("Erro: Cartao nao encontrado...","C",p_posmsg);
          break;
        case Sleeping:
          imprimeTexto("Erro: Dormindo...","C",p_posmsg);
          break;
        case SerialWrongStack:
          imprimeTexto("Erro: Get Wrong Stack...","C",p_posmsg);
          break;
        case CheckSumNotMatch:
          imprimeTexto("Erro: Check Sum Not Match...","C",p_posmsg);
          break;
        case FileIndexOut:
          imprimeTexto("Erro: Indice do Arquivo nao encontrado...","C",p_posmsg);
          break;
        case FileMismatch:
          imprimeTexto("Erro: Arquivo nao encontrado...","C",p_posmsg);
          break;
        case Advertise:
          imprimeTexto("Erro: Em Anuncio...","C",p_posmsg);
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}
