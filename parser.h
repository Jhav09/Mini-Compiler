#pragma once
// Grammar:
// expression = term { (+|-) term }
// term       = primary { (*|/) primary }
// primary    = number | '(' expression ')'

#include "lexer.h"
#include "ast.h"
#include <memory>

// current token state
static int CurTok;
static int getNextToken() { return CurTok = getTok(); }

std::unique_ptr<Expr> parseExpression();

static std::unique_ptr<Expr> parsePrimary() {
    if (CurTok == tok_number) {
        auto node = std::make_unique<NumberExpr>(NumVal);
        getNextToken();
        return node;
    }

    if (CurTok == '(') {
        getNextToken(); // eat '('
        auto E = parseExpression();
        if (CurTok == ')') getNextToken(); // eat ')'
        return E;
    }

    // error: unexpected token
    return nullptr;
}

static std::unique_ptr<Expr> parseTerm() {
    auto LHS = parsePrimary();
    while (CurTok == '*' || CurTok == '/') {
        char op = (char)CurTok;
        getNextToken();
        auto RHS = parsePrimary();
        LHS = std::make_unique<BinaryExpr>(op, std::move(LHS), std::move(RHS));
    }
    return LHS;
}

std::unique_ptr<Expr> parseExpression() {
    auto LHS = parseTerm();
    while (CurTok == '+' || CurTok == '-') {
        char op = (char)CurTok;
        getNextToken();
        auto RHS = parseTerm();
        LHS = std::make_unique<BinaryExpr>(op, std::move(LHS), std::move(RHS));
    }
    return LHS;
}
