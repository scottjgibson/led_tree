#!/usr/bin/python
# -*- coding: iso-8859-1 -*-
import tkMessageBox
import Tkinter
import time

class simpleapp_tk(Tkinter.Tk):
  def __init__(self,parent):
    Tkinter.Tk.__init__(self,parent)
    self.parent = parent
    self.initialize()

  def initialize(self):
    self.grid()

    self.checkboxes = [[0 for x in xrange(8)] for x in xrange(8)] 

    for _row in range(8):
      for _column in range(8):
        self.checkboxes[_row][_column] = Tkinter.IntVar()
        c = Tkinter.Checkbutton(self, variable = self.checkboxes[_row][_column])
        c.grid(row=_row, column=_column)

    button = Tkinter.Button(self,text=u"Go")
    button.grid(column=9,row=9)

    
if __name__ == "__main__":
  app = simpleapp_tk(None)
  app.title('my application')
  app.mainloop()

