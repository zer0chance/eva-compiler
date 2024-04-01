#include <regex>

#include "EvaLLVM.hpp"

void EvaLLVM::moduleInit() {
  ctx = std::make_unique<llvm::LLVMContext>();
  module = std::make_unique<llvm::Module>("EvaLLVM", *ctx);
  builder = std::make_unique<llvm::IRBuilder<>>(*ctx);
}

void EvaLLVM::saveModuleToFile(const std::string& fileName) {
  std::error_code errorCode;
  llvm::raw_fd_ostream outLL(fileName, errorCode);
  module->print(outLL, nullptr);
}

llvm::Function* EvaLLVM::createFunctionProto(const std::string& fnName, llvm::FunctionType* fnType) {
  auto fn = llvm::Function::Create(fnType, llvm::Function::ExternalLinkage, fnName, *module);
  verifyFunction(*fn);

  return fn;
}

llvm::Function* EvaLLVM::createFunction(const std::string& fnName, llvm::FunctionType* fnType) {
  // Function prototype might already be defined
  auto fn = module->getFunction(fnName); 

  // If not, allocate the function:
  if (fn == nullptr) {
    fn = createFunctionProto(fnName, fnType);
  }

  createFunctionBlock(fn);

  return fn;
}

void EvaLLVM::compile(const Exp& ast) {
  // Generate entry point
  fn = createFunction("main", llvm::FunctionType::get(builder->getInt32Ty(), false));
  createGlobalVar("VERSION", builder->getInt32(42));

  auto result = gen(ast);

  builder->CreateRet(builder->getInt32(0));
}

llvm::Value* EvaLLVM::gen(const Exp& exp) {
  switch (exp.type) {
    case ExpType::NUMBER:
      return builder->getInt32(exp.number);

    case ExpType::STRING: {
      auto re = std::regex("\\\\n");
      auto str = std::regex_replace(exp.string, re, "\n");

      return builder->CreateGlobalStringPtr(str);
    }

    case ExpType::SYMBOL: {
      if (exp.string == "true" || exp.string == "false") {
        // Booleans
        return builder->getInt1(exp.string == "true" ? true : false);
      } else {
        // Variables
        // Locals: 
        // TODO

        // Globals:
        return module->getNamedGlobal(exp.string)->getInitializer();
      }

    }
      return builder->getInt32(0);

    case ExpType::LIST:
      auto tag = exp.list[0];

      // -------- Special cases
      if (tag.type == ExpType::SYMBOL) {
        auto op = tag.string;

        if (op == "var") {
          auto varName = exp.list[1].string;

          auto init = gen(exp.list[2]);

          return createGlobalVar(varName, static_cast<llvm::Constant*>(init))->getInitializer();
        }

        // printf external function
        else if (op == "printf") {
          auto printfFn = module->getFunction("printf");

          std::vector<llvm::Value*> args{};

          for (auto i = 1; i < exp.list.size(); i++) {
            args.push_back(gen(exp.list[i]));
          }

          return builder->CreateCall(printfFn, args);
        }
      }    
  }

  // Unreachable
  return builder->getInt32(0);
}

llvm::GlobalVariable* EvaLLVM::createGlobalVar(const std::string& name, llvm::Constant* init) {
  module->getOrInsertGlobal(name, init->getType());
  auto variable = module->getNamedGlobal(name);
  variable->setAlignment(llvm::MaybeAlign(4));
  variable->setConstant(false);
  variable->setInitializer(init);
  return variable;
}

void EvaLLVM::setupExternalFunctions() {
  auto bytePtrTy = builder->getInt8Ty()->getPointerTo();

  module->getOrInsertFunction("printf",
      llvm::FunctionType::get(
        /* return type */ builder->getInt32Ty(),
        /* format arg  */ bytePtrTy,
        /* vararg      */ true
        ));
}

void EvaLLVM::createFunctionBlock(llvm::Function* fn) {
  auto entry = createBB("entry", fn);
  builder->SetInsertPoint(entry);
}

llvm::BasicBlock* EvaLLVM::createBB(std::string name, llvm::Function* fn) {
  return llvm::BasicBlock::Create(*ctx, name, fn);
}

