/* OOQP                                                               *
 * Authors: E. Michael Gertz, Stephen J. Wright                       *
 * (C) 2001 University of Chicago. See Copyright Notification in OOQP */

#ifndef OOQPSTARTSTRAGEGY
#define OOQPSTARTSTRAGEGY

class ProblemFormulation;
class Solver;
class Variables;
class Problem;
class Residuals;

class OoqpStartStrategy {
public:
  virtual void doIt( Solver * solver,
		     ProblemFormulation * formulation,
		     Variables * iterate, Problem * prob,
		     Residuals * resid, Variables * step ) = 0;

  virtual ~OoqpStartStrategy() {}
};

#endif
