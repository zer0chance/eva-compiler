
#ifndef EvaLLVM_HPP
#define EvaLLVM_HPP

#include <string>
#include <memory>
#include <iostream>

#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"

#include "parser/EvaParser.hpp"

using syntax::EvaParser;

class EvaLLVM {

  std::unique_ptr<llvm::LLVMContext> ctx;
  
  std::unique_ptr<llvm::Module> module;
  
  std::unique_ptr<llvm::IRBuilder<>> builder;

  std::unique_ptr<EvaParser> parser;

  /**
   * Currently compiling function
   */
  llvm::Function* fn;

  void moduleInit();

  void saveModuleToFile(const std::string& fileName);

  void compile(const Exp& ast);

  llvm::Value* gen(const Exp& exp);

  void setupExternalFunctions();

  llvm::Function* createFunction(const std::string& fnName, llvm::FunctionType* fnType);

  /**
   * Creates funtion prototype (declaration)
   */
  llvm::Function* createFunctionProto(const std::string& fnName, llvm::FunctionType* fnType);

  void createFunctionBlock(llvm::Function* fn);

  llvm::BasicBlock* createBB(std::string name, llvm::Function* fn = nullptr);

  llvm::GlobalVariable* createGlobalVar(const std::string& name, llvm::Constant* init);

public:
  EvaLLVM() : parser(std::make_unique<EvaParser>()){
    moduleInit();
    setupExternalFunctions();
  }

  void exec(const std::string& program) {
    auto ast = parser->parse(program);

    compile(ast);

    module->print(llvm::outs(), nullptr);
    std::cout << '\n';

    saveModuleToFile("./out.ll");
  }
};

#endif

