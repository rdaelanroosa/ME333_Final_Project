

import serial
import matplotlib.pyplot as plt 

PORT = '/dev/ttyUSB'

menu = '''

MENU:

    a: Read current (ticks)
    b: Read current (mA)
    c: Read encoder (ticks)
    d: Read encoder (deg)
    e: Reset encoder
    f: Set PWM (-100 to 100)
    g: Set current gains
    h: Get current gains
    i; Set position gains
    j: Get position gains
    k: Test current control
    l: Go to angle (deg)
    m: Load step trajectory
    n: Load cubic trajectory
    o: Execute Trajectory
    p: Unpower motor
    q: Quit client
    r: Get mode

    ?: Help (display this menu)

'''

mode_dict = {
    0:"IDLE", 
    1:"PWM", 
    2:"ITEST", 
    3:"HOLD", 
    4:"TRACK"}

def a():
    ser.write(b'a\n')
    print("\nISENSE TICKS: %d\n" % int(ser.read_until(b'\n', 50)))

def b():
    ser.write(b'b\n')
    print("\nISENSE mA: %f\n" % float(ser.read_until(b'\n', 50)))

def c():
    ser.write(b'c\n')
    ticks = int(ser.read_until(b'\n', 50))
    print("\nENCODER TICKS: %d\n" % ticks)
    if ticks == 0:
        print("\n!!! ENCODER SATURATED !!!\n")

def d():
    ser.write(b'd\n')
    deg = float(ser.read_until(b'\n', 50))
    print("\nENCODER DEGREES: %f\n" % deg)
    if deg == -30720.0:
        print("\n!!! ENCODER SATURATED !!!\n")

def e():
    ser.write(b'e\n')

def f():
    pwm = input('\n PWM: ')
    ser.write(b'f\n')
    ser.write(('%s\n' % pwm).encode())
    raw = ser.read_until(b'\n', 50)
    if float(raw) != float(pwm):
        print("\nERROR: PWM MAY NOT BE SET PROPERLY")
    print("")


def g():
    ser.write(b'g\n')
    kp = float(input('\nCurrent Kp: '))
    ki = float(input('Current Ki: '))
    print('\nSending gains... ', end='')
    ser.write(("%f %f\n" % (kp, ki)).encode())
    raw = ser.read_until(b'\n', 50)
    data = list(map(float,raw.split()))
    if data[0] == kp and data[1] == ki:
        print("Done\n")
    else:
        print("ERROR: CURRENT KP, CURRENT KI may have been written improperly. Read back to confirm\n")

def h():
    ser.write(b'h\n')
    raw = ser.read_until(b'\n', 50)
    data = raw.split()
    print('\nCURRENT KP: %f' % float((data[0])))
    print('CURRENT KI: %f\n' % float((data[1])))

def i():
    ser.write(b'i\n')
    kp = float(input('\nPosition Kp: '))
    ki = float(input('Position Ki: '))
    kd = float(input('Position Kd: '))
    print('\nSending gains... ', end='')
    ser.write(("%f %f %f\n" % (kp, ki, kd)).encode())
    raw = ser.read_until(b'\n', 50)
    data = list(map(float,raw.split()))
    if data[0] == kp and data[1] == ki and data[2] == kd:
        print("Done\n")
    else:
        print("ERROR: POSITION KP, POSITION KI, POSITION KD may have been written improperly. Read back to confirm.\n")


def j():
    ser.write(b'j\n')
    raw = ser.read_until(b'\n', 50)
    data = raw.split()
    print('\nCURRENT KP: %f' % float((data[0])))
    print('CURRENT KI: %f' % float((data[1])))
    print('CURRENT KD: %f\n' % float((data[2])))

def k():
    h()
    ser.write(b'k\n')
    target = []
    current = []
    output = []
    endflag = 0
    i = 0
    while not bool(endflag):
        data_read = ser.read_until(b'\n',50)
        data = str(data_read,'utf-8').split()
        if len(data) == 5:
            endflag = int(data[0])
            target.append(float(data[2]))
            current.append(float(data[3]))
            output.append(float(data[4]))
    
    print("\n\n<CLOSE PLOT TO CONTINUE>\r\n") # time array
    plt.plot(target)
    plt.plot(current)
    plt.plot(output)
    plt.ylabel('value')
    plt.xlabel('sample')
    plt.show()

def l():
    ntarg = input('Angle to HOLD (deg): ')
    ser.write(b'l\n')
    ser.write(('%s\n' % ntarg).encode())


def m():
    True

def n():
    True

def o():
    True

def p():
    ser.write(b'p\n')

def q():
    print("\nSetting MODE to IDLE...", end = '')
    ser.write(b'q\n')
    raw = ser.read_until(b'\n', 50)
    data = int(str(raw, 'utf-8'))
    if data == 0:
        print("Done\n")
        end = 'y'

    else:
        print("IDLE NOT SET")
        end = input("Force exit? (Y/n): ")

    if (end == 'y') or (end == 'Y'):
        print('Closing port...', end = '')
        ser.close()
        print('Done\n')
        print('Exiting...\n\n')
        exit()

def r():
    ser.write(b'r\n')
    raw = ser.read_until(b'\n', 50)
    data = int(str(raw, 'utf-8'))
    print('MODE: ' + mode_dict[data])

def t():
    pwm = input('\n PWM: ')
    ser.write(b'f\n')
    ser.write(('%s\n' % pwm).encode())
    raw = ser.read_until(b'\n', 50)

def help():
    print(menu)

def err():
    print('\nInvalid choice\n')
    switcher.get(input('(h for help) >>> '), err)()

switcher = {
    'a':a,
    'b':b,
    'c':c,
    'd':d,
    'e':e,
    'f':f,
    'g':g,
    'h':h,
    'i':i,
    'j':j,
    'k':k,
    'l':l,
    'm':m,
    'n':n,
    'o':o,
    'p':p,
    'q':q,
    'r':r,
    't':t,
    '?':help}

portname = "%s%s" % (PORT, input('Port number: '))
ser = serial.Serial(portname,230400,rtscts=1)
print('Opening port: ' + ser.name)

print(menu)

while (True):
   
    choice = input('>>> ')

    switcher.get(choice, err)()
    #ser.write(ks.encode()) # Send Kp, Ki

    """sampnum = 0
    read_samples = 10
    ADCval = []
    ref = []
    u = []        # List to store control output from PI controller
    while read_samples > 1:
        data_read = ser.read_until(b'\n',50)
        data_text = str(data_read,'utf-8')
        data = list(map(int,data_text.split()))
        print(data) # for debugging
        if(len(data)==4):
            read_samples = data[0]
            ADCval.append(data[1])
            ref.append(data[2])
            u.append(data[3])  # Read in PI control output
            sampnum = sampnum + 1"""




