
#ifndef EvaLLVM_HPP
#define EvaLLVM_HPP

#include <string>
#include <memory>
#include <iostream>

#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"

class EvaLLVM {
  std::unique_ptr<llvm::LLVMContext> ctx;
  
  std::unique_ptr<llvm::Module> module;
  
  std::unique_ptr<llvm::IRBuilder<>> builder;

  /**
   * Currently compiling function
   */
  llvm::Function* fn;

  void moduleInit();

  void saveModuleToFile(const std::string& fileName);

  void compile();

  llvm::Value* gen();

  llvm::Function* createFunction(const std::string& fnName, llvm::FunctionType* fnType);

  /**
   * Creates funtion prototype (declaration)
   */
  llvm::Function* createFunctionProto(const std::string& fnName, llvm::FunctionType* fnType);

  void createFunctionBlock(llvm::Function* fn);

  llvm::BasicBlock* createBB(std::string name, llvm::Function* fn = nullptr);

public:
  EvaLLVM() { moduleInit(); }

  void exec(const std::string& program) {
    compile();

    module->print(llvm::outs(), nullptr);
    std::cout << '\n';

    saveModuleToFile("./out.ll");
  }
};

#endif

