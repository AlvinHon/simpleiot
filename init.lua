-- applicatioh setup
wifi.setmode(wifi.STATION)
wifi.sta.config("NodesCenter","N3tw0rkMcu")
DETECTPIN = 2
DETECTPIN2 = 3
MAINLEDPIN = 4  -- by default
LEDPIN = 1

gpio.mode(LEDPIN,gpio.OUTPUT)
gpio.mode(DETECTPIN,gpio.INPUT)
gpio.mode(DETECTPIN2,gpio.INPUT)

-- functions

-- led

function blinkLed(lpin,freqhz)
    pwm.setup(lpin,freqhz,512)
    pwm.start(lpin)
end

function turnLed(lpin,ison,toggle)
    pwm.stop(lpin)
    pwm.close(lpin)    
    if (ison and toggle) or (not ison and not toggle) then
        gpio.write(lpin,gpio.LOW)
    elseif (ison and not toggle) or (not ison and toggle) then
        gpio.write(lpin,gpio.HIGH)
    end 
end

-- detect io
function isOn(pinNum)
    if gpio.read(pinNum) == 0 then
        return true
    else
        return false
    end
end
 
-- send data to mqtt broker
function mqttSendMsg(datajson)
    print('notidy '..datajson)
    turnLed(MAINLEDPIN,true,true)
    m = mqtt.Client("fuckingdevice",120,"fuser","pwdfuck")
    m:connect("192.168.1.19",1883,0,function(conn)
        print('connected broker .. ')
        conn:publish("atopic",datajson,0,0, function(conn)
            print("sent .. ")
        end)
        conn:close()
    end)
    turnLed(MAINLEDPIN,false,true)
end

-- check detected pins

-- return (isdetected, updatedstate)
function checkDetect(dpin, laststate)
    if isOn(dpin) then
        if not laststate then
            print("detected")
            return true, true
        end
        return false, true
    else
        if laststate then
            print("leave")
            return true, false
        end
        return false, false
    end
end

-- main blocks

function startDetection()
    print('started detecting .. ')
    isdetected = false
    isdetected2 = false
    
    tmr.alarm(0,100,1,function()
        isd1 , isdetected = checkDetect(DETECTPIN,isdetected)
        isd2 , isdetected2 = checkDetect(DETECTPIN2, isdetected2)
        if isd1 or isd2 then
            mqttSendMsg(cjson.encode({pinstate1=isdetected,pinstate2=isdetected2}))
        end
        
        if isdetected and isdetected2 then
            turnLed(LEDPIN,true,false)
        elseif isdetected or isdetected2 then
            blinkLed(LEDPIN,10)
        else
            turnLed(LEDPIN,false,false)
        end
    end)
end

-- main --

print('started')
blinkLed(MAINLEDPIN,1)
print("connect ... ")
wifi.sta.connect()
tmr.alarm(1,1000,1,function()
    if wifi.sta.getip() == nil then
        print(" .. "..wifi.sta.status())
    else
        print('connected')
        turnLed(MAINLEDPIN,false,true)
        startDetection()
        tmr.stop(1)
    end
end)

