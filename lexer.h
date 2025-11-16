#pragma once
#include <string>
#include <cctype>

enum Token {
    tok_eof = -1,
    tok_number = -2
};

static std::string Input;
static size_t IndexTok = 0;
static double NumVal = 0.0;

static int getTok() {
    // skip whitespace
    while (IndexTok < Input.size() && isspace((unsigned char)Input[IndexTok]))
        ++IndexTok;

    if (IndexTok >= Input.size()) return tok_eof;

    char c = Input[IndexTok];

    // number (integer or decimal)
    if (isdigit(c) || c == '.') {
        std::string num;
        bool seenDot = false;
        while (IndexTok < Input.size() &&
               (isdigit((unsigned char)Input[IndexTok]) || (!seenDot && Input[IndexTok]=='.'))) {
            if (Input[IndexTok] == '.') seenDot = true;
            num.push_back(Input[IndexTok++]);
        }
        NumVal = std::stod(num);
        return tok_number;
    }

    // single-character token
    ++IndexTok;
    return c;
}
