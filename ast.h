#pragma once
#include <memory>
#include <llvm/IR/Value.h>

class Expr {
public:
    virtual ~Expr() = default;
    // generate IR using provided context/builder/module
    virtual llvm::Value* codegen(llvm::LLVMContext &Ctx, llvm::IRBuilder<> &Builder, llvm::Module *M) = 0;
};

class NumberExpr : public Expr {
public:
    double Val;
    NumberExpr(double v) : Val(v) {}
    llvm::Value* codegen(llvm::LLVMContext &Ctx, llvm::IRBuilder<> &Builder, llvm::Module *M) override;
};

class BinaryExpr : public Expr {
public:
    char Op;
    std::unique_ptr<Expr> LHS, RHS;
    BinaryExpr(char op, std::unique_ptr<Expr> lhs, std::unique_ptr<Expr> rhs)
      : Op(op), LHS(std::move(lhs)), RHS(std::move(rhs)) {}
    llvm::Value* codegen(llvm::LLVMContext &Ctx, llvm::IRBuilder<> &Builder, llvm::Module *M) override;
};
