#!/usr/bin/python
# -*- coding: iso-8859-1 -*-
import tkMessageBox
import Tkinter
import time
import bitstring
from numpy import *

num_rows = 7
num_cols = 8

class Led():
  def __init__(self, parent, _row, _col):
    self.row = _row
    self.col = _col
    self.tk_value = Tkinter.IntVar()
    self.tk_cb = Tkinter.Checkbutton(parent, variable = self.tk_value) #, command = parent.update_checkboxes)
    self.tk_cb.grid(row=_row, column=_col)

  def update_box(self, value):
    if value:
      self.tk_value.set(1)
      self.tk_cb.select()
    else:
      self.tk_value.set(0)
      self.tk_cb.deselect()

class simpleapp_tk(Tkinter.Tk):
  def __init__(self,parent):
    self.output_file = open('frames.h','w')
    self.sequence_index = 0
    self.sequence_list = ""
    self.all_sequences = ""
    self.sequence_string = ""
    self.num_frames = 0
    Tkinter.Tk.__init__(self,parent)
    self.parent = parent
    self.tmp_byte = bitstring.BitArray('0x00')
    self.wrap = True
    self.grid()
    self.checkboxes = [[0 for x in xrange(num_cols)] for y in xrange(num_rows)] 
    self.led_array = zeros((num_rows, num_cols), dtype=uint8)

    for _row in range(num_rows):
      for _col in range(num_cols):
        self.checkboxes[_row][_col] = Led(self, _row, _col)

    button = Tkinter.Button(self,text=u"save_frame", command=self.save_frame)
    button.grid(column=9,row=0)
    button = Tkinter.Button(self,text=u"save_sequence", command=self.save_sequence)
    button.grid(column=10,row=0)
    button = Tkinter.Button(self,text=u"generate_file", command=self.generate_file)
    button.grid(column=11,row=0)
    button = Tkinter.Checkbutton(self,text=u"wrap", command=self.wrap_button)
    button.grid(column=10,row=1)
    button = Tkinter.Button(self,text=u"shift up", command=self.shift_up)
    button.grid(column=10,row=2)
    button = Tkinter.Button(self,text=u"shift down", command=self.shift_down)
    button.grid(column=10,row=4)
    button = Tkinter.Button(self,text=u"shift left", command=self.shift_left)
    button.grid(column=9,row=3)
    button = Tkinter.Button(self,text=u"shift right", command=self.shift_right)
    button.grid(column=11,row=3)
    button = Tkinter.Button(self,text=u"invert", command=self.invert)
    button.grid(column=10,row=3)

  def save_sequence(self):
    self.sequence_string = self.sequence_string[0:len(self.sequence_string)-2]
    self.sequence_string =  ("\nstatic const uint8_t frame_%d[%d][NUM_COLS] =\n{\n" % (self.sequence_index, self.num_frames)) + self.sequence_string + "\n};"
    self.all_sequences += self.sequence_string
#    print self.sequence_string
    self.sequence_string = ""
    self.sequence_index += 1
    self.sequence_list += "  { &frame_%d[0][0], %d },\n" % ((self.sequence_index-1), self.num_frames)
    self.num_frames = 0

  def save_frame(self):
    self.update_led_array()
    a = packbits(self.led_array)
    header_str = "  {"
    for b in a:
      header_str += "0x%02x, " % b
    header_str = header_str[0:len(header_str)-2]
    header_str += "},\n"
    self.num_frames += 1
    self.sequence_string +=  header_str
    
  def generate_file(self):
   self.sequence_list = self.sequence_list[0:len(self.sequence_list)-2]
   #print self.all_sequences
   self.output_file.write(self.all_sequences)
   self.output_file.write("\n\n#define NUM_SEQUENCES %d\n" % self.sequence_index);
   self.output_file.write("\n\ntypedef struct\n{\n    const uint8_t* seq_p;\n    int num_frames;\n} LedSequence;\n\nLedSequence sequence_list[NUM_SEQUENCES] =\n  {\n")
   self.output_file.write( self.sequence_list)
   self.output_file.write("\n};\n\n")
   self.output_file.close()

  def update_checkboxes(self):
    for _row in reversed(range(num_rows)):
      for _col in range(num_cols):
        if self.led_array[_row, _col]:
          self.checkboxes[_row][_col].update_box(1)
        else:
          self.checkboxes[_row][_col].update_box(0)

  def update_led_array(self):
    for _row in range(num_rows):
      for _col in range(num_cols):
        self.led_array[(_row, _col)] = self.checkboxes[_row][_col].tk_value.get()

  def shift_down(self):
    self.update_led_array()
    self.led_array = roll(self.led_array,1, axis = 0)
    #update the boxes (gui)
    if not self.wrap:
      self.led_array[0,:] = 0
    self.update_checkboxes()

  def shift_up(self):
    #ensure the led_array is up to date
    self.update_led_array()
    self.led_array = roll(self.led_array,(num_rows - 1), axis = 0)
    #update the boxes (gui)
    if not self.wrap:
      self.led_array[(num_rows - 1),:] = 0
    self.update_checkboxes()

  def shift_left(self):
    #ensure the led_array is up to date
    self.update_led_array()
    self.led_array = roll(self.led_array,(num_cols - 1), axis = 1)
    #update the boxes (gui)
    if not self.wrap:
      self.led_array[(num_cols - 1),:] = 0
    self.update_checkboxes()

  def shift_right(self):
    #ensure the led_array is up to date
    self.update_led_array()
    self.led_array = roll(self.led_array,1, axis = 1)
    #update the boxes (gui)
    if not self.wrap:
      self.led_array[(num_cols - 1),:] = 0
    self.update_checkboxes()

  def wrap_button(self):
    self.wrap != self.wrap

  def invert(self):
    for _row in range(num_rows):
      for _col in range(num_cols):
        self.checkboxes[_row][_col].tk_value.set(not self.checkboxes[_row][_col].tk_value.get())

    
if __name__ == "__main__":
  app = simpleapp_tk(None)
  app.title('led generator')
  app.mainloop()

