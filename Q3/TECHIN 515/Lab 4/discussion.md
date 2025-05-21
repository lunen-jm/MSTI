# Lab Discussion Questions

This document houses all of the discussion questions from the lab

## Overview

With this lab, I opted to go a slightly different route. As the rubric did not explicitly say that we needed a physical button, and there were not buttons in the hardware list, I decided to create a wand that does the following:
* Utilizes gyroscope data as well for additional datapoints
* Constantly runs tests looking for a "begin" gesture (this is essentially my button)
* Once the "begin" gesture is detected, the LED lights up to a brighter white, and it then listens for one of the other three gestures
    * "fire" is a fireball, with a flickering flame LED pattern that fades quickly
    * "shield" is a shield, with a blue, fading LED effect
    * "heal" is a healing spell, and pulses between different shades of green to act like it is healing the user
* The wand also uses a dimmer light to indicate it is in "idle" mode, i.e. waiting for a begin gesture
I believe that this would constitute having a button and I should not get marked down for not having a button as I did a bunch of extra work to get this to work correctly

## Part 1 - Data Collection

2. Discussion: Why should you use training data collected by multiple students rather than using your own collected data only? Think about the effectiveness and reliability of your wand.

If this wand were to be used by others, you would want other gestures to be recorded as different people have different bodily proportions and certain gestures may not be recognized if they're not trained for a variety of use cases. For example, I'm 6'3, and my gestures would likely be tracked differently than a classmate that is 5' as they may complete it faster, and less gyroscopic change may be detected. However, in a ficitonal world, I believe that we should only have our own data for security as we would not want others to be able to use our wand and be liable for any damage that may occur. Overall though, multiple people tracking the data will help with edge cases and ubiquitous usage of the wand. Collecting data from others also increases your sample size which is vital.

## Part 2 - Edge Impulse Model Development

2. Design and implement the model:
    * Processing block: Spectral Analysis since it is accelerometer data and repetitive motions will be analyzed
    * Learning block: Classification since we need to classify the movement accordingly
    * Discussion: Discuss the effect of window size. Consider
        * the number of samples generated
        * the number of neurons in your input layer of neural network
        * effectiveness when capturing slow-changing patterns

The window size corresponds to the length of the sample / data that will be processed and classified. Since we did one-second increments, we are going to want the window size to stay at 1000ms. Lowering the window size caused gestures to not be recognized since they were shorter than the sample data. Larger windows may cause more movements to be picked up than the user intended.

3. Screenshot and justification for parameters: 

![feature explorer](/images/parameters.png)

Above are the parameters I eventually settled on. I spent a couple hours trying out different parameters before I swapped to an autotuned set and then changed it around a bit myself.

While I thought I needed highpass filtering since the movements may be erratic, it turns out that lowpass filtering worked best for separating out the features. Autotune set it to around 16hz as a cutoff, but pushing it up to 18 caused healing and shielding to be separated better. 

I also believed that I needed to use wavelets instead of FFT due to the time aspect of it, but every wavelet I tried had issues separating healing and shielding (begin and fire were separated better however).

![feature explorer](/images/features.png)

As you can see, we now have some fairly decent separation between all 4 gestures that were trained for. I believe that the main issue is a lack of true separation of gesture patterns and too low of sample size, as it is difficult for the model to pick up differences since some are too close to eachother.

4. Report the learning performance, your choices of hyper-parameters, and architecture.

![learning performance results](/images/results.png)

As you can see in the photo above, the model had an 85.9% accuracy rating with a loss of .67. 

For the settings, I kept it at 30 training cycles, kept the learning rate low so it didn't overfit, and used the CPU processor since my PC doesn't have a GPU. On the architecture side of things, I kept them as the default settings since the model is small overall. 

It uses 2ms of inferencing time, 1.4k peak ram usage, and 16.3k flash usage, so low enough to work with the ESP32.

5. Use "Live classification" and "Model testing" in sidebar to test your model performance. Please clearly document all metrics being used, e.g., accuracy, TP, FP, F1, etc.

Tried to use `edge-impulse-data-forwarder --frequency 100` to get it to work with gesture_capture_data_forward.cpp, but after a couple hours I was made aware that we just needed to test samples instead of connecting our device.

![model test results](/images/modelTest.png)

I discussed the actual metrics for the model in question 4.

7. Discussion: Give at least two potential strategies to further enhance your model performance.

With my model, it is clear that the gestures are too similar and there is some overlap causing it to not pick them all up correctly. I was able to edit the code to have different confidence thresholds for different gestures, but it is still difficult to get it to reliably do the heal spell.

Here are some general strategies I would use to make it better:
* More distinct gestures for the individual spells as they are a bit too close
* Add more training data
    * my original project had around 20-30 for each, but this version had 100 samples for each gesture. I would make it around 250 to make the model better
* Add a button as a failsafe
* Figure out additional pre-filtering methods to use in tandem with edge impulse
* 

## Issues