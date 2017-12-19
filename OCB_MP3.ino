//////////////////////
// Inicializa MP3   //
/////////////////////
bool iniciaMP3()
{
  mySoftwareSerial.begin(9600);                         	// Inicializa softwareSerial
  if (!mp3player.begin(mySoftwareSerial)) {             	// Usa softwareSerial para comunicacar com player
    return -1;
  }
  else {
    mp3player.reset();                                  	// Reset o modulo
    mp3player.setTimeOut(500);                          	// Define o time out (500ms) da comunicacao serial
    mp3player.volume(FIX_VOLUME);                       	// Define o volume inicial
    mp3player.EQ(DFPLAYER_EQ_CLASSIC);                  	// Define a equalizacao do som
    mp3player.outputDevice(DFPLAYER_DEVICE_SD);         	// Configura o tipo de device inicial a usar - SD eh default
    mp3player.outputSetting(true, 15);                  	// output setting, enable the output and set the gain to 15
    return 1;
  }
}

///////////////////
// Executa MP3   //
///////////////////
void executaMp3()
{

#ifdef DEBUG
  Serial.print("Inicializando MP3 Player... ");
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
  
  int                       lastPosVol         = -1;
  
  #define                   ROTARYSTEPS        1
  #define                   ROTARYMIN          0
  #define                   ROTARYMAX          30

  encoderVol.setPosition(mp3player.readVolume() / ROTARYSTEPS);

/*  
  //----Mp3 control----
  mp3player.sleep();     //sleep
  mp3player.enableDAC();  //Enable On-chip DAC
  mp3player.disableDAC();  //Disable On-chip DAC
  mp3player.outputSetting(true, 15); //output setting, enable the output and set the gain to 15
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

    static unsigned long timer = millis();
  
    monitor.setCursor(50,40);
    monitor.println(mp3player.readCurrentFileNumber());
    
    if (mp3player.readType() == DFPlayerPlayFinished) {
      mp3player.next();  //Play next mp3 every 3 second.
    }
    if (millis() - timer > 30000) {
      timer = millis();
      Serial.println("Tocando ... ");
      Serial.print("No. Arq Corrente "); Serial.println(mp3player.readCurrentFileNumber()); //read current play file number
      mp3player.next();  //Play next mp3 every 3 second.
    }
*/
  
#ifdef DEBUG  
  Serial.print("Estado do MP3      "); Serial.println(mp3player.readState()); 			// read mp3 state
  Serial.print("Volume             "); Serial.println(mp3player.readVolume()); 			// read current volume
  Serial.print("Equalizador        "); Serial.println(mp3player.readEQ()); 			// read EQ setting
  Serial.print("Qtde Arquivos      "); Serial.println(mp3player.readFileCounts()); 		// read all file counts in SD card
  Serial.print("No. Arq Corrente   "); Serial.println(mp3player.readCurrentFileNumber()); 	// read current play file number
  Serial.print("Qtde Arq Pasta MP3 "); Serial.println(mp3player.readFileCountsInFolder(0)); 	// read fill counts in folder SD:/03
#endif

  monitor.setTextColor(WHITE,BLACK);  
  monitor.setTextSize(2);
  
  while (!digitalRead(btnModoPin))
  {
    
    encoderVol.tick();                                          // Verifica o encoder do Volume
    int newPosVol = encoderVol.getPosition() * ROTARYSTEPS;     // captura a posicao fisica atual e calcula a posicao logica
    if (newPosVol < ROTARYMIN) {
      encoderVol.setPosition(ROTARYMIN / ROTARYSTEPS);
      newPosVol = ROTARYMIN;
    } else if (newPosVol > ROTARYMAX) {
      encoderVol.setPosition(ROTARYMAX / ROTARYSTEPS);
      newPosVol = ROTARYMAX;
    } 
    if (lastPosVol != newPosVol) {
#ifdef DEBUG
      Serial.print("Rotary Volume : ");
      Serial.println(newPosVol);
#endif    
      lastPosVol = newPosVol;
      mp3player.volume(lastPosVol);
      mostraVolumeMP3();    
    } 
  
    mostraEQMP3();                                              // Mostra a equalizacao do som
        
    if (mp3player.available()) {
      printDetail(mp3player.readType(), mp3player.read());      // Imprime a mensagem detalhada do DFPlayer para verificar com diferentes erros e/ou estados.
    }
    
  }
  mp3player.stop();    						// Reset the module
}
////////////////////////////////////////////////////////////////
void mostraEQMP3()
{
  String                    DESC_EQ[]         = {"Normal ", "Pop    ", "Rock   ", "Jazz   ", "Classic", "Bass   "};

#ifdef DEBUG
  Serial.print(" Equalizacao  : ");Serial.println(mp3player.readEQ());
#endif

  monitor.setTextColor(WHITE,BLACK);  
  monitor.setTextSize(1);

  imprimeTexto(DESC_EQ[mp3player.readEQ()],"D",60);
}
////////////////////////////////////////////////////////////////
void mostraVolumeMP3()
{
  int16_t vlinMax = monitor.height();
#ifdef DEBUG
  Serial.print(" Volume  : ");Serial.println(mp3player.readVolume());
#endif
  mostraTermometro("Vol", mp3player.readVolume(), 30, 28, 10, vlinMax-43, 20 );
}
//////////////////////////////////
// Apresenta detalhes do Player //
//////////////////////////////////
void printDetail(uint8_t type, int value){
  monitor.setTextColor(WHITE,BLACK);  
  monitor.setTextSize(2);
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      imprimeTexto("Tempo Esgotado!","C",80);
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      imprimeTexto("Erro de Empilhamento!","C",80);
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      imprimeTexto("Cartão Inserido!","C",80);
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      imprimeTexto("Cartao Removido!","C",80);
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      imprimeTexto("Cartao OnLine!","C",80);
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
          imprimeTexto("DFPlayer Erro: Cartao nao encontrado...","C",80);
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          imprimeTexto("DFPlayer Erro: Dormindo...","C",80);
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          imprimeTexto("DFPlayer Erro: Get Wrong Stack...","C",80);
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          imprimeTexto("DFPlayer Erro: Check Sum Not Match...","C",80);
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          imprimeTexto("DFPlayer Erro: Indice do Arquivo nao encontrado...","C",80);
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          imprimeTexto("DFPlayer Erro: Arquivo nao encontrado...","C",80);
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          imprimeTexto("DFPlayer Erro: Em Anuncio...","C",80);
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}
