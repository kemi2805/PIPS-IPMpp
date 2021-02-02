/* PIPS
   Authors: Cosmin Petra and Miles Lubin
   See license and copyright information in the documentation */

#ifndef SROOTLINSYS
#define SROOTLINSYS

#include "sLinsys.h"
#include "StochGenMatrix.h"
#include "SCsparsifier.h"

class SCsparsifier;
class sFactory;
class sData;

// DEBUG only
//#include "ScaDenSymMatrix.h"

/** 
 * ROOT (= NON-leaf) linear system
 */
class sLinsysRoot : public sLinsys {
 struct MatrixEntryTriplet
 {
    double val;
    int row;
    int col;
 };

 protected:
  void createChildren(sData* prob);
  void deleteChildren() override;

 private:
  void init();
 public:
  std::vector<sLinsys*> children;

  sLinsysRoot(sFactory * factory_, sData * prob_, bool is_hierarchy_root = false);
  sLinsysRoot(sFactory* factory, sData* prob_, OoqpVector* dd_, OoqpVector* dq_,
        OoqpVector* nomegaInv_, OoqpVector* rhs_);

  void factor2(sData *prob, Variables *vars) override;
  /* Atoms methods of FACTOR2 for a non-leaf linear system */
  virtual void initializeKKT(sData* prob, Variables* vars);
  virtual void assembleLocalKKT( sData* prob ) = 0;
  void addTermToSchurCompl(sData* prob, size_t childindex, bool use_local_RAC_mat);
  virtual void reduceKKT(sData *prob);
  virtual void factorizeKKT(); 
  virtual void factorizeKKT( sData* prob );
  virtual void finalizeKKT( sData* prob, Variables* vars ) = 0;
  virtual void finalizeKKTdist( sData* /*prob*/ ) {assert("not implemented here \n" && 0);};

  virtual void Ltsolve2( sData *prob, StochVector& x, SimpleVector& xp);

  /* compute B0_{outer} - buffer */
  virtual void finalizeZ0Hierarchical( DenseGenMatrix& buffer, BorderLinsys& Br );
  /* compute SC += B0_{outer}^T X0 */
  virtual void finalizeInnerSchurComplementContribution( DoubleMatrix& SC, SparseGenMatrix& A0_border, SparseGenMatrix& C0_border, SparseGenMatrix& F0vec_border,
        SparseGenMatrix& F0cons_border, SparseGenMatrix& G0vec_border, SparseGenMatrix& G0cons_border, DenseGenMatrix& X0, bool is_sym, bool is_sparse );

  /* compute -SUM_i Bi_{inner} Ki^-1 Bi_{outer} */
  void LsolveHierarchyBorder( DenseGenMatrix& result, BorderLinsys& Br, bool use_local_RAC_mat ) override;

  /* compute SUM_i Bli^T X_i = Bli^T Ki^-1 (Bri - Bi_{inner} X0) */
  void LtsolveHierarchyBorder( DoubleMatrix& SC, const DenseGenMatrix& X0, BorderLinsys& Bl, BorderLinsys& Br, bool sym_res, bool sparse_res ) override;

  void addBorderTimesRhsToB0( StochVector& rhs, SimpleVector& b0, BorderLinsys& border ) override;

  void addBorderX0ToRhs( StochVector& rhs, const SimpleVector& x0, BorderLinsys& border ) override;

  void putXDiagonal( OoqpVector& xdiag ) override;
  void putZDiagonal( OoqpVector& zdiag ) override;
 
  virtual void AddChild(sLinsys* child);

  virtual bool usingSparseKkt() { return hasSparseKkt; };

  ~sLinsysRoot() override;

  //utilities
  void myAtPutZeros(DenseSymMatrix* mat);
  void myAtPutZeros(DenseSymMatrix* mat, 
		    int row, int col, 
		    int rowExtent, int colExtent);

  // all_reduces specified submatrix (in chunks)
  void submatrixAllReduce(DenseSymMatrix* A, 
			  int startRow, int startCol, int nRows, int nCols,
			  MPI_Comm comm);

  void submatrixAllReduceFull(DenseSymMatrix* A, int startRow, int startCol, int nRows, int nCols, MPI_Comm comm);
  void submatrixAllReduceFull(DenseGenMatrix* A, int startRow, int startCol, int nRows, int nCols, MPI_Comm comm);

  // all_reduces specified submatrix as a while
  void submatrixAllReduceFull(double** A, int startRow, int startCol, int nRows, int nCols, MPI_Comm comm);

  // all_reducees lower half (including diagonal) of specified submatrix
  void submatrixAllReduceDiagLower(DenseSymMatrix* A,
            int substart, int subsize,
            MPI_Comm comm);

  SCsparsifier precondSC;

 protected: //buffers

  SparseSymMatrix* kktDist{};

  OoqpVector* zDiag{};
  OoqpVector* zDiagLinkCons{};
  OoqpVector* xDiag{};

  double* sparseKktBuffer{};

  int childrenProperStart; // first non-dummy child
  int childrenProperEnd;   // end of non-dummy children range (not included)
  bool hasSparseKkt;
  bool usePrecondDist;
  bool allreduce_kkt;

 private:
  void initProperChildrenRange();
  void registerMatrixEntryTripletMPI();
  void reduceKKTdist(sData* prob);
  void reduceKKTdense();
  void reduceKKTsparse();
  void reduceToProc0(int size, double* values);
  void reduceToAllProcs(int size, double* values);
  void syncKKTdistLocalEntries(sData* prob);
  void sendKKTdistLocalEntries(const std::vector<MatrixEntryTriplet>& prevEntries) const;
  std::vector<MatrixEntryTriplet> receiveKKTdistLocalEntries() const;
  std::vector<MatrixEntryTriplet> packKKTdistOutOfRangeEntries(sData* prob, int childStart, int childEnd) const;

  MPI_Datatype MatrixEntryTriplet_mpi;

#ifdef STOCH_TESTING
 protected: 
  static void dumpRhs(int proc, const char* nameToken,  SimpleVector& rhs);
  static void dumpMatrix(int scen, int proc, const char* nameToken, DenseSymMatrix& M);
#endif
#ifdef TIMING
 protected:
  void afterFactor();
#endif
};

#endif

