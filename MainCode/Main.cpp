#include <Adafruit_LiquidCrystal.h>

Adafruit_LiquidCrystal lcd_1(0);

int tonePin = 2;
int toneFreq = 1000;
int ledPin = 13;
int buttonPin = 8;
int debounceDelay = 30;

int dotLength = 100; 
// dotLength = basic unit of speed in milliseconds
// 240 gives 5 words per minute (WPM) speed.
// WPM = 1200/dotLength.
// For other speeds, use dotLength = 1200/(WPM)
//
// Other lengths are computed from dot length
  int dotSpace = dotLength;
  int dashLength = dotLength*3;
  int letterSpace = dotLength*3;
  int wordSpace = dotLength*7; 
  float wpm = 1200./dotLength;
  
int t1, t2, onTime, gap;
bool newLetter, newWord, letterFound, keyboardText;
int lineLength = 0;
int maxLineLength = 20; 

char* letters[] = 
{
".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", // A-I
".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", // J-R 
"...", "-", "..-", "...-", ".--", "-..-", "-.--", "--.." // S-Z
};

char* numbers[] = 
{
"-----", ".----", "..---", "...--", "....-", //0-4
".....", "-....", "--...", "---..", "----." //5-9
};

String dashSeq = "";
char keyLetter, ch;
int i, index;

void setup() 
{
  delay(500);
  pinMode(ledPin, OUTPUT);
  pinMode(tonePin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  Serial.begin(9600);
  Serial.println();
  Serial.println("-------------------------------");
  Serial.println("Morse Code decoder/encoder");
  Serial.print("Speed=");
  Serial.print(wpm);
  Serial.print("wpm, ");
  Serial.print("dot=");
  Serial.print(dotLength);
  Serial.println("ms");   
  
  // Test the LED and tone
  tone(tonePin, toneFreq);
  digitalWrite(ledPin, HIGH);
  delay(2000);
  digitalWrite(ledPin, LOW);
  noTone(tonePin);
  delay(600);

  // Initialize the LCD
   lcd_1.begin(16, 2);
  lcd_1.clear();
  lcd_1.print("Morse Code");
  delay(2000);
  lcd_1.clear();
  
  newLetter = false;
  newWord = false;
  keyboardText = false;
}


void loop() 
{
// Check to see if something has been entered on the keyboard
  if (Serial.available() > 0)
  {
    if (keyboardText == false) 
    {
      Serial.println();
      Serial.println("-------------------------------");
    }
    keyboardText = true;
    ch = Serial.read();
    if (ch >= 'a' && ch <= 'z')
    { ch = ch-32; }
    
    if (ch >= 'A' && ch <= 'Z')
    {
      Serial.print(ch); 
      Serial.print(" ");
      Serial.println(letters[ch-'A']);
      flashSequence(letters[ch-'A']);
      delay(letterSpace);
    }
    if (ch >= '0' && ch <= '9')
    {
      Serial.print(ch);
      Serial.print(" ");
      Serial.println(numbers[ch-'0']);
      flashSequence(numbers[ch-'0']);
      delay(letterSpace);
    }
    if (ch == ' ')
    {
      Serial.println("_");
      delay(wordSpace);    
    } 

// Print a header after last keyboard text    
     if (Serial.available() <= 0) 
     {
      Serial.println();
      Serial.println("Enter text or Key in:");
      Serial.println("-------------------------------");
      keyboardText = false;
     }
  }
 
  if (digitalRead(buttonPin) == LOW ) //button is pressed
  {
    newLetter = true; 
    newWord = true;
    t1=millis(); //time at button press
    digitalWrite(ledPin, HIGH); //turn on LED and tone
    tone(tonePin, toneFreq);
    delay(debounceDelay);     
    while (digitalRead(buttonPin) == LOW ) // wait for button release
      {delay(debounceDelay);}
      delay(debounceDelay);
       
    t2 = millis();  //time at button release
    onTime=t2-t1;  //length of dot or dash keyed in
    digitalWrite(ledPin, LOW); //torn off LED and tone
    noTone(tonePin); 
    
//check if dot or dash 

    if (onTime <= dotLength*1.5) //allow for 50% longer 
      {dashSeq = dashSeq + ".";} //build dot/dash sequence
    else 
      {dashSeq = dashSeq + "-";}
  }  //end button press section
  
// look for a gap >= letterSpace to signal end letter
// end of letter when gap >= letterSpace

  gap=millis()-t2; 
  if (newLetter == true && gap>=letterSpace)  
  { 
    
//check through letter sequences to find matching dash sequence

    letterFound = false; keyLetter = 63; //char 63 is "?"
    for (i=0; i<=25; i++)
    {
      if (dashSeq == letters[i]) 
      {
        keyLetter = i+65;
        letterFound = true;   
        break ;    //don't keep checking if letter found  
      }
    }
    if(letterFound == false) //now check for numbers
    {
      for (i=0; i<=10; i++)
      {
      if (dashSeq == numbers[i]) 
        {
          keyLetter = i+48;
          letterFound = true;   
          break ;    //don't keep checking if number found  
        }
      }
    }    
    lcd_1.print(keyLetter);
    if(letterFound == false) //buzz for unknown key sequence
    {
      tone(tonePin, 100, 500);
    }  
    newLetter = false; //reset
    dashSeq = "";
    lineLength=lineLength+1;
  }  
  
// keyed letter has been identified and printed

// when gap is >= wordSpace, insert space between words
// lengthen the word space by 50% to allow for variation

  if (newWord == true && gap>=wordSpace*1.5)
    { 
     newWord = false; 
     Serial.print("_");  
     lineLength=lineLength+1;
     
// flash to indicate new word

    digitalWrite(ledPin, HIGH);
    delay(25);
    digitalWrite(ledPin, LOW);       
    } 

// insert linebreaks

  if (lineLength >= maxLineLength) 
    {
      Serial.println();
      lineLength = 0;
    }      
} 

void flashSequence(char* sequence)
{
  int i = 0;
  while (sequence[i] == '.' || sequence[i] == '-')
  {
    flashDotOrDash(sequence[i]);
    i++;
  }
}

void flashDotOrDash(char dotOrDash)
{
  digitalWrite(ledPin, HIGH);
  tone(tonePin, toneFreq);
  if (dotOrDash == '.')
   { delay(dotLength); }
     else
   { delay(dashLength); }

  digitalWrite(ledPin, LOW);
  noTone(tonePin);
  delay(dotLength); 
}
//--- end of sketch ---
