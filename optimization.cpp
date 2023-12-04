#include <iostream>
#include <math.h>
#include <vector>
#include <iomanip>

using namespace std;

void endline(int novar){
    cout<<"|__________|__________|";
    for (int i = 0; i < novar; i++){
        cout<<"__________";
    }
    cout<<"|__________|"<<endl;
}

///__________________________________________________________________________________________________

void swap(int* a, int* b){
    int temp = *a;
    *a = *b;
    *b = temp;
}

///__________________________________________________________________________________________________

void doubleBubbleSort(vector<int>* v1, vector<int>* v2){
    unsigned long size = (*v1).size();
    for (int i = 0; i < size-1; i++){
        for (int j = 0; j < size-1; j++){
            if ((*v1)[j] < (*v1)[j+1]){
                swap(&(*v1)[j], &(*v1)[j+1]);
                swap(&(*v2)[j], &(*v2)[j+1]);
            }
        }
    }
}


///This is a function to check for the infeasibility of the Phase 1 table
int checkForInfeasible(vector<double>* optsol, int nofunc, int novar){
    for (int i = (novar - nofunc); i < novar; i++){
        if ((*optsol)[i] != 0){                                 ///if any aux var has the coef != 0, the table is deemed Infeasible
            return 0;
        }
    }
    return 1;
}


///This is a function to print out the head of the table
void createTableHead(int* novar, vector<double>* ntmin){
    cout<<endl<<".______________________";
    for (int i = 0; i < *novar; i++){
        cout<<"__________";
    }
    cout<<"___________.";
    
    cout<<endl<<"|"<<setw(10)<<"J"<<"|"<<setw(10)<<"C"<<"|";
    for (int i = 0; i < *novar; i++){
        cout<<setw(10)<<(*ntmin)[i];
    }
    cout<<"|"<<setw(10)<<"X"<<"|"<<endl;
}

///This is a function to add the original rows with aux vars
void addVector(vector<double>* tableLine, vector<double>* toAdd, int novar){
    double temp = (*tableLine)[novar];                          ///save the X
    (*tableLine).pop_back();                                    ///remove the X
    for (int i = 0; i < (*toAdd).size(); i++){
        (*tableLine).push_back((*toAdd)[i]);                    ///add the coef of the aux vars to the end of the original function
    }
    (*tableLine).push_back(temp);                               ///add back X
}


///This is a function to input the target function
void createMinFunc(vector<double>* ntmin, int novar){
    cout<<"Input function that need to be minimize: ";
    for (int i = 1; i <= novar; i++){
        double value;
        cin>>value;
        (*ntmin).push_back(value);
    }
}


///This is a function to create the new target function for phase 1
void createMinFunc2(vector<double>*ntmin2, int novar, int novar2){
    int i = 0;
    while (i < novar){
        (*ntmin2).push_back(0);                                 ///Change all coefficient of the original vars to 0
        i++;
    }
    while (i < novar2){
        (*ntmin2).push_back(1);                                 ///Change all coefficient of aux vars to 1
        i++;
    }
}


///This is a function to create the original input table
void createTable(vector<vector<double>>* table, int novar, int nofunc){
    for (int i = 1; i <= nofunc; i++){
        vector<double> tempv;
        cout<<"Please input the function no "<<i<<": ";
        for (int j = 0; j <= novar; j++){
            double tempd;
            cin>>tempd;
            tempv.push_back(tempd);                             ///create the vector that store the coef of the ith function
        }
        if (tempv[novar] < 0){
            for (int j = 0; j <= novar; j++){
                tempv[j] = -tempv[j];
            }
        }
        (*table).push_back(tempv);                              ///add that vector to the table
    }
}


///This is a function to create a new table for phase 1
void createPhase1Table(vector<vector<double>>* table, int novar2, int novar, int nofunc){
    for (int i = 0; i < nofunc; i++){
        vector<double> temp;
        for (int j = 0; j < novar2 - novar; j++){               ///in this iteration, we create the coef of the aux vars based on the function,
            if (j == i){                                        ///if the position is correct, we add 1, if not, we add 0
                temp.push_back(1);
            }
            else{
                temp.push_back(0);
            }
        }
        addVector(&(*table)[i], &temp, novar);                  ///here we combine the original coef vector with the aux vector created above
    }
    
}


///This is a function to create a new table for phase 2
void createPhase2Table(vector<vector<double>>* table, int novar, int novar2, int nofunc, vector<int>* J){
    for (int i = 0; i < (*J).size(); i++){                      ///in this iteration, we delete all column with the aux var
        if ((*J)[i] > novar){
            (*J).erase((*J).begin()+i);
            (*table).erase((*table).begin()+i);
        }
    }
    for (int i = 0; i < (*J).size(); i++){                      ///in this iteration, we delete all row with the aux var
        int temp = novar2 - novar;
        while(temp > 0){
            (*table)[i].erase((*table)[i].begin()+novar);
            temp--;
        }
    }
}


///This is a function to find the basic column, aka the J column
void findBasic(int nofunc, int novar, vector<double>* ntmin, vector<vector<double>>* table, vector<int>* J){
    vector<int> ar;
    vector<int> var;
    
    for (int i = 1; i <= novar; i++){
        var.push_back(i);                                       ///here we create a new vector, correspond to the number of var,
    }                                                           ///for example, if there is 7 vars, we create a vector [1,2,3,4,5,6,7]
    
    for (int i = 0; i < novar; i++){                            ///here we create a vector to store the number of times eache var appears
        int countPositive = 0;
        ar.push_back(0);
        for (int j = 0; j < nofunc; j++){
            if ((*table)[j][i] == 0){
                ar[i]++;
            }
            if ((*table)[j][i] > 0){
                countPositive++;
            }
        }
        if (countPositive == 0){
            ar[i] = 0;                                          ///if among all coef of that var, there is no positive value, then we skip that var
        }
    }
    
    doubleBubbleSort(&ar, &var);                                ///here we double sort those 2 above vector, based on the number of time a var appear
                                                                ///for example, if two array is [1,2,3,4] and [2,0,3,1], then the sorted will be [3,1,4,2] and [3,2,1,0]
    for (int i = 0; i < nofunc; i++){
        (*J).push_back(var[i]);                                 ///here we save the above sorted vector to the J vector (basic vector),
    }                                                           ///if the sorted vector is [3,1,2,4], then the basic of the table we will take as [3,1,2] if there are
}                                                               ///3 functions


///This is a function to find the new basic column for phase 2, aka J for phase 2
void findNewBasic(vector<int>* J, int novar){
    for (int i = 0; i < (*J).size(); i++){
        if ((*J)[i] > novar){
            (*J).erase((*J).begin()+i);
        }
    }
}


///This is a function to find the coefficient of a table
void findCoef(vector<double>* C, vector<int> J, vector<double> ntmin, int nofunc){
    (*C).clear();                                               ///here we take the J vector, then check the target function for the coef of those vars
    for (int i = 0; i < J.size(); i++){
        (*C).push_back(ntmin[J[i]-1]);
    }
}


///This function is to check if the table is unbounded
int checkUnbounded(int pst, vector<vector<double>>* table, int nofunc){
    for (int i = 0; i < nofunc; i++){                           ///check for a column, to see if that column contains only negative coef
        if ((*table)[pst][i] > 0){
            return 0;
        }
    }
    return 1;
}


///This is a function to pivot the table based on the original table and the position of the pivot point
void pivot(int novar, int nofunc, vector<vector<double>>* table, int pivotX, int pivotY){
    vector<vector<double>> table2;
    for (int i = 0; i < nofunc; i++){
        vector<double> temp;
        for (int j = 0; j <= novar; j++){
            temp.push_back((*table)[i][j]);
        }
        table2.push_back(temp);
    }
    for (int i = 0; i <= novar; i++){
        table2[pivotX][i] /= (*table)[pivotX][pivotY];
    }
    for (int i = 0; i < nofunc; i++){
        if (i == pivotX){
            continue;
        }
        for (int j = 0; j <= novar; j++){
            table2[i][j] -= (*table)[i][pivotY]*(*table)[pivotX][j]/(*table)[pivotX][pivotY];
        }
    }
    for (int i = 0; i < nofunc; i++){
        for (int j = 0; j <= novar; j++){
            (*table)[i][j] = table2[i][j];
        }
    }
}


///This is a function to display table
void displayTable(int nofunc, int novar, vector<int> *J, vector<double> *C, vector<vector<double>>* table){
    for (int i = 0; i < nofunc; i++){
        cout<<"|"<<setw(10)<<(*J)[i]<<"|"<<setw(10)<<(*C)[i]<<"|";
        for (int j = 0; j < novar; j++){
            cout<<setw(10)<<(*table)[i][j];
        }
        cout<<"|"<<setw(10)<<(*table)[i][novar]<<"|"<<endl;
    }
    endline(novar);
}


///This is a function to find the delta row of the table
int findDelta(int novar, int nofunc, vector<double>* C, vector<vector<double>>* table, vector<double>* ntmin, vector<int> *J){
    double delta[novar];
    int countpositive = 0;
    
    displayTable(nofunc, novar, J, C, table);
    
    ///find and display delta line
    cout<<"|"<<setw(10)<<" "<<"|"<<setw(10)<<" "<<"|";
    
    for (int i = 0; i < novar; i++){
        delta[i] = 0;
        for (int j = 0; j < nofunc; j++){
            delta[i] += (*C)[j] * (*table)[j][i];               ///here we calculate and display the delta
        }
        delta[i] -= (*ntmin)[i];
        cout<<setw(10)<<delta[i];
        if (delta[i] > 0){
            countpositive++;                                    ///check for the number of positive delta
        }
    }
    
    cout<<"|"<<setw(10)<<" "<<"|"<<endl;
    endline(novar);
    ///

    if (countpositive == 0){
        cout<<"This problem is optimized!"<<endl;               ///if there are no positive delta, the problem is considered optimized
                                                                ///we then proceed to save and display the optimal solution based on J and C column
        vector<double> optsol;
        for (int i = 0; i < novar; i++){
            optsol.push_back(0);
        }
        for (int i = 0; i < nofunc; i++){
            optsol[(*J)[i]-1] = (*table)[i][novar];
        }
        cout<<"Optimal solution set is: {";
        for (int i = 0; i < novar-1; i++){
            cout<<optsol[i]<<", ";
        }
        cout<<optsol[novar-1]<<"}"<<endl;
        cout<<"Optimal solution is: ";
        double g = 0;
        for (int i = 0; i < novar; i++){
            g+= (*ntmin)[i]*optsol[i];
        }
        cout<<g<<endl;
        return checkForInfeasible(&optsol, nofunc, novar);      ///here we check if this table is feasible to be used in phase 2
    }
    
    double maxDelta = delta[0];                                 ///here we find the maximum delta, then save 1 of the coordinate of the pivot point
    int pivotY = 0;
    for (int i = 0; i < novar; i++){
        if (delta[i] > 0){
            if (maxDelta < delta[i]){
                maxDelta = delta[i];
                pivotY = i;
            }
        }
    }
    int t = 0;
    while ((*table)[t][pivotY] <= 0){
        t++;
        if (t >= nofunc){
            cout<<"This problem is unbounded!"<<endl;
            return 0;
        }
    }
    int minfrac = (*table)[t][novar]/(*table)[t][pivotY];       ///here we check and get the other coordinate of the pivot point
    int pivotX = t;
    for (int i = t; i < nofunc; i++){
        if ((*table)[i][pivotY] > 0 && minfrac > (*table)[i][novar]/(*table)[i][pivotY]){
            minfrac = (*table)[i][novar]/(*table)[i][pivotY];
            pivotX = i;
        }
    }
    (*J)[pivotX] = pivotY+1;
    (*C)[pivotX] = (*ntmin)[pivotY];
    pivot(novar, nofunc, table, pivotX, pivotY);                ///here we pivot the table
    return findDelta(novar, nofunc, C, table, ntmin, J);        ///this function then recurse and call itself, this proceed will continue until the table is optimized or unbounded
}

///__________________________________________________________________________________________________

int main(){
    cout<<"Input the number of variables, including slack: ";
    int novar;
    cin>>novar;
    
    cout<<"Input the number of function in table: ";
    int nofunc;
    cin>>nofunc;
    
    cout<<endl<<"All functions must be in canon form"<<endl<<endl;
    cout<<"When asked to input a function, you are asked to input the coefficients of all variables"<<endl<<"in the function, plus the left side b"<<endl<<endl;
    cout<<"For example, if you want to input function [10x1 -2x2 + 0x3 + x4 = 20], you must input [10 -2 0 1 20],"<<endl<<"given that there are only 4 variables including slacks in the problem"<<endl<<endl;
    
    vector<double> ntmin;
    createMinFunc(&ntmin, novar);
    
    vector<vector<double>> table;
    createTable(&table, novar, nofunc);
    
    int novar2 = novar + nofunc;
    
    vector<double> ntmin2;
    createMinFunc2(&ntmin2, novar, novar2);
    
    createPhase1Table(&table, novar2, novar, nofunc);

    vector<int> J;
    findBasic(nofunc, novar2, &ntmin2, &table, &J);
    
    vector<double> C;
    findCoef(&C, J, ntmin2, nofunc);
    
    cout<<"PHASE 1:";
    
    createTableHead(&novar2, &ntmin2);
    
    endline(novar2);
    //Phase 1
    int temp = findDelta(novar2, nofunc, &C, &table, &ntmin2, &J);
    
    if (temp == 1){
        createPhase2Table(&table, novar, novar2, nofunc, &J);
        
        findCoef(&C, J, ntmin, nofunc);
        
        nofunc = (int) J.size();
        
        cout<<endl<<endl<<"PHASE 2:";
        
        createTableHead(&novar, &ntmin);
        
        endline(novar);
        //Phase 2
        findDelta(novar, nofunc, &C, &table, &ntmin, &J);
    }
    else{
        cout<<endl<<"Phase 2 is not possible!"<<endl<<endl;
    }
}
