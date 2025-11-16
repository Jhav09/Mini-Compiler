#include <iostream>
#include <memory>
#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include "codegen.h"

int main() {
    std::cout << "Mini Expression Compiler (type an expression and ENTER)\n";
    std::cout << "Supports: +  -  *  /  and parentheses\n\n";

    std::getline(std::cin, Input);
    if (Input.empty()) {
        std::cout << "No input.\n";
        return 0;
    }

    // init lexer
    IndexTok = 0;
    getNextToken();

    // parse
    auto AST = parseExpression();
    if (!AST) {
        std::cerr << "Parse error\n";
        return 1;
    }

    // Create a fresh LLVMContext for the module (we'll move it into the ThreadSafeModule later)
    auto Ctx = std::make_unique<llvm::LLVMContext>();
    // Create module that uses this context
    auto Mod = std::make_unique<llvm::Module>("expr_module", *Ctx);
    // IR builder bound to the context
    llvm::IRBuilder<> Builder(*Ctx);

    // Create function + IR
    createMainFunction(Mod, *Ctx, Builder, AST);

    // Print generated IR to stderr (useful)
    Mod->print(llvm::errs(), nullptr);

    // Run via JIT: pass ownership of module and context
    double result = runJIT(std::move(Mod), std::move(Ctx));
    std::cout << "\nResult = " << result << "\n";
    return 0;
}
