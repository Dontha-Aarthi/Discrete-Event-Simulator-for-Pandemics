# Discrete-Event-Simulator-for-Pandemics
A Discrete Event Infection Spread Simulator.  

To compile the file, use: ```g++ -std=c++20 simulator.cpp -o simulator```  
After compiling, to run the simulator: type ```./simulator``` in terminal. This gives the statistics as the output.  

Statistics include, the number of people Susceptible, Infected and Recovered for each timestamp untill everyone get recovered.  
It also displays the timestamp at which a node gets infected and its shortest distance from the start node(source node).
