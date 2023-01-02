#include<string>
#include<iostream>
#include<fstream>
#include<vector>

typedef struct 
{
  int     numBlocks;         /**< total number of blocks */
  int     blockID;           /**< block ID */
/* Counts */
  int     n0;                /**< number of variables in block 0 */
  int     ni;                /**< number of variables in block i */
  int     mA;                /**< number of constraint with equality fA */
  int     mC;                /**< number of constraint with inequality fC */
  int     mBL;               /**< number of linking constraint with equality fBL */
  int     mDL;               /**< number of linking constraint with inequality fDL */
  long int     nnzA;              /**< number of nonzeros in fA */
  long int     nnzB;              /**< number of nonzeros in recourse matrix fB */
  long int     nnzC;              /**< number of nonzeros in fC */
  long int     nnzD;              /**< number of nonzeros in recourse matrix fD */
  long int     nnzBL;             /**< number of nonzeros in fBL */
  long int     nnzDL;             /**< number of nonzeros in fDL */
/* Vectors */
  double*     c;                 /**< objective coefficients c_i, length : blockID==0? n0:ni */
  double*     b;                 /**< right hand side of A_i, length: mA */
  double*     clow;              /**< lower bound of C_i, length: mC */
  double*     cupp;              /**< upper bound of C_i, length: mC */
  int*    iclow;             /**< lower bound indicator of C_i, length: mC */
  int*    icupp;             /**< upper bound indicator of C_i, length: mC */
  double*     xlow;              /**< lower bound of x_i, length: blockID==0? n0:ni */
  double*     xupp;              /**< upper bound of x_i, length: blockID==0? n0:ni */
  int*    ixlow;             /**< lower bound indicator of x_i, length: blockID==0? n0:ni */
  int*    ixupp;             /**< upper bound indicator of x_i, length: blockID==0? n0:ni */
  double*     bL;                /**< right hand side of BL, length: mBL */
  double*     dlow;              /**< lower bound of DL, length: mDL */
  double*     dupp;              /**< upper bound of DL, length: mDL */
  int*    idlow;             /**< lower bound indicator of DL, length: mDL */
  int*    idupp;             /**< upper bound indicator of DL, length: mDL */
  int*    permN;             /**< GDX n vector with pointer to 0..(n0+ni-1) space */
  int*    permM;             /**< GDX m vector with pointer to 0..(mA+mC+mBL+mDL-1) space */
  int*    ixtyp;             /**< variable type indicator of x_i: 0=cont., 1=binary, 2=integer */  
/* Matrices */
  int*    rmA;               /**< row major of A, length: mA+1 */
  int*    ciA;               /**< column index of A, length: nnzA */
  double*     valA;              /**< coefficient value of A, length: nnzA */
  int*    rmB;               /**< row major of B, length: mA+1 */
  int*    ciB;               /**< column index of B, length: nnzB */
  double*     valB;              /**< coefficient value of B, length: nnzB */
  int*    rmC;               /**< row major of C, length: mC+1 */
  int*    ciC;               /**< column index of C, length: nnzC */
  double*     valC;              /**< coefficient value of C, length: nnzC */
  int*    rmD;               /**< row major of D, length: mD+1 */
  int*    ciD;               /**< column index of D, length: nnzD */
  double*     valD;              /**< coefficient value of D, length: nnzD */
  int*    rmBL;              /**< row major of BL, length: (blockID==0? n0:ni)+1 */
  int*    ciBL;              /**< column index of BL, length: nnzBL */
  double*     valBL;             /**< coefficient value of BL, length: nnzBL */
  int*    rmDL;              /**< row major of DL, length: (blockID==0? n0:ni)+1 */
  int*    ciDL;              /**< column index of DL, length: nnzDL */
  double*     valDL;             /**< coefficient value of DL, length: nnzDL */
} linopyPIPSBlockData_t;

void freeBlock(linopyPIPSBlockData_t* blk);
template<typename T>
int read_in_size(linopyPIPSBlockData_t* block, int id, std::string name, std::string Filepath);

void read_in_block(int numBlocks, linopyPIPSBlockData_t* block, int id, std::string Filepath);

template<typename T>
T* read_in_file(linopyPIPSBlockData_t* block, int id, std::string Filepath ,std::string name);