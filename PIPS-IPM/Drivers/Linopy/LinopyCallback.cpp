#include "../../Core/Interface/PIPSIPMppInterface.hpp"
#include "../../Core/Options/PIPSIPMppOptions.h"
#include "DistributedInputTree.h"


#include "mpi.h"

//added Packages for reading directly from the files
#include <fstream>
#include <string>
#include "Linopy_Source/Linopy_reader.hpp"
#include "Linopy_Source/LinopySorter.hpp"

extern "C" typedef int (* FNNZ)(void* user_data, int id, int* nnz);

/* Row-major format */
extern "C" typedef int (* FMAT)(void* user_data, int id, int* krowM, int* jcolM, double* M);

extern "C" typedef int (* FVEC)(void* user_data, int id, double* vec, int len);


/** Problem parameters and data */
class ProbData {
public: //data
    int nScenarios;

public: //methods
    explicit ProbData(int nScenarios) {
        this->nScenarios = nScenarios;
    };

//  ~ProbData();
};
    


void printhelp() {
    std::cout << "You need help using the arguments correct.\n";
    std::cout << "Your Input should look something like this: \n";
    std::cout << "./pipsipmLinopyCallback N PATH/TO/FOLDER [--sort]\n";
    std::cout << "N is for the number of Blocks.\n ";
    std::cout << "PATH/TO/FOLDER is where we store the Folder that have to be read in\n";
    std::cout << "If the Data was never Processed by this code, you have to --sort";
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int nScenarios; //= std::stoi(argv[1]); //Argument aus der Main-Funktion
    std::string Filepath;// = argv[2];
    bool Flag = false;

    int rank;
    int size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if(rank == 0) {
        if(argc == 1) {
            std::cout << "If you need help with what you have to Input. Try again with the Flag  \"-h\"" << std::endl;
            return 1;
        }
        if (argc == 2) {
            char* s;
            s = argv[1];
            if (strcmp( s, "-h") == 0 || strcmp( s, "--help") == 0) {
                printhelp();
                return 1;
            }
        }
        else if(argc == 3) {
            nScenarios = std::stoi(argv[1]);
            Filepath = argv[2];
        }
        else if(argc == 4) {
            nScenarios = std::stoi(argv[1]);
            Filepath = argv[2];
            char* s;
            s = argv[3];
            Flag = strcmp(s,"--sort") == 0;
        }
    }
    if(rank == 0) {
        if (Flag) { //ändern
            std::cout << "The Data from Linopy will now get PIPSified." << std::endl;
            Linopy_Sorter Sorter(Filepath, nScenarios);
            Filepath += "_PIPS-Structured";
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);
    ProbData probData(nScenarios);
    linopy_reader reader(Filepath,nScenarios);

    std::unique_ptr<DistributedInputTree> root{reader.read_problem()};

    if (rank == 0)
        std::cout << "Using a total of " << size << " MPI processes.\n";
   
    /* use BiCGStab for outer solve */
    pipsipmpp_options::set_int_parameter("INNER_SC_SOLVE", 0);
    if (rank == 0)
        std::cout << "PIPSIPMppInterface pipsIpm" << std::endl;


    PIPSIPMppInterface pipsIpm(root.get(), InteriorPointMethodType::PRIMAL, MPI_COMM_WORLD, ScalerType::GEOMETRIC_MEAN, PresolverType::PRESOLVE); 

    if (rank == 0)
        std::cout << "PIPSIPMppInterface created\n";

    if (rank == 0)
        std::cout << "solving...\n";

    pipsIpm.run();

    
    if (rank == 0) {
        const double objective = pipsIpm.getObjective();
        std::cout << "solving finished ... objective value: " << objective << "\n";
        reader.write_solution(pipsIpm, Filepath);
    }
    MPI_Finalize();
    
    return 0;
}
