/*
 * BorderedGenMatrix.h
 *
 *  Created on: Sep 14, 2020
 *      Author: bzfkempk
 */

#ifndef BORDEREDMATRIX_H
#define BORDEREDMATRIX_H

#include "AbstractMatrix.h"

#include <vector>
#include "mpi.h"

class StripMatrix;

class DistributedMatrix;

class SparseMatrix;


/*
 * representing a matrix of type
 *
 * [ B  K ]
 * [ C  B']
 *
 * Where K is a DistributedMatrix
 */

// TODO : make more general ? K B B' and C can be any matrices in theory..
class BorderedMatrix : public GeneralMatrix {
public:
   DistributedMatrix* const inner_matrix{};
   StripMatrix* const border_left{};
   StripMatrix* const border_bottom{};

   // TODO: is SparseGenMatrix appropriate? What does this block look like -> it has parts of the diagonals in it for inequality linking constraints and nothing else?
   SparseMatrix* const bottom_left_block{};

protected:

   MPI_Comm mpi_comm{MPI_COMM_NULL};
   const bool distributed{false};
   const int rank{-1};

   long long m{0};
   long long n{0};

public:
   BorderedMatrix(DistributedMatrix* inner_matrix, StripMatrix* border_left, StripMatrix* border_bottom, SparseMatrix* bottom_left_block,
         MPI_Comm mpi_comm_);
   ~BorderedMatrix() override;

   [[nodiscard]] int is_a(int matrixType) const override;

   /** y = beta * y + alpha * this * x */
   void mult(double beta, Vector<double>& y, double alpha, const Vector<double>& x) const override;
   /** y = beta * y + alpha * this^T * x */
   void transMult(double beta, Vector<double>& y, double alpha, const Vector<double>& x) const override;

   [[nodiscard]] double inf_norm() const override;
   void columnScale(const Vector<double>& vec) override;
   void rowScale(const Vector<double>& vec) override;

   void scalarMult(double num) override;

   void getSize(long long& m_, long long& n_) const override;
   void getSize(int& m_, int& n_) const override;

   void getRowMinMaxVec(bool getMin, bool initializeVec, const Vector<double>* colScaleVec, Vector<double>& minmaxVec) const override;
   void getColMinMaxVec(bool getMin, bool initializeVec, const Vector<double>* rowScaleVec, Vector<double>& minmaxVec) const override;

   void writeToStreamDense(std::ostream& out) const override;

   void addRowSums(Vector<double>& vec) const override;
   void addColSums(Vector<double>& vec) const override;

   /* methods not needed for Hierarchical approach */
   using GeneralMatrix::abminnormNonZero;
   [[nodiscard]] double abminnormNonZero(double) const override {
      assert(false && "TODO: implement");
      return 0.0;
   };

   void writeToStream(std::ostream&) const override { assert(0 && "not implemented"); }; // TODO : implement maybe?

   void getDiagonal(Vector<double>&) const override {
      assert(0 && "not implemented");
   }; // TODO : not sure - maybe we want this to get forwarded to the underlying matrix?
   void setToDiagonal(const Vector<double>&) override {
      assert(0 && "not implemented");
   }; // TODO : not sure - maybe we want this to get forwarded to the underlying matrix?
   void atPutDiagonal(int, const Vector<double>&) override {
      assert(0 && "not implemented");
   }; // TODO : not sure - maybe we want this to get forwarded to the underlying matrix?
   void atAddDiagonal(int, const Vector<double>&) override {
      assert(0 && "not implemented");
   }; // TODO : not sure - maybe we want this to get forwarded to the underlying matrix?
   void fromGetDiagonal(int, Vector<double>&) const override {
      assert(0 && "not implemented");
   }; // TODO : not sure - maybe we want this to get forwarded to the underlying matrix?

   void matTransDMultMat(const Vector<double>&, SymmetricMatrix**) const override { assert(0 && "not implemented"); }; // TODO : needed?
   void matTransDinvMultMat(const Vector<double>&, SymmetricMatrix**) const override { assert(0 && "not implemented"); }; // TODO : needed?

   void getNnzPerRow(Vector<int>&) const override { assert(0 && "not implemented"); };
   void getNnzPerCol(Vector<int>&) const override { assert(0 && "not implemented"); };
   void fromGetDense(int, int, double*, int, int, int) const override { assert(0 && "not implemented"); };
   void fromGetSpRow(int, int, double*, int, int*, int&, int, int&) const override { assert(0 && "not implemented"); };
   void putSparseTriple(const int*, int, const int*, const double*, int&) override { assert(0 && "not implemented"); };
   void symmetricScale(const Vector<double>&) override { assert(0 && "not implemented"); };
   void atPutSubmatrix(int, int, const AbstractMatrix&, int, int, int, int) override { assert(0 && "not implemented"); };
   void atPutDense(int, int, const double*, int, int, int) override { assert(0 && "not implemented"); };
   void atPutSpRow(int, const double*, int, const int*, int&) override { assert(0 && "not implemented"); };
private:

   template<typename T>
   bool hasVecStructureForBorderedMat(const Vector<T>& vec, bool row_vec) const;
};

#endif /* BORDEREDMATRIX_H */