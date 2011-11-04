#!/usr/bin/python

import serial
import Gnuplot
import numpy
import time

dev = serial.Serial('/dev/ttyUSB0', 115200, timeout=1)
time.sleep(0.5)
dev.flush()

start_time = time.time()
g = Gnuplot.Gnuplot()
g.xlabel('time')
g.ylabel('data')

PARA_NUM = 4
plot_data = numpy.zeros((275, PARA_NUM))

timebase = 0.0
while True:
    now = time.time()
    
    raw = dev.readline().strip()
    if not raw:
        print 'no data'
        continue

    #print repr(new_data)
    try:
        new_data = map(float, raw.split('|'))
        new_data = [new_data[0]] + [new_data[1] - new_data[2]] + new_data[3:]
        print raw
    except:
        # print '!!!!!!!!!!!!!!!! error data'
        continue
        
    if len(new_data) == PARA_NUM:
        timebase += new_data[0] / 1000
        new_data[0] = timebase
    else:
        print 'PARA_NUM failed'
        continue
        
    #new_data.insert(0, now-start_time)

    plot_data = numpy.concatenate( (plot_data[1:], numpy.array([new_data])) )


    plot_exec = ''
    for i in xrange(PARA_NUM-1):
        exec('plot_%d = Gnuplot.Data(plot_data, with_="line", using="1:%d", title="%d")' %(i+1, i+2, i))
        plot_exec += 'plot_%d,' %(i+1)
    exec('g.plot(%s)' %(plot_exec))
    #plot_1 = Gnuplot.Data(plot_data, with_='lines', using='1:2')
    #plot_2 = Gnuplot.Data(plot_data, with_='lines', using='1:3')
    #g.plot(plot_1, plot_2)
    
raw_input()
