#include <iostream>
#include <limits>
#include <map>
#include <string>
#include <stdexcept>

#include "shunting-yard.h"

using namespace std;

void assert(double actual, double expected, const char* expr = 0) {
  double diff = actual - expected;
  if (diff < 0) diff *= -1;
  if (diff < 1e-15) {
    if(expr) {
      cout << "  '" << expr << "' indeed evaluated to " <<
        expected << "." << endl;
    } else {
      cout << "  actual value '" << actual <<
        "' indeed matches the expected value '" << expected << "'" << endl;
    }
  } else {
    if(expr) {
      cout << "  FAILURE '" << expr << "' evaluated to " <<
        actual << " and NOT " << expected << "!" << endl;
    } else {
      cout << "  FAILURE, actual value '" << actual <<
        "' does not match the expected value '" << expected <<
        "'" << endl;
    }
  }
}
void assert(const char* expr, double expected,
    map<string, double>* vars = 0) {
  double actual = calculator::calculate(expr, vars);
  assert(actual, expected, expr);
}

int main(int argc, char** argv) {
  map<string, double> vars;
  vars["pi"] = 3.14;
  vars["b1"] = 0;

  cout << "\nTests with static calculate::calculate()\n" << endl;

  assert("-pi+1", -2.14, &vars);
  assert("-pi+1 + b1", -2.14, &vars);

  assert("(20+10)*3/2-3", 42.0);
  assert("1 << 4", 16.0);
  assert("1+(-2*3)", -5);

  cout << "\nTests with calculate::compile() & calculate::eval()\n" << endl;

  calculator c1;
  c1.compile("-pi+1", &vars);
  assert(c1.eval(), -2.14);

  calculator c2("pi+4", &vars);
  assert(c2.eval(), 7.14);
  assert(c2.eval(), 7.14);

  calculator c3("pi+b1+b2", &vars);

  vars["b2"] = 1;
  assert(c3.eval(&vars), 4.14);

  vars["b2"] = .86;
  assert(c3.eval(&vars), 4);

  cout << "\nTesting exception management\n" << endl;

  try {
    c3.eval();
  } catch(domain_error err) {
    cout << "  THROWS as expected" << endl;
  }

  try {
    vars.erase("b2");
    c3.eval(&vars);
  } catch(domain_error err) {
    cout << "  THROWS as expected" << endl;
  }

  try {
    vars.erase("b1");
    vars["b2"] = 0;
    c3.eval(&vars);
    cout << "  Do not THROW as expected" << endl;
  } catch(domain_error err) {
    cout << "  If it THROWS it's a problem!" << endl;
  }

  cout << "\nEnd testing" << endl;

  return 0;
}
