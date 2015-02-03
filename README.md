# freeVDV421
:station: Open implementation of public transit IR positioning beacons according to VDV-421

## What is this?

(At least some) German buses and trams use infrared communications to communicate with fixed roadside beacons. Basically, the conversation goes as such:

conversation goes as this:

> HELLO THIS IS BUS HELLO THIS IS BUS HELLO THI–

> YEAH. GOTCHA. THIS IS BEACON 12345. THIS IS BEACON 12345. … YOU STILL THERE?

> …

> HELLO THIS IS BUS HELLO THIS IS BUS HELLO THIS IS BUS

So, the bus transceivers constantly announce their presence, while the roadside beacons mostly sleep (what a job!). Once a bus passes by a beacon, the beacon “wakes up” and announces its presence, transmitting whatever data they are pre-set to transmit. This could be their designation (re-setting the bus's odometry system, for location obtainment), or a code to submit to the next junction processor (think: traffic light preemption).

In turn, once the bus recognizes a beacon's transmission, it shuts up as not to interfere with reception. After a while, it re-starts shouting it's presence into the wild.

## What is this project good for?

The associated standard and hardware date from the 1980s – yet, nothing in the way of a Free/Libre implementation exists. To get things straight: This is not about phreaking, haxx0ring or anything else script-kiddie-break-things-just-because-we-can-related. We do this, because we find it interesting and amusing, and because it opens the way for applications such as:

 * tracking whether that bus is *really* always late, as you suspect
 * Implementing a DIY-early-warning-system that reminds everybody at your space that the next bus just passed the corner two streets away
 * Opening possibilities for next-generation public transit stops that gently shake the waiting bench as the bus arrives
 * or simply because it is a fascinating field for testing your discrete electronics and/or microcontroller programming skills.

## How does the IR transmission work, and how do you know that?

Through educated guesses, hints, and the practical application of reverse-biased photodiodes and a handy Digital Storage Oscilloscope we infer that

 * buses modulate their data with a 28.8 kHz square carrier wave
 * the data rate should be 4800 bps
 * Encoding is [Biphase Mark Encoding], with transitions on the 0

As you might already have figured out, that makes for just three pulses in a zero, and six pulses in a 1. Unfortunately, this rules out using standard IR demodulators like the TSOP3* models – since those need a series of at least 10 pulses, and pauses of at least 10 pulses in between.

[Biphase Mark Encoding]:http://en.wikipedia.org/wiki/Differential_Manchester_encoding
