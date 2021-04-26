/* PIPS
   Authors: Cosmin Petra
   See license and copyright information in the documentation */

#ifndef SAUGLINSYS
#define SAUGLINSYS

#ifdef WITH_PARDISO
#include "PardisoProjectIndefSolver.h"
#endif

#ifdef WITH_MKL_PARDISO
#include "PardisoMKLIndefSolver.h"
#endif

#ifdef WITH_MA57
#include "Ma57SolverRoot.h"
#endif

#ifdef WITH_MA27
#include "Ma27SolverRoot.h"
#endif

#ifdef WITH_MUMPS
#include "MumpsSolverRoot.h"
#endif

#include "sLinsysRoot.h"
#include "pipsport.h"
#include <memory>

class DistributedQP;
/** 
 * ROOT (= NON-leaf) linear system in reduced augmented form
 */
class sLinsysRootAug : public sLinsysRoot {

 public:
  sLinsysRootAug(DistributedFactory * factory_, DistributedQP * prob_);
  sLinsysRootAug(DistributedFactory* factory,
		 DistributedQP* prob_,
		 OoqpVector* dd_, OoqpVector* dq_,
		 OoqpVector* nomegaInv_,
		 OoqpVector* rhs_, bool creat_solvers);
  ~sLinsysRootAug() override = default;

  void finalizeKKT( DistributedQP* prob, Variables* vars) override;
  void finalizeKKTdist(DistributedQP* prob) override;

  void Lsolve(DistributedQP *prob, OoqpVector& x) override;
  void Dsolve(DistributedQP *prob, OoqpVector& x) override;
  void Ltsolve(DistributedQP *prob, OoqpVector& x) override;

  using sLinsys::LsolveHierarchyBorder;
  void LsolveHierarchyBorder( DenseGenMatrix& result, BorderLinsys& Br, std::vector<BorderMod>& Br_mod_border, bool two_link_border, int begin_cols, int end_cols ) override;

  using sLinsys::LtsolveHierarchyBorder;
  void LtsolveHierarchyBorder( DoubleMatrix& res, const DenseGenMatrix& X0, BorderLinsys& Bl, BorderLinsys& Br,
        std::vector<BorderMod>& br_mod_border, bool sym_res, bool sparse_res, int begin_cols, int end_cols ) override;

 protected:
  SymMatrix* createKKT (DistributedQP* prob) const;
  void createSolversSparse( SolverType solver );
  void createSolversDense();

  void assembleLocalKKT( DistributedQP* prob ) override;
//  void solveReduced( DistributedQP *prob, SimpleVector& b);
  void solveReducedLinkCons( DistributedQP *prob, SimpleVector& b);
  void solveReducedLinkConsBlocked( DistributedQP* data, DenseGenMatrix& rhs_mat_transp, int rhs_start, int n_rhs );
  void addBlTKiInvBrToRes( DoubleMatrix& result, BorderLinsys& Bl, BorderLinsys& Br, std::vector<BorderMod>& Br_mod_border,
        bool sym_res, bool sparse_res ) override;
  void addBlTKiInvBrToResBlockwise( DoubleMatrix& result, BorderLinsys& Bl, BorderLinsys& Br, std::vector<BorderMod>& Br_mod_border,
        bool sym_res, bool sparse_res, DenseGenMatrix& buffer_b0, int begin_cols, int end_cols );

 private:
  void createSolversAndKKts(DistributedQP* prob);
  void finalizeKKTdense( DistributedQP* prob, Variables* vars);
  void finalizeKKTsparse( DistributedQP* prob, Variables* vars);
  void solveWithIterRef( DistributedQP *prob, SimpleVector& b);
  void solveWithBiCGStab( DistributedQP *prob, SimpleVector& b);

  void DsolveHierarchyBorder( DenseGenMatrix& b, int n_cols ) override;

  // add specified columns of given matrix Ht (either Ft or Gt) to Schur complement
  void addLinkConsBlock0Matrix( DistributedQP *prob, SparseGenMatrix& Ht, int nHtOffsetCols, int nKktOffsetCols, int startCol, int endCol);

  /** y = beta*y - alpha* SC * x */
  void SCmult ( double beta, SimpleVector& y, double alpha, SimpleVector& x, DistributedQP* prob);

  SymMatrix* CtDC{};

  std::vector<double> reduced_rhss_blocked;
  std::unique_ptr<SimpleVector> redRhs;
};

#endif

