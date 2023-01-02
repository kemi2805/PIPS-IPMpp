#include<fstream>
#include<string>
#include <vector>
#include <memory>
#include <iostream>
#include <algorithm>
#include <tuple>

#include <sys/stat.h>
#include <cstdio>
#include <boost/filesystem.hpp>


class Linopy_Sorter {
    protected:
    std::shared_ptr<std::vector<double>> c;
    std::shared_ptr<std::vector<double>> b;
    std::shared_ptr<std::vector<double>> dl;
    std::shared_ptr<std::vector<double>> du;
    std::shared_ptr<std::vector<double>> xl;
    std::shared_ptr<std::vector<double>> xu;
    std::shared_ptr<std::vector<int>> idl;
    std::shared_ptr<std::vector<int>> idu;
    std::shared_ptr<std::vector<int>> ixl;
    std::shared_ptr<std::vector<int>> ixu;

    std::shared_ptr<std::vector<double>> bl;
    std::shared_ptr<std::vector<double>> dll;
    std::shared_ptr<std::vector<double>> dlu;
    std::shared_ptr<std::vector<int>> idll;
    std::shared_ptr<std::vector<int>> idlu;

    std::shared_ptr<std::vector<int>> x;
    std::shared_ptr<std::vector<int>> xvec;//x but only its invervall
    std::shared_ptr<std::vector<int>> xvec0;
    std::shared_ptr<std::vector<int>> xvec_Subtractor;
    std::shared_ptr<std::vector<int>> xvec0_Subtractor;

    std::shared_ptr<std::vector<int>> Rows_in_A;
    std::shared_ptr<std::vector<int>> Rows_in_C;
    std::shared_ptr<std::vector<int>> Rows_in_BL;
    std::shared_ptr<std::vector<int>> Rows_in_DL;

    std::shared_ptr<std::vector<double>> A_data;
    std::shared_ptr<std::vector<int>> A_row;
    std::shared_ptr<std::vector<int>> A_col;

    std::shared_ptr<std::vector<double>> B_data;
    std::shared_ptr<std::vector<int>> B_row;
    std::shared_ptr<std::vector<int>> B_col;

    std::shared_ptr<std::vector<double>> C_data;
    std::shared_ptr<std::vector<int>> C_row;
    std::shared_ptr<std::vector<int>> C_col;

    std::shared_ptr<std::vector<double>> D_data;
    std::shared_ptr<std::vector<int>> D_row;
    std::shared_ptr<std::vector<int>> D_col;

    std::shared_ptr<std::vector<std::shared_ptr<std::vector<int>>>> BL_row;
    std::shared_ptr<std::vector<std::shared_ptr<std::vector<int>>>> BL_col;
    std::shared_ptr<std::vector<std::shared_ptr<std::vector<double>>>> BL_data;
    
    std::shared_ptr<std::vector<std::shared_ptr<std::vector<int>>>> DL_row;
    std::shared_ptr<std::vector<std::shared_ptr<std::vector<int>>>> DL_col;
    std::shared_ptr<std::vector<std::shared_ptr<std::vector<double>>>> DL_data;

    std::string Filepath;
    int nScenarios;
    
public:
    Linopy_Sorter(std::string _Filepath, int _nScenarios);

private:
    void Create_Folders();
    void Allocate_Linking_Mem();

    void Write_in(int id);
    template<typename T>
    void Write_File_in(std::shared_ptr<std::vector<T>>& M,std::string name, int id);

    bool Are_there_int_errors();
    void Correct_potential_int_erros(int id);
    void Delete_every_second_entry(std::shared_ptr<std::vector<int>>& M);

    void Get_All_Active_Vectors(int id);
    void Get_Active_Vector(std::shared_ptr<std::vector<double>>& vec, std::shared_ptr<std::vector<int>>& Active_vec);

    void Get_xvec_and_Substractor_from_x();
    void Resize_All_Col(int id);
    void Resize_Col(std::shared_ptr<std::vector<int>>& M_col, std::shared_ptr<std::vector<int>>& M_xvec, std::shared_ptr<std::vector<int>>& M_xvec_Subtractor);
    void Add_Both_Rows();
    void Add_Rows(std::shared_ptr<std::vector<int>>& M_row, std::shared_ptr<std::vector<int>>& K_row, std::shared_ptr<std::vector<int>>& Combined_Rows, int size);
    
    void Transform_non_Linking_Matrices(int id);
    void Transform_Matrix(std::shared_ptr<std::vector<int>>& M_row,std::shared_ptr<std::vector<int>>& M_col, std::shared_ptr<std::vector<double>>& M_data, std::shared_ptr<std::vector<int>>& Combined_Rows);
    void Resize_non_Linking_Rows(int id);
    void Resize_Row(std::shared_ptr<std::vector<int>>& M_row, unsigned size);  


    void Write_out_and_delete(int id);
    template<typename T>
    void Write_File_out(std::shared_ptr<std::vector<T>>& M, std::string name, int id);

    void Add_Linking_Rows();
    void Transform_Linking_Matrices();
    void Resize_Linking_Rows();
    void Resize_Row(std::shared_ptr<std::vector<int>>& M_row, std::shared_ptr<std::vector<int>>& Rows_in_Linking, unsigned size);
    void Write_out_and_delete_linking();
    

};