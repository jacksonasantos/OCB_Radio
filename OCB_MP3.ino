//////////////////////
// Inicializa MP3   //
/////////////////////
bool iniciaMP3(uint8_t device)
{
  Serial1.begin(9600);
  if (!mp3player.begin(Serial1)) {             	// Usa softwareSerial para comunicacar com player
    return -1;
  }
  else {
    mp3player.reset();                                  	// Reset o modulo
    mp3player.setTimeOut(500);                          	// Define o time out (500ms) da comunicacao serial
    mp3player.enableDAC();                                // Enable On-chip DAC
    mp3player.EQ(DFPLAYER_EQ_NORMAL);                   	// Define a equalizacao do som
    mp3player.outputDevice(device);                       // Configura o tipo de device a usar
    mp3player.outputSetting(true, 15);                  	// output setting, enable the output and set the gain to 15
    mp3player.volume(FIX_VOLUME);                         // Define o volume inicial
    delay(300);
#ifdef DEBUG  
  Serial.println(" ");
  Serial.print  ("    Device             "); Serial.println(device);                                    // dispositivo 1 - USB 2 -SD
  Serial.print  ("    Estado             "); Serial.println(mp3player.readState());                     // read mp3 state
  Serial.print  ("    Volume             "); Serial.println(mp3player.readVolume());                    // read current volume
  Serial.print  ("    Equalizador        "); Serial.println(mp3player.readEQ());                        // read EQ setting
  Serial.print  ("    Qtde Arquivos      "); Serial.println(mp3player.readFileCounts(device));          // read all file counts in SD card
  Serial.print  ("    No. Arq Corrente   "); Serial.println(mp3player.readCurrentFileNumber(device));   // read current play file number
//  Serial.print  ("    Qtde Arq Pasta MP3 "); Serial.println(mp3player.readFileCountsInFolder(device));  // read fill counts in folder SD:/03
#endif

  return 1;
  }
}

///////////////////
// Executa MP3   //
///////////////////
void executaMp3(uint8_t device)
{

#ifdef DEBUG
  Serial.print("Inicializando MP3 Player... ");
  Serial.print(device);
#endif  
  if (!iniciaMP3(device)){
#ifdef DEBUG
     Serial.println(" Falhou!");
#endif  
     return;
  }
  else {
#ifdef DEBUG
      Serial.println(" OK!");
#endif  
  }
  
  int                       _lastPosVol        = -1;
  
  #define                   ROTARYvolSTEPS     1
  #define                   ROTARYvolMIN       0
  #define                   ROTARYvolMAX       30
  
  encoderVolMP3.setPosition(FIX_VOLUME / ROTARYvolSTEPS);

/*  
  //----Mp3 control----
  mp3player.sleep();     //sleep
  mp3player.volumeUp(); //Volume Up
  mp3player.volumeDown(); //Volume Down  
  
  //----Mp3 play----
  mp3player.next();  //Play next mp3
  mp3player.previous();  //Play previous mp3
  mp3player.play(1);  //Play the first mp3
  mp3player.loop(1);  //Loop the first mp3
  mp3player.pause();  //pause the mp3
  mp3player.start();  //start the mp3 from the pause
  mp3player.playFolder(15, 4);  //play specific mp3 in SD:/15/004.mp3; Folder Name(1~99); File Name(1~255)
  mp3player.enableLoopAll(); //loop all mp3 files.
  mp3player.disableLoopAll(); //stop loop all mp3 files.
  mp3player.playMp3Folder(4); //play specific mp3 in SD:/MP3/0004.mp3; File Name(0~65535)
  mp3player.advertise(3); //advertise specific mp3 in SD:/ADVERT/0003.mp3; File Name(0~65535)
  mp3player.stopAdvertise(); //stop advertise
  mp3player.playLargeFolder(2, 999); //play specific mp3 in SD:/02/004.mp3; Folder Name(1~10); File Name(1~1000)
  mp3player.loopFolder(5); //loop all mp3 files in folder SD:/05.
  mp3player.randomAll(); //Random play all the mp3.
  mp3player.enableLoop(); //enable loop.
  mp3player.disableLoop(); //disable loop.
  
    if (mp3player.readType() == DFPlayerPlayFinished) {
      mp3player.next();  //Play next mp3 every 3 second.

*/
  monitor.setTextColor(WHITE,BLACK);  
  monitor.setTextSize(2);

  while ( !digitalRead(btnModoPin) )
  {
   // boolean play_state = digitalRead(vg_PlayPin);                  // connect Pin3 to BUSY pin of player 
   // if (play_state == HIGH ){
   //   mp3player.play();
   // }
    
    encoderVolMP3.tick();                                          // Verifica o encoder do Volume
    
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
      mp3player.volume(_lastPosVol);
      mostraVolumeMP3();    
  } 

    btnMuteState = digitalRead(btnMutePin);
    btnPrevState = digitalRead(btnPrevPin);
    btnNextState = digitalRead(btnNextPin);
    
//    imprimeTexto( String(mp3player.readState()),"C", 150);
    
    if (btnMuteState == LOW) {                              // Faz a Leitura do Botão MUTE        
      if ( mp3player.readState() == 513 ) {                 // Estado 513 - Tocando Musica
        mp3player.pause();
        monitor.setTextColor(YELLOW,BLACK);  
        monitor.setTextSize(2);
        imprimeTexto("PAUSE","C",50);
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
    
    monitor.setTextColor(WHITE,BLACK);  
    monitor.setTextSize(2);
    imprimeTexto( String(mp3player.readFileCounts()),"C", 70);
    imprimeTexto( String(mp3player.readCurrentFileNumber()),"C", 90);
    
    mostraEQMP3();                                                  // Mostra a equalizacao do som
        
    if (mp3player.available()) {
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
#ifdef DEBUG
  Serial.print(" Volume  : ");Serial.println(mp3player.readVolume()/2);
#endif
  mostraTermometro("Vol", mp3player.readVolume()/2, 30, 28, 10, _linMax-43, 20 );
}
//////////////////////////////////
// Apresenta detalhes do Player //
//////////////////////////////////
void printDetail(uint8_t type, int value, int vposmsg ){
  
  monitor.setTextColor(WHITE,BLUE);  
  monitor.setTextSize(1);
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      imprimeTexto("Tempo Esgotado!","C",vposmsg);
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      imprimeTexto("Erro de Empilhamento!","C",vposmsg);
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      imprimeTexto("Cartão Inserido!","C",vposmsg);
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      imprimeTexto("Cartao Removido!","C",vposmsg);
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      imprimeTexto("Cartao OnLine!","C",vposmsg);
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayer Erro: "));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          imprimeTexto("Erro: Cartao nao encontrado...","C",vposmsg);
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          imprimeTexto("Erro: Dormindo...","C",vposmsg);
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          imprimeTexto("Erro: Get Wrong Stack...","C",vposmsg);
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          imprimeTexto("Erro: Check Sum Not Match...","C",vposmsg);
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          imprimeTexto("Erro: Indice do Arquivo nao encontrado...","C",vposmsg);
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          imprimeTexto("Erro: Arquivo nao encontrado...","C",vposmsg);
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          imprimeTexto("Erro: Em Anuncio...","C",vposmsg);
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}
