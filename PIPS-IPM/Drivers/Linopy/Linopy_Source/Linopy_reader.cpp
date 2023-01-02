#include "Linopy_reader.hpp"
#include <fstream>
#include "../../../Core/Interface/PIPSIPMppInterface.hpp"
#include "../../../Core/InteriorPointMethod/TerminationStatus.hpp"

extern "C" typedef int (* FNNZ)(void* user_data, int id, int* nnz);

/* Row-major format */
extern "C" typedef int (* FMAT)(void* user_data, int id, int* krowM, int* jcolM, double* M);

extern "C" typedef int (* FVEC)(void* user_data, int id, double* vec, int len);

int numBlocks = 0;//Für die Funktion read_in_block wird das so gemacht
std::string Filepath;

#define checkAndAlloc(blk)                                                             \
   if (!blocks[blk])                                                                   \
   {                                                                                   \
      blocks[blk] = (linopyPIPSBlockData_t*) malloc(sizeof(linopyPIPSBlockData_t));    \
      read_in_block(numBlocks, blocks[blk],blk,Filepath);                              \
   }                                          


#define nCB(nType)                                                            \
   int fsize##nType(void* user_data, int id, int* nnz)                        \
   {                                                                          \
      linopyPIPSBlockData_t** blocks = (linopyPIPSBlockData_t**) user_data;   \
      checkAndAlloc(id);                                                      \
      linopyPIPSBlockData_t* blk = blocks[id];                                \
      *nnz = blk->nType;                                                      \
      return 0;                                                               \
   }

nCB(ni)
nCB(mA)
nCB(mC)
nCB(mBL)
nCB(mDL)

#define nnzCB(nnzType)                                                        \
   int fnonzero##nnzType(void* user_data, int id, int* nnz)                   \
   {                                                                          \
      linopyPIPSBlockData_t** blocks = (linopyPIPSBlockData_t**) user_data;   \
      checkAndAlloc(id);                                                      \
      linopyPIPSBlockData_t* blk = blocks[id];                                \
      *nnz = blk->nnz##nnzType;                                               \
      return 0;                                                               \
   }

nnzCB(A)
nnzCB(B)
nnzCB(C)
nnzCB(D)
nnzCB(BL)
nnzCB(DL)

#define vecCB(vecType, size)                                               \
   int fvec##vecType(void* user_data, int id, double* vec, int len)        \
   {                                                                       \
      linopyPIPSBlockData_t** blocks = (linopyPIPSBlockData_t**) user_data;\
      checkAndAlloc(id);                                                   \
      linopyPIPSBlockData_t* blk = blocks[id];                             \
      if(blk->vecType == nullptr) return 0;                                \
      for( int i = 0; i < len; i++ ) {                                     \
         vec[i] = blk->vecType[i];                                         \
      }                                                                    \
      return 0;                                                            \
   }

vecCB(c, ni)
vecCB(xlow, ni)
vecCB(ixlow, ni)
vecCB(xupp, ni)
vecCB(ixupp, ni)
//vecCB(ixtyp, ni)
vecCB(b, mA)
vecCB(clow, mC)
vecCB(iclow, mC)
vecCB(cupp, mC)
vecCB(icupp, mC)
vecCB(bL, mBL)
vecCB(dlow, mDL)
vecCB(idlow, mDL)
vecCB(dupp, mDL)
vecCB(idupp, mDL)


#define matCB(mat, mmat)                                                     \
   int fmat##mat(void* user_data, int id, int* krowM, int* jcolM, double* M) \
   {                                                                         \
      linopyPIPSBlockData_t** blocks = (linopyPIPSBlockData_t**) user_data;  \
      checkAndAlloc(id);                                                     \
      linopyPIPSBlockData_t* blk = blocks[id];                               \
      if ( 0==blk->m##mmat || nullptr == blk->rm##mat)                       \
      {                                                                      \
        krowM[0] = 0;                                                        \
        return 0;                                                            \
      }                                                                      \
                                                                             \
      for( int i = 0; i <= blk->m##mmat; i++ ) {                             \
         krowM[i] = blk->rm##mat[i];                                         \
      }                                                                      \
                                                                             \
      for( int k = 0; k < blk->nnz##mat; k++ ) {                             \
         jcolM[k] = blk->ci##mat[k];                                         \
         M[k] = blk->val##mat[k];                                            \
      }                                                                      \
      return 0;                                                              \
   }


matCB(A, A)
matCB(B, A)
matCB(C, C)
matCB(D, C)
matCB(BL, BL)
matCB(DL, DL)

int fnonzeroQ(void*, int, int* nnz) {
   *nnz = 0;
   return 0;
}

int fmatQ(void* user_data, int id, int* krowM, int*, double*) {
    krowM[0] = 0;
    return 0;
}


    
linopy_reader::linopy_reader(std::string path_to_problem_, size_t n_blocks_) :
   pips_reader(std::move(path_to_problem_), n_blocks_) {
   blocks.resize(n_blocks+1);
   numBlocks = n_blocks;
   Filepath =path_to_problem;
}
linopy_reader::~linopy_reader() {
   for (auto& block : blocks) {
      freeBlock(block);
      free(block);
   }
}

std::unique_ptr<DistributedInputTree> linopy_reader::read_problem() {
   FNNZ fsni = &fsizeni;
   FNNZ fsmA = &fsizemA;
   FNNZ fsmC = &fsizemC;
   FNNZ fsmBL = &fsizemBL;
   FNNZ fsmDL = &fsizemDL;
   FNNZ fnnzQ = &fnonzeroQ;
   FNNZ fnnzA = &fnonzeroA;
   FNNZ fnnzB = &fnonzeroB;
   FNNZ fnnzC = &fnonzeroC;
   FNNZ fnnzD = &fnonzeroD;
   FNNZ fnnzBL = &fnonzeroBL;
   FNNZ fnnzDL = &fnonzeroDL;
   FVEC fc = &fvecc;
   FVEC fxlow = &fvecxlow;
   FVEC fixlow = &fvecixlow;
   FVEC fxupp = &fvecxupp;
   FVEC fixupp = &fvecixupp;
   //FVEC fixtyp = &fvecixtyp; //No idea for what this is
   FVEC fb = &fvecb;
   FVEC fclow = &fvecclow;
   FVEC ficlow = &fveciclow;
   FVEC fcupp = &fveccupp;
   FVEC ficupp = &fvecicupp;
   FVEC fbL = &fvecbL;
   FVEC fdlow = &fvecdlow;
   FVEC fidlow = &fvecidlow;
   FVEC fdupp = &fvecdupp;
   FVEC fidupp = &fvecidupp;

   FMAT fA = &fmatA;
   FMAT fB = &fmatB;
   FMAT fC = &fmatC;
   FMAT fD = &fmatD;
   FMAT fBL = &fmatBL;
   FMAT fDL = &fmatDL;
   FMAT fQ = &fmatQ;

   //build the problem tree
   std::unique_ptr<DistributedInputTree::DistributedInputNode> data_root = std::make_unique<DistributedInputTree::DistributedInputNode>(blocks.data(), 0, fsni, fsmA, fsmBL, fsmC, fsmDL, fQ, fnnzQ, fc, fA, fnnzA, fB, fnnzB, fBL, fnnzBL, fb, fbL,
         fC, fnnzC, fD, fnnzD, fDL, fnnzDL, fclow, ficlow, fcupp, ficupp, fdlow, fidlow, fdupp, fidupp, fxlow, fixlow, fxupp, fixupp, nullptr, false);
   std::unique_ptr<DistributedInputTree> root = std::make_unique<DistributedInputTree>(std::move(data_root));

   for (int blk = 1; blk <= numBlocks; blk++) {
      std::unique_ptr<DistributedInputTree::DistributedInputNode> data = std::make_unique<DistributedInputTree::DistributedInputNode>(blocks.data(), blk, fsni, fsmA, fsmBL, fsmC, fsmDL, fQ, fnnzQ, fc, fA, fnnzA, fB, fnnzB, fBL, fnnzBL, fb,
            fbL, fC, fnnzC, fD, fnnzD, fDL, fnnzDL, fclow, ficlow, fcupp, ficupp, fdlow, fidlow, fdupp, fidupp, fxlow, fixlow, fxupp, fixupp, nullptr, false);

      root->add_child(std::make_unique<DistributedInputTree>(std::move(data)));
   }

   return root;
}

void linopy_reader::write_solution(PIPSIPMppInterface& solver_instance, const std::string& file_name) const {

   if (solver_instance.termination_status() != TerminationStatus::SUCCESSFUL_TERMINATION) {
      if (my_rank == 0)
         std::cout << "Not printing solution even though requested - PIPSIPMpp solve did not terminate successfully!\n";

      return;
   }

   //const double objective = solver_instance.getObjective();
   auto primalSolVec = solver_instance.gatherPrimalSolution();
   auto dualSolEqVec = solver_instance.gatherDualSolutionEq();
   auto dualSolIneqVec = solver_instance.gatherDualSolutionIneq();
   auto dualSolVarBounds = solver_instance.gatherDualSolutionVarBounds();

   auto eqValues = solver_instance.gatherEqualityConsValues();
   auto ineqValues = solver_instance.gatherInequalityConsValues();

   std::string Path_to_Solution = Filepath;
   Path_to_Solution += "/PrimalSolution_File.sol";
   std::cout << "PrimalSolution = " << Path_to_Solution << std::endl;
   std::ofstream Solution;
   Solution.open(Path_to_Solution);
   for(long unsigned int i = 0; i < primalSolVec.size();i++) {
   Solution << primalSolVec[i] << "\n";
   }
   Solution.close();

   Path_to_Solution = Filepath;
   Path_to_Solution += "/dualSolEqVec.sol";
   std::cout << "dualSolEqVec = " << Path_to_Solution << std::endl;
   Solution.open(Path_to_Solution);
   for(long unsigned int i = 0; i < dualSolEqVec.size();i++) {
   Solution << dualSolEqVec[i] << "\n";
   }
   Solution.close();

   Path_to_Solution = Filepath;
   Path_to_Solution += "/dualSolIneqVec.sol";
   std::cout << "dualSolIneqVec = " << Path_to_Solution << std::endl;
   Solution.open(Path_to_Solution);
   for(long unsigned int i = 0; i < dualSolIneqVec.size();i++) {
   Solution << dualSolIneqVec[i] << "\n";
   }
   Solution.close();

   Path_to_Solution = Filepath;
   Path_to_Solution += "/dualSolVarBounds.sol";
   std::cout << "dualSolVarBounds = " << Path_to_Solution << std::endl;
   Solution.open(Path_to_Solution);
   for(long unsigned int i = 0; i < dualSolVarBounds.size();i++) {
   Solution << dualSolVarBounds[i] << "\n";
   }
   Solution.close();
}

void linopy_reader::read_in_problem(linopyPIPSBlockData_t* block, int id) {
    block->numBlocks = numBlocks;
    block->blockID = id;

    block->n0 = read_in_size<double>(block, 0, "c");
    block->ni = read_in_size<double>(block, id, "c");
    block->mA = read_in_size<double>(block, id, "b");
    block->mC = read_in_size<double>(block, id, "dl");
    block->mBL = read_in_size<double>(block, 0, "bl");
    block->mDL = read_in_size<double>(block,id, "dll");
    block->nnzA = read_in_size<double>(block, id, "A_data");
    block->nnzC = read_in_size<double>(block, id, "C_data");
    block->nnzBL = read_in_size<double>(block, id, "BL_data");
    block->nnzDL = read_in_size<double>(block, id, "DL_data");
    if(id) {
        block->nnzB = read_in_size<double>(block, id, "B_data");
        block->nnzD = read_in_size<double>(block, id, "D_data");
    }
    block->c = read_in_file<double>(block, id, "c");
    block->b = read_in_file<double>(block, id, "d");
    block->clow = read_in_file<double>(block, id, "dl");
    block->cupp = read_in_file<double>(block, id, "du");
    block->iclow = read_in_file<int>(block, id, "idl");
    block->icupp = read_in_file<int>(block, id, "idu");
    block->xlow = read_in_file<double>(block, id, "xl");
    block->xupp = read_in_file<double>(block, id, "xu");
    block->ixlow = read_in_file<int>(block, id, "ixl");
    block->ixupp = read_in_file<int>(block, id, "ixu");
    block->bL = read_in_file<double>(block, id, "bl");
    block->dlow = read_in_file<double>(block, id, "dll");
    block->dupp = read_in_file<double>(block, id, "dlu");
    block->idlow = read_in_file<int>(block, id, "idll");
    block->idupp = read_in_file<int>(block, id, "idlu");
    block->permN = nullptr;
    block->permM = nullptr;
    block->ixtyp = nullptr;             /**< variable type indicator of x_i: 0=cont., 1=binary, 2=integer */

    block->rmA = read_in_file<int>(block, id, "A_row");
    block->ciA = read_in_file<int>(block, id, "A_col");
    block->valA = read_in_file<double>(block, id, "A_data");
    block->rmC = read_in_file<int>(block, id, "C_row");
    block->ciC = read_in_file<int>(block, id, "C_col");
    block->valC = read_in_file<double>(block, id, "C_data");
    block->rmBL = read_in_file<int>(block, id, "BL_row");
    block->ciBL = read_in_file<int>(block, id, "BL_col");
    block->valBL = read_in_file<double>(block, id, "BL_data");
    block->rmDL = read_in_file<int>(block, id, "DL_row");
    block->ciDL = read_in_file<int>(block, id, "DL_col");
    block->valDL = read_in_file<double>(block, id, "DL_data");
    if(!id) {
        block->rmB = read_in_file<int>(block, id, "B_row");
        block->ciB = read_in_file<int>(block, id, "B_col");
        block->valB = read_in_file<double>(block, id, "B_data");
        block->rmD = read_in_file<int>(block, id, "D_row");
        block->ciD = read_in_file<int>(block, id, "D_col");
        block->valD = read_in_file<double>(block, id, "D_data");
    }
}

template<typename T>
T* linopy_reader::read_in_file(linopyPIPSBlockData_t* block, int id, std::string name) {
    std::string PathToFile = path_to_problem;
    PathToFile += "/block";
    PathToFile += std::to_string(id);
    PathToFile += "/";
    PathToFile += name;
    std::ifstream File(PathToFile);
    if(!File.is_open()) return nullptr;
    File.seekg(0, std::ios_base::end);
    std::size_t size=File.tellg();
    std::vector<T>* Temp_Data;
    if(size!=0) {
        File.seekg(0, std::ios_base::beg);
        // Create a vector to store the data
        Temp_Data = new std::vector<T>(size/sizeof(T));
        // Load the data
        File.read(reinterpret_cast<char*>(Temp_Data->data()), size);
        // Close the file
        File.close();
    }
    File.close();
    return (T*)Temp_Data->data();
}

template<typename T>
int linopy_reader::read_in_size(linopyPIPSBlockData_t* block, int id, std::string name) {
    std::string PathToFile = path_to_problem;
    PathToFile += "/block";
    PathToFile += std::to_string(id);
    PathToFile += "/";
    PathToFile += name;
    std::ifstream File(PathToFile);
    if(!File.is_open()) return 0;
    File.seekg(0, std::ios_base::end);
    std::size_t size=File.tellg();
    File.close();
    size = size/sizeof(T);
    return size;
}