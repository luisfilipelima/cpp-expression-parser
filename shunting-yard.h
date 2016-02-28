// Source: http://www.daniweb.com/software-development/cpp/code/427500/calculator-using-shunting-yard-algorithm#
// Author: Jesse Brown
// Modifications: Brandon Amos

#ifndef _SHUNTING_YARD_H
#define _SHUNTING_YARD_H

#include <map>
#include <stack>
#include <string>
#include <queue>

using namespace std;

struct TokenBase
{
  virtual ~TokenBase() {}
};

template<class T> class Token : public TokenBase
{
public:
  Token ( T t ) : val( t ) {}
  T val;
};

typedef std::queue<TokenBase*> TokenQueue_t;

class calculator
{
private:
  static map<string, int> opPrecedence;
  static map<string, int> buildOpPrecedence();

public:
  static double calculate( const char* expr,
      map<string, double>* vars = 0 );

private:
  static double calculate( TokenQueue_t RPN,
      map<string, double>* vars = 0 );
  static void cleanRPN( TokenQueue_t& rpn );
  static TokenQueue_t toRPN( const char* expr,
      map<string, double>* vars,
      map<string, int> opPrecedence=opPrecedence );

private:
  TokenQueue_t RPN;
public:
  ~calculator();
  calculator() {}
  calculator( const char* expr, map<string, double>* vars = 0,
      map<string, int> opPrecedence=opPrecedence );
  void compile( const char* expr, map<string, double>* vars = 0,
      map<string, int> opPrecedence=opPrecedence);
  double eval( map<string, double>* vars = 0);
  string str();
};

#endif // _SHUNTING_YARD_H
