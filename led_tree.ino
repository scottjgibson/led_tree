#include <FrequencyTimer2.h>
#include "frames.h"

#define NUM_COLS 8
#define NUM_ROWS 7
#define SWITCH_PIN 10
int rowPins[NUM_ROWS] = {A0, A1, A2, A3, A4, A5, 13};
int columnPins[NUM_COLS] = {2, 3, 4, 5, 6, 7, 8, 9};
byte currentRow = 0;
int currentFrame = 0;
int currentSequence = 0;

#define IDLE_MODE 0
#define SEQUENCE_MODE 1
#define AUTO_MODE 2

int displayMode = 0;

#define NUM_FRAMES 166

#define FRAME_SIZE NUM_ROWS

static const uint8_t frame_0[8][NUM_COLS] =
{
  {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
  {0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40},
  {0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20},
  {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10},
  {0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08},
  {0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04},
  {0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02},
  {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01}
};
static const uint8_t frame_1[8][NUM_COLS] =
{
  {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02},
  {0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01},
  {0x20, 0x10, 0x08, 0x04, 0x02, 0x01, 0x80},
  {0x10, 0x08, 0x04, 0x02, 0x01, 0x80, 0x40},
  {0x08, 0x04, 0x02, 0x01, 0x80, 0x40, 0x20},
  {0x04, 0x02, 0x01, 0x80, 0x40, 0x20, 0x10},
  {0x02, 0x01, 0x80, 0x40, 0x20, 0x10, 0x08},
  {0x01, 0x80, 0x40, 0x20, 0x10, 0x08, 0x04}
};
static const uint8_t frame_2[14][NUM_COLS] =
{
  {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02},
  {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x04},
  {0x80, 0x40, 0x20, 0x10, 0x08, 0x08, 0x08},
  {0x80, 0x40, 0x20, 0x10, 0x10, 0x10, 0x10},
  {0x80, 0x40, 0x20, 0x20, 0x20, 0x20, 0x20},
  {0x80, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40},
  {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
  {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x01},
  {0x80, 0x80, 0x80, 0x80, 0x80, 0x01, 0x02},
  {0x80, 0x80, 0x80, 0x80, 0x01, 0x02, 0x04},
  {0x80, 0x80, 0x80, 0x01, 0x02, 0x04, 0x08},
  {0x80, 0x80, 0x01, 0x02, 0x04, 0x08, 0x10},
  {0x80, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20},
  {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40}
};

#define NUM_SEQUENCES 3


typedef struct
{
    const uint8_t* seq_p;
    int num_frames;
} LedSequence;

LedSequence sequenceList[NUM_SEQUENCES] =
  {
  { &frame_0[0][0], 8 },
  { &frame_1[0][0], 8 },
  { &frame_2[0][0], 14 }
};

;

void setup()
{
     
  for (int i = 0; i < NUM_ROWS; i++)
  {
    pinMode(rowPins[i], OUTPUT);
    digitalWrite(rowPins[i], HIGH);
  }

  for (int i = 0; i < NUM_COLS; i++)
  {
    pinMode(columnPins[i], OUTPUT);
    digitalWrite(columnPins[i], LOW);
  }
  
  pinMode(SWITCH_PIN, INPUT);
  digitalWrite(SWITCH_PIN, HIGH);
  currentFrame = 0;
  
  FrequencyTimer2::disable();
  FrequencyTimer2::setPeriod(2000);
  FrequencyTimer2::setOnOverflow(display);

}

// Interrupt routine
void display() {
  
  digitalWrite(rowPins[currentRow], HIGH);  // Turn whole previous column off
  currentRow++;
  
  if (currentRow == NUM_ROWS) {
    currentRow = 0;
  }
  uint8_t row_byte = *(sequenceList[currentSequence].seq_p + (currentFrame*NUM_ROWS) + currentRow);
  
  if (displayMode == IDLE_MODE)
  {
    //IDLE MODE - All LEDs on
    for (int i = 0; i < NUM_COLS; i++)
    {
      digitalWrite(columnPins[i], HIGH);
    }
    digitalWrite(rowPins[currentRow], LOW); // Turn whole column on at once (for equal lighting times)
  
  }
  else
  {
    if (currentRow == (NUM_ROWS - 1))
    {
      //Top row only has one LED
     // if (frames[currentFrame][currentRow])
      if (row_byte)    
      {
        digitalWrite(rowPins[currentRow], LOW); // Turn whole column on at once (for equal lighting times)     
      }
      else
      {
        digitalWrite(rowPins[currentRow], HIGH); // Turn whole column on at once (for equal lighting times)
      }
      
    }
    else
    {
      for (int i = 0; i < NUM_COLS; i++)
      {
        //if ((frames[currentFrame][currentRow] >> i) & 0x01)
        if ((row_byte >> i) & 0x01)
        {
          digitalWrite(columnPins[i], HIGH);
        }
        else
        {
          digitalWrite(columnPins[i], LOW);
        }
      }
      digitalWrite(rowPins[currentRow], LOW); // Turn whole column on at once (for equal lighting times)
    }
  }
}



long frameTimer_prev = 0; 
long sequenceTimer_duration = 5000;
long sequenceTimer_prev = 0; 
long frameTimer_duration = 100;
long switchTimer_start = 0;
long switchTimer_elapsed = 0;


int buttonState = HIGH;
int buttonState_prev = HIGH;



void loop()
{
  unsigned long currentMillis = millis();
  
  buttonState_prev = buttonState;
  buttonState = digitalRead(SWITCH_PIN);
  
  //button pressed
  if ((buttonState == LOW) && (buttonState_prev == HIGH))
  {
    //start timer
    switchTimer_start = currentMillis;
    
  }

  //button released
  if ((buttonState == HIGH) && (buttonState_prev == LOW))
  {
    switchTimer_elapsed = currentMillis - switchTimer_start;
    
    if (switchTimer_elapsed < 50)
    {
      //debouncing
    }
    else if (switchTimer_elapsed < 300)
    {
      currentFrame = 0;
      if (currentSequence == NUM_SEQUENCES)
      {
        currentSequence = 0;
      }
      else
      {
        currentSequence++;
      }
    }
    else if (switchTimer_elapsed < 1000)
    {
      //Short Press
      if (displayMode != IDLE_MODE)
      {
        displayMode = IDLE_MODE;
      }
      else
      {
        displayMode = SEQUENCE_MODE;
      } 
    }
    else
    {
      displayMode = AUTO_MODE;
    }
  }
  
  
  if(currentMillis - sequenceTimer_prev > sequenceTimer_duration)
  {
    sequenceTimer_prev = currentMillis;  
   
   if (displayMode == AUTO_MODE)
   {   
      currentFrame = 0;
      if (currentSequence == NUM_SEQUENCES)
      {
        currentSequence = 0;
      }
      else
      {
        currentSequence++;
      }
   }
  }
 
  if(currentMillis - frameTimer_prev > frameTimer_duration)
  {
    frameTimer_prev = currentMillis;   
    
    if (currentFrame == sequenceList[currentSequence].num_frames)
    {
      currentFrame = 0;
    }
    else
    {
      currentFrame++;
    }
  }
  
  

}

