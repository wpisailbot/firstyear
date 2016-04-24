There is no longer any useful code outside of the `bbb/` folder.

###Usage

Basically all commands must be run as root in order to access the
IO devices.

While on the BBB (none of these commands will work from a PC),
you can do the following:

In order to update the arduino code, go to `bbb/arduino` and
do a `sudo make upload`. You may need to delete the build
directory if `make` is having trouble with timestamps.

In order to run the BBB code, go to `bbb/py` and run `sudo ./main.py`.

The main python code does not run by default when the BBB starts up.
Someone should make it do that.

The arduino code waits on the IMU to calibrate before going.
