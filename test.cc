// test.cc
// Solve the traveling Salesman problem.
//
// To compile, use this command:
//  bazel build :tsp_main
// To run, use this command:
//  bazel run :tsp_main -- --graph_file=file_name.txt

#include <algorithm>
#include <cctype>
#include <climits>
#include <fstream>
#include <functional>
#include <iostream>
#include <locale>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/flags/usage.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_cat.h"
#include "glog/logging.h"

ABSL_FLAG(std::string, graph_file, "", "File with input graph");
ABSL_FLAG(int, algorithm , 1, "Which algorithm to choose.");

typedef std::unordered_map< std::string, std::unordered_map<std::string, int> > Graph;
typedef std::pair< std::string, int > TspResult;
typedef std::unordered_set< std::string > FoundCities;

enum class Algorithm { kUndefined, kClosestNeighbor };

template<typename K, typename V>
void print_map(std::unordered_map<K,V> const &m) {
    for (auto const& pair: m) {
        LOG(INFO) << "{" << pair.first << ": " << pair.second << "}";
    }
}

// trim from start
static inline std::string &ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int c) {return !std::isspace(c);}));
    return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int c) {return !std::isspace(c);}).base(), s.end());
    return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
    return ltrim(rtrim(s));
}

std::string least(const Graph& graph, const FoundCities& completed, std::string current_city, int& cost) {
  std::string nc;
  int min = INT_MAX;
  auto cities = graph.find(current_city);
  if (cities != graph.end()) {
    for (auto const& connected_city: cities->second) {
      std::string next_city = connected_city.first;
      if (completed.find(next_city) == completed.end()) {
        if (graph.at(current_city).at(next_city) < min) {
          min = graph.at(current_city).at(next_city);
          nc = next_city;
        }
      }
    }
    if (min != INT_MAX) {
      cost += min;
    }
  }
  return nc;
}

// closest_neigher hueristically solves TSP by making a graph of closest neighbors
//
// params:
//  graph -> Graph object with entire graph in map<key, map<key, cost>> structure
//
// returns:
//  statusor<TspResult> containing an error if something fails. else the best tsp
//    result found
//
// Basically, we go through all the nodes as the start city. From the start city,
//  we choose the closest neighbor and go through. We continue until we have went
//  through all the citys. Store path and cost. Try the next start_city. If that
//  result is better than the last result, switch them. repeat until all start
//  cities have been done. Return result.
absl::StatusOr<TspResult> closest_neighbor(const Graph& graph) {
  TspResult best_result = {"", INT_MAX};
  for(auto it = graph.begin(); it != graph.end(); ++it) {
    std::string start_city = it->first;
    std::string next_city = start_city;
    FoundCities completed;
    completed.insert(start_city);
    int cost = 0;
    std::string path = next_city;
    do {
      next_city = least(graph, completed, next_city, cost);
      if (!next_city.empty()) {
        path.append("--->");
        path.append(next_city);
        completed.insert(next_city);
      } else {
        next_city = start_city;
      }
    } while (next_city != start_city);
    if (completed.size() != graph.size()) {
      LOG(INFO) << completed.size() << " " << graph.size();;
      return absl::FailedPreconditionError("Graph is not complete.");
    } else {
      if (cost < best_result.second) {
        best_result.first = path;
        best_result.second = cost;
      }
    }
  }
  return best_result;
}

std::vector<std::string> split(const std::string& s, char delimiter) {
  std::vector<std::string> tokens;
  std::string token;
  std::istringstream tokenStream(s);
  while (std::getline(tokenStream, token, delimiter)) {
    tokens.push_back(trim(token));
  }
  return tokens;
}

Graph readInGraph(std::string filename) {
  Graph graph;
  std::string line;
  std::ifstream f (filename);
  bool doneWithNodes = false;
  while(std::getline(f, line)) {
    if ((trim(line).compare("") == 0)) {
      doneWithNodes = true;
    } else if (doneWithNodes) {
      auto tokens = split(line, ' ');
      auto a_edges = graph.find(tokens[0]);
      auto b_edges = graph.find(tokens[1]);
      if (a_edges != graph.end() && b_edges != graph.end()) {
        a_edges->second.insert({tokens[1], std::stoi(tokens[2])});
        b_edges->second.insert({tokens[0], std::stoi(tokens[2])});
      }
    } else {
      std::unordered_map<std::string, int> temp;
      graph.insert(std::make_pair(trim(line), temp));
    }
  }
  return graph;
}

int main(int argc, char** argv) {
  google::InitGoogleLogging(argv[0]);
  absl::SetProgramUsageMessage(
    "This program solves TSP with an algorithm of your choosing.");
  absl::ParseCommandLine(argc, argv);

  const Graph& graph = readInGraph(absl::GetFlag(FLAGS_graph_file));
  LOG(INFO) << "Graph size: " << graph.size();

  const Algorithm algo = Algorithm(absl::GetFlag(FLAGS_algorithm));

  absl::StatusOr<TspResult> result;
  switch (algo) {
    case Algorithm::kClosestNeighbor:
      LOG(INFO) << "Running the closest neighbor algorithm...";
      result = closest_neighbor(graph);
      break;
    default:
      LOG(ERROR) << "Please input a valid algorithm.";
      return 1;
  }

  if (result.ok()) {
    std::cout << "Path: " << result->first << "\n";
    std::cout << "Cost of this path is: " << result->second << "\n";
  } else {
    std::cout << result.status().ToString() << "\n";
  }
  return 0;
}
