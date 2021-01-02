# TSP Implementations
Zhach's Implementation and Optimization of the TSP problem

## Goals
The main goal is to combine some TSP solutions together in C++ and try to compare them.

## How
I want to make a structure where I can read in a graph, choose and algorithm with a group of enums, and then it can run said algorithm.

The idea is really just to have a plug an play option with algorithms. All I need to do is send them is a "Graph" structure and then I expect either a string of the shortest path or an error with the graph (like it's not a completed graph).

## Structure
The structure is simple enough.

1. Read in arguments
2. Parse in graph file into an object
3. Send graph to chosen algorithm
4. Time the algorithm
5. Get result and print out path and time it took

## Algorithms to look at
I want to take a look at a few listed in [Traveling Salesman Problem: An Overview of Applications, Formulations, and Solution Approaches](https://cdn.intechopen.com/pdfs/12736/intechtraveling_salesman_problem_an_overview_of_applications_formulations_and_solution_approaches.pdf):

1. Brute force (check all permutations)
2. Held–Karp algorithm
3. Closest neighbor hueristic
4. Shortest edge hueristic
5. Subtour hueristic
6. Genetic algorithm
7. Ant colony
8. Christofide heuristic