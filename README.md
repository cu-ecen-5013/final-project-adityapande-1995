# Final Project repository for AESD course project - Aditya Pande

Check out this [wiki](https://github.com/cu-ecen-5013/final-project-adityapande-1995/wiki) for project overview and schedule.

## Usage :
```
cd morse-without-clock
make;
sudo ./pulse "SOS" 500.0 20 # text, clock_in_milliseconds, modulation_frequnecy_in_Hz
```
To run without modulation,
```
sudo ./pulse "SOS" 500.0;
```

### Limits : 
The parameter ```clock_in_milliseconds``` gives reliable transmission only when set to a value of 2.0 or above as of now. 




