// code for plotting 
import serial
import time
import matplotlib.pyplot as plt
import numpy as np
# open  serial port
serialObj = serial.Serial('COM4', 4800, timeout=1)

# initializing time and create empty lists for Time and Y values for plotting
time_val = 0
Time = []
Y = []

#  interactive mode on for continuous display of the graph
plt.ion()

# initializing min and max values
min_value = float('inf')
max_value = float('-inf')

while True:
# read a line of text from the serial port and convert it to a float
x = int(serialObj.readline().decode('utf-8').strip())
# calculate the new value of y
y = 0.09 * x + 23
comparison_value = y
if comparison_value < min_value:
min_value = comparison_value
if comparison_value > max_value:
max_value = comparison_value

# write the y(here y is nominal value) value to the serial port
# for displaying max or minimum we can use max_value and min_value instead of y in the given below code
serialObj.write(str(y).encode())
# print the result
print(f"y = {y}")
# adding the current time and y value to the lists for plotting
Time.append(time_val)
Y.append(y)
# plot the values
plt.plot(Time, Y)
plt.draw()
plt.pause(0.001)
# increment time_val by 1, time is increasing that is being displayed on the x axis
time_val += 1
# waiting for 1 second before repeating, so it doesnt create a load
time.sleep(1)
