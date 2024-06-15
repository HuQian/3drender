#ifndef _AST_H_
#define _AST_H_
#include <string>
#include <vector>

class BaseAST {
 public:
  virtual ~BaseAST() = default;
  virtual std::string dump() const = 0;
};

class CompUnitAST : public BaseAST {
public:
  std::vector<std::unique_ptr<BaseAST>> line_list;

  std::string dump() const override {
    std::cout << "333dHello" << std::endl;

    return "hi";
  }
};

class LineVAST : public BaseAST {
public:
  std::string type;
  double v0;
  double v1;
  double v2;

  std::string dump() const override {
    std::cout << "333dHello" << std::endl;

    return "hi";
  }

};

#endif

