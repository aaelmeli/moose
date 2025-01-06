#!/usr/bin/env python
#write three columns data here
#def rst_render():
from __future__ import print_function
import random
import six
seed = 0
with open("fake_app_input", "r") as input:
    seed = int(input.readline())
if six.PY3:
 random.seed(seed,version=1)
else:
 random.seed(seed)
def rnd_string(iCol):
    strLine=""
    for x in range(iCol):
        if six.PY3:
         strLine=strLine + str((int(random.random()*101)+1)*(int(random.random()*(x+2))+1)*0.25) + " "
        else: 
         strLine=strLine + str(random.randint(1,101)*random.randint(1,x+2)*0.25) + " "
    return strLine + "\n"

tmpFile = open("fake_driver_app.csv","w")
tmpFile.write("x: "+rnd_string(3)) 
tmpFile.write("v: "+rnd_string(2)) 
tmpFile.write("x: "+rnd_string(3)) 
tmpFile.write("Table Header\n") 
tmpFile.write(rnd_string(4)) 
tmpFile.write(rnd_string(4)) 
tmpFile.write(rnd_string(4)) 
tmpFile.write("Table Footer\n") 
tmpFile.write(rnd_string(4))
tmpFile.close()

