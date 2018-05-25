# README

+ Choose ANSI C/C++ version for high amount of input data (Visual C++ is mandatory on Windows).
+ Chosse C# Version for a nice and better graphical output.

### Short explanation of the c# .exe

![alt text](https://github.com/FBRDNLMS/NLMSvariants/blob/master/img/CS_exe_screenshot_1.PNG "C# .exe with loaded picture")

1. Choose the algorithm u want to calculate on and press "start" or "enter"

2. Choose the number of pixels you want to go for. For more than 20,000 pixels, please use the c .exe, because
   this version is not optimized for high number of pixels. Only for visualising small amount of pixel results.

3. Clears the screen and reprints the actual values of your picture or predefined values

4. Choose the learnrate and window size you want, learnrate has to be over 0.0 and under or even 1.0.
   The window size has to be an integer.

5. Loads a picture, it has to be BMP, GIF, EXIF, JPG, PNG or TIFF

6. Check this for generating outputfiles in the same direktory you started your .exe in
   Outputfiles are : weights.txt, weights_after.txt, localMean|directPredecessorer|differentialPredecessorer.txt, results.txt

7. Chart where the algorithems output is displayed


![alt text](https://github.com/FBRDNLMS/NLMSvariants/blob/master/img/CS_exe_screenshot_2.PNG "C# .exe with loaded picture and calculated")

1. Displays the average error of the calculated algorithm. The first "window size" (M) pixels are ignored for this
   calulation, because the not updated weights are causing wrong calculations. You can see it by entering more than
   20 into the window size.

2. Displays the variance of the error of the calculated algorithm. Same as average error, first "window size" (M) pixels are ignored.

3. The history of graphs displayed in the chart, it is possible to have graphs of all 3 LMS algorithems in the history.
   There is no way to hide graphs, simply use "Clear" and then calculate again.


### Explanation of the ANSI C/C++ bin

Choose an input file `$ CPP_NLMS -i <inputfile>`
Any __PPM__ file can be used as input.

![alt text](https://github.com/FBRDNLMS/NLMSvariants/blob/master/img/CPP_NLMS.png  "ANSI_C/C++ bin with loaded picture")

The Output contains the default parameters for window size, color channel, seed for randomization, randomly generated weights and outcome of the nlms functions. Default parameters can be seen in the following picture.
Parameters can be changed. They've got a default value, as long as you don't change them.
Please use `$ CPP_NLMS -h` so you can see an overview of the given parameters.

![alt text](https://github.com/FBRDNLMS/NLMSvariants/blob/master/img/args.png "ANSI_C/C++ bin, overview of args")

Tested ANSI C version with the biggest picture I could find as input data. `$./lms -i cherry_blossom_3456x2592.ppm -n 8999999`
You can see the size has been reduced to the max pixel size of the image. Only limiting factor is HDD/SSD space. Logfiles can get quite big in size.
![alt text](https://github.com/FBRDNLMS/NLMSvariants/blob/master/img/big_input.png "ANSI_C/C++ bin, overview of args")

Logs are created by default:
+ input data (pixels of __PPM__ file)
+ created weigths
+ each nlms function.Predicted value, input value and error value as well as square mean and deviation
+ updated weights for each nlms calculation

`CPP_NLMS -i <inputfile> -g true` prints a graphical overview to `graphResults.html`. The output is an __SVG__ embedded in html.

![alt text](https://github.com/FBRDNLMS/NLMSvariants/blob/master/img/lms_graphing.png "ANSI_C/C++ bin, generate graphical output ")


![alt text](https://github.com/FBRDNLMS/NLMSvariants/blob/master/img/graphing.png "ANSI_C/C++ bin, graphical output ")

