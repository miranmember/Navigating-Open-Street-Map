# Navigating-Open-Street-Map
This project is a tool for navigating the University of Illinois at Chicago (UIC) campus using data from the Open Street Map (OSM) project. It allows a user to find the shortest path between two buildings on the UIC campus, given their names.

## How to Use
To use the program, compile and run the main.cpp file. The program will prompt the user to enter the name of an OSM map file, or press enter to use the default map.osm file. The program will then read in the map data and print out statistics about the number of nodes, footways (walking paths), and buildings in the map.

The user can then enter the names of two buildings to find the shortest path between them. The program will search for the buildings in the map and calculate the shortest path using Dijkstra's algorithm on a graph representation of the map. The program will print the distance of the path and the names of the buildings along the path.

The program can be exited by entering Q when prompted for building names.


## Map Data
The program reads in map data from an OSM XML file and stores it in the following data structures:

+ Nodes: a map that maps a Node ID to its coordinates (latitude and longitude)
+ Footways: a vector of FootwayInfo structures that contain information about each footway (walking path) in the map, including the IDs of the nodes that form the path
+ Buildings: a vector of BuildingInfo structures that contain information about each building in the map, including its name and the ID of its nearest node


## Graph Representation
The program represents the map as a graph, with each node representing a position on the map and each edge representing a footway. The weights of the edges are the distances between the nodes, calculated using the Haversine formula.

## Shortest Path Calculation
The shortest path between two buildings is calculated using Dijkstra's algorithm on the graph representation of the map. The algorithm calculates the shortest path from the starting building to all other buildings, and the path to the destination building is extracted from the results.


## Other Notes
The program includes error handling for invalid input and for cases where the specified buildings are not found in the map.
