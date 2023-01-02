############## callbackExample ##############
add_executable(pipsipmLinopyCallback LinopyCallback.cpp)

add_library(Linopy
            Linopy_Source/Linopyio.cpp
            Linopy_Source/Linopy_reader.cpp
            Linopy_Source/LinopySorter.cpp
           )
find_package(Boost REQUIRED COMPONENTS filesystem)
include_directories(${Boost_INCLUDE_DIRS})
           

target_include_directories(pipsipmLinopyCallback PRIVATE ${includes_for_interface})
target_compile_options(pipsipmLinopyCallback PRIVATE ${OpenMP_CXX_FLAGS})
target_link_libraries(pipsipmLinopyCallback
        PRIVATE pips-ipmpp
        PRIVATE OpenMP::OpenMP_CXX
        PRIVATE MPI::MPI_CXX
        PRIVATE Linopy
        PRIVATE Boost::filesystem
        )

           
