// test.cc
// Solve the traveling Salesman problem.
//
// To compile, use this command:
//  g++ test.cc -lstdc++ -std=c++11
// To run, use this command:
//  ./a.out file_name.txt

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <climits>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>

typedef std::unordered_map<std::string, std::unordered_map<std::string, int>> Graph;

Graph graph;
std::unordered_map<std::string, bool> completed;
int cost = 0;
std::string start_city;

// trim from start
static inline std::string &ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
            std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
            std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
    return ltrim(rtrim(s));
}

std::string least(std::string current_city) {
  std::string nc;
  int min = INT_MAX, kmin;
  auto cities = graph.find(current_city);
  if (cities != graph.end()) {
    for (auto const& connected_city: graph[ current_city.c_str() ]) {
      std::string next_city = connected_city.first;
      if (!completed[next_city]) {
        if (graph[current_city][next_city] +
            graph[next_city][current_city] < min) {
          min = graph[current_city][next_city] + graph[next_city][current_city];
          kmin = graph[current_city][next_city];
          nc = next_city;
        }
      }
    }
    if (min != INT_MAX) {
      cost += kmin;
    }
  }
  return nc;
}

void tsp(const std::string& begin_city) {
  std::string next_city;
  completed[begin_city] = true;
  std::cout << begin_city << "--->";
  next_city = least(begin_city);
  if (next_city.empty()) {
    next_city = begin_city;
    std::cout << start_city << "\n";
    cost += graph[next_city][start_city];
    return;
  }
  tsp(next_city);
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

int main(int argc, char** argv) {
  // Take in graph
  if (argc < 2) {
    std::cout << "Please provide the file name.\n";
    return 0;
  }
  std::string line;
  std::ifstream f (argv[1]);
  bool doneWithNodes = false;
  while(std::getline(f, line)) {
    if (start_city.empty()) {
      start_city = trim(line);
    }
    if ((trim(line).compare("") == 0)) {
      doneWithNodes = true;
    } else if (doneWithNodes) {
      auto tokens = split(line, ' ');
      auto a_edges = graph.find(tokens[0]);
      auto b_edges = graph.find(tokens[1]);
      if (a_edges != graph.end() && b_edges != graph.end()) {
        a_edges->second.insert({tokens[1], std::stoi(tokens[2])}).second;
        b_edges->second.insert({tokens[0], std::stoi(tokens[2])}).second;
      }
    } else {
      std::unordered_map<std::string, int> temp;
      graph.insert(std::make_pair(trim(line), temp));
      completed.insert(std::make_pair(trim(line), false));
    }
  }

  tsp(start_city);
  bool completed_all_cities = true;
  for (auto const& city : completed) {
    completed_all_cities &= city.second;
  }
  if (completed_all_cities)
    std::cout << "Cost of this path is: " << cost << "\n";
  else
    std::cout << "This path does not contain all the nodes in the graph.\n";
  return 0;
}
