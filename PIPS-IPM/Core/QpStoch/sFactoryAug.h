/* PIPS
   Authors: Cosmin Petra
   See license and copyright information in the documentation */

#ifndef STOCHACTORYAUG
#define STOCHACTORYAUG

#include "sFactory.h"

class sFactoryAug : public sFactory {
public:
   sFactoryAug(StochInputTree* tree, MPI_Comm comm = MPI_COMM_WORLD);

   ~sFactoryAug() override = default;

   DoubleLinearSolver* newRootSolver() override;

   sLinsysRoot* newLinsysRoot() override;

   sLinsysRoot* newLinsysRootHierarchical() override;

      sLinsysRoot* newLinsysRoot(DistributedQP *prob, OoqpVector *dd, OoqpVector *dq,
        OoqpVector *nomegaInv, OoqpVector* regP, OoqpVector* regDy, OoqpVector* regDz, OoqpVector *rhs ) override;

   Problem* switchToHierarchicalData(Problem* prob_in) override;

   void switchToOriginalTree() override;

};

#endif
