# EE6470 Homework 4 Report
###### tags: `EE6470`
## Problems and Solutions
### CLOCK_PERIOD defination
Q: In lab4, it used **CLOCK_PERIOD** in the system as the clock period, but I can not find the actual defination of **CLOCK_PERIOD** in code.
A: This parameter is defined in Cmake list.
```
# Set clock period = 1 ns
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D CLOCK_PERIOD=1.0")
```
And I define this in code but not in Cmake list in hw4.
```
#define CLOCK_PERIOD 1
```

## Implementation details 
Since the demostrating of lab04 to TA has shown the implementation detail of Bus, so I only describe the differnece of socket binding.
### Socket binding
Since this model is one-to-one binding, so I only declare one-to-one bus in this homework. Then I bind the socket ans set the decoding parameters.

```    
Testbench tb("tb");
Filter filt("filt");
SimpleBus<1, 1> bus("bus");

tb.initiator.i_skt(bus.t_skt[0]);
bus.set_clock_period(sc_time(CLOCK_PERIOD, SC_NS));
bus.setDecode(0, FILTER_MM_BASE, FILTER_MM_BASE + FILTER_MM_SIZE - 1);
bus.i_skt[0](filt.t_skt); 
```


## Additional features
### Simplier Cmake list
In lab4, it define the **CLOCK_PERORD** in Cmake list, but I define it in **filter_def.h**, which make the code easy to read.

## Experimental results
### Multiple port of SimpleBus
I tried to declare a one-to-two simple bus as below, then I experiment binding different filter.

In this version, I set port ID in setDecode function to be 0, which will bind the testbench to **filter0** as line 24. And the message print in ubuntu shows that it actually run id 0 filter.
![](https://i.imgur.com/8AYZJup.png)
Here I set port ID in setDecode to 1, then the message shows it running id 1 filter.
![](https://i.imgur.com/6qOS0zQ.png)

## Discussions and conclusions
For the 1-to-1 socket binding in homework 3, **SimpleBus** module allows us to bind multiple module in diferent combination with simple select port binding, which is similar to the real main bus system in computer. For example, if there is two module A and B need handshaking when computing, but they both need to communicate with testbench to recieve source data, here is the proper application for simple bus that implement dynamic binding.
