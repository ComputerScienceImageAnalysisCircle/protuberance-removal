# Protuberance Removal
Program for removing any protuberance in bronchial walls on CT images

Licensed under GPL 3.0

Abstract
--------

On many CT images we can see the disturbance in image quality
often created by inaccurate equipment. As a cheap solution, software
comes to mind to find out all the problems in CT imagery and
repair images. Doing something that normaly could take years
of development of hardware technology, now could be achived in
seconds by postprocessin images by computer algorithms.

Basic Concepts
--------------

Here I am presenting the algorithm for removal of bronchial wall protuberance.
The algorithm is very similar to region growing techniques. 
It takes the middle of bronchial lumen and calculates radius of
bronchial wall for any given angle. If at any point the radius is more 
than avarege thicknes it remove the fragment and replaces it with
calculated avarege thick wall.

Contact Info
------------

I am providing paid support for the software to implement it for low-level, even hardware level
and high-level image analysis software. Please contact me at pzabkiewicz@gmail.com

