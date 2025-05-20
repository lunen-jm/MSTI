# Discussion

With this lab, I reached out to Luyao about my issues with running it and have added some assets in here. When I was trying to run the virtual environment, I was having issues with packages and still had to install the latest versions of pandas, scikit-learn, and micromlgen. 

Additionally, I could not get my laptop to work within the virtua environment and upload code to my ESP32, so I got permission from Luyao to use Shareef's device for my demonstration video.

I did end up getting the python script to work eventually, and have all of those assets in this repository, as well as my attempt at rewriting the code to be uploaded to my ESP32.

## Q1 - Are all 10 questions important to create the sorting hat? If you were to remove some questions to improve user experience, which questions would you remove and justify your answer.

I think they all have varying levels of importance, and some are more important than others. I think the very first question is the most important as it represents the ethos of the houses themselves, but all of the others are to figure out if that one was accurate or not in my opinion. Here are the ones I would remove:

* Preferred pet?; this one is by person not by house, so I don't think it should impact it, especially since animals such as a snake are not present
* What to do with a mystery book?; I just don't think this one makes much sense and there oculd be amuch better question in here relating to like a lost wallet or something
* What kind of friends do you like?; This one is just a repeat of the first one with slightly different wording, but different options
* How do you solve problems / how do you face challenges?; just like last one, two questions that are similar and seem redundant

At the same time, I would actually increase the number of questions and repeat some on purpose to get the user to reduce their internal bias trying to get a specific house.

## Q2 - If you were to improve the sorting hat, what technical improvements would you make? 

Consider:

* How could you improve the model's accuracy or efficiency?
* What additional sensors or hardware could enhance the user experience?
* Does decision tree remain suitable for your choice of new sensors? If yes, carefully justify your answer. If not, what ML model would you use and explain why.

I would make the following improvements to the hat:

* I would add more questions to the overall test, and put a time limit on each one so the user goes with their first response. I would also introduce some true or false questions, or some with more or less than 4 response options so you can get more granular with the analysis and account for ambiguity. I would also prune out the questions that are similar as they could cause confusion and increase the model size for no reason
* A voice sensor that allows users to answer the question out loud instead would be nice, but it would be more costly and could take longer. With that, we could embed (or connect) a model that can parse out voice and categorize it into a few different options, and determine which answer the user was closest to
* I think that a decision tree would still work, as we would be using an additional model to classify freeform responses into categorized buckets from the decision tree. This would also work well with the addition of different types of questions, as you could add additional categories to responses, such as reasons for true or false (if the user provides at least).