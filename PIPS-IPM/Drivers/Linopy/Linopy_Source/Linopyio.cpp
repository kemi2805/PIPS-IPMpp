#include "Linopyio.hpp"
#include <cstdlib>
void freeBlock(linopyPIPSBlockData_t* blk)
{
   if ( NULL==blk)
      return;

   if ( blk->ni )
   {
      free(blk->c);
      free(blk->xlow);
      free(blk->xupp);
      free(blk->ixlow);
      free(blk->ixupp);
   }
   if (blk->mA)
   {
      free(blk->b);
      free(blk->rmA);
      if (blk->nnzA)
      {
         free(blk->ciA);
         free(blk->valA);
      }
      if ( blk->blockID != 0 )
      {
         free(blk->rmB);
         if (blk->nnzB)
         {
            free(blk->ciB);
            free(blk->valB);
         }
      }
   }
   if (blk->mC)
   {
      free(blk->clow);
      free(blk->cupp);
      free(blk->iclow);
      free(blk->icupp);
      free(blk->rmC);
      if (blk->nnzC)
      {
         free(blk->ciC);
         free(blk->valC);
      }
      if ( blk->blockID != 0 )
      {
         free(blk->rmD);
         if (blk->nnzD)
         {
            free(blk->ciD);
            free(blk->valD);
         }
      }
   }
   if (blk->mBL)
   {
      free(blk->bL);
      free(blk->rmBL);
      if (blk->nnzBL)
      {
         free(blk->ciBL);
         free(blk->valBL);
      }
   }
   if (blk->mDL)
   {
      free(blk->dlow);
      free(blk->dupp);
      free(blk->idlow);
      free(blk->idupp);
      free(blk->rmDL);
      if (blk->nnzDL)
      {
         free(blk->ciDL);
         free(blk->valDL);
      }
   }
}

template<typename T>
T* read_in_file(linopyPIPSBlockData_t* block, int id,std::string name, std::string Filepath ) {
   std::string PathToFile = Filepath;
   PathToFile += "/block";
   PathToFile += std::to_string(id);
   PathToFile += "/";
   PathToFile += name;
   std::ifstream File(PathToFile);
   if(!File.is_open()) return nullptr;
   File.seekg(0, std::ios_base::end);
   std::size_t size = File.tellg();
   std::vector<T>* Temp_Data = nullptr;
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
   if(size==0) return nullptr;
   return (T*)Temp_Data->data();
}

template<typename T>
int read_in_size(linopyPIPSBlockData_t* block, int id, std::string name, std::string Filepath) {
   std::string PathToFile = Filepath;
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

void read_in_block(int numBlocks, linopyPIPSBlockData_t* block, int id, std::string Filepath) {
   block->numBlocks = numBlocks;
   block->blockID = id;

   block->n0 = read_in_size<double>(block, 0, "c", Filepath);
   block->ni = read_in_size<double>(block, id, "c", Filepath);
   block->mA = read_in_size<double>(block, id, "b", Filepath);
   block->mC = read_in_size<double>(block, id, "dl", Filepath);
   block->mBL = read_in_size<double>(block, 0, "bl", Filepath);
   block->mDL = read_in_size<double>(block,0, "dll", Filepath);
   block->nnzA = read_in_size<double>(block, id, "A_data", Filepath);
   block->nnzC = read_in_size<double>(block, id, "C_data", Filepath);
   block->nnzBL = read_in_size<double>(block, id, "BL_data", Filepath);
   block->nnzDL = read_in_size<double>(block, id, "DL_data", Filepath);
   block->nnzB = read_in_size<double>(block, id, "B_data", Filepath);
   block->nnzD = read_in_size<double>(block, id, "D_data", Filepath);

   block->c = read_in_file<double>(block, id, "c", Filepath);
   block->b = read_in_file<double>(block, id, "b", Filepath);
   block->clow = read_in_file<double>(block, id, "dl", Filepath);
   block->cupp = read_in_file<double>(block, id, "du", Filepath);
   block->iclow = read_in_file<int>(block, id, "idl", Filepath);
   block->icupp = read_in_file<int>(block, id, "idu", Filepath);
   block->xlow = read_in_file<double>(block, id, "xl", Filepath);
   block->xupp = read_in_file<double>(block, id, "xu", Filepath);
   block->ixlow = read_in_file<int>(block, id, "ixl", Filepath);
   block->ixupp = read_in_file<int>(block, id, "ixu", Filepath);
   if(!id) {
      block->bL = read_in_file<double>(block, 0, "bl", Filepath);
      block->dlow = read_in_file<double>(block, 0, "dll", Filepath);
      block->dupp = read_in_file<double>(block, 0, "dlu", Filepath);
      block->idlow = read_in_file<int>(block, 0, "idll", Filepath);
      block->idupp = read_in_file<int>(block, 0, "idlu", Filepath);
   }
   block->permN = nullptr;
   block->permM = nullptr;
   block->ixtyp = nullptr;             /**< variable type indicator of x_i: 0=cont., 1=binary, 2=integer */

   block->rmA = read_in_file<int>(block, id, "A_row", Filepath);
   block->ciA = read_in_file<int>(block, id, "A_col", Filepath);
   block->valA = read_in_file<double>(block, id, "A_data", Filepath);
   block->rmC = read_in_file<int>(block, id, "C_row", Filepath);
   block->ciC = read_in_file<int>(block, id, "C_col", Filepath);
   block->valC = read_in_file<double>(block, id, "C_data", Filepath);
   block->rmBL = read_in_file<int>(block, id, "BL_row", Filepath);
   block->ciBL = read_in_file<int>(block, id, "BL_col", Filepath);
   block->valBL = read_in_file<double>(block, id, "BL_data", Filepath);
   block->rmDL = read_in_file<int>(block, id, "DL_row", Filepath);
   block->ciDL = read_in_file<int>(block, id, "DL_col", Filepath);
   block->valDL = read_in_file<double>(block, id, "DL_data", Filepath);
   block->rmB = read_in_file<int>(block, id, "B_row", Filepath);
   block->ciB = read_in_file<int>(block, id, "B_col", Filepath);
   block->valB = read_in_file<double>(block, id, "B_data", Filepath);
   block->rmD = read_in_file<int>(block, id, "D_row", Filepath);
   block->ciD = read_in_file<int>(block, id, "D_col", Filepath);
   block->valD = read_in_file<double>(block, id, "D_data", Filepath);
}
