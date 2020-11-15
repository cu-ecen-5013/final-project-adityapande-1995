# Final Project repository for AESD course project - Aditya Pande

Check out this [wiki](https://github.com/cu-ecen-5013/final-project-adityapande-1995/wiki) for project overview and schedule.

## Usage :
For Raspberry Pi 3B+ model, this assumes the sending pin in GPIO15 (pin 10 physically) and receiving pin is GPIO21 (pin 40 physically) 
```
make;
sudo ./morse-loopback "SOS" 500.0 20 
# sudo ./morse-loopback text_to_send, clock_in_milliseconds, modulation_frequnecy_in_Hz
```
To run without modulation,
```
sudo ./morse-loopback "SOS" 500.0;
```

### Limitations : 
The parameter ```clock_in_milliseconds``` gives reliable transmission only when set to a value of 2.0 or above as of now. 




