#!/usr/bin/python

import serial
import Gnuplot
import numpy
import time
import atexit

def close_dump():
    print 'close dump'
    dev.write('[accl,dump,0]')

atexit.register(close_dump)

dev = serial.Serial('/dev/ttyUSB0', 576000, timeout=1)
time.sleep(0.5)
dev.write('[accl,dump]')

dev.flush()

start_time = time.time()
g = Gnuplot.Gnuplot()
g.xlabel('time')
g.ylabel('data')

PARA_NUM = 4
plot_data = numpy.zeros((32, PARA_NUM))

timebase = 0.0
while True:
    now = time.time()
    
    raw = dev.readline().strip()
    if not raw:
        print 'no data'
        continue

    #print repr(new_data)
    try:
        new_data = raw.split(':')[1]
        #new_data = new_data.split('|')
        new_data = map(int, new_data.split('|'))
    except Exception ,ex:
        print '!!!!!!!!!!!!!!!! error data', raw, ex
        continue
        
    if len(new_data) == PARA_NUM-1:
        timebase += 0.01
        new_data.insert(0, timebase)
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
