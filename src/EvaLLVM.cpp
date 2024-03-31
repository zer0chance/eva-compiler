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

void EvaLLVM::compile() {
  fn = createFunction("main", llvm::FunctionType::get(builder->getInt32Ty(), false));

  auto result = gen();

  auto i32Result = builder->CreateIntCast(result, builder->getInt32Ty(), true);

  builder->CreateRet(i32Result);
}

llvm::Value* EvaLLVM::gen() {
  return builder->getInt32(42);
}

void EvaLLVM::createFunctionBlock(llvm::Function* fn) {
  auto entry = createBB("entry", fn);
  builder->SetInsertPoint(entry);
}

llvm::BasicBlock* EvaLLVM::createBB(std::string name, llvm::Function* fn) {
  return llvm::BasicBlock::Create(*ctx, name, fn);
}

