# check if the 'pyserial' module is installed and install it if not
try:
    import serial
    import serial.tools.list_ports
except ImportError:
    print("The 'pyserial' module is not installed. Installing now...")
    subprocess.call([sys.executable, "-m", "pip", "install", "pyserial"])
    import serial
    import serial.tools.list_ports

# get all the connected devices
ports = list(serial.tools.list_ports.comports())

# print the list of connected devices
for port in ports:
    print(f'{port} - {port.vid}:{port.pid}')
