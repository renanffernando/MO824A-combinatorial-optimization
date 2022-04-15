from itertools import combinations
import numpy as np
import gurobipy as gp
from gurobipy import GRB

def kstsp(number_of_points, number_of_similar_edges, number_of_cycles, points_by_cycle):

    # edges cost

    def distance (source, target):
        return np.linalg.norm(target - source, ord=2)

    def make_edges_cost (points, number_of_points):
        assert number_of_points <= points.shape[0]
        return {
            (i, j): distance(points[i], points[j])
            for i in range(number_of_points)
            for j in range(i)
        }
        return

    # model auxiliary methods

    ## variables

    ### x

    def make_variables (model, cycle_number, edges, costs):
        variables = model.addVars(edges, obj=costs, vtype=GRB.BINARY, name=f'e{cycle_number}')
        for i, j in edges:
            variables[j, i] = variables[i, j]  # edge in opposite direction
        return variables

    def make_variables_by_cycle (model, edges, edges_cost_by_cycle):
        variables_by_cycle = [
            make_variables(
                model,
                cycle_number,
                edges,
                edges_cost_by_cycle[cycle_number]
            )
            for cycle_number in range(number_of_cycles)
        ]
        return variables_by_cycle

    ### D
    def make_edge_duplication_variables(model, edges):
        edge_duplication_variables = model.addVars(edges, obj=0, vtype=GRB.BINARY, name=f'D')
        return edge_duplication_variables

    ## constraints

    ### x

    def make_vertex_constraints_by_cycle (model, variables_by_cycle, number_of_cycles, number_of_points):
        vertex_constraints_by_cycle = []
        for cycle_number in range(number_of_cycles):
            vertex_constraints = []
            vars = variables_by_cycle[cycle_number]
            for source_point in range(number_of_points):
                edges_into_point = [
                    (source_point, target_point)
                    for target_point in range(number_of_points)
                    if target_point != source_point
                ]
                constr = model.addConstr(gp.quicksum([vars[edge] for edge in edges_into_point]) == 2)
                vertex_constraints.append(constr)
            vertex_constraints_by_cycle.append(vertex_constraints)
        return vertex_constraints_by_cycle

    ### x and D

    def make_compatibility_constraints(model, variables_by_cycle, edge_duplication_variables, edges):
        constraints = []
        for edge in edges:
            constr = model.addConstr(
                variables_by_cycle[0][edge] + variables_by_cycle[1][edge] >= 2 * edge_duplication_variables[edge]
            )
            constraints.append(constr)
        return constraints

    ### D and k
    def make_similarity_constraint(model, edge_duplication_variables, number_of_similar_edges):
        constraint = model.addConstr(
            gp.quicksum(edge_duplication_variables) >= number_of_similar_edges
        )
        return constraint

    ## lazy constraint evaluation

    def lazy_subtour_elimination(model, where):
        if where == GRB.Callback.MIPSOL:
            variables_by_cycle_current_solution = [
                model.cbGetSolution(model._variables_by_cycle[cycle_number])
                for cycle_number in range(number_of_cycles)
            ]
            for cycle_number in range(number_of_cycles):
                tour = find_shortest_subtour_in_edges(variables_by_cycle_current_solution[cycle_number])
                if len(tour) < number_of_points:
                    # add subtour elimination constr. for every pair of cities in tour
                    variables = model._variables_by_cycle[cycle_number]
                    model.cbLazy(
                        gp.quicksum(variables[i, j] for i, j in combinations(tour, 2)) <= len(tour) - 1
                    )
        return


    # Given a tuplelist of edges, find the shortest subtour
    def find_shortest_subtour_in_edges(variables):
        # make a list of edges selected in the solution
        edges = gp.tuplelist((i, j) for i, j in variables.keys()
                            if variables[i, j] > 0.5)
        unvisited = list(range(number_of_points))
        cycle = range(number_of_points+1)  # initial length has 1 more city
        while unvisited:  # true if list is non-empty
            thiscycle = []
            neighbors = unvisited
            while neighbors:
                current = neighbors[0]
                thiscycle.append(current)
                unvisited.remove(current)
                neighbors = [j for i, j in edges.select(current, '*')
                            if j in unvisited]
            if len(cycle) > len(thiscycle):
                cycle = thiscycle
        return cycle

    # generate data
    edges_cost_by_cycle = [
        make_edges_cost(points_by_cycle[cycle_number], number_of_points)
        for cycle_number in range(number_of_cycles)
    ]
    edges = list(edges_cost_by_cycle[0].keys())

    # create gurobi model
    model = gp.Model()

    # variables
    ## x
    variables_by_cycle = make_variables_by_cycle(model, edges, edges_cost_by_cycle)
    model._variables_by_cycle = variables_by_cycle
    ## D
    edge_duplication_variables = make_edge_duplication_variables(model, edges)

    # constraints
    constraints_by_cycle = make_vertex_constraints_by_cycle(model, variables_by_cycle, number_of_cycles, number_of_points)
    compatibility_constraints = make_compatibility_constraints(model, variables_by_cycle, edge_duplication_variables, edges)
    similarity_constraint = make_similarity_constraint(model, edge_duplication_variables, number_of_similar_edges)

    # lazy constraints
    model.Params.LazyConstraints = 1

    # time limit
    model.Params.TimeLimit = 30*60 # 30 min

    # optimize
    model.optimize(lazy_subtour_elimination)

    # result
    vals = [
        model.getAttr('X', variables_by_cycle[cycle_number])
        for cycle_number in range(number_of_cycles)
    ]
    tours = [
        find_shortest_subtour_in_edges(vals[i])
        for i in range(number_of_cycles)
    ]

    # check solution - partially
    for i in range(number_of_cycles):
        if (len(tours[i]) != number_of_points):
            print('\n\nERROR IN THIS CASE')
    return {
        'number_of_points': number_of_points,
        'number_of_similar_edges': number_of_similar_edges,
        'optimal_value': model.ObjVal,
        'optimal_tours': tours
    }

def display_solution(solution):
    print('# Setup:')
    print(f"\tnumber of points/vertices: {solution['number_of_points']}")
    print(f"\tnumber of similar edges: {solution['number_of_similar_edges']}")
    print('\n# Solution')
    for i in range(number_of_cycles):
        this_tour = solution['optimal_tours'][i]
        print(f'\tSalesman {i} optimal tour: {str(this_tour)}')
    print('\n# Cost')
    print(f"\tOptimal cost: {solution['optimal_value']}")

if __name__ == '__main__':
    from problem_data import number_of_cycles, points_by_cycle
    print('\n\n========================= kSTSP =========================\n\n')
    solution = kstsp(
        number_of_points = 15,
        number_of_similar_edges = 15,
        number_of_cycles = number_of_cycles,
        points_by_cycle = points_by_cycle,
    )
    display_solution(solution)
    print('\n\n========================= end =========================\n\n')
