There is no longer any useful code outside of the `bbb/` folder.

#Usage

Basically all commands must be run as root in order to access the
IO devices.

While on the BBB (none of these commands will work from a PC),
you can do the following:

In order to update the arduino code, go to `bbb/arduino` and
do a `sudo make upload`. `make` will likely complain about timestamps.
I generally run the `touch_all.sh` script which will update all the timestamps
for everything in the directory. Note that this does mean you have to `touch`
any files that you want recompiled, but it avoids having to have `make` recompile
all the libraries.

`make` will call the `reset.sh` script in `bbb/arduino`, which will use the
appropriate GPIO pin to reset the arduino when `avrdude` runs. The sleep times
in the script are tuned to be what happens to work well for running the Makefile.
There is no good reason to have to change it other than if the GPIO pin being
used changes.

In order to run the BBB code, go to `bbb/py` and run `sudo ./main.py`.

The main python code does not run by default when the BBB starts up.
Someone should make it do that.

#File Locations on BBB

##Wireless

The network configuration is stored in `/etc/network/interfaces`. The only lines that affect the wireless interface network are the ones on and following `iface wlan0 inet static`. My machine happens to create an access point that uses the 10.42.0 subnet, but your computer may do something else (and it will certainly have a different SSID and key).

The network configuration should get run at startup, by running `/home/debian/bin/start_wireless.sh`. In theory, the only line of that script that should be necessary is `/sbin/ifup wlan0`, but I was running into issues and have to wait a bit and run `iwconfig` separately (note: This forces you to write in the SSID and key for the network both in `/etc/network/interfaces` and in this file). I know that there were some other issues that I've fixed since then, so it is possible that it is no longer necessary.

For whatever reason, I've also found that in general the beaglebone can not ping my computer until I have pinged the beaglebone with my computer. I haven't investigated why, and that may have been transient.

##Connecting the BBB to the internet
In order to connect the BBB to the internet (while plugged in via USB), you must run the configuration necessary on both your own machine and on the BBB. On the BBB, if you run`/home/debian/bin/route.sh`, that will do all the necessary BBB configuration. 

On your PC, you should run:
```
sudo iptables -A POSTROUTING -t nat -j MASQUERADE
sudo echo 1 | sudo tee /proc/sys/net/ipv4/ip_forward > /dev/null
```

##Cron jobs

Currently, the wireless configuration is run as a cron job on startup. The cronjob is entered in the root crontab:
```
sudo su
crontab -e
```

##Code

The git repository is checked out at `/home/debian/sailbot`. Whenever I've worked with code on the BBB, I will commit the code on the repository on the BBB, pull from the BBB to my PC and then push from my PC to the github repository.

##I/O

###Serial ports

Serial ports are located at `/dev/ttyO1`, `/dev/ttyO2`, `/dev/ttyO4`, etc.
Strictly speaking, you should never need to access these directly if you are just using the python code. However, you may need to access them if you are debugging stuff.

You can access the serial ports as you would any on linux; if you just want to receive and send from the ports, you can do something like `screen /dev/ttyO1 9600` to access `/dev/ttyO1` at 9600 baud.

If the serial port does not exist in `/dev`, then it needs to be added. This can be done by doing `echo BB-UART4 > /sys/devices/bone_capemgr.9/slots` as root (using `BB-UART1`, `BB-UART2`, etc for other ports).

To make a serial port startup by default, then edit `/boot/uEnv.txt`; there is already a line for `/dev/ttyO1`, and you can do the same thing for other ports (I have not poked at this all that much).

###GPIO

GPIO pins can be manually controlled through the device tree as follows:

To enable a pin (say, 60), do `echo 60 > /sys/class/gpio/export` as root. The directory `/sys/class/gpio/gpio60/` will then be created and you can manipulate the settings and behavior of the pin by reading/writing from various files (you can read more about this on the internet).

By the way, the GPIOs are split into sets of 32. This means that GPIO60 is also GPIO1.28 (1 * 32 + 28 = 60), and the different conventions are used in different places and you have to be able to convert between them.

#External Links

## BeagleBoneBlack

[Home Page](https://beagleboard.org/black): Contains useful links to vearious places.

[Latest Hardware Documentation](http://elinux.org/Beagleboard:BeagleBoneBlack#LATEST_PRODUCTION_FILES_.28C.29): Schematics, System Reference Manual, etc. Useful for checking what connects to what pinouts, or trying to figure out which pin names correspond to which things.

[Wiki](http://elinux.org/Beagleboard:BeagleBoneBlack): General useful information.

## XBee
[Datasheet](https://www.sparkfun.com/datasheets/Wireless/Zigbee/XBee-2.5-Manual.pdf): This is the datasheet for the precise version of the XBees that we are using. The 1.2xx (End device, transparent operation) firmware is installed on the boat. Other XBees have other versions installed (the one connected to the USB shield that I've been using has the coordinator transparent operation installed on it).

[Cape Schematic](https://raw.githubusercontent.com/lgxlogic/CBB-XBEE/master/CBB-XBEE-sch.png): The schematic for the cape that we are using for the XBee on the BBB (the Cape only gets power if the BBB is being powered from the battery or wall power.

## IMU (BNO055)
[Datasheet](https://cdn-shop.adafruit.com/datasheets/BST_BNO055_DS000_12.pdf): Contains information on what the different modes do, how calibration works, etc. Informative, although we do not need to worry about the minutiae so long as we are using Adafruit's libraries.

[Adafruit Page](https://learn.adafruit.com/adafruit-bno055-absolute-orientation-sensor/overview): The Adafruit page for the BNO055.

## GPS
[Datasheet](http://cdn.sparkfun.com/datasheets/Sensors/GPS/GP-635T-121130.pdf)
