# the file name, its location and the target device name
file_name = "USB-PD_Power_Supply.uf2"
file_path = "../build/" + file_name
device_target = "RPI-RP2"
# the vendor id of the device
vendor_id = "2E8A"
product_id = "000A"
# baudrate
baudrate = 9600
timeout = 1
# reset message
reset_message = "reboot_to_bootloader"


# Thanks https://github.com/diminDDL for assisting in development and testing on Linux


import shutil
import os
import sys
import subprocess
import time


# Fix the path so that the script can be run from any directory
file_path = os.path.abspath(file_path)

# check if the 'pyserial' module is installed and install it if not
try:
    import serial
    import serial.tools.list_ports
except ImportError:
    print("The 'pyserial' module is not installed. Installing now...")
    subprocess.call([sys.executable, "-m", "pip", "install", "pyserial"])
    import serial
    import serial.tools.list_ports

# check if the 'psutil' module is installed and install it if not
try:
    import psutil
except ImportError:
    print("The 'psutil' module is not installed. Installing now...")
    subprocess.call([sys.executable, "-m", "pip", "install", "psutil"])
    import psutil

# if the operating system is windows, check if the 'pywin32' module is installed and install it if not
if os.name == "nt":
    # check if the 'pywin32' module is installed and install it if not
    try:
        import win32api
    except ImportError:
        print("The 'pywin32' module is not installed. Installing now...")
        subprocess.call([sys.executable, "-m", "pip", "install", "pywin32"])
        import win32api


# Scan for a particular device and return true if found
def Scan_For_MassStorage(device_name):
    if os.name == "nt":  # Windows
        # get a list of all mounted volumes using the win32api module
        volumes = win32api.GetLogicalDriveStrings()
        volumes = volumes.split('\x00')[:-1]

        # loop over the mounted volumes and look for a volume with the correct label
        for volume in volumes:
            try:
                label = win32api.GetVolumeInformation(volume)[0]
            except:
                # skip this volume if there is an error accessing the label
                continue

            if label == device_name:
                return True
            
    else:  # Linux
        # get all the connected devices
        for partition in psutil.disk_partitions():
            # check if the device name is in the partition device name
            if device_name in partition.mountpoint:
                return True
            
        return False


# Reset the device by sending a specific message over the serial port
def Reset_Device():
    # get all the connected devices
    ports = list(serial.tools.list_ports.grep(f"VID:PID={vendor_id}:{product_id}"))
    # if there are no connected devices, exit
    if not ports:
        print("No devices found")
        exit()
    else:
        # connect to the first device in the list
        port = ports[0].device
        print(f"Found device at {port}")
        # connect to the port
        try:
            ser = serial.Serial(port, baudrate=baudrate, timeout=timeout)
        except:
            print(f"Could not connect to the device at {port}, please disconnect any program that is using the device and try again")
            exit()

        # send the reset message
        ser.write(reset_message.encode())
        # close the connection
        ser.close()


# Wait for the device to be mounted
def Wait_For_MassStorage_Device():
    print("Waiting for the device to be mounted...")
    while(True):
        # check if the device is mounted
        if Scan_For_MassStorage(device_target):
            break
        else:
            time.sleep(0.5)


# Transfer the uf2 file to the mounted device
def Transfer_File():
    # i am too lazy to write a proper check if this works or not so we just wrap it in a try except block
    try:
        # check the operating system and create the destination file path accordingly
        if os.name == "nt":  # Windows
            # get a list of all mounted volumes using the win32api module
            volumes = win32api.GetLogicalDriveStrings()
            volumes = volumes.split('\x00')[:-1]

            # loop over the mounted volumes and look for a volume with the correct label
            destination_found = False
            for volume in volumes:
                try:
                    label = win32api.GetVolumeInformation(volume)[0]
                except:
                    # skip this volume if there is an error accessing the label
                    continue

                if label == device_target:
                    # create the destination file path by joining the volume and the file name
                    destination_file_path = os.path.join(volume, file_name)
                    destination_found = True
                    shutil.copy2(file_path, destination_file_path)
                    output = f"The uf2 has been copied to '{destination_file_path}'"
                    print(output)

            if not destination_found:
                output = f"Could not find a volume with the label '{device_target}'"
                print(output)
                exit()

        else:  # Linux
            # create the destination file path by joining the mount point and the file name
            destination_file_path = os.path.join("/media/", os.getlogin(), device_target, file_name)
            shutil.copy2(file_path, destination_file_path)
            output = f"The uf2 has been copied to '{destination_file_path}'"
            print(output)
    except:
        print("Something went wrong, please try again")
        exit()


# Check if the uf2 file exists
if not os.path.exists(file_path):
    print(f"The uf2 file '{file_path}' does not exist, please build the project first")
    exit()

# Check if the mount point exists
if Scan_For_MassStorage(device_target):
    # Transfer the uf2 file to the mounted device
    Transfer_File()
else:
    # Reset the device
    Reset_Device()
    # Wait for the device to be mounted
    Wait_For_MassStorage_Device()
    # Transfer the uf2 file to the mounted device
    Transfer_File()
