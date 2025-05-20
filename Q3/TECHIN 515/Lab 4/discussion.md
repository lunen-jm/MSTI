# Lab Discussion Questions

This document houses all of the discussion questions from the lab

## Part 1 - Data Collection

2. Discussion: Why should you use training data collected by multiple students rather than using your own collected data only? Think about the effectiveness and reliability of your wand.

If this wand were to be used by others, you would want other gestures to be recorded as different people have different bodily proportions and certain gestures may not be recognized if they're not trained for a variety of use cases. For example, I'm 6'3, and my gestures would likely be tracked differently than a classmate that is 5' as they may complete it faster, and less gyroscopic change may be detected. However, in a ficitonal world, I believe that we should only have our own data for security as we would not want others to be able to use our wand and be liable for any damage that may occur.Overall though, multiple people tracking the data will help with edge cases and ubiquitous usage of the wand.

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

With this, I eventually came to find that FFT mixed with a highpass filter worked best, as there was too much label overlap with other settings. As this is capturing a gesture overtime, I felt that a highpass was better than a lowpass since we wanted to filter out slow sways that may occur with the wand.

7. Discussion: Give at least two potential strategies to further enhance your model performance.


## Issues

`cd "c:\Users\jaden\Documents\GitHub\MSTI\Q3\TECHIN 515\Lab 4\gesture_capture"; python -m process_gesture_data --list-ports`

python process_gesture_data.py --gesture begin