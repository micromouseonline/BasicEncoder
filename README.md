# BasicEncoder

A simple library class for reading encoders as used in control knobs.

The complete code is in a single header file ```BasicEncoder.h``` so that you can just add the file to your project directly ratrher than instal a complete library.

This class does not handle the pushbutton usually found on these rotary encoders. For that it is better to use a separate pushbutton library.

Most of the logic in this code was derived from this page:

https://www.mikrocontroller.net/articles/Drehgeber


## Initialisation

The simplest constructor needs only the pin names for the encoder A and B lines.

    #include <BasicEncoder.h>
    BasicEncoder encoder(2,3);


This will create an encoder object using pins 2 and 3 for the two required input lines. 

The pins will be initialised as inputs with pullups. 

The encoder object will assume that the resting state of the inputs is high and that there are 4 signal steps per detent. That is typical for the most common type of inexpensive encoder knob.

You can optionally specify that the pins rest in the low state and that the number of steps per detent is different with constructors like


    #include <basicEncoder.h>
    BasicEncoder encoder1(2,3,HIGH,2) / pins rest low, 2 steps per detent


If the count is reversed then just swap the two pins. Alternatively, the direction of counting can be reversed or restored to normal in software with

    encoder.set_reverse();
    encoder.set_forward();

If the encoder will be polled (see below) then you are free to choose any two digital inputs pins.

If a pin change interrupt (see interrupts below)will be used then both channels should be in the same group so that they are serviced by the same interrupt. Pick pairs of pins from the following lists.

 * PCINT0 - D8, D9, D10, D11, D12, D13,
 * PCINT1 - A0, A1, A2, A3, A4, A5
 * PCINT2 - D0, D1, D2, D3, D4, D5, D6, D7

Although it is possible to put more than one encoder on the same group the code may get a little messy so it is best to use different groups if more than pne encoder is to be connected.

With polling, it will not matter except that the interrupt service routine may start to get a little time-consuming.

## Update the counts

To make the encoder class react to any movement of the actual encoder device, it must regularly, and frequently examine the state of the input lines.

To have the encoder check the lines and respond to changes, you must call the ```service()``` method. e.g.

    encoder.service();

## Polling

Polling is the name given to methods where the code is made to go and look for any changes. Many Arduino programs poll for changes by calling a function at the beginning of ```loop()```. So long as the rest of the code in ```loop()``` is short and executes quickly, this is likely to be adequate. The encoder should be polled as frequently as possible and certainly often enough to reliably detect changes. 

## Polling with a timer interrupt

To be sure that the encoder is polled frequently enough, it is probably best to call the ```service()``` method from a timer interrupt running at seveeral hudreds of kHertz or more. If you use the ```TimerOne``` library, then the polling might be set up like this:

    #include <Arduino.h>
    #include <BasicEncoder.h>
    #include <TimerOne.h>

    BasicEncoder encoder(2, 3);

    void timer_service() {
      encoder.service();
    }

    void setup() {
      Serial.begin(115200);
      Timer1.initialize(1000);
      Timer1.attachInterrupt(timer_service);
    }

    void loop() {
      int encoder_change = encoder.get_change();
      if (encoder_change) {
        Serial.println(encoder_change);
      }      
    }

In this example the TimerOne library is used to generate an interrupt every 1000 microseconds (1kHz).  Changes are accumulated in the service routine and processed as needed by the main program loop.  This works well even on switches with a lot of bounce. The entire function is compact and could be faster with hard coded pin reads or by using digitalReadFast if you wanted to modify the library source code.

## Reading changes

The encoder object tracks changes in the actual encoder and keeps a tally of the number of steps and the direction of rotation. There are two ways to get at this information:

#### ```get_change()```

By calling the ```get_change()``` method your program can receive the number of counts since the last call to the method as a signed integer. This is a destructive call in that the change count is reset to zero during the call. Thus you should assign the change to a local variable so that you can use the value later in the code. The example above shows this method. The default contructor assums that there are four steps per detent, or click, of the encoder knob so the number returned by ```get_change()``` is the number of clicks, not the number of signal changes. In that example the printed value will almost certainly be just +1 or -1 because the loop executes very quickly. Try adding a delay in ```loop()``` to see larger changes being reported.

#### ```get_count()```

The ```get_count()``` method will also return the number of clicks (not signal changes) recorded by the encoder object. This time however, the number returned will be the accumulated count since the last time the ```reset()``` method was called. The value is not cleared when read.

### Motor encoders

Polled encoders are not likely to work well for motor applications. If you specifically want motor applications there are many ways to optimise the code for better performance at high frequencies. Such optimisations may rely on the encoder channels being clean. That is, the pulses switch reliably without any contact bounce. The technique used in this code is reliable even with low quality encoders that have considerable contact bounce.

## Bouncing encoders

On the subject of contact bounce, the code assumes that the detents of a typical control knob coincide with stable states of the control signals. It is possible that some controls have detents that coincide with transitions and there may be some jitter in the output even when the knob is at rest. Encoder controls without detents may come to rest at such a position by chance.

If this is a problem in your application the article linked in the comments provides an alternative solution that uses a lookup table to decode the state transitions.

https://www.mikrocontroller.net/articles/Drehgeber


## Pin Change Interrupts

 If you like, this service routine could be called from the pin change interrupt.
 
 See the examples for code setup to use pin change interrupt operation. 
 
 See also:

 https://playground.arduino.cc/Main/PinChangeInterrupt/
 https://thewanderingengineer.com/2014/08/11/arduino-pin-change-interrupts/

## Multiple Encoders

The library should work with multiple encoder instances. There is an example `two-encoders` that demonstrates this using a timer interrupt for polling. Pin-change interrupt implementations may also be fine if the encoders are in different groups though that has not yet been tested.

## A note about interrupts

In a few places, the code needs to disable interrupts to ensure that data values are not corrupted. Immediately before a call to `noInterrupts()`, the current value of the status register, `SREG`, is saved into a local variable. Once the critical code section is complete, the saved state of the interrupt enable flag is restored by simply copying the saved value of the status register back. The other flags are not critical in this context. 

This method is used rather than simply enabling interrupts because interrupts may already have been disabled at the time the function is called and to re-enable interrupts when they should not be enabled might cause hard to track bugs in the user code.
