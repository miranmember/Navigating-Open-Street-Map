// application.cpp
// Ayush Patel and Miran Member
//
// University of Illinois at Chicago
// CS 251: Fall 2020
// Project #7 - Openstreet Maps
//
// References:
// TinyXML: https://github.com/leethomason/tinyxml2
// OpenStreetMap: https://www.openstreetmap.org
// OpenStreetMap docs:
//   https://wiki.openstreetmap.org/wiki/Main_Page
//   https://wiki.openstreetmap.org/wiki/Map_Features
//   https://wiki.openstreetmap.org/wiki/Node
//   https://wiki.openstreetmap.org/wiki/Way
//   https://wiki.openstreetmap.org/wiki/Relation
//

#include <iostream>
#include <iomanip>  /*setprecision*/
#include <string>
#include <vector>
#include <map>
#include <queue>
#include <stack>
#include <limits>
#include <cstdlib>
#include <cstring>
#include <cassert>

#include "tinyxml2.h"
#include "dist.h"
#include "osm.h"
#include "graph.h"

using namespace std;
using namespace tinyxml2;

void prindBuildingInfo(BuildingInfo& building);

bool findBuilding(vector<BuildingInfo>& Buildings,
				   int buildingCount,
				   int& buildingIndex,
				   string buildingName);

long long findMin(map<long long, Coordinates>& Nodes,
				   vector<FootwayInfo>& paths,
				   BuildingInfo building);

bool checkVisited(long long vertex, vector<long long> v);

map<long long, long long> Dijkstra(graph<long long, double>& G,
						   long long startV,
						   map<long long, double>& distances);

int main() {
  // maps a Node ID to it's coordinates (lat, lon)
  map<long long, Coordinates>  Nodes;
  // info about each footway, in no particular order
  vector<FootwayInfo>          Footways;
  // info about each building, in no particular order
  vector<BuildingInfo>         Buildings;
  XMLDocument                  xmldoc;
  graph<long long, double>	   G;

  cout << "** Navigating UIC open street map **" << endl;
  cout << endl;
  cout << std::setprecision(8);

  string def_filename = "map.osm";
  string filename;

  cout << "Enter map filename> ";
  getline(cin, filename);

  if (filename == "") {
    filename = def_filename;
  }

  //
  // Load XML-based map file
  //
  if (!LoadOpenStreetMap(filename, xmldoc)) {
    cout << "**Error: unable to load open street map." << endl;
    cout << endl;
    return 0;
  }

  //
  // Read the nodes, which are the various known positions on the map:
  //
  int nodeCount = ReadMapNodes(xmldoc, Nodes);

  //
  // Read the footways, which are the walking paths:
  //
  int footwayCount = ReadFootways(xmldoc, Footways);

  //
  // Read the university buildings:
  //
  int buildingCount = ReadUniversityBuildings(xmldoc, Nodes, Buildings);

  //
  // Stats
  //
  assert(nodeCount == (int)Nodes.size());
  assert(footwayCount == (int)Footways.size());
  assert(buildingCount == (int)Buildings.size());

  cout << endl;
  cout << "# of nodes: " << Nodes.size() << endl;
  cout << "# of footways: " << Footways.size() << endl;
  cout << "# of buildings: " << Buildings.size() << endl;

  // we read the Nodes map and add them to the graph using for range loop.
  for (auto & e : Nodes) {
  	G.addVertex(e.first);
  }

  // read the footways vector and adding them to the graph as edges.
  for (auto & e : Footways) {
  	int size = e.Nodes.size();
  	double lat1, lat2, lon1, lon2, distance;
  	for (int i = 0; i < size-1; i++) {
  		lat1 = Nodes.at(e.Nodes[i]).Lat;
  		lon1 = Nodes.at(e.Nodes[i]).Lon;
  		lat2 = Nodes.at(e.Nodes[i+1]).Lat;
  		lon2 = Nodes.at(e.Nodes[i+1]).Lon;

  		distance = distBetween2Points(lat1, lon1, lat2, lon2);  // calculates the distBetween2Points using the given function.
  		G.addEdge(e.Nodes[i], e.Nodes[i+1], distance);
  		G.addEdge(e.Nodes[i+1], e.Nodes[i], distance);
  	}
  }


	cout << "# of vertices: " << G.NumVertices() << endl;
	cout << "# of edges: " << G.NumEdges() << endl;
	cout << endl;

  //
  // Navigation from building to building
  //
  string startBuilding, destBuilding;

  cout << "Enter start (partial name or abbreviation), or #> ";
  getline(cin, startBuilding);

  while (startBuilding != "#") {
    cout << "Enter destination (partial name or abbreviation)> ";
    getline(cin, destBuilding);


    int startBuildingIndex = -1;
	int destBuildingIndex = -1;
	long long closestStartNode = -1;
	long long closestDestNode = -1;
	// error checks to see if a building exists using the function we wrote.
	if (!findBuilding(Buildings, buildingCount, startBuildingIndex, startBuilding)) {
		cout << "Start building not found" << endl;
	} else {
		// error checks the Destination to see if the Destination building exists
		if (!findBuilding(Buildings, buildingCount, destBuildingIndex, destBuilding)) {
			cout << "Destination building not found" << endl;
		} else {
			cout << "Starting point:" << endl;
			prindBuildingInfo(Buildings.at(startBuildingIndex));
			cout << "Destination point:" << endl;
			prindBuildingInfo(Buildings.at(destBuildingIndex));

			// finds the closes Nodes to the Buildings.
			closestStartNode = findMin(Nodes, Footways, Buildings[startBuildingIndex]);
			closestDestNode = findMin(Nodes, Footways, Buildings[destBuildingIndex]);

			cout << "\nNearest start node:\n"
				 << " " << closestStartNode << endl
				 << " (" << Nodes[closestStartNode].Lat << ", "
				 << Nodes[closestStartNode].Lon << ")\n";

			cout << "Nearest destination node:\n"
				 << " " << closestDestNode << endl
				 << " (" << Nodes[closestDestNode].Lat << ", "
				 << Nodes[closestDestNode].Lon << ")\n";

			cout << "\nNavigating with Dijkstra..." << endl;

			long long temp = closestDestNode;
			map <long long, double> distances;
			vector <long long> path;
			map <long long, long long> predecessor;
			predecessor = Dijkstra(G, closestStartNode, distances);
			double INF = numeric_limits<double>::max();

			if (distances.at(closestDestNode) == INF) {
				cout << "Sorry, destination unreachable" << endl;
			} else {
				cout << "Distance to dest: " << distances.at(closestDestNode)
					 << " miles" << endl
					 << "Path: ";

				while (temp != closestStartNode) {
					temp = predecessor.at(temp);
					path.push_back(temp);
				}
				for (int i = (int)path.size() - 1; i >= 0; i--) {
					cout << path.at(i) << "->";
				}

				cout << closestDestNode << endl;
			}
		}
	}


    //
    // another navigation?
    //
    cout << endl;
    cout << "Enter start (partial name or abbreviation), or #> ";
    getline(cin, startBuilding);
  }

  //
  // done:
  //
  cout << "** Done **" << endl;
  return 0;
}

// prints the information of the Building.
void prindBuildingInfo(BuildingInfo& building) {
	cout << " " << building.Fullname << endl
		 << " (" << building.Coords.Lat << ", "
		 << building.Coords.Lon << ")\n";
}

// looks if a given building name exists in the buildings vector
// returns false if it does not exists and trur if it does.
bool findBuilding(vector<BuildingInfo>& Buildings,
				   int buildingCount,
				   int& buildingIndex,
				   string buildingName) {
	for (int i = 0; i < buildingCount; i++) {
		if (buildingName.compare(Buildings.at(i).Abbrev) == 0) {
			buildingIndex = i;
			return true;
		} else if (Buildings.at(i).Fullname.find(buildingName) != string::npos) {
			buildingIndex = i;
			return true;
		}
	}

	return false;
}

// find the closese footway node to the given building using the paths
// vector and utizling the distBetween2Points function.
long long findMin(map<long long, Coordinates>& Nodes,
				   vector<FootwayInfo>& paths,
				   BuildingInfo building) {
	long long closestNode;
	double minDist = 9999999999;
	double temp;

	for (auto& e : paths) {
		for (auto& node : e.Nodes) {
			temp =  distBetween2Points(building.Coords.Lat, building.Coords.Lon, Nodes[node].Lat, Nodes[node].Lon);
			if (temp < minDist) {
				minDist = temp;
				closestNode = node;
			}
		}
	}

	return closestNode;
}

bool checkVisited(long long vertex, vector<long long> v) {
	for(unsigned i = 0; i < v.size(); i++) {
		if(v[i] == vertex) {
			return true;
		}
	}
	return false;
}

// happy thank you giving.
map<long long, long long> Dijkstra(graph<long long, double>& G,
						   long long startV,
						   map<long long, double>& distances) {
	class prioritize {
	 public:
		  bool operator()(const pair<long long, double>& p1, const pair<long long, double>& p2) const{
		      if (p1.second > p2.second) {
		          return true;
		      } else if (p1.second < p2.second) {
		          return false;
		      } else {
		          return p1.first > p2.first;
		      }
		  }
	};

	vector<long long>  visited;
	priority_queue<pair<long long, double>, vector<pair<long long, double>>, prioritize> unvisited;
	vector<long long> V = G.getVertices();
	map <long long, long long> predecessor;
	double INF = numeric_limits<double>::max();
	double edgeWeight;
	long long adjVertex;
	double alternativePathDistance;
	double y;

	for (auto curV : V) {
		distances[curV] = INF;
		unvisited.push(make_pair(curV, INF));
		predecessor.emplace(curV, 0);
	}

	distances[startV] = 0;
	unvisited.push(make_pair(startV, 0));
	while (!unvisited.empty()) {
		pair<long long, double> currentV = unvisited.top();
		auto x = currentV.first;
		y = currentV.second;
		unvisited.pop();
		bool m = checkVisited(x, visited);
		if (y == INF) {
			break;
		} else if (m == true) {
			continue;
		} else {
			visited.push_back(x);
		}
		set<long long> adjV = G.neighbors(x);

		for(long long val : adjV) {
			G.getWeight(x, val, edgeWeight);
			alternativePathDistance = distances[x]+edgeWeight;
			if(alternativePathDistance < distances[val]) {
				distances[val] = alternativePathDistance;
				predecessor[val] = x;
				unvisited.push(make_pair(val, alternativePathDistance));
			}
		}
	}
	return predecessor;
}
