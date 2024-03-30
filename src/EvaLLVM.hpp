
#ifndef EvaLLVM_HPP
#define EvaLLVM_HPP

#include <string>
#include <memory>

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

class EvaLLVM {
  std::unique_ptr<llvm::LLVMContext> ctx;
  
  std::unique_ptr<llvm::Module> module;
  
  std::unique_ptr<llvm::IRBuilder<>> builder;

  void moduleInit() {
    ctx = std::make_unique<llvm::LLVMContext>();
    module = std::make_unique<llvm::Module>("EvaLLVM", *ctx);
    builder = std::make_unique<llvm::IRBuilder<>>(*ctx);
  }

  void saveModuleToFile(const std::string& fileName);

public:
  EvaLLVM() { moduleInit(); }

  void exec(const std::string& program) {
    module->print(llvm::outs(), nullptr);

    saveModuleToFile("./out.ll");
  }
};

#endif

