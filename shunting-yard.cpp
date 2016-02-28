// Source:
// http://www.daniweb.com/software-development/cpp/code/427500/calculator-using-shunting-yard-algorithm#
// Author: Jesse Brown
// Modifications: Brandon Amos, redpois0n

#include <cstdlib>
#include <iostream>
#include <math.h>
#include <sstream>
#include <stdexcept>

#include "shunting-yard.h"

using namespace std;

map<string, int> calculator::buildOpPrecedence()
{
  map<string, int> opPrecedence;

  // Create the operator precedence map.
  opPrecedence["("] = -1;
  opPrecedence["<<"] = 1;
  opPrecedence[">>"] = 1;
  opPrecedence["+"] = 2;
  opPrecedence["-"] = 2;
  opPrecedence["*"] = 3;
  opPrecedence["/"] = 3;
  opPrecedence["%"] = 3;
  opPrecedence["^"] = 4;

  return opPrecedence;
}

// Builds the opPrecedence map only once:
map<string, int> calculator::opPrecedence = calculator::buildOpPrecedence();

#define isvariablechar(c) (isalpha(c) || c == '_')

TokenQueue_t calculator::toRPN( const char *expr, map<string, double> *vars,
                               map<string, int> opPrecedence )
{
  TokenQueue_t rpnQueue;
  stack<string> operatorStack;
  bool lastTokenWasOp = true;

  // In one pass, ignore whitespace and parse the expression into RPN
  // using Dijkstra's Shunting-yard algorithm.
  while ( *expr && isspace( *expr ) )
    ++expr;
  while ( *expr )
  {
    if ( isdigit( *expr ) )
    {
      // If the token is a number, add it to the output queue.
      char *nextChar = 0;
      double digit = strtod( expr, &nextChar );
#ifdef DEBUG
      cout << digit << endl;
#endif
      rpnQueue.push( new Token<double>(digit) );
      expr = nextChar;
      lastTokenWasOp = false;
    }
    else if ( isvariablechar( *expr ) )
    {
      // If the function is a variable, resolve it and
      // add the parsed number to the output queue.
      stringstream ss;
      ss << *expr;
      ++expr;
      while ( isvariablechar( *expr ) || isdigit( *expr ) )
      {
        ss << *expr;
        ++expr;
      }

      double *val = NULL;
      string key = ss.str();

      if ( vars )
      {
        map<string, double>::iterator it = vars->find( key );
        if ( it != vars->end() )
          val = &( it->second );
      }

      if ( val )
      {
// Save the number
#ifdef DEBUG
        cout << val << endl;
#endif
        rpnQueue.push( new Token<double>( *val ) );
        ;
      }
      else
      {
// Save the variable name:
#ifdef DEBUG
        cout << key << endl;
#endif
        rpnQueue.push( new Token<string>( key ) );
      }

      lastTokenWasOp = false;
    }
    else
    {
      // Otherwise, the variable is an operator or paranthesis.
      switch ( *expr )
      {
          case '(':
            operatorStack.push( "(" );
            ++expr;
            break;
          case ')':
            while ( operatorStack.top().compare( "(" ) )
            {
              rpnQueue.push( new Token<string>(operatorStack.top() ) );
              operatorStack.pop();
            }
            operatorStack.pop();
            ++expr;
            break;
          default:
          {
            // The token is an operator.
            //
            // Let p(o) denote the precedence of an operator o.
            //
            // If the token is an operator, o1, then
            //   While there is an operator token, o2, at the top
            //       and p(o1) <= p(o2), then
            //     pop o2 off the stack onto the output queue.
            //   Push o1 on the stack.
            stringstream ss;
            ss << *expr;
            ++expr;
            while ( *expr && !isspace( *expr ) && !isdigit( *expr ) &&
                   !isvariablechar( *expr ) && *expr != '(' && *expr != ')' )
            {
              ss << *expr;
              ++expr;
            }
            ss.clear();
            string str;
            ss >> str;
#ifdef DEBUG
            cout << str << endl;
#endif

            if ( lastTokenWasOp )
            {
              // Convert unary operators to binary in the RPN.
              if ( !str.compare("-") || !str.compare("+") )
              {
                rpnQueue.push(new Token<double>(0));
              } else {
                throw domain_error( "Unrecognized unary operator: '" + str + "'." );
              }
            }

            while ( !operatorStack.empty() &&
                opPrecedence[str] <= opPrecedence[operatorStack.top()] )
            {
              rpnQueue.push( new Token<string>(operatorStack.top() ) );
              operatorStack.pop();
            }
            operatorStack.push( str );
            lastTokenWasOp = true;
          }
      }
    }
    while ( *expr && isspace( *expr ) )
      ++expr;
  }
  while ( !operatorStack.empty() )
  {
    rpnQueue.push( new Token<string>( operatorStack.top() ) );
    operatorStack.pop();
  }
  return rpnQueue;
}

double calculator::calculate( const char *expr, map<string, double> *vars )
{

  // Convert to RPN with Dijkstra's Shunting-yard algorithm.
  TokenQueue_t rpn = toRPN( expr, vars );

  double ret = calculate( rpn );

  cleanRPN( rpn );

  return ret;
}

double calculator::calculate( TokenQueue_t rpn, map<string, double> *vars )
{

  // Evaluate the expression in RPN form.
  stack<double> evaluation;
  while ( !rpn.empty() )
  {
    TokenBase *base = rpn.front();
    rpn.pop();

    Token<string> *strTok = dynamic_cast<Token<string> *>( base );
    Token<double> *doubleTok = dynamic_cast<Token<double> *>( base );

    // Operator:
    if ( strTok && !isvariablechar( strTok->val[0] ) )
    {
      string str = strTok->val;
      if ( evaluation.size() < 2 )
      {
        throw domain_error( "Invalid equation." );
      }
      double right = evaluation.top();
      evaluation.pop();
      double left = evaluation.top();
      evaluation.pop();
      if (!str.compare( "+" ) )
      {
        evaluation.push( left + right );
      }
      else if ( !str.compare( "*" ) )
      {
        evaluation.push( left * right );
      }
      else if ( !str.compare( "-" ) )
      {
        evaluation.push(left - right);
      }
      else if ( !str.compare( "/" ) )
      {
        evaluation.push( left / right );
      }
      else if (!str.compare( "<<" ) )
      {
        evaluation.push( ( int ) left << ( int ) right );
      }
      else if ( !str.compare( "^" ) )
      {
        evaluation.push( pow( left, right ) );
      }
      else if ( !str.compare( ">>" ) )
      {
        evaluation.push( ( int ) left >> ( int ) right );
      }
      else if ( !str.compare( "%" ) )
      {
        evaluation.push( ( int ) left % ( int ) right );
      }
      else
      {
        throw domain_error( "Unknown operator: '" + str + "'." );
      }
    }
    else if ( doubleTok )
    { // Number
      evaluation.push( doubleTok->val );
    }
    else if ( strTok )
    { // Variable
      if ( !vars )
      {
        throw domain_error( "Detected variable, but the variable map is null." );
      }

      string key = strTok->val;
      map<string, double>::iterator it = vars->find( key );

      if ( it == vars->end() )
      {
        throw domain_error( "Unable to find the variable '" + key + "'." );
      }
      evaluation.push( it->second );
    }
    else
    {
      throw domain_error( "Invalid token." );
    }
  }
  return evaluation.top();
}

void calculator::cleanRPN( TokenQueue_t &rpn )
{
  while ( rpn.size() ) {
    delete rpn.front();
    rpn.pop();
  }
}

/* * * * * Non Static Functions * * * * */

calculator::~calculator() { cleanRPN( this->RPN ); }

calculator::calculator( const char *expr, map<string, double> *vars,
                       map<string, int> opPrecedence )
{
  compile( expr, vars, opPrecedence );
}

void calculator::compile( const char *expr, map<string, double> *vars,
                         map<string, int> opPrecedence )
{

  // Make sure it is empty:
  cleanRPN( this->RPN );

  this->RPN = calculator::toRPN( expr, vars, opPrecedence );
}

double calculator::eval( map<string, double> *vars )
{
  return calculate( this->RPN, vars );
}

/* * * * * For Debug Only * * * * */

string calculator::str()
{
  stringstream ss;
  TokenQueue_t rpn = this->RPN;

  ss << "calculator { RPN: [ ";
  while ( rpn.size() )
  {
    TokenBase *base = rpn.front();

    Token<double> *doubleTok = dynamic_cast<Token<double> *>( base );
    if ( doubleTok )
      ss << doubleTok->val;

    Token<string> *strTok = dynamic_cast<Token<string> *>( base );
    if ( strTok )
      ss << "'" << strTok->val << "'";

    rpn.pop();

    ss << ( rpn.size() ? ", " : "" );
  }
  ss << " ] }";
  return ss.str();
}
