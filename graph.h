// graph.h
// Ayush Patel and Miran Member
//
// Basic graph class using adjacency matrix representation.  Currently
// limited to a graph with at most 100 vertices.
//
// University of Illinois at Chicago
// CS 251: Fall 2020
// Project #7 - Openstreet Maps
//

#pragma once

#include <iostream>
#include <stdexcept>
#include <vector>
#include <set>
#include <unordered_map>

using namespace std;

template<typename VertexT, typename WeightT>
class graph {
 private:
  // using unordered_map gives us the advantage of having really
  // good time complexity and makes coding more easier.
  // the unordered_map map holds the Vertex and edge.
  // the edge also holds another map for the weight and value.
  unordered_map<VertexT, unordered_map<VertexT, WeightT>> AdjList;
  
 public:
  // since we are not allocating any memory we just have
  // an empty default constructor.
  graph() {}

  // Copy constructor
  // for our copy constructor we just use the = operator
  // which comes with the include map.
  graph(const graph& other) {
  	this->AdjList = other.AdjList;
  }

  // Equals operator

  // we use the = operator and the clear fucntion from the map
  // for our = operator function.
  graph& operator=(const graph& other) {
  	this->AdjList.clear();
  	this->AdjList = other.AdjList;
  }


  //
  // NumVertices
  //
  // Returns the # of vertices currently in the graph.
  //
  // simply return the size of the map.
  int NumVertices() const {
    return static_cast<int>(this->AdjList.size());
  }

  //
  // NumEdges
  //
  // Returns the # of edges currently in the graph.
  //
  // count the amount of edges there are by using the size function
  // on the second map inside the orignal map and adding it to the
  // count which is returned to the user.
  int NumEdges() const {
    int count = 0;
    for (auto & e : AdjList) {
      count += e.second.size();
    }
    return count;
  }

  //
  // addVertex
  // this just adds the vertec using emplace from map and initilizes
  // an empty map for the edges. the only time this will reutn false
  // is if the Vertex already exists. we used the count function from
  // map which will return the numver of times that specific Vertex
  // appeared in the map. if its greater then 0 that means its already
  // there.
  bool addVertex(VertexT v) {
    //
    // is the vertex already in the graph?  If so, we do not
    // insert again otherwise Vertices may fill with duplicates:
    //
    if (AdjList.count(v) > 0) {
      return false;
    }

    //
    // if we get here, vertex does not exist so insert.
    //
    AdjList.emplace(v, unordered_map<VertexT, WeightT>());

    return true;
  }

  //
  // addEdge
  //
  // Adds the edge (from, to, weight) to the graph, and returns
  // true.  If the vertices do not exist or for some reason the
  // graph is full, false is returned.
  //
  // NOTE: if the edge already exists, the existing edge weight
  // is overwritten with the new edge weight.
  //
  // when adding an edge if its vertex is not found than we have
  // to return false. then we use just add in the WeightT
  // and the value in the map of map.
  bool addEdge(VertexT from, VertexT to, WeightT weight) {
    if (AdjList.count(from) <= 0) {  // not found:
      return false;
    }

    if (AdjList.count(to) <= 0) {  // not found:
      return false;
    }

    //
    // the vertices exist and we have the row and col of the
    //
    if (this->AdjList.at(from).count(to) <= 0) {  // if no edge, then add edge
    	AdjList[from].emplace(to, weight);
    } else {  // else update weight
    	AdjList[from][to] = weight;
    }

    return true;
  }

  //
  // getWeight
  //
  // Returns the weight associated with a given edge.  If
  // the edge exists, the weight is returned via the reference
  // parameter and true is returned.  If the edge does not
  // exist, the weight parameter is unchanged and false is
  // returned.
  //
  // just checks with multiple edge cases in case it does not exist
  // returns false and if it does exist just reuturn the weight
  // using the at fucntion which comes with maps.
  bool getWeight(VertexT from, VertexT to, WeightT& weight) const {
    if (AdjList.count(from) <= 0) {  // not found:
      return false;
    }

    if (AdjList.count(to) <= 0) {  // not found:
      return false;
    }

    //
    // the vertices exist, but does the edge exist?
    //
    if (this->AdjList.at(from).count(to) <= 0) {  // no:
      return false;
    }

    //
    // Okay, the edge exists, return the weight via the
    // reference parameter of the map.
    //
    weight = this->AdjList.at(from).at(to);

    return true;
  }

  //
  // neighbors
  //
  // Returns a set containing the neighbors of v, i.e. all
  // vertices that can be reached from v along one edge.
  // Since a set is returned, the neighbors are returned in
  // sorted order; use foreach to iterate through the set.
  //
  // from a specific vertex insert all the values from the map
  // in to the set using for each loop.
  set<VertexT> neighbors(VertexT v) const {
    set<VertexT>  S;
    if (AdjList.count(v) <= 0) {  // not found:
      return S;
    }
    for (auto& elem : AdjList.at(v)) {
	    S.insert(elem.first);
    }
    return S;
  }

  //
  // getVertices
  //
  // Returns a vector containing all the vertices currently in
  // the graph.
  //
  // loops through the map and inserts them in to the vector
  vector<VertexT> getVertices() const {
  	vector<VertexT> vertices;
  	for (auto& e : AdjList) {
  		vertices.push_back(e.first);
  	}

    return vertices;  // returns a copy:
  }

  //
  // dump
  //
  // Dumps the internal state of the graph for debugging purposes.
  //
  // Example:
  //    graph<string,int>  G(26);
  //    ...
  //    G.dump(cout);  // dump to console
  void dump(ostream& output) const {
  	int counter = 1;
    output << "***************************************************" << endl;
    output << "********************* GRAPH ***********************" << endl;

    output << "**Num vertices: " << this->NumVertices() << endl;
    output << "**Num edges: " << this->NumEdges() << endl;

    output << endl;
    output << "**Vertices:" << endl;

    for (auto &e : AdjList) {
    	output << " " << counter << ". " << e.first << endl;
    	counter++;
    }

    output << endl;
    output << "**Edges:" << endl;
    for (auto &e : AdjList) {
      output << " row " << e.first << ": ";

      for (auto &elem : e.second) {
      	output << "(" << e.first << "," << elem.first << "," << elem.second << ")  ";
      }
      output << endl;
    }
    output << "**************************************************" << endl;
  }
};
