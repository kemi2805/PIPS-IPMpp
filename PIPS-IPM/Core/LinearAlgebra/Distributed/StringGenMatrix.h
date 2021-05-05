/*
 * StringGenMatrix.h
 *
 *  Created on: Sep 14, 2020
 *      Author: bzfkempk
 */

#ifndef PIPS_IPM_CORE_STOCHLINEARALGEBRA_STRINGGENMATRIX_H_
#define PIPS_IPM_CORE_STOCHLINEARALGEBRA_STRINGGENMATRIX_H_

#include "DoubleMatrix.h"
#include "SparseGenMatrix.h"
#include "DoubleMatrixTypes.h"

#include "mpi.h"

class DistributedTreeCallbacks;

class StringGenMatrix : public GenMatrix {
public:
   // TODO : keep public? ...
   // like stoch gen matrix possibly infinite but we will only have one layer of children at all times...
   std::vector<StringGenMatrix*> children;
   GenMatrix* mat{}; // never null
   GenMatrix* mat_link{}; // possibly null
   bool is_vertical{false};

protected:
   long long m{0};
   long long n{0};
   const MPI_Comm mpi_comm{MPI_COMM_NULL};
   const bool distributed{false};
   const int rank{-1};
   long long nonzeros{0};

   // will not delete its data when deleted
   const bool is_view{false};
public:
   StringGenMatrix() = default;

   StringGenMatrix(bool is_vertical, GenMatrix* mat, GenMatrix* mat_link, MPI_Comm mpi_comm_, bool is_view = false);

   ~StringGenMatrix() override;

   [[nodiscard]] MPI_Comm getComm() const { return mpi_comm; };

   virtual void addChild(StringGenMatrix* child);

   [[nodiscard]] virtual bool isEmpty() const;
   [[nodiscard]] int isKindOf(int matrix) const override;

   /** y = beta * y + alpha * this * x */
   void mult(double beta, Vector<double>& y, double alpha, const Vector<double>& x) const override;
   /** y = beta * y + alpha * this^T * x */
   void transMult(double beta, Vector<double>& y, double alpha, const Vector<double>& x) const override;

   [[nodiscard]] double abmaxnorm() const override;
   void scalarMult(double num) override;

   void writeToStreamDense(std::ostream&) const override;
   void writeToStreamDenseRow(std::ostream& out, int row) const override;
   void writeDashedLineToStream(std::ostream& out) const override;

   void getRowMinMaxVec(bool getMin, bool initializeVec, const Vector<double>* colScaleVec, Vector<double>& minmaxVec) const override;
   void getColMinMaxVec(bool getMin, bool initializeVec, const Vector<double>* rowScaleVec, Vector<double>& minmaxVec) const override;

   void columnScale(const Vector<double>& vec) override;
   void rowScale(const Vector<double>& vec) override;

   void addRowSums(Vector<double>& vec) const override;
   void addColSums(Vector<double>& vec) const override;

   void getSize(long long& m_, long long& n_) const override {
      m_ = m;
      n_ = n;
   };
   void getSize(int& m_, int& n_) const override {
      m_ = static_cast<int>(m);
      n_ = static_cast<int>(n);
   };

   /** split the current children according to map_child_subchild: the new StringGenMatrices has one additional layer of StringGenMatrices */
   virtual void combineChildrenInNewChildren(const std::vector<unsigned int>& map_child_subchild, const std::vector<MPI_Comm>& child_comms);
   virtual void splitAlongTree(const DistributedTreeCallbacks& tree);

   virtual void recomputeNonzeros();
   [[nodiscard]] int numberOfNonZeros() const override;

   [[nodiscard]] GenMatrix* shaveBottom(int n_rows) override;

   /* methods not needed for Hierarchical approach */
   [[nodiscard]] double abminnormNonZero(double) const override {
      assert(false && "TODO: implement");
      return 0.0;
   };
   void atPutDiagonal(int, const Vector<double>&) override { assert("not implemented" && 0); };
   void atAddDiagonal(int, const Vector<double>&) override { assert("not implemented" && 0); };
   void fromGetDiagonal(int, Vector<double>&) const override { assert("not implemented" && 0); };
   void fromGetDense(int, int, double*, int, int, int) const override { assert("not implemented" && 0); };
   void fromGetSpRow(int, int, double[], int, int[], int&, int, int&) const override { assert("not implemented" && 0); };
   void getDiagonal(Vector<double>&) const override { assert("not implemented" && 0); };
   void setToDiagonal(const Vector<double>&) override { assert("not implemented" && 0); };
   void matTransDMultMat(const Vector<double>&, SymMatrix**) const override { assert("not implemented" && 0); };
   void matTransDinvMultMat(const Vector<double>&, SymMatrix**) const override { assert("not implemented" && 0); };
   void symmetricScale(const Vector<double>&) override { assert("not implemented" && 0); };
   void writeToStream(std::ostream&) const override { assert("not implemented" && 0); };
   void atPutSubmatrix(int, int, const DoubleMatrix&, int, int, int, int) override { assert("not implemented" && 0); };
   void atPutDense(int, int, const double*, int, int, int) override { assert("not implemented" && 0); };
   void atPutSpRow(int, const double[], int, const int[], int&) override { assert("not implemented" && 0); };
   void putSparseTriple(const int[], int, const int[], const double[], int&) override { assert("not implemented" && 0); };

protected:
   virtual void multVertical(double beta, Vector<double>& y, double alpha, const Vector<double>& x) const;
   virtual void multHorizontal(double beta, Vector<double>& y, double alpha, const Vector<double>& x, bool root) const;

   virtual void transMultVertical(double beta, Vector<double>& y, double alpha, const Vector<double>& x, bool root) const;
   virtual void transMultHorizontal(double beta, Vector<double>& y, double alpha, const Vector<double>& x) const;

   virtual void columnScaleVertical(const Vector<double>& vec);
   virtual void columnScaleHorizontal(const Vector<double>& vec);

   virtual void rowScaleVertical(const Vector<double>& vec);
   virtual void rowScaleHorizontal(const Vector<double>& vec);

   virtual void getRowMinMaxVecVertical(bool get_min, bool initialize_vec, const Vector<double>* col_scale, Vector<double>& minmax) const;
   virtual void getRowMinMaxVecHorizontal(bool get_min, bool initialize_vec, const Vector<double>* col_scale, Vector<double>& minmax) const;

   virtual void getColMinMaxVecVertical(bool get_min, bool initialize_vec, const Vector<double>* row_scale, Vector<double>& minmax) const;
   virtual void getColMinMaxVecHorizontal(bool get_min, bool initialize_vec, const Vector<double>* row_scale, Vector<double>& minmax) const;

   virtual void addRowSumsVertical(Vector<double>& vec) const;
   virtual void addRowSumsHorizontal(Vector<double>& vec) const;

   virtual void addColSumsVertical(Vector<double>& vec) const;
   virtual void addColSumsHorizontal(Vector<double>& vec) const;
};

/**
 * Dummy version ...
 */
class StringGenDummyMatrix : public StringGenMatrix {
public:
   StringGenDummyMatrix() = default;
   ~StringGenDummyMatrix() override = default;

   void addChild(StringGenMatrix*) override {};

   [[nodiscard]] bool isEmpty() const override { return true; };
   [[nodiscard]] int isKindOf(int type) const override { return type == kStringGenDummyMatrix || type == kStringMatrix || type == kStringGenMatrix; };
   void mult(double, Vector<double>&, double, const Vector<double>&) const override {};
   void transMult(double, Vector<double>&, double, const Vector<double>&) const override {};
   [[nodiscard]] double abmaxnorm() const override { return -std::numeric_limits<double>::infinity(); };
   void scalarMult(double) override {};
   void writeToStream(std::ostream&) const override {};
   void writeToStreamDenseRow(std::ostream&, int) const override {};
   void writeDashedLineToStream(std::ostream&) const override {};

   void getRowMinMaxVec(bool, bool, const Vector<double>*, Vector<double>&) const override {};
   void getColMinMaxVec(bool, bool, const Vector<double>*, Vector<double>&) const override {};
   void columnScale(const Vector<double>&) override {};
   void rowScale(const Vector<double>&) override {};

   void addRowSums(Vector<double>&) const override {};
   void addColSums(Vector<double>&) const override {};

   void recomputeNonzeros() override {};
   [[nodiscard]] int numberOfNonZeros() const override { return 0; };

   GenMatrix* shaveBottom(int) override { return new StringGenDummyMatrix(); };

   void splitAlongTree(const DistributedTreeCallbacks&) override { assert(false && "should not end up here"); };

protected:
   void multVertical(double, Vector<double>&, double, const Vector<double>&) const override {};
   void multHorizontal(double, Vector<double>&, double, const Vector<double>&, bool) const override {};

   void transMultVertical(double, Vector<double>&, double, const Vector<double>&, bool) const override {};
   void transMultHorizontal(double, Vector<double>&, double, const Vector<double>&) const override {};

   void columnScaleVertical(const Vector<double>&) override {};
   void columnScaleHorizontal(const Vector<double>&) override {};

   void rowScaleVertical(const Vector<double>&) override {};
   void rowScaleHorizontal(const Vector<double>&) override {};

   void getRowMinMaxVecVertical(bool, bool, const Vector<double>*, Vector<double>&) const override {};
   void getRowMinMaxVecHorizontal(bool, bool, const Vector<double>*, Vector<double>&) const override {};

   void getColMinMaxVecVertical(bool, bool, const Vector<double>*, Vector<double>&) const override {};
   void getColMinMaxVecHorizontal(bool, bool, const Vector<double>*, Vector<double>&) const override {};

   void addRowSumsVertical(Vector<double>&) const override {};
   void addRowSumsHorizontal(Vector<double>&) const override {};

   void addColSumsVertical(Vector<double>&) const override {};
   void addColSumsHorizontal(Vector<double>&) const override {};
};

#endif /* PIPS_IPM_CORE_STOCHLINEARALGEBRA_STRINGGENMATRIX_H_ */