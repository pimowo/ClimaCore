/********************************************************************************************
/  AQma Computer - version 1.1.2
/  Copyright (c) 2015 Marcin Grunt
/  Wszelkie prawa do AQma Computer są zastrzeżone przez autora: Marcina Grunta. 
/  Poniższy kod programu można używać i rozpowszechniać za darmo, pod warunkiem 
/  zachowania informacji o licencji i autorstwie. 
/  Poniższy kod udostępniony jest bez żadnej gwarancji, używasz go na własne ryzyko.
/  Autor nie ponosi odpowiedzialności za szkody, utratę zysków, lub jakiekolwiek
/  inne straty wynikłe w konsekwencji uzywania, lub niemożności użycia poniższego kodu.
/  
/  Pozdrawiam i życzę bezproblemowej pracy:

/                           Marcin Grunt, magu@magu.pl
*********************************************************************************************/


#include <Wire.h>
#include <Time.h>
#include <EEPROM.h>
#include <DS1307RTC.h>


#define btnT1Pin 21
#define btnT2Pin 20
#define btnT3Pin 19
#define buzzerPin 18
#define ledPin 13

/* Przekazniki sterowane stanem niskim
    ||
    ||
   \  /
    \/
*/
boolean ON=false, OFF=true;

////////////////////////////////////
//// Odworcenie dzialania PWMow  ///
boolean pwm1Invert = false;      ///
boolean pwm2Invert = false;      ///
boolean pwm3Invert = false;      ///
////////////////////////////////////

/////////// PWM Soft start ////////////////
int  pwmSilkySmootTimeSec = 30;         //
//////////////////////////////////////////


tmElements_t tm;
long unsigned currentTimeSec;

int currentDayOfWeek;

// T1
 byte t1Pin, t1Status, t1HOn, t1MOn, t1SOn, t1HOff, t1MOff, t1SOff; 
// T2
 byte t2Pin, t2Status, t2HOn, t2MOn, t2SOn, t2HOff, t2MOff, t2SOff;
// T3 
 byte t3Pin, t3Status, t3HOn, t3MOn, t3SOn, t3HOff, t3MOff, t3SOff;

// T4 
 byte t4Status, t4HOn, t4MOn, t4HOff, t4MOff;
// T5 
 byte t5Status, t5HOn, t5MOn, t5HOff, t5MOff;
// T6
 byte t6Status, t6HOn, t6MOn, t6HOff, t6MOff;

// D1
 byte d1Status, d1Flow, d1FlowDec, d1Pin; 
 byte d1Su ,d1Mo, d1Tu, d1We, d1Th, d1Fr, d1Sa, d1Mode;
        
 byte d11HOn, d11MOn, d11Ml, d11MlDec;
 byte d12HOn, d12MOn, d12Ml, d12MlDec;
 byte d13HOn, d13MOn, d13Ml, d13MlDec;
 byte d14HOn, d14MOn, d14Ml, d14MlDec;

 // D2
 byte d2Status, d2Flow, d2FlowDec, d2Pin; 
 byte d2Su ,d2Mo, d2Tu, d2We, d2Th, d2Fr, d2Sa, d2Mode;
        
 byte d21HOn, d21MOn, d21Ml, d21MlDec;
 byte d22HOn, d22MOn, d22Ml, d22MlDec;
 byte d23HOn, d23MOn, d23Ml, d23MlDec;
 byte d24HOn, d24MOn, d24Ml, d24MlDec;

// D3
 byte d3Status, d3Flow, d3FlowDec, d3Pin; 
 byte d3Su ,d3Mo, d3Tu, d3We, d3Th, d3Fr, d3Sa, d3Mode;
        
 byte d31HOn, d31MOn, d31Ml, d31MlDec;
 byte d32HOn, d32MOn, d32Ml, d32MlDec;
 byte d33HOn, d33MOn, d33Ml, d33MlDec;
 byte d34HOn, d34MOn, d34Ml, d34MlDec;

// PWM - 1
 byte pwm1Pin, pwm1Status, pwm1HOn, pwm1MOn, pwm1SOn;
 byte pwm1HOff, pwm1MOff, pwm1SOff, pwm1Min, pwm1Max;
 byte pwm1Sr, pwm1Ss, pwm1KeepLight;

// PWM - 2
 byte pwm2Pin, pwm2Status, pwm2HOn, pwm2MOn, pwm2SOn;
 byte pwm2HOff, pwm2MOff, pwm2SOff, pwm2Min, pwm2Max;
 byte pwm2Sr, pwm2Ss, pwm2KeepLight;

// PWM - 3
 byte pwm3Pin, pwm3Status, pwm3HOn, pwm3MOn, pwm3SOn;
 byte pwm3HOff, pwm3MOff, pwm3SOff, pwm3Min, pwm3Max;
 byte pwm3Sr, pwm3Ss, pwm3KeepLight;

// Feeding 
 byte feedingPin, feedingBtnPin, feedingTime, feedingMode;

// Delay 
 boolean delayExecute = false; 

// Refill
 byte refillPin, refillBtnPin, refillDelayStart;
 
// Manual mode
 boolean t1ManualModeOn = false, t1ManualModeStatus =  false, t2ManualModeOn = false, t2ManualModeStatus =  false, t3ManualModeOn = false, t3ManualModeStatus =  false;

 boolean justTurnedOn = true;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////// SETUP //////////////////////////////////////////////////////////////////////

void setup()
{
Serial.begin(9600); 

if (EEPROM.read(0) != 1 && EEPROM.read(1) != 0 && EEPROM.read(2) != 2) { EEPROM.write(0,1);EEPROM.write(1,0);EEPROM.write(2,2); }  
  
  eEpromRead();
  
  pinMode(0,OUTPUT);     pinMode(1,OUTPUT);     pinMode(4, OUTPUT);   pinMode(5, OUTPUT);   
  pinMode(6, OUTPUT);    pinMode(7, OUTPUT);    pinMode(8, OUTPUT);   pinMode(9, OUTPUT);   
  pinMode(10, OUTPUT);   pinMode(11, OUTPUT);   pinMode(12, OUTPUT);  pinMode(13, OUTPUT);  
  pinMode(A0, OUTPUT);   
  
  pinMode(A1, INPUT_PULLUP);   pinMode(A2, INPUT_PULLUP);  
  pinMode(A3, INPUT_PULLUP);   pinMode(A4, INPUT_PULLUP);   
  pinMode(A5, INPUT_PULLUP);
  
// Ustaw stany poczatkowe 
  digitalWrite(0,OFF);    digitalWrite(1,OFF);    digitalWrite(4,OFF);  
  digitalWrite(5,OFF);    digitalWrite(6,OFF);    digitalWrite(7,OFF);  
  digitalWrite(8,OFF);    digitalWrite(9,LOW);     digitalWrite(10,LOW); 
  digitalWrite(11,LOW);    digitalWrite(13,HIGH);
  
  if (feedingMode == 0) { digitalWrite(feedingPin,ON); } else { digitalWrite(feedingPin,OFF); }  

// Pobierz czas
    RTC.read(tm);
    
} 
////////////////////////////////////////////////////////////// END SETUP //////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// *************************************************************************************************************************************** ///
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////// LOOP ///////////////////////////////////////////////////////////////////////

void loop()
{  
   long unsigned currentMillis= millis();
   char cmdOutputArray[64];
   char cmdChar = 0;
   String cmd = "";

   if (RTC.read(tm)) 
   {
   currentTimeSec =  (long(tm.Hour)*3600) + (long(tm.Minute)*60) + long(tm.Second);
   currentDayOfWeek = tm.Wday;
   }
    
// Serial

    while (Serial.available() > 0)
     {
       cmdChar = Serial.read();
      // Odkomentuj delay, jezeli uzywasz MEGA 2560, UNO, NANO!!!!!!!!!!!!!!!!!!!!!!!!!!
      // delay(4);
       cmd.concat(cmdChar);
     }
     
   
     if (cmd != "") {
         cmd.toCharArray(cmdOutputArray,64);

           if (commandAnalysis(cmdOutputArray)) 
           { 
              eEpromRead();
           } 
         
           else 
           { 
             Serial.print("666,Bledne dane\n"); 
           }
          
     }
     cmd = "";
      
     
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////     
//////////////////////////////////////// SILNIK //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

          keyboard(currentMillis);
          silkySmoothPowerOn(pwmSilkySmootTimeSec,currentMillis);
          
          if ( !t1ManualModeOn && !t2ManualModeOn && !t3ManualModeOn ) { digitalWrite(ledPin, HIGH); }
          
          if (currentMillis%22 == 0 && !t1ManualModeOn) { timer(t1Pin,t1HOn,t1MOn,t1SOn,t1HOff,t1MOff,t1SOff,t1Status); } // T1
          if (currentMillis%22 == 0 && t1ManualModeOn)  { digitalWrite(t1Pin, t1ManualModeStatus); manualModeBlink(ledPin,currentMillis);}
         
          if (currentMillis%32 == 0 && !t2ManualModeOn) { timer(t2Pin,t2HOn,t2MOn,t2SOn,t2HOff,t2MOff,t2SOff,t2Status); } // T2
          if (currentMillis%32 == 0 && t2ManualModeOn)  { digitalWrite(t2Pin, t2ManualModeStatus); manualModeBlink(ledPin,currentMillis);}
        
          if (currentMillis%42 == 0 && !t3ManualModeOn) { timer(t3Pin,t3HOn,t3MOn,t3SOn,t3HOff,t3MOff,t3SOff,t3Status); } // T3
          if (currentMillis%42 == 0 && t3ManualModeOn)  { digitalWrite(t3Pin, t3ManualModeStatus); manualModeBlink(ledPin,currentMillis);}

  
          // Pompa 1 
          if ( d1Mode == 1 && currentMillis%143 == 0 )
          {
            if (dayWhenPumpIsOn(1)) 
            { 
              doser(d1Pin, d1Flow, d1FlowDec, d1Status, d11HOn, d11MOn, d11Ml, d11MlDec, d12HOn, d12MOn, d12Ml, d12MlDec, d13HOn, d13MOn, d13Ml, d13MlDec, d14HOn, d14MOn, d14Ml, d14MlDec ); 
            }
          }
    
          // Pompa 2 
          if ( d2Mode == 1 && currentMillis%153 == 0) 
          { 
            if (dayWhenPumpIsOn(2)) 
            {
               doser(d2Pin, d2Flow, d2FlowDec, d2Status, d21HOn, d21MOn, d21Ml, d21MlDec, d22HOn, d22MOn, d22Ml, d22MlDec, d23HOn, d23MOn, d23Ml, d23MlDec, d24HOn, d24MOn, d24Ml, d24MlDec ); 
            }
          }
          
          // Pompa 3 
          if ( d3Mode == 1 && currentMillis%163 == 0) 
          { 
            if (dayWhenPumpIsOn(3)) 
            {
               doser(d3Pin, d3Flow, d3FlowDec, d3Status, d31HOn, d31MOn, d31Ml, d31MlDec, d32HOn, d32MOn, d32Ml, d32MlDec, d33HOn, d33MOn, d33Ml, d33MlDec, d34HOn, d34MOn, d34Ml, d34MlDec ); 
            }
          }
          
          
          // Jak nie Pompa Dozujaca to Timer
          if (currentMillis%303 == 0 && d1Mode == 0) { timer(d1Pin,t4HOn,t4MOn,0,t4HOff,t4MOff,0,t4Status);  }
          if (currentMillis%313 == 0 && d2Mode == 0) { timer(d2Pin,t5HOn,t5MOn,0,t5HOff,t5MOff,0,t5Status);  }
          if (currentMillis%323 == 0 && d3Mode == 0) { timer(d3Pin,t6HOn,t6MOn,0,t6HOff,t6MOff,0,t6Status);  }          
         
          // PWM 1 - 3
          if (currentMillis%404 == 0 ) {pwm(pwm1Pin, pwm1Status, pwm1HOn, pwm1MOn, pwm1SOn, pwm1HOff, pwm1MOff, pwm1SOff, pwm1Min, pwm1Max, pwm1Sr, pwm1Ss, pwm1KeepLight, pwm1Invert); }
          if (currentMillis%414 == 0 ) {pwm(pwm2Pin, pwm2Status, pwm2HOn, pwm2MOn, pwm2SOn, pwm2HOff, pwm2MOff, pwm2SOff, pwm2Min, pwm2Max, pwm2Sr, pwm2Ss, pwm2KeepLight, pwm2Invert); }
          if (currentMillis%424 == 0 ) {pwm(pwm3Pin, pwm3Status, pwm3HOn, pwm3MOn, pwm3SOn, pwm3HOff, pwm3MOff, pwm3SOff, pwm3Min, pwm3Max, pwm3Sr, pwm3Ss, pwm3KeepLight, pwm3Invert); } 
           
          
          // Feeding
          if ( feedingMode != 0 ) { feeding(feedingPin, feedingBtnPin, feedingTime, currentMillis); }
          
          
          // Delay On
          if (feedingMode == 0  && currentMillis % 1981 == 0) 
          { 
            if (currentMillis / 1000  < feedingTime * 60 && !delayExecute)
           {
             digitalWrite(feedingPin, ON);
           } 
            else 
           {
             digitalWrite(feedingPin, OFF);
             delayExecute = true;
           }
          }
          
          // Refill 
          refill(refillPin, refillBtnPin, refillDelayStart, currentMillis);
          
         

} 

////////////////////////////////////////////////////////////// END LOOP////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////////////////
/////                       eEpromRead                                ///// 
///////////////////////////////////////////////////////////////////////////

void eEpromRead() {
           
             // T1 
                t1Pin = EEPROM.read(11); t1Status = EEPROM.read(13);
                t1HOn = EEPROM.read(14);t1MOn = EEPROM.read(15); t1SOn =EEPROM.read(16);
                t1HOff = EEPROM.read(17);t1MOff = EEPROM.read(18);t1SOff = EEPROM.read(19);
                 
             // T2
             	t2Pin = EEPROM.read(31); t2Status = EEPROM.read(33);
                t2HOn = EEPROM.read(34);t2MOn = EEPROM.read(35);t2SOn = EEPROM.read(36);
                t2HOff = EEPROM.read(37);t2MOff = EEPROM.read(38);t2SOff = EEPROM.read(39);

             // T3
                t3Pin = EEPROM.read(51); t3Status = EEPROM.read(53);
                t3HOn = EEPROM.read(54);t3MOn = EEPROM.read(55); t3SOn= EEPROM.read(56);
                t3HOff = EEPROM.read(57);t3MOff = EEPROM.read(58);t3SOff = EEPROM.read(59);
  
             // T4 
                t4Status = EEPROM.read(251);
                t4HOn = EEPROM.read(252);t4MOn = EEPROM.read(253); 
                t4HOff = EEPROM.read(254);t4MOff = EEPROM.read(255);

             // T5 
                t5Status = EEPROM.read(261);
                t5HOn = EEPROM.read(262);t5MOn = EEPROM.read(263); 
                t5HOff = EEPROM.read(264);t5MOff = EEPROM.read(265);

             // T6 
                t6Status = EEPROM.read(271);
                t6HOn = EEPROM.read(272);t6MOn = EEPROM.read(273); 
                t6HOff = EEPROM.read(274);t6MOff = EEPROM.read(275);
                
            // D1
              	d1Status = EEPROM.read(71); d1Flow = EEPROM.read(72);
        	d1FlowDec = EEPROM.read(73); d1Pin = EEPROM.read(74);
                d1Su = EEPROM.read(75);d1Mo = EEPROM.read(76);d1Tu = EEPROM.read(77);
                d1We = EEPROM.read(78);d1Th = EEPROM.read(79);d1Fr = EEPROM.read(80);
                d1Sa = EEPROM.read(81);d1Mode = EEPROM.read(82);
        
	        d11HOn = EEPROM.read(91); d11MOn = EEPROM.read(92);
                d11Ml = EEPROM.read(93); d11MlDec =EEPROM.read(94);
	        
                d12HOn = EEPROM.read(101); d12MOn = EEPROM.read(102);
                d12Ml = EEPROM.read(103); d12MlDec = EEPROM.read(104);
        	
                d13HOn = EEPROM.read(111); d13MOn =EEPROM.read(112);
                d13Ml = EEPROM.read(113); d13MlDec = EEPROM.read(114);
          	
                d14HOn = EEPROM.read(121); d14MOn = EEPROM.read(122);
                d14Ml =EEPROM.read(123); d14MlDec = EEPROM.read(124);
                
             // D2
              	d2Status = EEPROM.read(131); d2Flow = EEPROM.read(132);
        	d2FlowDec = EEPROM.read(133); d2Pin = EEPROM.read(134);
                d2Su = EEPROM.read(135);d2Mo = EEPROM.read(136);d2Tu = EEPROM.read(137);
                d2We = EEPROM.read(138);d2Th = EEPROM.read(139);d2Fr = EEPROM.read(140);
                d2Sa = EEPROM.read(141);d2Mode = EEPROM.read(142);
                
	        d21HOn = EEPROM.read(151); d21MOn = EEPROM.read(152);
                d21Ml = EEPROM.read(153); d21MlDec =EEPROM.read(154);
	        
                d22HOn = EEPROM.read(161); d22MOn = EEPROM.read(162);
                d22Ml = EEPROM.read(163); d22MlDec = EEPROM.read(164);
        	
                d23HOn = EEPROM.read(171); d23MOn =EEPROM.read(172);
                d23Ml = EEPROM.read(173); d23MlDec = EEPROM.read(174);
          	
                d24HOn = EEPROM.read(181); d24MOn = EEPROM.read(182);
                d24Ml =EEPROM.read(183); d24MlDec = EEPROM.read(184);
                
             // D3
              	d3Status = EEPROM.read(191); d3Flow = EEPROM.read(192);
        	d3FlowDec = EEPROM.read(193); d3Pin = EEPROM.read(194);
                d3Su = EEPROM.read(195);d3Mo = EEPROM.read(196);d3Tu = EEPROM.read(197);
                d3We = EEPROM.read(198);d3Th = EEPROM.read(199);d3Fr = EEPROM.read(200);
                d3Sa = EEPROM.read(201);d3Mode = EEPROM.read(202);
        
	        d31HOn = EEPROM.read(211); d31MOn = EEPROM.read(212);
                d31Ml = EEPROM.read(213); d31MlDec =EEPROM.read(214);
	        
                d32HOn = EEPROM.read(221); d32MOn = EEPROM.read(222);
                d32Ml = EEPROM.read(223); d32MlDec = EEPROM.read(224);
        	
                d33HOn = EEPROM.read(231); d33MOn =EEPROM.read(232);
                d33Ml = EEPROM.read(233); d33MlDec = EEPROM.read(234);
          	
                d34HOn = EEPROM.read(241); d34MOn = EEPROM.read(242);
                d34Ml =EEPROM.read(243); d34MlDec = EEPROM.read(244);
                
             // PWM - 1
                pwm1Pin = EEPROM.read(281); pwm1Status = EEPROM.read(282); 
                pwm1HOn = EEPROM.read(283); pwm1MOn = EEPROM.read(284);
                pwm1SOn = EEPROM.read(285); pwm1HOff = EEPROM.read(286); 
	        pwm1MOff = EEPROM.read(287); pwm1SOff = EEPROM.read(288);
                pwm1Min = EEPROM.read(289); pwm1Max = EEPROM.read(290); 
	        pwm1Sr = EEPROM.read(291); pwm1Ss = EEPROM.read(292);      
                pwm1KeepLight = EEPROM.read(293);

              // PWM - 2
                pwm2Pin = EEPROM.read(301); pwm2Status = EEPROM.read(302); 
                pwm2HOn = EEPROM.read(303); pwm2MOn = EEPROM.read(304);
                pwm2SOn = EEPROM.read(305); pwm2HOff = EEPROM.read(306); 
	        pwm2MOff = EEPROM.read(307); pwm2SOff = EEPROM.read(308);
                pwm2Min = EEPROM.read(309); pwm2Max = EEPROM.read(310); 
	        pwm2Sr = EEPROM.read(311); pwm2Ss = EEPROM.read(312);      
                pwm2KeepLight = EEPROM.read(313);
                
              // PWM - 3
                pwm3Pin = EEPROM.read(331); pwm3Status = EEPROM.read(332); 
                pwm3HOn = EEPROM.read(333); pwm3MOn = EEPROM.read(334);
                pwm3SOn = EEPROM.read(335); pwm3HOff = EEPROM.read(336); 
	        pwm3MOff = EEPROM.read(337); pwm3SOff = EEPROM.read(338);
                pwm3Min = EEPROM.read(339); pwm3Max = EEPROM.read(340); 
	        pwm3Sr = EEPROM.read(341); pwm3Ss = EEPROM.read(342);      
                pwm3KeepLight = EEPROM.read(343);
              
              // Feeding 
                feedingPin = EEPROM.read(351); feedingBtnPin = EEPROM.read(352); 
                feedingTime = EEPROM.read(353); feedingMode = EEPROM.read(354);
      
              // Refill
                refillPin = EEPROM.read(361); refillBtnPin =EEPROM.read(362); 
                refillDelayStart = EEPROM.read(363);
            
}


///////////////////////////////////////////////////////////////////////////////////////
/////                      TIMER Function                                         /////
///////////////////////////////////////////////////////////////////////////////////////

boolean timer( byte tPin, byte hOn, byte mOn, byte sOn, byte hOff, byte mOff, byte sOff, byte tStatus) //, long currentTimeSec)
{
  if  (tStatus == 0) { digitalWrite(tPin,OFF); return false; };

  boolean state = false;
  long tOn = (long(hOn)*3600)+(long(mOn)*60)+long(sOn);  
  long tOff = (long(hOff)*3600)+(long(mOff)*60)+long(sOff);
  
  
  if (tOn < tOff) {         // Normalnie  
      if (currentTimeSec >= tOn && currentTimeSec < tOff) { state = true; }  
  }  
  
  if (tOn > tOff) {         // Zasuwaj dookola
      if (currentTimeSec >= tOn && currentTimeSec <= 86399) { state = true; }
      if (currentTimeSec >= 0 && currentTimeSec < tOff) { state = true; }
  }
  
  if (state) { digitalWrite(tPin,ON);  return true; } else { digitalWrite(tPin,OFF); return false; }
}



///////////////////////////////////////////////////////////////////////////////////////
/////                     Perystaltic Pump                                        ///// 
///////////////////////////////////////////////////////////////////////////////////////

boolean doser(byte dPin, byte dFlow, byte dFlowDec, byte dStatus, byte d1HOn, byte d1MOn, byte d1Ml, byte d1MlDec, byte d2HOn, byte d2MOn, byte d2Ml, byte d2MlDec, byte d3HOn, byte d3MOn, byte d3Ml, byte d3MlDec, byte d4HOn, byte d4MOn, byte d4Ml, byte d4MlDec ) 
{

  if (dStatus == 0) { 
    digitalWrite(dPin,OFF); 
    return false; 
  } 
      
  long d1On,d2On,d3On,d4On;

  d1On = (long(d1HOn)*3600)+(long(d1MOn)*60);
  d2On = (long(d2HOn)*3600)+(long(d2MOn)*60);
  d3On = (long(d3HOn)*3600)+(long(d3MOn)*60);
  d4On = (long(d4HOn)*3600)+(long(d4MOn)*60);
  
  float dosingTimeD1, dosingTimeD2, dosingTimeD3, dosingTimeD4;
  
  dosingTimeD1 = (d1Ml+(0.1*d1MlDec))/((dFlow+(0.1*dFlowDec))/60);
  dosingTimeD2 = (d2Ml+(0.1*d2MlDec))/((dFlow+(0.1*dFlowDec))/60);
  dosingTimeD3 = (d3Ml+(0.1*d3MlDec))/((dFlow+(0.1*dFlowDec))/60);
  dosingTimeD4 = (d4Ml+(0.1*d4MlDec))/((dFlow+(0.1*dFlowDec))/60);
  
 
 
  
  // DX.1 ON/OFF
  if (d1On + dosingTimeD1 <= 86399 && currentTimeSec >= d1On && currentTimeSec < d1On + dosingTimeD1) 
  {
   digitalWrite(dPin,ON);
   return true;
  }
   
  if (d1On + dosingTimeD1 > 86399 && currentTimeSec >= d1On && currentTimeSec <= 86399) 
  {
   digitalWrite(dPin,ON);
   return true;
  }
  
  if ( d1On + dosingTimeD1 > 86399 && currentTimeSec >= 0 && currentTimeSec <= (d1On+dosingTimeD1)-86399) 
  {
   digitalWrite(dPin,ON);
   return true;
  }


  // DX.2 ON/OFF
  if (d2On + dosingTimeD2 <= 86399 && currentTimeSec >= d2On && currentTimeSec < d2On + dosingTimeD2) 
  {
   digitalWrite(dPin,ON);
   return true;
  }
   
  if (d2On + dosingTimeD2 > 86399 && currentTimeSec >= d2On && currentTimeSec <= 86399) 
  {
   digitalWrite(dPin,ON);
   return true;
  }
  
  if ( d2On + dosingTimeD2 > 86399 && currentTimeSec >= 0 && currentTimeSec <= (d2On+dosingTimeD2)-86399) 
  {
   digitalWrite(dPin,ON);
   return true;
  }
 ///////////////// 
 
 // DX.3 ON/OFF
  if (d3On + dosingTimeD3 <= 86399 && currentTimeSec >= d3On && currentTimeSec < d3On + dosingTimeD3) 
  {
   digitalWrite(dPin,ON);
   return true;
  }
   
  if (d3On + dosingTimeD3 > 86399 && currentTimeSec >= d3On && currentTimeSec <= 86399) 
  {
   digitalWrite(dPin,ON);
   return true;
  }
  
  if ( d3On + dosingTimeD3 > 86399 && currentTimeSec >= 0 && currentTimeSec <= (d3On+dosingTimeD3)-86399) 
  {
   digitalWrite(dPin,ON);
   return true;
  }
 ///////////////// 
 
 // DX.4 ON/OFF
  if (d4On + dosingTimeD4 <= 86399 && currentTimeSec >= d4On && currentTimeSec < d4On + dosingTimeD4) 
  {
   digitalWrite(dPin,ON);
   return true;
  }
   
  if (d4On + dosingTimeD4 > 86399 && currentTimeSec >= d4On && currentTimeSec <= 86399) 
  {
   digitalWrite(dPin,ON);
   return true;
  }
  
  if ( d4On + dosingTimeD4 > 86399 && currentTimeSec >= 0 && currentTimeSec <= (d4On+dosingTimeD4)-86399) 
  {
   digitalWrite(dPin,ON);
   return true;
  }
 ///////////////// 
 
 digitalWrite(dPin,OFF);
 return false;

}

////////////////////////////////////////////////////////////////////////////////////////
/////                     dayWhenPumpIsOn Function                                 /////
////////////////////////////////////////////////////////////////////////////////////////

boolean dayWhenPumpIsOn ( byte pumpNumber) 
{
// D1
 if ( pumpNumber == 1 ) {
   if ( currentDayOfWeek == 1 && d1Su == 1) { return true;} // Niedziela
   if ( currentDayOfWeek == 2 && d1Mo == 1) { return true;} // Poniedzialek
   if ( currentDayOfWeek == 3 && d1Tu == 1) { return true;} // Wtorek
   if ( currentDayOfWeek == 4 && d1We == 1) { return true;} // Sroda
   if ( currentDayOfWeek == 5 && d1Th == 1) { return true;} // Czwartek
   if ( currentDayOfWeek == 6 && d1Fr == 1) { return true;} // Piatek
   if ( currentDayOfWeek == 7 && d1Sa == 1) { return true;} // Sobota
   
   digitalWrite(d1Pin,OFF);
   return false;
 }

// D2
  if ( pumpNumber == 2 ) {
   if ( currentDayOfWeek == 1 && d2Su == 1) { return true;} // Niedziela
   if ( currentDayOfWeek == 2 && d2Mo == 1) { return true;} // Poniedzialek
   if ( currentDayOfWeek == 3 && d2Tu == 1) { return true;} // Wtorek
   if ( currentDayOfWeek == 4 && d2We == 1) { return true;} // Sroda
   if ( currentDayOfWeek == 5 && d2Th == 1) { return true;} // Czwartek
   if ( currentDayOfWeek == 6 && d2Fr == 1) { return true;} // Piatek
   if ( currentDayOfWeek == 7 && d2Sa == 1) { return true;} // Sobota
   
   digitalWrite(d2Pin,OFF);
   return false;
 }

// D3
 if ( pumpNumber == 3 ) {
   if ( currentDayOfWeek == 1 && d3Su == 1) { return true;} // Niedziela
   if ( currentDayOfWeek == 2 && d3Mo == 1) { return true;} // Poniedzialek
   if ( currentDayOfWeek == 3 && d3Tu == 1) { return true;} // Wtorek
   if ( currentDayOfWeek == 4 && d3We == 1) { return true;} // Sroda
   if ( currentDayOfWeek == 5 && d3Th == 1) { return true;} // Czwartek
   if ( currentDayOfWeek == 6 && d3Fr == 1) { return true;} // Piatek
   if ( currentDayOfWeek == 7 && d3Sa == 1) { return true;} // Sobota
   
   digitalWrite(d3Pin,OFF);
   return false;
 } 
  
}

////////////////////////////////////////////////////////////////////////////////////////
/////                               PWM Function                                   /////
////////////////////////////////////////////////////////////////////////////////////////

byte pwm(byte pwmPin, byte pwmStatus, byte pwmHOn, byte pwmMOn, byte pwmSOn, byte pwmHOff, byte pwmMOff, byte pwmSOff, byte pwmMin, byte pwmMax, byte pwmSr, byte pwmSs, byte pwmKeepLight, boolean pwmInvert)
{
  byte low=0, high=255;

  if (pwmInvert) { low = 255; high = 0;}
  if (!pwmInvert) { low = 0; high = 255;}
  
  if ( pwmStatus == 0 ) { analogWrite(pwmPin,map(0,0,255,low,high)); return map(0,0,255,low,high); }
  
  
  long pwmOn = (long(pwmHOn)*3600) + (long(pwmMOn)*60) + long(pwmSOn);
  long pwmOff  = (long(pwmHOff)*3600) + (long(pwmMOff)*60) + long(pwmSOff);
  
  boolean state = false;
    
  if (pwmOn < pwmOff) {         // Normalnie  
      if (currentTimeSec >= pwmOn && currentTimeSec < pwmOff) { state = true; }  
  }  
  
  if (pwmOn > pwmOff) {         // Zasuwaj dookola
      if (currentTimeSec >= pwmOn && currentTimeSec <= 86400) { state = true; }
      if (currentTimeSec >= 0 && currentTimeSec < pwmOff) { state = true; }
  }

  if (!state && !pwmKeepLight ) { analogWrite(pwmPin,map(0,0,255,low,high)); return map(0,0,255,low,high); } 
  if (!state && pwmKeepLight )  {   if (pwmMin > 1 && pwmMin <3) {pwmMin = 1;} analogWrite(pwmPin,map(pwmMin,0,255,low,high)); return map(pwmMin,0,255,low,high); } 
  
////////////////////////////////////
////  Swity 
///////////////////////////////////
   float jump;
   float sunriseLenght = pwmSr*60;
   long elapsed;
   long pwmSunriseStop = pwmOn + sunriseLenght;
   long pwm;
   if (sunriseLenght == 0 ) { sunriseLenght = 1;}
   jump = ((pwmMax-pwmMin)/sunriseLenght);
 
 if ( pwmSunriseStop < 86399 && currentTimeSec >= pwmOn &&  currentTimeSec <= pwmSunriseStop ) {         
    
   elapsed = sunriseLenght-(sunriseLenght-(currentTimeSec-pwmOn));
   pwm = pwmMin+(elapsed*jump);
   analogWrite(pwmPin,map(pwm,0,255,low,high));
   return map(pwm,0,255,low,high);           
  } 
  
   if (( pwmSunriseStop > 86399 && currentTimeSec >= pwmOn &&  currentTimeSec <= 86399 ) // przez zero
      || pwmSunriseStop > 86399 && currentTimeSec >= 0 && (currentTimeSec < (pwmSunriseStop - 86399)))
   {
    if (currentTimeSec>=pwmOn && currentTimeSec<=86399) 
     {
       elapsed = (sunriseLenght-(sunriseLenght-(currentTimeSec-pwmOn)));
     } 
    else
     {
      elapsed = sunriseLenght-(sunriseLenght -(86399-pwmOn+currentTimeSec));
     }
   
   pwm = pwmMin+(elapsed*jump);
   analogWrite(pwmPin,map(pwm,0,255,low,high));
   return map(pwm,0,255,low,high);
  }
 
 
 ///////////////////////
 ///// Zmierzchy ///////
 ///////////////////////
 float sunsetLenght = pwmSs*60;
 float sunsetStart;
 
  
 if ( (pwmOff - sunsetLenght) >= 0 )
 {
  sunsetStart = pwmOff - sunsetLenght;  
 }
 else
 {
  sunsetStart = 86400 + (pwmOff - sunsetLenght);
 }
 

   if (sunsetStart < pwmOff && sunsetStart >= 0 && currentTimeSec >= sunsetStart && currentTimeSec <= pwmOff) 
     { // Normalnie
       elapsed = (currentTimeSec - sunsetStart);
       if (sunsetLenght == 0 )  { sunsetLenght = 1; }
       jump = (pwmMax - pwmMin) / sunsetLenght;
       pwm = pwmMax - ( elapsed * jump );
       analogWrite(pwmPin,map(pwm,0,255,low,high));
       return map(pwm,0,255,low,high);
     } 
     
 
    if (sunsetStart > pwmOff && currentTimeSec >= sunsetStart && currentTimeSec <= 86400) 
    { 
       elapsed = (currentTimeSec - sunsetStart);
       if (sunsetLenght == 0 ) { sunsetLenght = 1;}
       jump = (pwmMax - pwmMin) / sunsetLenght;
       pwm = pwmMax - ( elapsed * jump );
       analogWrite(pwmPin,map(pwm,0,255,low,high));
       return map(pwm,0,255,low,high);
    }

    if (sunsetStart > pwmOff && currentTimeSec >= 0 && currentTimeSec < pwmOff) 
   {
       elapsed = (86400-sunsetStart)+currentTimeSec;
       if (sunsetLenght == 0 ) { sunsetLenght = 1;}
       jump = ( pwmMax - pwmMin) / sunsetLenght;
       pwm = pwmMax-(elapsed * jump);
       analogWrite(pwmPin,map(pwm,0,255,low,high));
       return map(pwm,0,255,low,high);
   }
   
 
 //// Jak sie nic nie zlapalo, to wypal oczy
   analogWrite(pwmPin,map(pwmMax,0,255,low,high));
   return map(pwmMax,0,255,low,high);
} 
////////////// 

////////////////////////////////////////////////////////////////////////////////////////
/////                   Silky Smooth Power On                                      /////
////////////////////////////////////////////////////////////////////////////////////////
void silkySmoothPowerOn(int silkySmoothTime, unsigned long currentMillis)
{
 static unsigned int silkySmoothCounter = 0;
 static unsigned long silkySmoothLastCounterTime = 0;
 silkySmoothTime = silkySmoothTime; 
  
  if (justTurnedOn && silkySmoothCounter <= silkySmoothTime) 
    {
    if ( currentMillis < 1000 ) { pwm1Max = 0; pwm2Max = 0; pwm3Max = 0; }
   
    if ( (currentMillis - silkySmoothLastCounterTime) >= 1000 ) 
       { 
        silkySmoothCounter++; 
        silkySmoothLastCounterTime = currentMillis; 
        pwm1Max = ((EEPROM.read(290) / silkySmoothTime)*silkySmoothCounter);
        pwm2Max = ((EEPROM.read(310) / silkySmoothTime)*silkySmoothCounter);
        pwm3Max = ((EEPROM.read(340) / silkySmoothTime)*silkySmoothCounter);
       } 

        if ( silkySmoothCounter == silkySmoothTime) 
           {
            justTurnedOn = false;
            eEpromRead();
           }
    
    }
    
  }



////////////////////////////////////////////////////////////////////////////////////////
/////                          Feeding Function                                    /////
////////////////////////////////////////////////////////////////////////////////////////

 void feeding(byte feedingPin, byte feedingBtnPin, byte feedingTime, unsigned long currentMillis) 
 {

    static unsigned long lastBtnFeedingCountersCheck = 0;
    static unsigned long feedingStartTime = 0; 
    static unsigned long feedingStopTime = 0;
    static boolean feeding = false;
    static byte btnFeedingCounter = 0;
    
    if ( currentMillis%20 == 0 && (currentMillis - lastBtnFeedingCountersCheck) > 1 )
    { 
  
      lastBtnFeedingCountersCheck = currentMillis;
      
      
       if (!digitalRead(feedingBtnPin))  
       {
         btnFeedingCounter++;
       }
       if (btnFeedingCounter >= 2 && !digitalRead(feedingBtnPin)) { digitalWrite(buzzerPin,HIGH); }
        
       if (btnFeedingCounter >= 2 && digitalRead(feedingBtnPin))  
       {
           feeding = !feeding; 
           btnFeedingCounter = 0;
           digitalWrite(buzzerPin,LOW);
          
          if (feeding) 
          {
            feedingStartTime = currentMillis;
            feedingStopTime = currentMillis + (60000 * feedingTime);
            digitalWrite(feedingPin, OFF);
            return;
          }
          
          if (!feeding) 
          {
           feedingStartTime = 0;
           feedingStopTime = 0;
           digitalWrite(feedingPin, ON);
           return;
          }
       }
     }  
   
   // Karmienie    
  if ( currentMillis%300 == 0) 
  { 
      if (!feeding)
      {
        digitalWrite(feedingPin, ON);
        return;
      }
      
     if (feedingStartTime == 0 && feedingStopTime == 0) { digitalWrite(feedingPin,ON); return; }
     if (feeding && currentMillis < feedingStartTime ) { digitalWrite(feedingPin,ON); feeding = false; return; }      
      
      if (feeding &&  currentMillis < feedingStopTime) 
      {
       digitalWrite(feedingPin, OFF); 
       return;
      }
      
      if (feeding && currentMillis >= feedingStopTime)
      {
        feeding = false;
        return;
      }
      
  }  
  
 } 

////////////////////////////////////////////////////////////////////////////////////////
/////                          Refill Function                                     /////
////////////////////////////////////////////////////////////////////////////////////////


  void refill(byte refillPin, byte refillBtnPin, byte refillDelayStart, unsigned long currentMillis) 
  {
    static unsigned long lastBtnRefillCountersCheck = 0;
    static byte btnRefillCounter = 0;
  
    if ( currentMillis%1000 < 3 && (currentMillis - lastBtnRefillCountersCheck) > 4 ) 
    {
      lastBtnRefillCountersCheck = currentMillis; 
       
       if (!digitalRead(refillBtnPin)) 
       { 
         btnRefillCounter++; 
       } 
       
       if ( btnRefillCounter <= refillDelayStart ) 
       {
         digitalWrite(refillPin,OFF); 
         return;
       }
       
       if ( btnRefillCounter > refillDelayStart && !digitalRead(refillBtnPin) ) 
       {
         digitalWrite(refillPin,ON); 
       }
       
       if ( btnRefillCounter > refillDelayStart && digitalRead(refillBtnPin) ) 
       {
         digitalWrite(refillPin,ON);
         btnRefillCounter = 0; 
       }
     }
        
    return; 
  }
  
////////////////////////////////////////////////////////////////////////////////////////
/////                          Keyboard - Function                                 /////
////////////////////////////////////////////////////////////////////////////////////////
 void keyboard(unsigned long currentMillis) 
  {
    static unsigned long lastBtnCountersCheck = 0;
    static unsigned int btnT1Counter = 0, btnT2Counter = 0, btnT3Counter = 0;
  
    if ( currentMillis%10 == 0 )  
    {
      lastBtnCountersCheck = currentMillis; 
       
       if (!digitalRead(btnT1Pin)) 
       { 
         btnT1Counter++; 
       } 
       
       if ( btnT1Counter > 2 && btnT1Counter <= 100 && digitalRead(btnT1Pin) ) 
       {  
          t1ManualModeOn = true;
          btnT1Counter = 0;
          t1ManualModeStatus = !digitalRead(t1Pin);
	  return;
       }
       
       
       if ( btnT1Counter > 100 && !digitalRead(btnT1Pin) ) 
       {
         t1ManualModeOn = false;
         digitalWrite(buzzerPin,HIGH);
       }
       
       if ( btnT1Counter > 100 && digitalRead(btnT1Pin) ) 
       {
         btnT1Counter= 0;
         t1ManualModeOn = false;
         digitalWrite(buzzerPin,LOW);
       } 
  
  
   /// T2
       if (!digitalRead(btnT2Pin)) 
       { 
         btnT2Counter++; 
       } 
       
       
       if ( btnT2Counter > 2 && btnT2Counter <= 100 && digitalRead(btnT2Pin) ) 
       {  
	  t2ManualModeOn = true;
          btnT2Counter = 0;
          t2ManualModeStatus = !digitalRead(t2Pin);
	  return;
       }
       
       
       if ( btnT2Counter > 100 && !digitalRead(btnT2Pin) ) 
       {
         t2ManualModeOn = false;
         digitalWrite(buzzerPin,HIGH);
       }
       
       if ( btnT2Counter > 100 && digitalRead(btnT2Pin) ) 
       {
         btnT2Counter= 0;
         t2ManualModeOn = false;
         digitalWrite(buzzerPin,LOW);
       } 
   

 
    /// T3
       if (!digitalRead(btnT3Pin)) 
       { 
         btnT3Counter++; 
       } 
       
       
       if ( btnT3Counter > 2 && btnT3Counter <= 100 && digitalRead(btnT3Pin) ) 
       {  
          t3ManualModeOn = true;
          btnT3Counter = 0;
          t3ManualModeStatus = !digitalRead(t3Pin);
	  return;
       }
       
       
       if ( btnT3Counter > 100 && !digitalRead(btnT3Pin) ) 
       {
         t3ManualModeOn = false;
         digitalWrite(buzzerPin,HIGH);
       }
       
       if ( btnT3Counter > 100 && digitalRead(btnT3Pin) ) 
       {
         btnT3Counter= 0;
         t3ManualModeOn = false;
         digitalWrite(buzzerPin,LOW);
       } 
   

  
   }
        
    return; 
  }


////////////////////////////////////////////////////////////////////////////////////////
/////                          manualModeBlink - Functions                         /////
////////////////////////////////////////////////////////////////////////////////////////

  void manualModeBlink(byte blinkPin, unsigned long currentMillis) 
  {
    static unsigned long lastBlinkTime = 0;
    static boolean blinkState = false;
   
     if ( (currentMillis - lastBlinkTime) > 800 ) 
     { 
       lastBlinkTime = currentMillis; 
       blinkState = !blinkState;
       digitalWrite(blinkPin,blinkState); 
       
     } 
  
  }
  

////////////////////////////////////////////////////////////////////////////////////////
/////                             Analysis                                         /////
////////////////////////////////////////////////////////////////////////////////////////

 boolean commandAnalysis(char cmdOutputArray[64]){

  unsigned int val[64];
  char *cmdVal;
  int i = 0;
  
  
  cmdVal = strtok(cmdOutputArray, ",");
  
      while (cmdVal) 
      {
       val[i++] = atoi(cmdVal);
       cmdVal = strtok(NULL, ",");
      }
 


 ///////////////////////////////////////////////////////////////////////////////////////
 /////                        Timer 1 EPROM                                        ///// 
 ///////////////////////////////////////////////////////////////////////////////////////

  if (val[0] == 1) {
   
   byte eT1Pin, eT1Status, eT1HOn, eT1MOn, eT1SOn, eT1HOff, eT1MOff, eT1SOff;
   
   if (val[1] >=0 && val[1] <= 99) { eT1Pin = val[1]; } else { return false;}       // eT1pin - 1
   if (val[2] >=0 && val[2] <= 1) { eT1Status = val[2];} else { return false;}      // eT1_1_status 3
   if (val[3] >=0 && val[3] <= 23) { eT1HOn = val[3]; } else { return false;}       // 4
   if (val[4] >=0 && val[4] <= 59) { eT1MOn = val[4]; } else { return false;}       // 5
   if (val[5] >=0 && val[5] <= 59) { eT1SOn = val[5]; } else { return false;}       // 6
   if (val[6] >=0 && val[6] <= 23) { eT1HOff = val[6]; } else { return false;}      // 7
   if (val[7] >=0 && val[7] <= 59) { eT1MOff = val[7]; } else { return false;}      // 8
   if (val[8] >=0 && val[8] <= 59) { eT1SOff = val[8]; } else { return false;}      // 9
   
   
   
   EEPROM.write(11,eT1Pin);EEPROM.write(13,eT1Status);
   EEPROM.write(14,eT1HOn);EEPROM.write(15,eT1MOn);EEPROM.write(16,eT1SOn);
   EEPROM.write(17,eT1HOff);EEPROM.write(18,eT1MOff);EEPROM.write(19,eT1SOff);

   return true;
  } // T1


 //////////////////////////////////////////////////////////////////////////////////////
 /////                        Timer 2 EPROM                                       /////
 //////////////////////////////////////////////////////////////////////////////////////

 if (val[0] == 2) {
  byte eT2Pin, eT2Status, eT2HOn, eT2MOn, eT2SOn, eT2HOff, eT2MOff, eT2SOff;
										       // Timer 2 
   if (val[1] >=0 && val[1] <= 99) { eT2Pin = val[1]; } else { return false;}          // eT2pin - 1
   if (val[2] >=0 && val[2] <= 1) { eT2Status = val[2];} else { return false;}         // eT2_1_status - 3
   if (val[3] >=0 && val[3] <= 23) { eT2HOn = val[3]; } else { return false;}          // eT2hOn1 - 4
   if (val[4] >=0 && val[4] <= 59) { eT2MOn = val[4]; } else { return false;}          // eT2mOn1 - 5 
   if (val[5] >=0 && val[5] <= 59) { eT2SOn = val[5]; } else { return false;}          // eT2sOn1 - 6
   if (val[6] >=0 && val[6] <= 23) { eT2HOff = val[6]; } else { return false;}         // eT2hOff1 - 7
   if (val[7] >=0 && val[7] <= 59) { eT2MOff = val[7]; } else { return false;}         // eT2mOff1 - 8 
   if (val[8] >=0 && val[8] <= 59) { eT2SOff = val[8]; } else { return false;}         // eT2sOff1 - 9
      
   
   EEPROM.write(31,eT2Pin);EEPROM.write(33,eT2Status);
   EEPROM.write(34,eT2HOn);EEPROM.write(35,eT2MOn);EEPROM.write(36,eT2SOn);
   EEPROM.write(37,eT2HOff);EEPROM.write(38,eT2MOff);EEPROM.write(39,eT2SOff);
  
   return true;
 } // T2
 

 ////////////////////////////////////////////////////////////////////////////////////////
 /////                       Timer 3 EPROM                                          /////
 ////////////////////////////////////////////////////////////////////////////////////////
 if (val[0] == 3) {
   
   byte eT3Pin, eT3Status, eT3HOn, eT3MOn, eT3SOn, eT3HOff, eT3MOff, eT3SOff;
                                                                                       // Timer 3 //
   if (val[1] >=0 && val[1] <= 99) { eT3Pin = val[1]; } else { return false;}          // eT3pin - 1
   if (val[2] >=0 && val[2] <= 1) { eT3Status = val[2]; } else { return false;}        // eT3state - 2
   if (val[3] >=0 && val[3] <= 23) { eT3HOn = val[3]; } else { return false;}          // eT3hOn1 - 4
   if (val[4] >=0 && val[4] <= 59) { eT3MOn = val[4]; } else { return false;}          // eT3mOn1 - 5 
   if (val[5] >=0 && val[5] <= 59) { eT3SOn = val[5]; } else { return false;}          // eT3sOn1 - 6
   if (val[6] >=0 && val[6] <= 23) { eT3HOff = val[6]; } else { return false;}         // eT3hOff1 - 7
   if (val[7] >=0 && val[7] <= 59) { eT3MOff = val[7]; } else { return false;}         // eT3mOff1 - 8 
   if (val[8] >=0 && val[8] <= 59) { eT3SOff = val[8]; } else { return false;}         // eT3sOff1 - 9
   

   EEPROM.write(51,eT3Pin); EEPROM.write(53,eT3Status);
   EEPROM.write(54,eT3HOn);EEPROM.write(55,eT3MOn);EEPROM.write(56,eT3SOn);
   EEPROM.write(57,eT3HOff);EEPROM.write(58,eT3MOff);EEPROM.write(59,eT3SOff);
   
   return true;
 
 } // T3
 
 

 /////////////////////////////////////////////////////////////////////////////////////////
 /////                    Peristaltic Pump 1, EPROM                                  /////
 /////////////////////////////////////////////////////////////////////////////////////////
 
 // D1
 if (val[0] == 4) {
   
   byte eD1Status, eD1Flow, eD1FlowDec, eD1Pin, eD1Su, eD1Mo, eD1Tu, eD1We, eD1Th, eD1Fr, eD1Sa, eD1Mode ;
                                                                                          // Dosing Pump //  
   if (val[1] >=0 && val[1] <= 1) { eD1Status = val[1]; } else { return false;}           // eD1Status -  1
   if (val[2] >=0 && val[2] <= 250) { eD1Flow = val[2]; } else { return false;}           // eD1Flow -    2
   if (val[3] >=0 && val[3] <= 9) { eD1FlowDec = val[3]; } else { return false;}          // eD1FlowDec - 3
   if (val[4] >=0 && val[4] <= 99) { eD1Pin = val[4]; } else { return false;}             // eD1Pin -     4
   if (val[5] >=0 && val[5] <= 1) { eD1Su = val[5]; } else { return false;}               // eD1Su -      5
   if (val[6] >=0 && val[6] <= 1) { eD1Mo = val[6]; } else { return false;}               // eD1Mo -      6
   if (val[7] >=0 && val[7] <= 1) { eD1Tu = val[7]; } else { return false;}               // eD1Tu -      7
   if (val[8] >=0 && val[8] <= 1) { eD1We = val[8]; } else { return false;}               // eD1we -      8
   if (val[9] >=0 && val[9] <= 1) { eD1Th = val[9]; } else { return false;}               // eD2Th -      9
   if (val[10] >=0 && val[10] <= 1) { eD1Fr = val[10]; } else { return false;}            // eD1Fr -      10
   if (val[11] >=0 && val[11] <= 1) { eD1Sa = val[11]; } else { return false;}            // eD1Sa -      11
   if (val[12] >=0 && val[12] <= 1) { eD1Mode = val[12]; } else { return false;}          // eD1Mode -    12
   

   EEPROM.write(71,eD1Status); EEPROM.write(72,eD1Flow);
   EEPROM.write(73,eD1FlowDec);EEPROM.write(74,eD1Pin);
   EEPROM.write(75,eD1Su);EEPROM.write(76,eD1Mo);EEPROM.write(77,eD1Tu);
   EEPROM.write(78,eD1We);EEPROM.write(79,eD1Th);EEPROM.write(80,eD1Fr);
   EEPROM.write(81,eD1Sa);EEPROM.write(82,eD1Mode);
   
   return true;
 
 } // 
 
 // D1.1 EPROM WRITE ///////////////
 if (val[0] == 5) {
   
   byte eD11HOn, eD11MOn, eD11Ml, eD11MlDec;
     
     if (val[1] >=0 && val[1] <= 23) { eD11HOn = val[1]; } else { return false;}           // eD11HOn   - 1
     if (val[2] >=0 && val[2] <= 59) { eD11MOn = val[2]; } else { return false;}           // eD11MOn   - 2
     if (val[3] >=0 && val[3] <= 250) { eD11Ml = val[3]; } else { return false;}           // eD11Ml    - 3
     if (val[4] >=0 && val[4] <= 9) { eD11MlDec = val[4]; } else { return false;}          // eD11MlDec - 4
     
     EEPROM.write(91,eD11HOn); EEPROM.write(92,eD11MOn);
     EEPROM.write(93,eD11Ml); EEPROM.write(94,eD11MlDec);
     
   
 }

// D1.2 EPROM WRITE ///////////////
 if (val[0] == 6) {
   
   byte eD12HOn, eD12MOn, eD12Ml, eD12MlDec;
     
     if (val[1] >=0 && val[1] <= 23) { eD12HOn = val[1]; } else { return false;}          // eD12HOn -   1
     if (val[2] >=0 && val[2] <= 59) { eD12MOn = val[2]; } else { return false;}          // eD12MOn -   2
     if (val[3] >=0 && val[3] <= 250) { eD12Ml = val[3]; } else { return false;}          // eD12Ml -    3
     if (val[4] >=0 && val[4] <= 9) { eD12MlDec = val[4]; } else { return false;}         // eD12MlDec - 4
     
     EEPROM.write(101,eD12HOn); EEPROM.write(102,eD12MOn);
     EEPROM.write(103,eD12Ml); EEPROM.write(104,eD12MlDec);
     
   
 }
 
 // D1.3 EPROM WRITE ///////////////
 if (val[0] == 7) {
   
   byte eD13HOn, eD13MOn, eD13Ml, eD13MlDec;
     
     if (val[1] >=0 && val[1] <= 23) { eD13HOn = val[1]; } else { return false;}           // eD13HOn -   1
     if (val[2] >=0 && val[2] <= 59) { eD13MOn = val[2]; } else { return false;}           // eD13MOn -   2
     if (val[3] >=0 && val[3] <= 250) { eD13Ml = val[3]; } else { return false;}           // eD13Ml -    3
     if (val[4] >=0 && val[4] <= 9) { eD13MlDec = val[4]; } else { return false;}          // eD13MlDec - 4
     
     EEPROM.write(111,eD13HOn); EEPROM.write(112,eD13MOn);
     EEPROM.write(113,eD13Ml); EEPROM.write(114,eD13MlDec);
     
   
 }
 
// D1.4 EPROM  WRITE ///////////////
 if (val[0] == 8) {
   
   byte eD14HOn, eD14MOn, eD14Ml, eD14MlDec;
     
     if (val[1] >=0 && val[1] <= 23) { eD14HOn = val[1]; } else { return false;}           // eD14HOn -   1
     if (val[2] >=0 && val[2] <= 59) { eD14MOn = val[2]; } else { return false;}           // eD14MOn -   2
     if (val[3] >=0 && val[3] <= 250) { eD14Ml = val[3]; } else { return false;}           // eD14Ml -    3
     if (val[4] >=0 && val[4] <= 9) { eD14MlDec = val[4]; } else { return false;}          // eD14MlDec - 4
     
     EEPROM.write(121,eD14HOn); EEPROM.write(122,eD14MOn);
     EEPROM.write(123,eD14Ml); EEPROM.write(124,eD14MlDec);
     
   
 } 

 /////////////////////////////////////////////////////////////////////////////////////////
 /////                    Peristaltic Pump 2, EPROM                                  /////
 /////////////////////////////////////////////////////////////////////////////////////////
 
 // D2
 if (val[0] == 9) {
   
   byte eD2Status, eD2Flow, eD2FlowDec, eD2Pin, eD2Su, eD2Mo, eD2Tu, eD2We, eD2Th, eD2Fr, eD2Sa, eD2Mode;
                                                                                          // Dosing Pump //  
   if (val[1] >=0 && val[1] <= 1) { eD2Status = val[1]; } else { return false;}           // eD2Status -  1
   if (val[2] >=0 && val[2] <= 250) { eD2Flow = val[2]; } else { return false;}           // eD2Flow -    2
   if (val[3] >=0 && val[3] <= 9) { eD2FlowDec = val[3]; } else { return false;}          // eD2FlowDec - 3
   if (val[4] >=0 && val[4] <= 99) { eD2Pin = val[4]; } else { return false;}             // eD2Pin -     4
   if (val[5] >=0 && val[5] <= 1) { eD2Su = val[5]; } else { return false;}               // eD2Su -      5
   if (val[6] >=0 && val[6] <= 1) { eD2Mo = val[6]; } else { return false;}               // eD2Mo -      6
   if (val[7] >=0 && val[7] <= 1) { eD2Tu = val[7]; } else { return false;}               // eD2Tu -      7
   if (val[8] >=0 && val[8] <= 1) { eD2We = val[8]; } else { return false;}               // eD2we -      8
   if (val[9] >=0 && val[9] <= 1) { eD2Th = val[9]; } else { return false;}               // eD2Th -      9
   if (val[10] >=0 && val[10] <= 1) { eD2Fr = val[10]; } else { return false;}            // eD2Fr -      10
   if (val[11] >=0 && val[11] <= 1) { eD2Sa = val[11]; } else { return false;}            // eD2Sa -      11
   if (val[12] >=0 && val[12] <= 1) { eD2Mode = val[12]; } else { return false;}          // eD2Mode -    12

   EEPROM.write(131,eD2Status); EEPROM.write(132,eD2Flow);
   EEPROM.write(133,eD2FlowDec);EEPROM.write(134,eD2Pin);
   EEPROM.write(135,eD2Su);EEPROM.write(136,eD2Mo);EEPROM.write(137,eD2Tu);
   EEPROM.write(138,eD2We);EEPROM.write(139,eD2Th);EEPROM.write(140,eD2Fr);
   EEPROM.write(141,eD2Sa);EEPROM.write(142,eD2Mode);   

   return true;
 
 } // 
 
 // D2.1 EPROM WRITE ///////////////
 if (val[0] == 10) {
   
   byte eD21HOn, eD21MOn, eD21Ml, eD21MlDec;
     
     if (val[1] >=0 && val[1] <= 23) { eD21HOn = val[1]; } else { return false;}           // eD21HOn   - 1
     if (val[2] >=0 && val[2] <= 59) { eD21MOn = val[2]; } else { return false;}           // eD21MOn   - 2
     if (val[3] >=0 && val[3] <= 250) { eD21Ml = val[3]; } else { return false;}           // eD21Ml    - 3
     if (val[4] >=0 && val[4] <= 9) { eD21MlDec = val[4]; } else { return false;}          // eD21MlDec - 4
     
     EEPROM.write(151,eD21HOn); EEPROM.write(152,eD21MOn);
     EEPROM.write(153,eD21Ml); EEPROM.write(154,eD21MlDec);
     
   
 }

// D2.2 EPROM WRITE ///////////////
 if (val[0] == 11) {
   
   byte eD22HOn, eD22MOn, eD22Ml, eD22MlDec;
     
     if (val[1] >=0 && val[1] <= 23) { eD22HOn = val[1]; } else { return false;}          // eD22HOn -   1
     if (val[2] >=0 && val[2] <= 59) { eD22MOn = val[2]; } else { return false;}          // eD22MOn -   2
     if (val[3] >=0 && val[3] <= 250) { eD22Ml = val[3]; } else { return false;}          // eD22Ml -    3
     if (val[4] >=0 && val[4] <= 9) { eD22MlDec = val[4]; } else { return false;}         // eD22MlDec - 4
     
     EEPROM.write(161,eD22HOn); EEPROM.write(162,eD22MOn);
     EEPROM.write(163,eD22Ml); EEPROM.write(164,eD22MlDec);
     
   
 }
 
 // D2.3 EPROM WRITE ///////////////
 if (val[0] == 12) {
   
   byte eD23HOn, eD23MOn, eD23Ml, eD23MlDec;
     
     if (val[1] >=0 && val[1] <= 23) { eD23HOn = val[1]; } else { return false;}           // eD23HOn -   1
     if (val[2] >=0 && val[2] <= 59) { eD23MOn = val[2]; } else { return false;}           // eD23MOn -   2
     if (val[3] >=0 && val[3] <= 250) { eD23Ml = val[3]; } else { return false;}           // eD23Ml -    3
     if (val[4] >=0 && val[4] <= 9) { eD23MlDec = val[4]; } else { return false;}          // eD23MlDec - 4
     
     EEPROM.write(171,eD23HOn); EEPROM.write(172,eD23MOn);
     EEPROM.write(173,eD23Ml); EEPROM.write(174,eD23MlDec);
     
   
 }
 
// D2.4 EPROM  WRITE ///////////////
 if (val[0] == 13) {
   
   byte eD24HOn, eD24MOn, eD24Ml, eD24MlDec;
     
     if (val[1] >=0 && val[1] <= 23) { eD24HOn = val[1]; } else { return false;}           // eD24HOn -   1
     if (val[2] >=0 && val[2] <= 59) { eD24MOn = val[2]; } else { return false;}           // eD24MOn -   2
     if (val[3] >=0 && val[3] <= 250) { eD24Ml = val[3]; } else { return false;}           // eD24Ml -    3
     if (val[4] >=0 && val[4] <= 9) { eD24MlDec = val[4]; } else { return false;}          // eD24MlDec - 4
     
     EEPROM.write(181,eD24HOn); EEPROM.write(182,eD24MOn);
     EEPROM.write(183,eD24Ml); EEPROM.write(184,eD24MlDec);
     
   
 } 


 /////////////////////////////////////////////////////////////////////////////////////////
 /////                    Peristaltic Pump 3, EPROM                                  /////
 /////////////////////////////////////////////////////////////////////////////////////////
 
 // D3
 if (val[0] == 14) {
   
   byte eD3Status, eD3Flow, eD3FlowDec, eD3Pin, eD3Su, eD3Mo, eD3Tu, eD3We, eD3Th, eD3Fr, eD3Sa, eD3Mode ;
                                                                                          // Dosing Pump //  
   if (val[1] >=0 && val[1] <= 1) { eD3Status = val[1]; } else { return false;}           // eD3Status -  1
   if (val[2] >=0 && val[2] <= 250) { eD3Flow = val[2]; } else { return false;}           // eD3Flow -    2
   if (val[3] >=0 && val[3] <= 9) { eD3FlowDec = val[3]; } else { return false;}          // eD3FlowDec - 3
   if (val[4] >=0 && val[4] <= 99) { eD3Pin = val[4]; } else { return false;}             // eD3Pin -     4
   if (val[5] >=0 && val[5] <= 1) { eD3Su = val[5]; } else { return false;}               // eD3Su -      5
   if (val[6] >=0 && val[6] <= 1) { eD3Mo = val[6]; } else { return false;}               // eD3Mo -      6
   if (val[7] >=0 && val[7] <= 1) { eD3Tu = val[7]; } else { return false;}               // eD3Tu -      7
   if (val[8] >=0 && val[8] <= 1) { eD3We = val[8]; } else { return false;}               // eD3we -      8
   if (val[9] >=0 && val[9] <= 1) { eD3Th = val[9]; } else { return false;}               // eD3Th -      9
   if (val[10] >=0 && val[10] <= 1) { eD3Fr = val[10]; } else { return false;}            // eD3Fr -      10
   if (val[11] >=0 && val[11] <= 1) { eD3Sa = val[11]; } else { return false;}            // eD3Sa -      11
   if (val[12] >=0 && val[12] <= 1) { eD3Mode = val[12]; } else { return false;}          // eD3Mode -    12
   

   EEPROM.write(191,eD3Status); EEPROM.write(192,eD3Flow);
   EEPROM.write(193,eD3FlowDec);EEPROM.write(194,eD3Pin);
   EEPROM.write(195,eD3Su);EEPROM.write(196,eD3Mo);EEPROM.write(197,eD3Tu);
   EEPROM.write(198,eD3We);EEPROM.write(199,eD3Th);EEPROM.write(200,eD3Fr);
   EEPROM.write(201,eD3Sa);EEPROM.write(202,eD3Mode);
   
   return true;
 
 } // 
 
 // D3.1 EPROM WRITE ///////////////
 if (val[0] == 15) {
   
   byte eD31HOn, eD31MOn, eD31Ml, eD31MlDec;
     
     if (val[1] >=0 && val[1] <= 23) { eD31HOn = val[1]; } else { return false;}           // eD31HOn   - 1
     if (val[2] >=0 && val[2] <= 59) { eD31MOn = val[2]; } else { return false;}           // eD31MOn   - 2
     if (val[3] >=0 && val[3] <= 250) { eD31Ml = val[3]; } else { return false;}           // eD31Ml    - 3
     if (val[4] >=0 && val[4] <= 9) { eD31MlDec = val[4]; } else { return false;}          // eD31MlDec - 4
     
     EEPROM.write(211,eD31HOn); EEPROM.write(212,eD31MOn);
     EEPROM.write(213,eD31Ml); EEPROM.write(214,eD31MlDec);
     
   
 }

// D3.2 EPROM WRITE ///////////////
 if (val[0] == 16) {
   
   byte eD32HOn, eD32MOn, eD32Ml, eD32MlDec;
     
     if (val[1] >=0 && val[1] <= 23) { eD32HOn = val[1]; } else { return false;}          // eD32HOn -   1
     if (val[2] >=0 && val[2] <= 59) { eD32MOn = val[2]; } else { return false;}          // eD32MOn -   2
     if (val[3] >=0 && val[3] <= 250) { eD32Ml = val[3]; } else { return false;}          // eD32Ml -    3
     if (val[4] >=0 && val[4] <= 9) { eD32MlDec = val[4]; } else { return false;}         // eD32MlDec - 4
     
     EEPROM.write(221,eD32HOn); EEPROM.write(222,eD32MOn);
     EEPROM.write(223,eD32Ml); EEPROM.write(224,eD32MlDec);
     
   
 }
 
 // D3.3 EPROM WRITE ///////////////
 if (val[0] == 17) {
   
   byte eD33HOn, eD33MOn, eD33Ml, eD33MlDec;
     
     if (val[1] >=0 && val[1] <= 23) { eD33HOn = val[1]; } else { return false;}           // eD33HOn -   1
     if (val[2] >=0 && val[2] <= 59) { eD33MOn = val[2]; } else { return false;}           // eD33MOn -   2
     if (val[3] >=0 && val[3] <= 250) { eD33Ml = val[3]; } else { return false;}           // eD33Ml -    3
     if (val[4] >=0 && val[4] <= 9) { eD33MlDec = val[4]; } else { return false;}          // eD33MlDec - 4
     
     EEPROM.write(231,eD33HOn); EEPROM.write(232,eD33MOn);
     EEPROM.write(233,eD33Ml); EEPROM.write(234,eD33MlDec);
     
   
 }
 
// D3.4 EPROM  WRITE ///////////////
 if (val[0] == 18) {
   
   byte eD34HOn, eD34MOn, eD34Ml, eD34MlDec;
     
     if (val[1] >=0 && val[1] <= 23) { eD34HOn = val[1]; } else { return false;}           // eD34HOn -   1
     if (val[2] >=0 && val[2] <= 59) { eD34MOn = val[2]; } else { return false;}           // eD34MOn -   2
     if (val[3] >=0 && val[3] <= 250) { eD34Ml = val[3]; } else { return false;}           // eD34Ml -    3
     if (val[4] >=0 && val[4] <= 9) { eD34MlDec = val[4]; } else { return false;}          // eD34MlDec - 4
     
     EEPROM.write(241,eD34HOn); EEPROM.write(242,eD34MOn);
     EEPROM.write(243,eD34Ml); EEPROM.write(244,eD34MlDec);
     
   
 } 
 
 
//////////////////////////////////////////////////////////////////////////////////////////////
/////                       Timer 1,2,3 Status EPROM                                     /////
//////////////////////////////////////////////////////////////////////////////////////////////
 if (val[0] == 21) {
   byte eT1Status;
   if (val[1] >=0 && val[1] <= 1) { eT1Status = val[1]; } else { return false;}
   EEPROM.write(13,eT1Status);   
 }
if (val[0] == 22) {
   byte eT2Status;
   if (val[1] >=0 && val[1] <= 1) { eT2Status = val[1]; } else { return false;}    
   EEPROM.write(33,eT2Status);
 }
if (val[0] == 23) {
   byte eT3Status;
   if (val[1] >=0 && val[1] <= 1) { eT3Status = val[1]; } else { return false;}  
   EEPROM.write(53,eT3Status);  
 }

//////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////
/////                       Timer 4,5,6 EPROM                                            /////
//////////////////////////////////////////////////////////////////////////////////////////////

// T4
 if (val[0] == 24) {
   
   byte eD1Pin, eT4Status, eT4HOn, eT4MOn, eT4HOff, eT4MOff, eD1Mode ;
     
     if (val[1] >=0 && val[1] <= 99) { eD1Pin = val[1]; } else { return false;}           // eD1pin -    1  
     if (val[2] >=0 && val[2] <= 1)  { eT4Status = val[2]; } else { return false;}        // eT4Status - 2
     if (val[3] >=0 && val[3] <= 23) { eT4HOn = val[3]; } else { return false;}           // eT4HOn -    3
     if (val[4] >=0 && val[4] <= 59) { eT4MOn = val[4]; } else { return false;}           // eT4MOn -    4
     if (val[5] >=0 && val[5] <= 23) { eT4HOff = val[5]; } else { return false;}          // eT4HOff -   5
     if (val[6] >=0 && val[6] <= 59) { eT4MOff = val[6]; } else { return false;}          // eT4MOff -   6
     if (val[7] >=0 && val[7] <= 1)  { eD1Mode = val[7]; } else { return false;}          // eD1Mode -   7

     EEPROM.write(251,eT4Status); EEPROM.write(252,eT4HOn);
     EEPROM.write(253,eT4MOn); EEPROM.write(254,eT4HOff);
     EEPROM.write(255,eT4MOff); EEPROM.write(74,eD1Pin);
     EEPROM.write(82,eD1Mode);
   
 } 

// T5
 if (val[0] == 25) {
   
   byte eD2Pin, eT5Status, eT5HOn, eT5MOn, eT5HOff, eT5MOff, eD2Mode ;
     
     if (val[1] >=0 && val[1] <= 99) { eD2Pin = val[1]; } else { return false;}           // eD2pin -    1  
     if (val[2] >=0 && val[2] <= 1)  { eT5Status = val[2]; } else { return false;}        // eT5Status - 2
     if (val[3] >=0 && val[3] <= 23) { eT5HOn = val[3]; } else { return false;}           // eT5HOn -    3
     if (val[4] >=0 && val[4] <= 59) { eT5MOn = val[4]; } else { return false;}           // eT5MOn -    4
     if (val[5] >=0 && val[5] <= 23) { eT5HOff = val[5]; } else { return false;}          // eT5HOff -   5
     if (val[6] >=0 && val[6] <= 59) { eT5MOff = val[6]; } else { return false;}          // eT5MOff -   6
     if (val[7] >=0 && val[7] <= 1)  { eD2Mode = val[7]; } else { return false;}          // eD2Mode -   7

     EEPROM.write(261,eT5Status); EEPROM.write(262,eT5HOn);
     EEPROM.write(263,eT5MOn); EEPROM.write(264,eT5HOff);
     EEPROM.write(265,eT5MOff); EEPROM.write(134,eD2Pin);
     EEPROM.write(142,eD2Mode);
   
 } 

// T6
 if (val[0] == 26) {
   
   byte eD3Pin, eT6Status, eT6HOn, eT6MOn, eT6HOff, eT6MOff, eD3Mode ;
     
     if (val[1] >=0 && val[1] <= 99) { eD3Pin = val[1]; } else { return false;}           // eD3pin -    1  
     if (val[2] >=0 && val[2] <= 1)  { eT6Status = val[2]; } else { return false;}        // eT6Status - 2
     if (val[3] >=0 && val[3] <= 23) { eT6HOn = val[3]; } else { return false;}           // eT6HOn -    3
     if (val[4] >=0 && val[4] <= 59) { eT6MOn = val[4]; } else { return false;}           // eT6MOn -    4
     if (val[5] >=0 && val[5] <= 23) { eT6HOff = val[5]; } else { return false;}          // eT6HOff -   5
     if (val[6] >=0 && val[6] <= 59) { eT6MOff = val[6]; } else { return false;}          // eT6MOff -   6
     if (val[7] >=0 && val[7] <= 1)  { eD3Mode = val[7]; } else { return false;}          // eD3Mode -   7

     EEPROM.write(271,eT6Status); EEPROM.write(272,eT6HOn);
     EEPROM.write(273,eT6MOn); EEPROM.write(274,eT6HOff);
     EEPROM.write(275,eT6MOff); EEPROM.write(194,eD3Pin);
     EEPROM.write(202,eD3Mode);
   
 } 


 ///////////////////////////////////////////////////////////////////////////////////////
 /////                        PWM - 1, EPROM                                       ///// 
 ///////////////////////////////////////////////////////////////////////////////////////
  if (val[0] == 31) 
  { //      1          2         3        4        5        6          7         8        9       10       11      12         13
    byte ePwm1Pin, ePwm1Status, ePwm1HOn, ePwm1MOn, ePwm1SOn, ePwm1HOff, ePwm1MOff, ePwm1SOff, ePwm1Min, ePwm1Max, ePwm1Sr, ePwm1Ss, ePwm1KeepLight;
      
      if (val[1] >=0 && val[1] <= 99) { ePwm1Pin = val[1]; } else { return false;}             // ePwm1Pin -       1 
      if (val[2] >=0 && val[2] <= 1)  { ePwm1Status = val[2]; } else { return false;}          // ePwm1Status -    2
      if (val[3] >=0 && val[3] <= 23) { ePwm1HOn = val[3]; } else { return false;}             // ePwm1HOn -       3
      if (val[4] >=0 && val[4] <= 59) { ePwm1MOn = val[4]; } else { return false;}             // ePwm1MOn -       4
      if (val[5] >=0 && val[5] <= 59) { ePwm1SOn = val[5]; } else { return false;}             // ePwm1SOn -       5
      if (val[6] >=0 && val[6] <= 23) { ePwm1HOff = val[6]; } else { return false;}            // ePwm1HOff -      6
      if (val[7] >=0 && val[7] <= 59) { ePwm1MOff = val[7]; } else { return false;}            // ePwm1MOff -      7
      if (val[8] >=0 && val[8] <= 59) { ePwm1SOff = val[8]; } else { return false;}            // ePwm1SOff -      8
      if (val[9] >=0 && val[9] <= 255) { ePwm1Min = val[9]; } else { return false;}            // ePwm1Min -       9
      if (val[10] >=0 && val[10] <= 255) { ePwm1Max = val[10]; } else { return false;}         // ePwm1Max -       10
      if (val[11] >=0 && val[11] <= 255) { ePwm1Sr = val[11]; } else { return false;}          // ePwm1Sr -        11
      if (val[12] >=0 && val[12] <= 255) { ePwm1Ss = val[12]; } else { return false;}          // ePwm1Ss -        12
      if (val[13] >=0 && val[13] <= 1)   { ePwm1KeepLight = val[13]; } else { return false;}   // ePwm1KeepLight - 13
      
     EEPROM.write(281,ePwm1Pin); EEPROM.write(282,ePwm1Status); EEPROM.write(283,ePwm1HOn); EEPROM.write(284,ePwm1MOn);
     EEPROM.write(285,ePwm1SOn); EEPROM.write(286,ePwm1HOff); EEPROM.write(287,ePwm1MOff); EEPROM.write(288,ePwm1SOff);
     EEPROM.write(289,ePwm1Min); EEPROM.write(290,ePwm1Max); EEPROM.write(291,ePwm1Sr); EEPROM.write(292,ePwm1Ss);      
     EEPROM.write(293,ePwm1KeepLight); 
  }  


 ///////////////////////////////////////////////////////////////////////////////////////
 /////                        PWM - 2, EPROM                                       ///// 
 ///////////////////////////////////////////////////////////////////////////////////////
  if (val[0] == 32) 
  { //      1          2         3        4        5        6          7         8        9       10       11      12         13
    byte ePwm2Pin, ePwm2Status, ePwm2HOn, ePwm2MOn, ePwm2SOn, ePwm2HOff, ePwm2MOff, ePwm2SOff, ePwm2Min, ePwm2Max, ePwm2Sr, ePwm2Ss, ePwm2KeepLight;
      
      if (val[1] >=0 && val[1] <= 99) { ePwm2Pin = val[1]; } else { return false;}             // ePwm2Pin -       1 
      if (val[2] >=0 && val[2] <= 1)  { ePwm2Status = val[2]; } else { return false;}          // ePwm2Status -    2
      if (val[3] >=0 && val[3] <= 23) { ePwm2HOn = val[3]; } else { return false;}             // ePwm2HOn -       3
      if (val[4] >=0 && val[4] <= 59) { ePwm2MOn = val[4]; } else { return false;}             // ePwm2MOn -       4
      if (val[5] >=0 && val[5] <= 59) { ePwm2SOn = val[5]; } else { return false;}             // ePwm2SOn -       5
      if (val[6] >=0 && val[6] <= 23) { ePwm2HOff = val[6]; } else { return false;}            // ePwm2HOff -      6
      if (val[7] >=0 && val[7] <= 59) { ePwm2MOff = val[7]; } else { return false;}            // ePwm2MOff -      7
      if (val[8] >=0 && val[8] <= 59) { ePwm2SOff = val[8]; } else { return false;}            // ePwm2SOff -      8
      if (val[9] >=0 && val[9] <= 255) { ePwm2Min = val[9]; } else { return false;}            // ePwm2Min -       9
      if (val[10] >=0 && val[10] <= 255) { ePwm2Max = val[10]; } else { return false;}         // ePwm2Max -       10
      if (val[11] >=0 && val[11] <= 255) { ePwm2Sr = val[11]; } else { return false;}          // ePwm2Sr -        11
      if (val[12] >=0 && val[12] <= 255) { ePwm2Ss = val[12]; } else { return false;}          // ePwm2Ss -        12
      if (val[13] >=0 && val[13] <= 1)   { ePwm2KeepLight = val[13]; } else { return false;}   // ePwm2KeepLight - 13
      
     EEPROM.write(301,ePwm2Pin); EEPROM.write(302,ePwm2Status); EEPROM.write(303,ePwm2HOn); EEPROM.write(304,ePwm2MOn);
     EEPROM.write(305,ePwm2SOn); EEPROM.write(306,ePwm2HOff); EEPROM.write(307,ePwm2MOff); EEPROM.write(308,ePwm2SOff);
     EEPROM.write(309,ePwm2Min); EEPROM.write(310,ePwm2Max); EEPROM.write(311,ePwm2Sr); EEPROM.write(312,ePwm2Ss);      
     EEPROM.write(313,ePwm2KeepLight); 
  }  
 
 ///////////////////////////////////////////////////////////////////////////////////////
 /////                        PWM - 3, EPROM                                       ///// 
 ///////////////////////////////////////////////////////////////////////////////////////
  if (val[0] == 33) 
  { //      1          2            3        4          5           6         7         8        9       10       11      12         13
    byte ePwm3Pin, ePwm3Status, ePwm3HOn, ePwm3MOn, ePwm3SOn, ePwm3HOff, ePwm3MOff, ePwm3SOff, ePwm3Min, ePwm3Max, ePwm3Sr, ePwm3Ss, ePwm3KeepLight;
      
      if (val[1] >=0 && val[1] <= 99) { ePwm3Pin = val[1]; } else { return false;}             // ePwm3Pin -       1 
      if (val[2] >=0 && val[2] <= 1)  { ePwm3Status = val[2]; } else { return false;}          // ePwm3Status -    2
      if (val[3] >=0 && val[3] <= 23) { ePwm3HOn = val[3]; } else { return false;}             // ePwm3HOn -       3
      if (val[4] >=0 && val[4] <= 59) { ePwm3MOn = val[4]; } else { return false;}             // ePwm3MOn -       4
      if (val[5] >=0 && val[5] <= 59) { ePwm3SOn = val[5]; } else { return false;}             // ePwm3SOn -       5
      if (val[6] >=0 && val[6] <= 23) { ePwm3HOff = val[6]; } else { return false;}            // ePwm3HOff -      6
      if (val[7] >=0 && val[7] <= 59) { ePwm3MOff = val[7]; } else { return false;}            // ePwm3MOff -      7
      if (val[8] >=0 && val[8] <= 59) { ePwm3SOff = val[8]; } else { return false;}            // ePwm3SOff -      8
      if (val[9] >=0 && val[9] <= 255) { ePwm3Min = val[9]; } else { return false;}            // ePwm3Min -       9
      if (val[10] >=0 && val[10] <= 255) { ePwm3Max = val[10]; } else { return false;}         // ePwm3Max -       10
      if (val[11] >=0 && val[11] <= 255) { ePwm3Sr = val[11]; } else { return false;}          // ePwm3Sr -        11
      if (val[12] >=0 && val[12] <= 255) { ePwm3Ss = val[12]; } else { return false;}          // ePwm3Ss -        12
      if (val[13] >=0 && val[13] <= 1)   { ePwm3KeepLight = val[13]; } else { return false;}   // ePwm3KeepLight - 13

      
     EEPROM.write(331,ePwm3Pin); EEPROM.write(332,ePwm3Status); EEPROM.write(333,ePwm3HOn); EEPROM.write(334,ePwm3MOn);
     EEPROM.write(335,ePwm3SOn); EEPROM.write(336,ePwm3HOff); EEPROM.write(337,ePwm3MOff); EEPROM.write(338,ePwm3SOff);
     EEPROM.write(339,ePwm3Min); EEPROM.write(340,ePwm3Max); EEPROM.write(341,ePwm3Sr); EEPROM.write(342,ePwm3Ss);      
     EEPROM.write(343,ePwm3KeepLight); 
  }  
  
 ///////////////////////////////////////////////////////////////////////////////////////
 /////                    Refill, Feeding - EPROM                                  ///// 
 ///////////////////////////////////////////////////////////////////////////////////////
  if (val[0] == 34) 
  { 
    byte eFeedingPin, eFeedingBtnPin, eFeedingTime, eFeedingMode, eRefillPin, eRefillBtnPin, eRefillDelayStart;
    
      if (val[1] >=0 && val[1] <= 99) { eFeedingPin = val[1]; } else { return false;}                 // eFeedingPin -        1 
      if (val[2] >=0 && val[2] <= 99)  { eFeedingBtnPin = val[2]; } else { return false;}             // eFeedingBtnPin -     2
      if (val[3] >=0 && val[3] <= 255) { eFeedingTime = val[3]; } else { return false;}               // eFeedingTime -       3
      if (val[3] >=0 && val[4] <= 1) { eFeedingMode = val[4]; } else { return false;}                 // eFeedingMode -       4
      if (val[4] >=0 && val[5] <= 99) { eRefillPin = val[5]; } else { return false;}                  // eRefillPin -         5
      if (val[5] >=0 && val[6] <= 99) { eRefillBtnPin = val[6]; } else { return false;}               // eRefillBtnPin -      6
      if (val[6] >=0 && val[7] <= 255) { eRefillDelayStart = val[7]; } else { return false;}          // eRefillDelayStart -  7
      
      EEPROM.write(351,eFeedingPin); EEPROM.write(352,eFeedingBtnPin); EEPROM.write(353,eFeedingTime); EEPROM.write(354,eFeedingMode); EEPROM.write(361,eRefillPin);
      EEPROM.write(362,eRefillBtnPin); EEPROM.write(363,eRefillDelayStart);
    
  }  
    
  
 /////////////////////////////////////////////////////////////////////////////////////////////
 /////                        Set Date                                                   /////
 /////////////////////////////////////////////////////////////////////////////////////////////
 byte setHour, setMinute, setSecond, setYear, setMonth, setDay;
 
 if (val[0] == 40) {
   if (val[1] >=0 && val[1] <= 23) { setHour = val[1]; } else { return false;}           // setHour -   1
   if (val[2] >=0 && val[2] <= 59) { setMinute = val[2]; } else { return false;}         // setMinute - 2
   if (val[3] >=0 && val[3] <= 59) { setSecond = val[3]; } else { return false;}         // setSecond - 3
   if (val[4] >=15 && val[4] <=99 ) { setYear = val[4]+2000; } else { return false;}     // setYear -   4
   if (val[5] >=1 && val[5] <=99 ) { setMonth = val[5]; } else { return false;}          // setMonth -  5
   if (val[6] >=1 && val[6] <=31 ) { setDay = val[6]; } else { return false;}            // setDay -    5
     
   setTime(setHour,setMinute,setSecond,setDay,setMonth,setYear); // godz, min, sek, dzien, miesiac, rok

   RTC.set(now());
   return true;
 }


 ///////////////////////////////////////////////////////////////////////////////////////////
 /////                       Get Settings                                              /////
 ///////////////////////////////////////////////////////////////////////////////////////////
 if (val[0] == 66 ) 
 {
   int eAddress = 0;
   Serial.write("*66,");
   
   while (eAddress <= 512) 
    {
        Serial.print(EEPROM.read(eAddress));
        Serial.write(",");
        eAddress++;
    }
  Serial.write("#\n");
 }

 ///////////////////////////////////////////////////////////////////////////////////////////
 /////                       Get Date                                                  /////
 ///////////////////////////////////////////////////////////////////////////////////////////
 if (val[0] == 83 ) 
 {
   Serial.print("*83");
   Serial.write(",");
   Serial.print(tm.Hour);
   Serial.write(",");
   Serial.print(tm.Minute);
   Serial.write(",");
   Serial.print(tm.Second);
   Serial.write(",");
   Serial.print(tmYearToCalendar(tm.Year));
   Serial.write(",");
   Serial.print(tm.Month);
   Serial.write(",");
   Serial.print(tm.Day);
   Serial.println();
    
 }

if (val[0] == 80) 
{
  Serial.println("AQma, version 1.1.2");
}

 
} 


