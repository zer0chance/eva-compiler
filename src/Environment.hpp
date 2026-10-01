// Environment class (Symbol Table)

#ifndef Environment_HPP
#define Environment_HPP

#include <cstddef>
#include <map>
#include <memory>

#include "llvm/IR/Value.h"
#include "Logger.hpp"

class Environment : public std::enable_shared_from_this<Environment> {
  private:
    // Bindings storage.
    std::map<std::string, llvm::Value*> _record;

    // Parent link.
    std::shared_ptr<Environment> _parent;

    std::shared_ptr<Environment> resolve(std::string& name) {
        if (_record.count(name) != 0)
            return shared_from_this();

        if (_parent == nullptr)
            DIE << "Variable \"" << name << "\" is not defined.";

        return _parent->resolve(name);
    }

  public:
    Environment(std::map<std::string, llvm::Value*> record,
                std::shared_ptr<Environment> parent)
                : _record(record), _parent(parent) {}

    llvm::Value* define(const std::string& name, llvm::Value* value) {
        _record[name] = value;
        return value;
    }

    llvm::Value* lookup(std::string& name) {
        return resolve(name)->_record[name];
    }
};

#endif
