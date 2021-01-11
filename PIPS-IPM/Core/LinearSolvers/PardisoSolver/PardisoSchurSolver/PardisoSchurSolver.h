/* PIPS-IPM
 * Authors: Cosmin G. Petra, Miles Lubin
 * (C) 2012 Argonne National Laboratory, see documentation for copyright
 */
#ifndef PARDISO_SCHUR_SOLVER
#define PARDISO_SCHUR_SOLVER

#include "DoubleLinearSolver.h"
#include "SparseSymMatrix.h"
#include "SparseGenMatrix.h"
#include "DenseSymMatrix.h"
#include "OoqpVectorHandle.h"
#include "SparseStorage.h"
#include "pipsport.h"
#include <map>

/** implements the linear solver class using the Pardiso SC solver
 */

class PardisoSchurSolver : public DoubleLinearSolver {

 constexpr static int symbFactorIntervalDefault = 8;
 constexpr static int pivotPerturbationExpDefault = 8;
 constexpr static int nIterativeRefinsDefault = 8;
 constexpr static bool parallelForwardBackwardDefault = true;
 constexpr static bool factorizationTwoLevelDefault = true;


 public:
  virtual void firstCall(); //first factorization call
  void firstSolveCall(SparseGenMatrix& R,
		      SparseGenMatrix& A,
		      SparseGenMatrix& C,
		      SparseGenMatrix& F,
		      SparseGenMatrix& G,
		      int nSC0); //first solve call

  /** sets mStorage to refer to the argument sgm */
  PardisoSchurSolver( SparseSymMatrix * sgm, OoqpVector* regularization );

  virtual void diagonalChanged( int idiag, int extent );
  virtual void matrixChanged();

  using DoubleLinearSolver::solve;
  virtual void solve( OoqpVector& rhs ) = 0;
  void solve( GenMatrix& ) override { assert(false && "Function not supported. Use PardisoSolver for this functionality."); };

  /** Functions specific to the Schur approach. The last argument is the Schur first
   * stage matrix that will be updated.
   * 1. schur_solver( rhs, SC)
   *  - this is the generic function
   *
   * 2. schur_solve(R,A,B, SC)
   *  - avoids forming the matrix rhs [R' A' B']'
   *  - assumes rhs does not change
   */
  virtual void schur_solve(/*const*/ SparseGenMatrix& R,
			   /*const*/ SparseGenMatrix& A,
			   /*const*/ SparseGenMatrix& C,
			   /*const*/ SparseGenMatrix& F,
			   /*const*/ SparseGenMatrix& G,
			   DenseSymMatrix& SC);

  virtual void schur_solve_sparse(/*const*/ SparseGenMatrix& R,
            /*const*/ SparseGenMatrix& A,
            /*const*/ SparseGenMatrix& C,
            /*const*/ SparseGenMatrix& F,
            /*const*/ SparseGenMatrix& G,
            SparseSymMatrix& SC);

 protected:

  SparseSymMatrix* Msys{}; // this is the (1,1) block in the augmented system
  bool first{true};
  bool firstSolve{true};
  void  *pt[64];
  int iparm[64];
  bool useSparseRhs{true};
  int symbFactorInterval;

  int* shrinked2orgSC{};

  int pivotPerturbationExp; // 10^-exp
  int nIterativeRefins;
  bool parallelForwardBackward;
  bool factorizationTwoLevel;

  /* pardiso params */
  int maxfct{1};
  int mnum{1};
  int phase{0};
  int msglvl{0};
  int mtype{-2};
  int nrhs{1};

  /** dimension of the PARDISO augmented system */
  int n{-1};
  /** dimension of the Schur complement (# of rhs) */
  int nSC{-1};
  /** number of nonzeros in the PARDISO augmented matrix */
  int nnz{-1};
  /** storage for the upper triangular (in row-major format) */
  int *rowptrAug{};
  int *colidxAug{};
  double *eltsAug{};
  /** mapping from from the diagonals of the PIPS linear systems to
      the diagonal elements of the (1,1) block  in the augmented system */
  map<int,int> diagMap;

  //temporary vector of size n
  double* nvec{};
  double* nvec2{};
  int nvec_size{-1}; // to be save

  virtual void initPardiso() = 0;
  virtual void setIparm(int* iparm) const = 0;
  bool iparmUnchanged();

  virtual void computeSC(int nSCO,
      /*const*/SparseGenMatrix &R,
      /*const*/SparseGenMatrix &A,
      /*const*/SparseGenMatrix &C,
      /*const*/SparseGenMatrix &F,
      /*const*/SparseGenMatrix &G, int *&rowptrSC, int *&colidxSC,
            double *&eltsSC) = 0;

  ~PardisoSchurSolver() override;
};

#endif
