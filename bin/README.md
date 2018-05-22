# README

### Short explenation of the c# .exe

![alt text](https://github.com/FBRDNLMS/NLMSvariants/blob/master/img/CS_exe_screenshot_1.PNG "C# .exe with loaded picture")

1. Choose the algorithem u want to calculate on and press "start" or "enter"

2. Choose the number of pixels you want to go for

3. Clears the screen and reprints the actual values of your picture or predefined values

4. Choose the learnrate and window size you want, learnrate has to be over 0.0 and under or even 1.0.
   The window size has to be an integer.

5. Loads a picture, it has to be BMP, GIF, EXIF, JPG, PNG or TIFF

6. Check this for generating outputfiles in the same direktory you started your .exe in
   Outputfiles are : weights.txt, weights_after.txt, localMean|directPredecessorer|differentialPredecessorer.txt, results.txt

7. Chart where the algorithems output is displayed


![alt text](https://github.com/FBRDNLMS/NLMSvariants/blob/master/img/CS_exe_screenshot_2.PNG "C# .exe with loaded picture and calculated")

1. Displayes the average error of the calculated algorithem

2. Displayes the variance of the erroer of the calculated algorithem

3. The history of graphs displayed in the chart, it is possible to have graphs of all 3 LMS algorithems in the history.
   There is no way to hide graphs, simply use "Clear" and then calculate again.