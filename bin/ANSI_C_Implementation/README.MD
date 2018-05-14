## Installation

Use the make file or compile it anywhere else.
`$ make`
It had to be ANSI C so it even compiles on VS.


## Features

This little piece of code compares 3 different implementations of a least mean square filter. 

+ local mean
+ direct predecessor
+ differential predecessor

Greyscale PPM files can be used for input at this iteration. Output will be genreeated as .txt file with predicted value genereated by the filter and its error value as well as given actual value from the PPM file. Furthermore there is an output as an svg graph to compare the implementatiosn on a visual level. Just open __graphResults.html__
You can hide graphs by clicking on its name for better visibility.
 

