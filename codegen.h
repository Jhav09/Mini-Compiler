#pragma once
#include <memory>
#include <iostream>

#include "ast.h"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/Module.h>

#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/ExecutionEngine/Orc/ThreadSafeModule.h>

using namespace llvm;
using namespace llvm::orc;

// AST node implementations

inline llvm::Value* NumberExpr::codegen(llvm::LLVMContext &Ctx, llvm::IRBuilder<> &Builder, llvm::Module *M) {
    return ConstantFP::get(llvm::Type::getDoubleTy(Ctx), Val);
}

inline llvm::Value* BinaryExpr::codegen(llvm::LLVMContext &Ctx, llvm::IRBuilder<> &Builder, llvm::Module *M) {
    llvm::Value* L = LHS->codegen(Ctx, Builder, M);
    llvm::Value* R = RHS->codegen(Ctx, Builder, M);
    if (!L || !R) return nullptr;

    switch (Op) {
        case '+': return Builder.CreateFAdd(L, R, "addtmp");
        case '-': return Builder.CreateFSub(L, R, "subtmp");
        case '*': return Builder.CreateFMul(L, R, "multmp");
        case '/': return Builder.CreateFDiv(L, R, "divtmp");
        default:
            std::cerr << "Unknown binary operator: " << Op << "\n";
            return nullptr;
    }
}

// Create a main() function in the module which returns the expression value.
// Note: We expect the Builder insertion point to be set before calling codegen on the expression.
inline llvm::Function* createMainFunction(std::unique_ptr<llvm::Module> &M,
                                         llvm::LLVMContext &Ctx,
                                         llvm::IRBuilder<> &Builder,
                                         std::unique_ptr<Expr> &RootExpr) {
    llvm::FunctionType* FT = llvm::FunctionType::get(llvm::Type::getDoubleTy(Ctx), false);
    llvm::Function* F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, "main", M.get());

    llvm::BasicBlock* BB = llvm::BasicBlock::Create(Ctx, "entry", F);
    Builder.SetInsertPoint(BB);

    llvm::Value* RetVal = RootExpr->codegen(Ctx, Builder, M.get());
    Builder.CreateRet(RetVal);

    verifyFunction(*F);
    return F;
}

// JIT runner compatible with LLVM 21's ORC API
inline double runJIT(std::unique_ptr<llvm::Module> M, std::unique_ptr<llvm::LLVMContext> Ctx) {
    // Create LLJIT
    auto JITOrErr = LLJITBuilder().create();
    auto JIT = cantFail(std::move(JITOrErr));

    // ThreadSafeModule wants ownership of a module + context; use unique_ptr<LLVMContext>
    ThreadSafeModule TSM(std::move(M), std::move(Ctx));

    cantFail(JIT->addIRModule(std::move(TSM)));

    // Lookup the 'main' symbol
    auto Sym = cantFail(JIT->lookup("main"));

    // getAddress returns JITTargetAddress (uint64_t). Cast to function pointer.
    auto Addr = Sym.getAddress();
    using FuncTy = double (*)();
    auto *Fptr = reinterpret_cast<FuncTy>(static_cast<uintptr_t>(Addr));
    return Fptr();
}
