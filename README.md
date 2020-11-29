# Final Project repository for AESD course project - Aditya Pande

Check out this [wiki](https://github.com/cu-ecen-5013/final-project-adityapande-1995/wiki) for project overview and schedule.

## Usage :
### Loopback usage :
For Raspberry Pi 3B+ model, this assumes the sending pin in GPIO12 and receiving pin is GPIO18
```
# To make without verbose printing
make CFLAGS=-DVERBOSE=false
# To make with verbose printing, default value of VERBOSE is true
make CFLAGS=-DVERBOSE=true

sudo ./morse-loopback "SOS--" 500.0 38000
# sudo ./morse-loopback text_to_send, clock_in_milliseconds, modulation_frequnecy_in_Hz
```
To run without modulation,
```
sudo ./morse-loopback "SOS--" 500.0;
```
### Running receiver on other Raspberry Pi :
To receive data on the other device, the following command should be run on the other device before the sending begins :
```
sudo ./receiver 500.0
# sudo ./receiver clock_in_milliseconds
```
The ```clock_in_milliseconds``` value should match the one used while sending, and text to be sent should end with a ```--``` to signify end of text.

### Limitations : 
The parameter ```clock_in_milliseconds``` gives reliable transmission only when set to a value of 20.0 (with modulation and IR LED-receiver setup) or above on Raspbeery Pi 3B+.




