## All Copyrights reserved. alvin.cpp@gmail.com , 2016
## 
## The Program listens to Redis channel (http://redis.io/) and observes any requests to display/increment numbers that
## issues a signals to machine (in this case arduino UNO https://www.arduino.cc/) via serial.
##
import redis
import time
import serial
import json

with open('.config') as df:
	CONFIG=json.load(df)

RQUEUE_NUMDISP=CONFIG["RQUEUE_NUMDISP"]
RQUEUE_NUMADD=CONFIG["RQUEUE_NUMADD"]
RQUEUE_RESP_PREFIX=CONFIG["RQUEUE_RESP_PREFIX"]
SERIAL_PORT=CONFIG["SERIAL_PORT"]


## num is a number or and single character '+'
def DisplayNumber(ser, num, retry):
        if retry < 0 :
                print('cannot have retry negative!!')
                return None
        while retry > 0:
                retry = retry - 1
                ser.flushInput()
                ser.write(str(num) + '\n')
                rnum = ser.readline()
                if len(rnum) > 0 and rnum.strip().isdigit():
                        print('set '+ rnum.strip() )
                        return int(rnum.strip())
        return None

r = redis.StrictRedis()
p = r.pubsub()

p.subscribe(RQUEUE_NUMDISP,RQUEUE_NUMADD)

ser = serial.Serial(port=SERIAL_PORT,baudrate=115200,timeout=5)
time.sleep(2)

try:
	for message in p.listen():
		if message is not None and message['type'] == 'message':
			mdata = message['data']
			mchan = message['channel']
			if mchan == RQUEUE_NUMDISP:
				if mdata is not None and len(mdata) > 0 and mdata.isdigit():
					DisplayNumber(ser,mdata,2)
			if mchan == RQUEUE_NUMADD:
				currentnum = DisplayNumber(ser,'+',1)
				if currentnum is not None and mdata is not None and len(mdata) > 0:
					print('resp '+mdata+' '+str(currentnum))
					r.set(RQUEUE_RESP_PREFIX+mdata,currentnum)
except:
	pass
finally:
	ser.close()
