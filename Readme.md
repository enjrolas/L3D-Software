This is a set of demo software for the L3D cube.  There are two types of programs in this repository:  Native programs,
that run on the [spark processor](https://www.spark.io/) in the cube, and don't require a wifi connection to work,
and Streaming programs, that stream data to the cube from a computer or phone over wifi.

For streaming programs, you'll need to load the Listener program(located in the /Streaming directory) onto the Spark, 
and then run the program on your computer that generates the pattern for the cube.  Data is streamed via UDP multicast on port 
6000, which most home routers support.  If you're not seeing any data come through, it's possible that your router is blocking
multicast packets.  If you can fiddle with the router's settings, check to see if it's blocking packets on port 6000, and if 
it's disabling multicast.  

If you have a network that requires you to click through a form to get access to the web, the processor in the cube

* Getting started

	You’ll need to do some quick setup so that you can load programs onto the processor in your cube.  You’ll need to tell the processor the credentials for your wifi network, so that it can connect to the internet and receive new programs.  First,  turn your cube on its side, and put your processor into *listening mode* by pressing and holding the ‘mode’ button for three seconds, until it starts flashing blue.  More instructions are available [here](http://docs.spark.io/connect/)

Follow the instructions [here](http://docs.spark.io/start/) to use your phone or tablet to set up the processor.  

If you prefer to work on the command line, you’ll need to plug the USB cable into your computer and follow the setup instructions [here](http://docs.spark.io/cli/) to install the command-line interface for the processor.  Once it’s installed, run the command ‘spark setup’, and a prompt will walk you through the process of creating a (free) account with Spark, loading your wifi credentials onto the processor and linking the processor to your account.

Once your processor is set up, you’re ready to load a program onto it.  We prefer to work from the [Spark web interface](https://www.spark.io/build) — all the code is written in the browser, the compiler is in the cloud, and as long as your cube has a wifi connection, you load a new program onto the cube just by clicking a button.  To get started, check out the [example included in our L3D library](LINK GOES HERE), and try flashing it onto your cube.

Unfortunately, Spark doesn’t have a great setup yet for sharing code online.  We’re working with them to set something up, but until then, the best way to ‘fork’ our code and start editing it is to create a new app in the spark editor, and copy-paste the code from the github repository into the app.  

To further complicate matters, the spark interface has a couple quirks that make copy-pasting a little tricky.  Here’s the best way we’ve found to paste in some new code:

Let’s say that there’s a native program that has three files:

demo.ino — this is the main file in the app.  Includes the spark 					 library ‘Neopixel’
colors.cpp — a couple functions for handling colors
colors.h — defines some datatypes

1. Create a new app in the spark editor — you can call it anything you like, but for this example, let’s say you call it ‘myApp’
![creating a new app](http://cl.ly/image/161p0M0j2z3C/Screen%20Shot%202014-10-01%20at%2012.29.23%20PM.png)
![naming the app](http://cl.ly/image/0i2a1J2U331u/Screen%20Shot%202014-10-01%20at%2012.29.29%20PM.png)

2. The editor will open up with a single file, myApp.ino  Copy all the code in your local copy of demo.ino and paste it into the myApp.ino file in your browser.  Once it’s pasted, ** be sure to click ‘save’ **.
![pasting in new code](http://cl.ly/image/39392f402n1a/Screen%20Shot%202014-10-01%20at%2012.30.12%20PM.png)

3. Next, you’ll need to add the spark neopixel library.  Click on the ‘libraries’ icon:
![library icon](http://cl.ly/image/1g3a3t0P3z3k/Screen%20Shot%202014-10-01%20at%2012.30.24%20PM.png)

Next, find the ‘neopixel’ library in the list of libraries (it’s quite popular, so it’s usually at the very top), and click on it.
![neopixel library](http://cl.ly/image/3C450h1x0L39/Screen%20Shot%202014-10-01%20at%2012.30.29%20PM.png)

Scroll down on the left side of the screen, and click the ‘include in app’ button:

![include in app](http://cl.ly/image/0X1A1l281q2G/Screen%20Shot%202014-10-01%20at%2012.30.40%20PM.png)

It will display a list of all your apps.  Scroll to the bottom and select ‘myApp’

![select your app](http://cl.ly/image/0q1B2x3i2f1T/Screen%20Shot%202014-10-01%20at%2012.30.46%20PM.png)

Scroll down to the bottom of a list again, and click ‘add to this app’

![add to this app](http://cl.ly/image/1y1K3M0N0X2u/Screen%20Shot%202014-10-01%20at%2012.30.54%20PM.png)

Great!  That’s all done

4. Finally, you need to add the colors.h and colors.cpp files to the sketch.  Click the ‘+’ icon on the top-right of the web interface.

![add files](http://cl.ly/image/0N2b0Z1m3O2J/Screen%20Shot%202014-10-01%20at%2012.59.25%20PM.png)

It will create a .cpp and .h file, and prompt you to enter a name for the files.  Type in ‘colors.h’, and it will automatically rename both files.

![renaming files](http://cl.ly/image/3r3u1N362W2J/Screen%20Shot%202014-10-01%20at%2012.59.28%20PM.png)

Paste in the code from both colors.h and colors.cpp

![pasting code](http://cl.ly/image/0G253T2n2g45/Screen%20Shot%202014-10-01%20at%2012.59.38%20PM.png)

Aaaand you’re good to go!

5.  To load the code onto your spark, just click the ‘flash program’ icon on the top left of the interface

![flash program](http://cl.ly/image/3d18301k1N3E/Screen%20Shot%202014-10-01%20at%201.01.49%20PM.png)

The color LED on your spark should quickly turn purple and begin flashing to show that the program is loading.  It usually takes about 20 seconds to load a new program, although we’ve seen it take up to a minute when we’re working outside of the US.


------

