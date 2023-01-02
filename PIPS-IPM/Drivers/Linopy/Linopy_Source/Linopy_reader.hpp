/*
 * Linopy_reader.hpp
 *
 *  Created on: 29.12.2022
 *      Author: Keneth Miler
 */

#ifndef PIPS_IPM_DRIVERS_Linopy_HPP_
#define PIPS_IPM_DRIVERS_Linopy_HPP_

#include <string>
#include <cassert>

#include "../../pips_reader.h"
#include "Linopyio.hpp"
#include "../../../Core/Readers/Distributed/DistributedInputTree.h"

class linopy_reader : pips_reader {
public:
   linopy_reader(std::string path_to_problem, size_t n_blocks);
   ~linopy_reader() override;
   template<typename T>
   T* read_in_file(linopyPIPSBlockData_t* block, int id, std::string name);
   std::unique_ptr<DistributedInputTree> read_problem() override;
   void write_solution(PIPSIPMppInterface& solver_instance, const std::string& file_name) const override;

protected:
   const std::string path_to_linopy;
   bool log_reading{false};
   std::vector<linopyPIPSBlockData_t*> blocks;
   
private: 
    void read_in_problem(linopyPIPSBlockData_t* block, int id);
    template<typename T>
    int read_in_size(linopyPIPSBlockData_t* block, int id, std::string name);
    
};





#endif /* PIPS_IPM_DRIVERS_GMSPIPS_GMSPIPSREADER_HPP_ */