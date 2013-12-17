#include <FrequencyTimer2.h>
#include "frames.h"

#define NUM_COLS 8
#define NUM_ROWS 7
#define SWITCH_PIN 10
int row_pins[NUM_ROWS] = {A0, A1, A2, A3, A4, A5, 13};
int col_pins[NUM_COLS] = {2, 3, 4, 5, 6, 7, 8, 9};
byte current_row = 0;
int current_frame = 0;
int current_sequence = 0;

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

LedSequence sequence_list[NUM_SEQUENCES] =
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
    pinMode(row_pins[i], OUTPUT);
    digitalWrite(row_pins[i], HIGH);
  }

  for (int i = 0; i < NUM_COLS; i++)
  {
    pinMode(col_pins[i], OUTPUT);
    digitalWrite(col_pins[i], LOW);
  }
  
  pinMode(SWITCH_PIN, INPUT);
  digitalWrite(SWITCH_PIN, HIGH);
  current_frame = 0;
  
  FrequencyTimer2::disable();
  FrequencyTimer2::setPeriod(2000);
  FrequencyTimer2::setOnOverflow(display);

}

// Interrupt routine
void display() {
  
  digitalWrite(row_pins[current_row], HIGH);  // Turn whole previous column off
  current_row++;
  
  if (current_row == NUM_ROWS) {
    current_row = 0;
  }
  uint8_t row_byte = *(sequence_list[current_sequence].seq_p + (current_frame*NUM_ROWS) + current_row);
  
  if (current_row == (NUM_ROWS - 1))
  {
    //Top row only has one LED
   // if (frames[current_frame][current_row])
    if (row_byte)    
    {
      digitalWrite(row_pins[current_row], LOW); // Turn whole column on at once (for equal lighting times)     
    }
    else
    {
      digitalWrite(row_pins[current_row], HIGH); // Turn whole column on at once (for equal lighting times)
    }
    
  }
  else
  {
    for (int i = 0; i < NUM_COLS; i++)
    {
      //if ((frames[current_frame][current_row] >> i) & 0x01)
      if ((row_byte >> i) & 0x01)
      {
        digitalWrite(col_pins[i], HIGH);
      }
      else
      {
        digitalWrite(col_pins[i], LOW);
      }
    }
    digitalWrite(row_pins[current_row], LOW); // Turn whole column on at once (for equal lighting times)
  }
}



long frameTimer_prev = 0; 
long sequenceTimer_duration = 5000;
long sequenceTimer_prev = 0; 
long frameTimer_duration = 100;
long switchTimer_start = 0;
long switchTimer_elapsed = 0;


int button_state = HIGH;
int prev_button_state = HIGH;
int idleMode = 0;


void loop()
{
  unsigned long currentMillis = millis();
  
  prev_button_state = button_state;
  button_state = digitalRead(SWITCH_PIN);
  
  //button pressed
  if ((button_state == LOW) && (prev_button_state == HIGH))
  {
    //start timer
    switchTimer_start = currentMillis;
    
  }

  //button released
  if ((button_state == HIGH) && (prev_button_state == LOW))
  {
    switchTimer_elapsed = currentMillis - switchTimer_start;
    
    if (switchTimer_elapsed < 100)
    {
      //debouncing
    }
    else if (switchTimer_elapsed < 1000)
    {
      //Short Press
    }
    else
    {
      //Long Press: toggle in and out of idle mode
      if (idleMode == 1)
      {
        idleMode = 0;
      }
      else
      {
        idleMode = 1;
      } 
    }
  }
  
  
  if(currentMillis - sequenceTimer_prev > sequenceTimer_duration)
  {
    sequenceTimer_prev = currentMillis;   
    
    current_frame = 0;
    
    if (current_sequence == NUM_SEQUENCES)
    {
      current_sequence = 0;
    }
    else
    {
      current_sequence++;
    }
  }
 
  if(currentMillis - frameTimer_prev > frameTimer_duration)
  {
    frameTimer_prev = currentMillis;   
    
    if (current_frame == sequence_list[current_sequence].num_frames)
    {
      current_frame = 0;
    }
    else
    {
      current_frame++;
    }
  }
  
  

}

