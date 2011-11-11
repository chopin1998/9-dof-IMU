#!/usr/bin/python

import string

AX_OFFSET = 15

def main():
    data = open('data').readlines()
    data = map(string.split, map(string.strip, data))

    S = 0.0
    Vi = 0.0
    dT = 0.0

    last_elapse = 0.0
    print 'elpase', '\t\t', 'dt', '\t\t', 'ax', '\t\t', 'Vi', '\t\t', 'S', '\t\t'
    for elapse, ax, qd, qd_delta in data:
        #continue

        elapse = float(elapse)
        ax = int(ax) - AX_OFFSET

        dT = elapse - last_elapse
        last_elapse = elapse        
        
        Vi += ax * dT

        S += Vi * dT + 0.5 * (ax-AX_OFFSET) * (dT**2)

        #print round(elapse,4), '\t\t', round(dT,4), '\t\t', ax, '\t\t', round(Vi,3), '\t\t', round(S,4), '\t\t'

    print 's ->', S




################################
if __name__ == '__main__':

    main()
