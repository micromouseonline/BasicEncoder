A demonstration of one of some flexible code for reading encoders as used in control knobs.

Most of the logic in this code was derived from this page:

https://www.mikrocontroller.net/articles/Drehgeber


## Initialisation

The CLOCK and DATA lines (A and B) for the encoder are defined here.

If the count is reversed then just swap pins A and B. If the encoder will be polled (read in a timer interrupt) then you are free to choose any two digital inputs pins.

If a pin change interrupt will be used then both channels should be in the same group so that they are serviced by the same interrupt:

 * PCINT0 - D8, D9, D10, D11, D12, D13,
 * PCINT1 - A0, A1, A2, A3, A4, A5
 * PCINT2 - D0, D1, D2, D3, D4, D5, D6, D7

Although it is possible to put more than one encoder on the same group the code may get a little messy so it is best to use different groups if more than pne encoder is to be connected.

With polling, it will not matter except that the interrupt service routine may start to get a little time-consuming.

Polled encoders are not likely to work well for motor applications. If you specifically want motor applications there are many ways to optimise the code for better performance at high frequencies. Such optimisations may rely on the encoder channels being clean. That is, the pulses switch reliably without any contact bounce. The technique used in this code is reliable even with low quality encoders that have considerable contact bounce.

On the subject of contact bounce, the code assumes atht the detents of a typical control knob coincide with stable states of the control sugnals. It is possible that some controls have detents that coincide with transitions and there may be some jitter in the output even when the knob is at rest. encoder controls without detents may come to rest at such a position by chance.

If this is a problem in your application the article linked in the comments provides an alternative solution that uses a lookup table to decode the state transitions.

https://www.mikrocontroller.net/articles/Drehgeber

If it suits your purpose, you could combine interrupt driven and polled encoders in the same system. Reserve the interrupt driver for motors and use polling for control knobs.


# Applications
 
 If the encoder is being used to change some value under user control it will be important for the code to be responsive. While pulses will not be missed, the user will expect to see something happen at each click.
 
 Thus, if your main loop has other tasks, make sure that the encoder service is prioritised.
 

 ## Polled Operation

 Code for polled operation. The service function should be called as frequently as possible. In this example  the TimerOne library is used to generate an interrupt every 1000 microseconds (1kHz).  Changes are accumulated in the service routine and processed as needed by the main program loop.  This works well even on switches with a lot of bounce. The entire function is compact and could be faster with hard coded pin reads or by using digitalReadFast.

 If you like, this service routine could be called from the pin change interrupt.

 NOTE: about the structure of this function

 You will see a static variable that retains the previous state of the input pins. static variables, of course, retain their value from call to call because they are really stored in global memory pace but are not visible to any code outside the function. These static variables can be initialised to any value. The initialisation only the very first time the function is called. The compiler keeps an extra guard variable that allow it to detect that first use and perform the initialisation.

 Normally, you might expect to see this static variable and its initialisation defined at the start of a function. It can be done anywhere that is convenient. Here, it is important that the previous state is set to be the same as the current state the first time the function is called to make
 sure that the encoder change is zero first time around.

 That is why the previous-state variable is not defined until _after_ the current pin state has been read.


## Pin Change Interrupts

 Code setp for pin change interrupt operation. see:
 https://playground.arduino.cc/Main/PinChangeInterrupt/
 https://thewanderingengineer.com/2014/08/11/arduino-pin-change-interrupts/


## getting the encoder change

Both the polled and interrupt driven versions work the same way in that the interrupt service accumulates counts that are read and processed in the main program loop.
  
All four states of the encoder are processed. For encoder control knobs, the usual operation is for each detent to correspond with four counts of the encoder inside.

So, for a control knob, the changes must be divided by four to get the number of 'clicks'. If you are reading a motor encoder and want the full resolution then the accumulated change should be used as-is. For convenience, the number of encoder counts per 'step' is passed in as an argument of the function. Changes are read destructively. To prevent corruption, interrupts are temporarily disabled and then restored before returning. Notice that any 'remaining' counts are retained for next time around.


