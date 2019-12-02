#include "calc.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <ctime>

using namespace std;

int main(){

    srand(time(0));
    // --------------Test Code for base = 10----------------

    for(int i=0; i<10; i++)  // Test 10 times
    {
        int a = rand() % 100000, b = rand() % 1000 + 1, m=2;
        Num<10> aa(a), bb(b);
        if (to_string(a+b) != (aa+bb).str())
            cout<< "a="<< a<< " b="<<b <<" a+b="<<a+b<<", but get " << (aa+bb) << endl;
        if (to_string(a>=b ? a-b :0) != (aa-bb).str())
            cout<< "a="<< a<< " b="<<b <<" a-b="<< (a>=b ? a-b :0)<<", but get " << (aa-bb) << endl;
        if (to_string(a*b) != (aa*bb).str())
            cout<< "a="<< a<< " b="<<b <<" a*b="<<a*b<<", but get " << (aa*bb) << endl;
        if (to_string(a/b) != (aa/bb).str())
            cout<< "a="<< a<< " b="<<b <<" a/b="<<a/b<<", but get " << (aa/bb) << endl;
        if (to_string(a%b) != (aa%bb).str())
            cout<< "a="<< a<< " b="<<b <<" a%b="<<a%b<<", but get " << (aa%bb) << endl;
        if (to_string(a*b/100) != sharp(aa,bb,m).str())
            cout<< "a="<< a<< " b="<<b <<" a*b/100="<<a*b/100<<", but get " << sharp(aa,bb,m) << endl;
    }


// -------------- Test codes for input file, with base = 100------

    string infilename = "fin.txt";
    string outfilename = "fout.txt";

    ifstream fin(infilename);
    ofstream fout(outfilename);

    string sop;

    Num<100> a, b;
    
    int n; 
    fin>>n;
    for(int i=0; i<n; ++i)
    {
        fin >> sop >> a >> b;
        if (sop == "<")      
            fout << a.isSmallerThan(b);
        else if(sop == "+")
            fout << (a + b);
        else if (sop == "-")
            fout << (a - b);
        else if (sop == "*")
            fout << (a * b);
        else if (sop == "/")
            fout << (a / b) << ' ' << (a % b); 
        else if (sop == "#")
        {
            int m;
            fin >> m;
            fout << sharp(a, b, m);
        }
        fout<< endl;
    }
    
    return 0;
}

