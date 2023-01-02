#include "LinopySorter.hpp"

Linopy_Sorter::Linopy_Sorter(std::string _Filepath, int _nScenarios) : 
    Filepath{_Filepath},
    nScenarios(_nScenarios) {
    for(int i=0; i<=nScenarios; i++) {
        if(!i) {
            Create_Folders();
            Allocate_Linking_Mem();
        }
        Write_in(i);
        Get_All_Active_Vectors(i);
        if(Are_there_int_errors()) 
            Correct_potential_int_erros(i);
        Get_xvec_and_Substractor_from_x();
        if(!i) {
            xvec0 = xvec;
            xvec0_Subtractor = xvec_Subtractor;
        }
        Resize_All_Col(i);
        if(i)
            Add_Both_Rows();
        Transform_non_Linking_Matrices(i);
        Resize_non_Linking_Rows(i);
        Write_out_and_delete(i);
        std::cout << nScenarios - i << " Blocks left to be written down" << std::endl;
    }
    Add_Linking_Rows();
    Transform_Linking_Matrices();
    Resize_Linking_Rows();
    Write_out_and_delete_linking();
    std::cout << "The Data has been written onto: " << Filepath << "_PIPS-Structured" << std::endl;
}
void Linopy_Sorter::Create_Folders() {
    std::string PathToFile = Filepath;
    PathToFile += "_PIPS-Structured";
    PathToFile += "/block";
    for(int i = 0; i<=nScenarios;i++) {
        std::string PathToFolder = PathToFile; 
        PathToFolder += std::to_string(i);
        boost::filesystem::create_directories(PathToFolder.c_str());
    }
}

void Linopy_Sorter::Allocate_Linking_Mem() {
    if(BL_row!=nullptr&&DL_row!=nullptr) return;
    BL_row = std::make_shared<std::vector<std::shared_ptr<std::vector<int>>>>(nScenarios+1);
    BL_col = std::make_shared<std::vector<std::shared_ptr<std::vector<int>>>>(nScenarios+1);
    BL_data = std::make_shared<std::vector<std::shared_ptr<std::vector<double>>>>(nScenarios+1);
    DL_row = std::make_shared<std::vector<std::shared_ptr<std::vector<int>>>>(nScenarios+1);
    DL_col = std::make_shared<std::vector<std::shared_ptr<std::vector<int>>>>(nScenarios+1);
    DL_data = std::make_shared<std::vector<std::shared_ptr<std::vector<double>>>>(nScenarios+1);
}

void Linopy_Sorter::Write_in(int id) {
    Write_File_in(c,"c",id);
    Write_File_in(b,"b",id);
    Write_File_in(dl,"dl",id);
    Write_File_in(du,"du",id);
    Write_File_in(xl,"xl",id);
    Write_File_in(xu,"xu",id);

    Write_File_in(x,"x",id);

    if(!id) {
        Write_File_in(bl,"b",nScenarios+1);
        Write_File_in(dll,"dl",nScenarios+1);
        Write_File_in(dlu,"du",nScenarios+1);
    }

    Write_File_in(A_data,"A_data",id);
    Write_File_in(A_row,"A_row",id);
    Write_File_in(A_col,"A_col",id);

    Write_File_in(C_data,"C_data",id);
    Write_File_in(C_row,"C_row",id);
    Write_File_in(C_col,"C_col",id);

    if(id) {
        Write_File_in(B_data,"B_data",id);
        Write_File_in(B_row,"B_row",id);
        Write_File_in(B_col,"B_col",id);

        Write_File_in(D_data,"D_data",id);
        Write_File_in(D_row,"D_row",id);
        Write_File_in(D_col,"D_col",id);
    }

    Write_File_in(BL_data->at(id),"BL_data",id);
    Write_File_in(BL_row->at(id),"BL_row",id);
    Write_File_in(BL_col->at(id),"BL_col",id);

    Write_File_in(DL_data->at(id),"DL_data",id);
    Write_File_in(DL_row->at(id),"DL_row",id);
    Write_File_in(DL_col->at(id),"DL_col",id);
}

template<typename T>
void Linopy_Sorter::Write_File_in(std::shared_ptr<std::vector<T>>& M,std::string name, int id) {
    std::string PathToFile = Filepath;
    PathToFile += "/block";
    PathToFile += std::to_string(id);
    PathToFile += "/";
    PathToFile += name;
    std::ifstream File(PathToFile);
    if(!File.is_open()) return;
    File.seekg(0, std::ios_base::end);
    std::size_t size=File.tellg();
    if(size==0) return;
    File.seekg(0, std::ios_base::beg);
    // Create a vector to store the data
    M = std::make_shared<std::vector<T>>(size/sizeof(T));
    // Load the data
    File.read(reinterpret_cast<char*>(M->data()), size);
    // Close the file
    File.close();
    return;
}
void Linopy_Sorter::Get_xvec_and_Substractor_from_x() {
    unsigned int i=1;
    xvec = std::make_shared<std::vector<int>>(x->size()*2);
    xvec->at(0) = x->front();
    for(std::vector<int>::iterator it = x->begin()+1; it!=x->end();it++) {
        if(*(it-1) +1 == *it);
        else {
            xvec->at(i) = *(it-1);
            xvec->at(i+1) = *it;
            i += 2;
        }
    }
    xvec->at(i) = x->back();
    xvec->resize(i+1);

    int Subtractor = 0;
    xvec_Subtractor =  std::make_shared<std::vector<int>>(xvec->size()/2);
    for(i = 0; i < xvec->size(); i+=2) {
        for(int j=i; j!=-1; j--) {
            if(!(j%2))
                Subtractor -= xvec->at(j);            
            else
                Subtractor += xvec->at(j);
        }
        Subtractor += i/2;
        xvec_Subtractor->at(i/2) = Subtractor;
        Subtractor = 0;
    }
}
void Linopy_Sorter::Correct_potential_int_erros(int id) {
    Delete_every_second_entry(x);
    Delete_every_second_entry(A_row);
    Delete_every_second_entry(A_col);
    Delete_every_second_entry(C_row);
    Delete_every_second_entry(C_col);
    Delete_every_second_entry(BL_row->at(id));
    Delete_every_second_entry(BL_col->at(id));
    Delete_every_second_entry(DL_row->at(id));
    Delete_every_second_entry(DL_col->at(id));
    if(id) {
        Delete_every_second_entry(B_row);
        Delete_every_second_entry(B_col);
        Delete_every_second_entry(D_row);
        Delete_every_second_entry(D_col);
    }
}

void Linopy_Sorter::Delete_every_second_entry(std::shared_ptr<std::vector<int>>& M) {
    if(M==nullptr) return;
    std::unique_ptr<std::vector<int>> M_Temp = std::make_unique<std::vector<int>>(M->size()/2);
    int i = 0;
    for(std::vector<int>::iterator it = M->begin(); it != M->end(); it+=2) {
        M_Temp->at(i) = *it;
        i++;
    }
    M.reset();
    M = std::move(M_Temp);
}
bool Linopy_Sorter::Are_there_int_errors() {
    if(x->at(1) == 0)
        return true;
    return false;
}

void Linopy_Sorter::Resize_Col(std::shared_ptr<std::vector<int>>& M_col, std::shared_ptr<std::vector<int>>& M_xvec, std::shared_ptr<std::vector<int>>& M_xvec_Subtractor) {
    if(M_xvec == nullptr || M_col ==nullptr) return;
    int Guess = 0;

    for(std::vector<int>::iterator it=M_col->begin(); it!=M_col->end();it++) {
        if(*it==0);
        else if(*it > M_xvec->back())
            *it = 0;
        else if(*it < M_xvec->front())
            *it = 0;
        else {
            Guess = ((double(*it)-double(M_xvec->front()))/((M_xvec->back()-M_xvec->front())))*(M_xvec->size()-2);
            if((unsigned) Guess > M_xvec->size() || Guess < 0) {
                std::cout << "With Resize_Col went something wrong" << std::endl;
                break;
            }
            if(Guess%2)
                Guess--;
            int Upper_Guess = M_xvec->size()-2;
            int Lower_Guess = 0;
            while(true) {
                if(((*it >= M_xvec->at(Guess)) && (*it <= M_xvec->at(Guess+1)))) {
                    *it += M_xvec_Subtractor->at(Guess/2);
                    break;
                }
                else if(*it < M_xvec->at(Guess)) {
                    if(*it > M_xvec->at(Guess-1)) {
                        *it = 0;
                        std::cout << "Something went wrong with my columns 1" << std::endl;
                        break;
                    }
                    else {
                        Upper_Guess = Guess;
                        Guess -= double(Guess-Lower_Guess)/2;
                        if(Guess%2) Guess--;
                        if(Upper_Guess == Lower_Guess) {
                            Lower_Guess -= 2;
                            Guess -= 2;
                        }                            
                        continue;
                    }

                }
                else {//(*it > M_xvec->at(Guess+1))
                    if(*it < M_xvec->at(Guess+2)) {
                        *it = 0;
                        std::cout << "Something went very wrong with my columns 2" << std::endl;
                        break;
                    }
                    else {
                        Lower_Guess = Guess;
                        Guess += double(Upper_Guess-Guess)/2;
                        if(Guess%2) Guess++;
                        if(Upper_Guess == Lower_Guess) {
                            Upper_Guess += 2;
                            Guess += 2;
                        }               
                        continue;
                    }
                }
            }
        }
    }
}

void Linopy_Sorter::Resize_All_Col(int id) {
    Resize_Col(A_col, xvec0, xvec0_Subtractor);
    Resize_Col(C_col, xvec0, xvec0_Subtractor);
    Resize_Col(BL_col->at(id), xvec, xvec_Subtractor);
    Resize_Col(DL_col->at(id), xvec, xvec_Subtractor);
    if(id) {
        Resize_Col(B_col, xvec, xvec_Subtractor);
        Resize_Col(D_col, xvec, xvec_Subtractor);
    }
}
void Linopy_Sorter::Add_Rows(std::shared_ptr<std::vector<int>>& M_row, std::shared_ptr<std::vector<int>>& K_row, std::shared_ptr<std::vector<int>>& Combined_Rows, int size) {
    if(K_row==nullptr || M_row==nullptr || size == 1) return;
    unsigned int i=0; unsigned int j=0;
    std::unique_ptr<std::vector<int>> Combined_Rows_temp = std::make_unique<std::vector<int>>(size);
    int k=0;
    if(M_row->front()<=K_row->front())
        Combined_Rows_temp->at(k) = M_row->front();
    else    
        Combined_Rows_temp->at(k) = K_row->front();
    k++;
        while(i<M_row->size() && j<K_row->size()) {
            if(M_row->at(i) < K_row->at(j)) {
                if(Combined_Rows_temp->at(k-1) != M_row->at(i)) {
                    Combined_Rows_temp->at(k) = M_row->at(i);
                    k++;
                }
                i++;
            }
            else if(M_row->at(i) > K_row->at(j)) {
                if(Combined_Rows_temp->at(k-1) != K_row->at(j)) {
                    Combined_Rows_temp->at(k) = K_row->at(j);
                    k++;
                }
                j++;
            }
            else if(M_row->at(i) == K_row->at(j)) {
                if(Combined_Rows_temp->at(k-1) != M_row->at(i)) {
                    Combined_Rows_temp->at(k) = M_row->at(i);
                    k++;
                }
                i++;j++;
            }
        }
    if(i==M_row->size()) {
        while(j<K_row->size()) {
            if(Combined_Rows_temp->at(k-1) != K_row->at(j)) {
                Combined_Rows_temp->at(k) = K_row->at(j);
                k++;
            }
            j++;
        }
    }
    else if(j==K_row->size()) {
        while(i<M_row->size()) {
            if(Combined_Rows_temp->at(k-1) != M_row->at(i)) {
                Combined_Rows_temp->at(k) = M_row->at(i);
                k++;
            }
            i++;
        }
    }
    Combined_Rows.reset();
    Combined_Rows_temp->resize(k);
    Combined_Rows = std::move(Combined_Rows_temp);
}
void Linopy_Sorter::Add_Both_Rows() {
    Add_Rows(A_row,B_row,Rows_in_A, b->size());
    Add_Rows(C_row,D_row,Rows_in_C, dl->size());
}

void Linopy_Sorter::Transform_Matrix(std::shared_ptr<std::vector<int>>& M_row,std::shared_ptr<std::vector<int>>& M_col, std::shared_ptr<std::vector<double>>& M_data, std::shared_ptr<std::vector<int>>& Combined_Rows) {
    if(Combined_Rows==nullptr || M_row == nullptr || Combined_Rows->size() == 1) return;
    std::unique_ptr<std::vector<int>> M_row_temp = std::make_unique<std::vector<int>>(M_row->size()+Combined_Rows->size());
    std::unique_ptr<std::vector<int>> M_col_temp = std::make_unique<std::vector<int>>(M_col->size()+Combined_Rows->size());
    std::unique_ptr<std::vector<double>> M_data_temp = std::make_unique<std::vector<double>>(M_data->size()+Combined_Rows->size());
    unsigned int i = 0; unsigned int j = 0;
    *M_row;
    for(int rows : *Combined_Rows) {
        if(i<M_row->size()) {
            if(rows < M_row->at(i)) {
                M_row_temp->at(j) = rows;
                M_col_temp->at(j) = 0;
                M_data_temp->at(j) = 0.0;
                j++;
            }
            while(rows == M_row->at(i)) {
                M_row_temp->at(j) = M_row->at(i);
                M_col_temp->at(j) = M_col->at(i);
                M_data_temp->at(j) = M_data->at(i);
                i++;j++;
                if(i==M_row->size()) break;
            }
        }
        else {
            M_row_temp->at(j) = rows;
            M_col_temp->at(j) = 0;
            M_data_temp->at(j) = 0.0;
            j++;
        }
    }
    M_row_temp->resize(j);
    M_col_temp->resize(j);
    M_data_temp->resize(j);

    M_row.reset();
    M_col.reset();
    M_data.reset();

    M_row = std::move(M_row_temp);
    M_col = std::move(M_col_temp);
    M_data = std::move(M_data_temp);
}

void Linopy_Sorter::Transform_non_Linking_Matrices(int id) {
    Transform_Matrix(A_row, A_col, A_data, Rows_in_A);
    Transform_Matrix(C_row, C_col, C_data, Rows_in_C);
    
    if(id) {
        Transform_Matrix(B_row, B_col, B_data, Rows_in_A);
        Transform_Matrix(D_row, D_col, D_data, Rows_in_C);
    }
}

void Linopy_Sorter::Resize_non_Linking_Rows(int id) {
    if(b != nullptr)
        Resize_Row(A_row, b->size());
    if(dl != nullptr)
        Resize_Row(C_row, dl->size());
    if(id) {
        Resize_Row(B_row, b->size());
        Resize_Row(D_row, dl->size());
    }
}

void Linopy_Sorter::Resize_Row(std::shared_ptr<std::vector<int>>& M_row, unsigned size) {
    std::unique_ptr<std::vector<int>> M_row_temp = std::make_unique<std::vector<int>>(size+1);
    long long temp = 0;
    unsigned i=0;
    if(M_row==nullptr) return;
    M_row_temp->at(i) = temp;
    i++;
    for(std::vector<int>::iterator it = M_row->begin() + 1; it != M_row->end(); it++) {
        if(*(it-1) != *it) {
            temp++;
            M_row_temp->at(i) = temp;
            i++;
        }
        else
            temp++;
    }
    temp++;
    M_row_temp->at(i) = temp;
    M_row.reset();
    M_row = std::move(M_row_temp);
}

void Linopy_Sorter::Get_Active_Vector(std::shared_ptr<std::vector<double>>& vec, std::shared_ptr<std::vector<int>>& Active_vec) {
    if(vec == nullptr) return;
    Active_vec = std::make_shared<std::vector<int>>(vec->size());
    for( unsigned i=0; i<vec->size();i++) {
        if(vec->at(i) == std::numeric_limits<double>::infinity() || vec->at(i) == -std::numeric_limits<double>::infinity()) {
            Active_vec->at(i) = 0;
            vec->at(i) = -12345;
        }
        else
            Active_vec->at(i) = 1;
    }
}
void Linopy_Sorter::Get_All_Active_Vectors(int id) {
    Get_Active_Vector(dl,idl);
    Get_Active_Vector(du,idu);
    Get_Active_Vector(xl,ixl);
    Get_Active_Vector(xu,ixu);
    if(!id) {
        Get_Active_Vector(dll,idll);
        Get_Active_Vector(dlu,idlu);
    }
}

void Linopy_Sorter::Write_out_and_delete(int id) {
    Write_File_out(c,"c",id);
    Write_File_out(b,"b",id);
    Write_File_out(dl,"dl",id);
    Write_File_out(du,"du",id);
    Write_File_out(xl,"xl",id);
    Write_File_out(xu,"xu",id);
    Write_File_out(idl,"idl",id);
    Write_File_out(idu,"idu",id);
    Write_File_out(ixl,"ixl",id);
    Write_File_out(ixu,"ixu",id);

    Write_File_out(x,"x",id);

    Rows_in_A.reset();
    Rows_in_C.reset();

    Write_File_out(A_data,"A_data",id);
    Write_File_out(A_row,"A_row",id);
    Write_File_out(A_col,"A_col",id);

    Write_File_out(C_data,"C_data",id);
    Write_File_out(C_row,"C_row",id);
    Write_File_out(C_col,"C_col",id);

    if(id) {
        Write_File_out(B_data,"B_data",id);
        Write_File_out(B_row,"B_row",id);
        Write_File_out(B_col,"B_col",id);

        Write_File_out(D_data,"D_data",id);
        Write_File_out(D_row,"D_row",id);
        Write_File_out(D_col,"D_col",id);
    }
}

template<typename T>
void Linopy_Sorter::Write_File_out(std::shared_ptr<std::vector<T>>& M, std::string name, int id) {
    std::string PathToFile = Filepath;
    PathToFile += "_PIPS-Structured";
    PathToFile += "/block";
    PathToFile += std::to_string(id);
    PathToFile += "/";
    PathToFile += name;
    FILE* File;
    File = fopen(PathToFile.c_str(), "w+");
    /*if(M!=nullptr && !M->empty()) {
        for(unsigned i=0; i <M->size(); i++) {
            if(sizeof(T)==8) 
                fprintf(File, "%f\n", M->at(i));
            else if(sizeof(T)==4) 
                fprintf(File, "%d\n", M->at(i));
        }
    }*/
    if(M!=nullptr && !M->empty())
        fwrite(M->data(), sizeof(T), M->size(), File);
    fclose(File);
    M.reset();
}

void Linopy_Sorter::Add_Linking_Rows() {
    if(!(bl==nullptr)) {
        std::shared_ptr<std::vector<int>> Combined_Rows = std::make_shared<std::vector<int>>(bl->size());
        if(DL_row->at(0)!=nullptr)
            Add_Rows(BL_row->at(0),BL_row->at(0),Combined_Rows, bl->size());
        else {
            Combined_Rows->resize(1);
            Combined_Rows->at(0) = BL_row->at(1)->at(0);
        }
        for(int i=1; i<=nScenarios; i++) {
            Add_Rows(Combined_Rows,BL_row->at(i),Combined_Rows,bl->size());
            *Combined_Rows;
        }
        Rows_in_BL = std::move(Combined_Rows);
    }
    if(dll == nullptr) return;
    std::shared_ptr<std::vector<int>> Combined_Rows = std::make_shared<std::vector<int>>(dll->size());
    if(DL_row->at(0)!=nullptr)
        Add_Rows(DL_row->at(0),DL_row->at(0),Combined_Rows, dll->size());
    else {
        Combined_Rows->resize(1);
        Combined_Rows->at(0) = DL_row->at(1)->at(0);
    }
    for(int i=1; i<=nScenarios; i++) {
        Add_Rows(Combined_Rows,DL_row->at(i),Combined_Rows,dll->size());
    }
    Rows_in_DL = std::move(Combined_Rows);
}

void Linopy_Sorter::Transform_Linking_Matrices() {
    for(int i=0; i<=nScenarios; i++) {
        Transform_Matrix(BL_row->at(i), BL_col->at(i), BL_data->at(i), Rows_in_BL);
        Transform_Matrix(DL_row->at(i), DL_col->at(i), DL_data->at(i), Rows_in_DL);
    }
}

void Linopy_Sorter::Resize_Linking_Rows() {
    for(int i=0; i<=nScenarios; i++) {
        if(bl != nullptr){
            Resize_Row(BL_row->at(i), bl->size());
        }
        if(dll != nullptr)
            Resize_Row(DL_row->at(i), dll->size());
    }
}

void Linopy_Sorter::Write_out_and_delete_linking() {
    Write_File_out(bl,"bl",0);
    Write_File_out(dll,"dll",0);
    Write_File_out(dlu,"dlu",0);
    Write_File_out(idll,"idll",0);
    Write_File_out(idlu,"idlu",0);

    for(int id = 0; id<=nScenarios; id++) {
        Write_File_out(BL_data->at(id),"BL_data",id);
        Write_File_out(BL_row->at(id),"BL_row",id);
        Write_File_out(BL_col->at(id),"BL_col",id);

        Write_File_out(DL_data->at(id),"DL_data",id);
        Write_File_out(DL_row->at(id),"DL_row",id);
        Write_File_out(DL_col->at(id),"DL_col",id);
    }
    BL_data.reset();
    BL_col.reset();
    BL_row.reset();
    DL_data.reset();
    DL_col.reset();
    DL_row.reset();
    xvec0.reset();
    xvec0_Subtractor.reset();
}