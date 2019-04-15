# buildPlate

Code is unfortunately incomplete. Was trying to build a function to arrange items on a build plate by adding new items to a growing blob and recentering the blob every time. The missing component is adding items at "concave points" see below with the *. At the moment the function will ignore those possible placement locations and therefore grows very quickly. The goal would be to grow like a "spiral" to guarantee the most dense placement of components around the center of the build plate

|||||||||<br/>
|||||||||<br/>
|||||||||* <br/>
||||||||||||||<br/>
||||||||||||||<br/>


To run the code:
clone the repo and using the following:
g++ -o ./outputs/arrange arrange.cpp

then to run simply call:
./outputs/arrange

You will then find an output.bmp in the outputs folder showing how the parts are arranged on the build plate
