
#include "src/EvaLLVM.hpp"

int main(int argc, char const* argv[]) {
  std::string program = R"(
    // (var VERSION 42)
    // (begin
    //     (var VERSION "Hello")
    //     (printf "Version: %S\n" VERSION)
    // )

    (printf "Version: %d\n" VERSION)
  )";

  EvaLLVM vm;

  vm.exec(program);

  return 0;
}
