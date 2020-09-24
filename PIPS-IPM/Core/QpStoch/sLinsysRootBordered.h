/*
 * sLinsysRootBordered.h
 *
 *  Created on: Sep 22, 2020
 *      Author: bzfkempk
 */
#ifndef PIPS_IPM_CORE_QPSTOCH_SLINSYSROOTBORDERED_H_
#define PIPS_IPM_CORE_QPSTOCH_SLINSYSROOTBORDERED_H_

#include "sLinsysRoot.h"

class sLinsysRootBordered : public sLinsysRoot
{
   public:
      sLinsysRootBordered(sFactory * factory_, sData * prob_);

      virtual ~sLinsysRootBordered();

      void finalizeKKT(sData* prob, Variables* vars) override;
      void solveReduced( sData *prob, SimpleVector& b) override;

   protected:
      SymMatrix* createKKT(sData* prob) override;
      DoubleLinearSolver* createSolver(sData* prob, SymMatrix* kktmat) override;

   private:

};

#endif /* PIPS_IPM_CORE_QPSTOCH_SLINSYSROOTBORDERED_H_ */
