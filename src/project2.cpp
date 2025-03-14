/*
 * Copyright (C) Mohsen Zohrevandi, 2017
 *               Rida Bazzi 2019
 * Do not share this file with anyone
 */
#include "analysis.h"
#include "consts.h"
#include "parser.h"
#include "types.h"
#include "util.h"
#include <cstdlib>
#include <iostream>

using namespace std;

/*
 * Task 1:
 * Printing the terminals, then nonterminals of grammar in appearing order
 * output is one line, and all names are space delineated
 */
void Task1(const Grammar &grammar) {
    std::string terms = util::join_vec_string(grammar.term_order, " ");
    std::string non_terms = util::join_vec_string(grammar.non_term_order, " ");
    std::cout << terms << " " << non_terms;
}

/*
 * Task 2:
 * Print out nullable set of the grammar in specified format.
 */
void Task2(const Grammar &grammar) { analysis::print_nullable_set(grammar); }

// Task 3: FIRST sets
void Task3(const Grammar &grammar) { analysis::print_first_sets(grammar); }

// Task 4: FOLLOW sets
void Task4(const Grammar &grammar) { analysis::print_follow_sets(grammar); }

// Task 5: left factoring
void Task5(const Grammar &grammar) {
    analysis::print_left_factored_grammar(grammar);
}

// Task 6: eliminate left recursion
void Task6(const Grammar &grammar) {
    analysis::print_grammar_without_left_recursion(grammar);
}

auto main(int argc, char *argv[]) -> int {

    int task = 0;

    if (argc < 2) {
        cout << "Error: missing argument\n";
        return 1;
    }

    /*
       Note that by convention argv[0] is the name of your executable,
       and the first argument to your program is stored in argv[1]
     */

    task = atoi(argv[1]);

    Parser parser = Parser();
    parser.parse_input();
    auto grammar = parser.generate_grammar();

    switch (task) {
    case TASK_1:
        Task1(grammar);
        break;

    case TASK_2:
        Task2(grammar);
        break;

    case TASK_3:
        Task3(grammar);
        break;

    case TASK_4:
        Task4(grammar);
        break;

    case TASK_5:
        Task5(grammar);
        break;

    case TASK_6:
        Task6(grammar);
        break;

    default:
        cout << "Error: unrecognized task number " << task << "\n";
        break;
    }
    return 0;
}
