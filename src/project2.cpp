/*
 * Copyright (C) Mohsen Zohrevandi, 2017
 *               Rida Bazzi 2019
 * Do not share this file with anyone
 */
#include "parser.h"
#include "types.h"
#include "util.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>

using namespace std;

// read grammar
void ReadGrammar() {}

/*
 * Task 1:
 * Printing the terminals, then nonterminals of grammar in appearing order
 * output is one line, and all names are space delineated
 */
void Task1(const ExecutionContext &exec_ctxt) {
    std::string terms = util::join_vec_string(exec_ctxt.term_order, " ");
    std::string non_terms =
        util::join_vec_string(exec_ctxt.non_term_order, " ");
    std::cout << terms << " " << non_terms;
}

/*
 * Task 2:
 * Print out nullable set of the grammar in specified format.
 */
void Task2() {}

// Task 3: FIRST sets
void Task3() {}

// Task 4: FOLLOW sets
void Task4() {}

// Task 5: left factoring
void Task5() {}

// Task 6: eliminate left recursion
void Task6() {}

int main(int argc, char *argv[]) {
    int task;

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
    auto exec_ctxt = parser.generate_execution_context();
    switch (task) {
    case 1:
        Task1(exec_ctxt);
        break;

    case 2:
        Task2();
        break;

    case 3:
        Task3();
        break;

    case 4:
        Task4();
        break;

    case 5:
        Task5();
        break;

    case 6:
        Task6();
        break;

    default:
        cout << "Error: unrecognized task number " << task << "\n";
        break;
    }
    return 0;
}
