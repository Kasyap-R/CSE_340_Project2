#pragma once

#include "parser.h"
#include "lexer.h"
#include "util.h"

#include <iostream>
#include <stdexcept>

Parser::Parser() : lexer(LexicalAnalyzer()) {};

void Parser::parse_input() {
    parse_grammar();
    expect(END_OF_FILE);
}

void Parser::parse_grammar() {
    /*Grammar → Rule-list HASH*/
    parse_rule_list();
    expect(HASH);
}

void Parser::parse_rule_list() {
    /*Rule-list → Rule Rule-list | Rule*/
    util::merge_vectors(rules, parse_rule());
    if (parser_util::starts_rule(lexer.peek(1))) {
        parse_rule_list();
    }
}

auto Parser::parse_rule() -> std::vector<Rule> {
    /*Rule → ID ARROW Right-hand-side STAR*/
    Token rule_id = expect(ID);
    update_universe(rule_id);
    expect(ARROW);
    auto rhs = parse_rhs();
    expect(STAR);

    non_terms.insert(rule_id.lexeme); // Update non_term set
    std::vector<Rule> rules;
    for (IDList &id_list : rhs) {
        rules.push_back(Rule{rule_id.lexeme, std::move(id_list)});
    }
    return rules;
}

auto Parser::parse_rhs() -> std::vector<IDList> {
    /*Right - hand - side → Id - list | Id - list OR Right - hand - side*/
    std::vector<IDList> id_lists;
    id_lists.push_back(parse_id_list());

    if (lexer.peek(1).token_type == OR) {
        expect(OR);
        util::merge_vectors(id_lists, parse_rhs());
    }
    return id_lists;
}

auto Parser::parse_id_list() -> IDList { /*Id-list → ID Id-list | epsilon*/
    IDList id_list;
    if (parser_util::is_follow_id_list(lexer.peek(1))) { // Epsilon
        return id_list;
    }

    Token id_tok = expect(ID);
    id_list.push_back(id_tok.lexeme);
    update_universe(id_tok);

    if (lexer.peek(1).token_type == ID) {
        util::merge_vectors(id_list, parse_id_list());
    }

    return id_list;
}

void Parser::update_universe(const Token &tok) {
    if (tok.token_type != ID) {
        throw std::runtime_error(
            "Attempting to add non-ID token to ID list; Fatal Error;");
    }

    if (universe.count(tok.lexeme) != 0) {
        return;
    }

    universe.insert(tok.lexeme);
    universe_order.push_back(tok.lexeme);
}

auto Parser::generate_grammar() -> Grammar {
    for (const std::string &id : universe_order) {
        if (non_terms.count(id) == 0) {
            terms.insert(id);
            term_order.push_back(id);
        } else {
            non_term_order.push_back(id);
        }
    }

    return Grammar{std::move(non_terms), std::move(terms),
                   std::move(non_term_order), std::move(term_order),
                   std::move(rules)};
}

void Parser::syntax_error() {
    std::cout << "SYNTAX ERROR !!!!!&%!!\n";
    exit(1);
}

auto Parser::expect(TokenType expected_type) -> Token {
    Token token = lexer.GetToken();
    if (token.token_type != expected_type) {
        syntax_error();
    }
    return token;
}
