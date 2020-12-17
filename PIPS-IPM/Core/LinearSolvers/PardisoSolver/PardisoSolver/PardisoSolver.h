/* PIPS-IPM
 * Authors: Cosmin G. Petra, Miles Lubin, Murat Mut
 * (C) 2012 Argonne National Laboratory, see documentation for copyright
 */
#ifndef PARDISOLINSYS_H
#define PARDISOLINSYS_H

#include "DoubleLinearSolver.h"
#include "DenseSymMatrix.h"
#include "pipsport.h"

#include <map>

/** implements the linear solver class using the Pardiso solver
 */

class PardisoSolver : public DoubleLinearSolver {

public:
  virtual void firstCall() = 0;

  /** sets mStorage to refer to the argument sgm */
  PardisoSolver( SparseSymMatrix * sgm );
  PardisoSolver( DenseSymMatrix* m);

  virtual void diagonalChanged( int idiag, int extent );
  virtual void matrixChanged();

  void solve( OoqpVector& rhs ) override;
  void solve( GenMatrix& rhs) override;
  void solve( int nrhss, double* rhss, int* colSparsity ) override;
  void solve( GenMatrix& rhs, int *colSparsity);

protected:
  virtual void setIparm(int* iparm) const = 0;
  virtual void pardisoCall(void *pt, int* maxfct, int* mnum, int* mtype, int* phase, int* n, double* M, int* krowM, int* jcolM,
        int* perm, int* nrhs, int* iparm, int* msglvl, double* rhs, double* sol, int* error) = 0;

  void initSystem();
  bool iparmUnchanged() const;


  ~PardisoSolver();

  SparseSymMatrix* Msys;
  DenseSymMatrix* Mdsys;
  bool first;
  void  *pt[64];
  int iparm[64];
  int n;

  int maxfct, mnum, phase, msglvl, mtype;

  int nrhs;
  int error;

  /** storage for the upper triangular (in row-major format) */
  int     *krowM,    *jcolM;
  double  *M;

  /** number of nonzeros in the matrix */
  int      nnz;

  /** mapping from from the diagonals of the PIPS linear systems to
      the diagonal elements of the (1,1) block  in the augmented system */
  std::map<int,int> diagMap;

  /** temporary storage for the factorization process */
  double* nvec; //temporary vec
  double* sol; //solution
  int sz_sol; //allocated size
};

#endif