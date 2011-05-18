#ifndef _PARAMETER_
#define _PARAMETER_

#include <string>
#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
using namespace std;

#include "method_names.hpp"


/*
 * defines / parses parameters 
 *
 * change as appropriate at the lines identified by the tag: "EDIT HERE"
 */
class Parameter {
public:
    /// constructor
    Parameter() { init_0(); }
    /// initialize with string 
    void init(const string & cmdline);
    /// initialize with cmd-line args
    void init(int argc, char* argv[]);
    //void init(int argc, const char* argv[]);
    /// defaults
    void init_0();
    /// check
    void check_parameters();
    /// print help
    void print_help();
    /// print
    void print();


    /// for parsing
    typedef char* pchar;
    /// controls amount of debug output
    int verbosity()  { return 1; };
    /// "deal" args to argc/argv
    void getArgs(const string & cmdline, int & argc, char **& argv);
    /// free argc/argv
    void freeArgs(int & argc, char **& argv);
    /// error msg
    void unrecognizedOptionError(const char * opt_str) ;
    /// to create argv
    void *my_malloc(long size);


    /*
     * EDIT HERE
     */

    string  weights;
    string  degrees;
    string  output;
    int     const_u;
    int     const_l;
    int     sparse;
    int     method;
    int     verbose;

};

inline void Parameter::
init_0()
{
    /*
     * EDIT HERE
     */
    weights     = string("");
    degrees     = string("");
    output      = string("");
    const_u     = 0;
    const_l     = 0;
    sparse      = 0;
    method      = 0;
    verbose     = 0;
}



// initialize with string 
inline void Parameter::
init(const string & cmdline)
{ 
    int argc; 
    char ** argv; 
    getArgs(cmdline, argc, argv); 
    init(argc,argv); 
    freeArgs(argc, argv); 
}

// initialize data-set with cmd-line args
inline void Parameter::
init(int argc, char* argv[])
//init(int argc, const char* argv[])
{
    long i;
    for(i=1;(i<argc) && ((argv[i])[0] == '-');i++) {


        // cerr << "[[" << argv[i] << "]]" << endl;

        if ((argv[i][1]=='?')||(argv[i][1]=='-')) {
            print_help();
            break;
        }
        if((i+1)>=argc) {
            // cerr << ".. Not enough input parameters!" << endl;
            print_help();
            break;
        }

        /*
         * EDIT HERE
         */

        if (string("-weights")==argv[i]) {
            weights = string(argv[++i]); continue;
        }
        if (string("-w")==argv[i]) {
            weights = string(argv[++i]); continue;
        }
        if (string("-degrees")==argv[i]) {
            degrees = string(argv[++i]); continue;
        }
        if (string("-d")==argv[i]) {
            degrees = string(argv[++i]); continue;
        }
        if (string("-output")==argv[i]) {
            output = string(argv[++i]); continue;
        }
        if (string("-o")==argv[i]) {
            output = string(argv[++i]); continue;
        }
        if (string("-const_u")==argv[i]) {
            const_u = atoi(argv[++i]); continue;
        }
        if (string("-u")==argv[i]) {
            const_u = atoi(argv[++i]); continue;
        }
        if (string("-const_l")==argv[i]) {
            const_l = atoi(argv[++i]); continue;
        }
        if (string("-l")==argv[i]) {
            const_l = atoi(argv[++i]); continue;
        }
        if (string("-sparse")==argv[i]) {
            sparse = atoi(argv[++i]); continue;
        }
        if (string("-s")==argv[i]) {
            sparse = atoi(argv[++i]); continue;
        }
        if (string("-method")==argv[i]) {
            method = atoi(argv[++i]); continue;
        }
        if (string("-m")==argv[i]) {
            method = atoi(argv[++i]); continue;
        }
        if (string("-verbose")==argv[i]) {
            verbose = atoi(argv[++i]); continue;
        }
        if (string("-v")==argv[i]) {
            verbose = atoi(argv[++i]); continue;
        }

        unrecognizedOptionError(argv[i++]);
    }

    check_parameters();
}


inline void Parameter::
print_help()
{
    int W(7);
    /*
     * EDIT HERE
     */
    cerr << "Usage> bmatch [-arg1 value] [-arg2 value] ..." << endl << endl;
    cerr << "Solves:" << endl;
    cerr << setw(W) << " " << "optimize_Yij sum_ij W_ij Y_ij  s.t. l_i <= sum_j Y_ij <= u_i, 1 <= i <= n" << endl;
    cerr << setw(W) << " " << "and Wij and Yij are symmetric" << endl << endl;
    cerr << "Arguments [with default values]:" << endl << endl;
    /* */
    print();

    /* */
    cerr << endl << "Method: " << endl << endl;
    cerr << setw(W) << "1. " << COMPLEMENTARYGOBLINEXACTBMATCH << endl;
    cerr << setw(W) << "2. " << GOBLINEXACTBMATCH << endl;
    cerr << setw(W) << "3. " << GREEDYAPPROXBMATCH << endl;
    cerr << setw(W) << "4. " << RECURSIVEGREEDYAPPROXBMATCH << endl;
    cerr << setw(W) << "5. " << BELIEFPROPBMATCH << endl;

    cerr << "           " << endl;
    cerr << " Example 4:" << endl;
    cerr << "bmatch -weights ""data/ijw_in_5.txt"" -degrees ""data/degree_in_5.txt"" -output ""data/ijw_out_5.txt"" -sparse 1 -verbose 1 -method 4" << endl;
    cerr << "bmatch -weights ""data/ijw_in_5.txt"" -degrees ""data/degree_in_5.txt"" -output ""data/ijw_out_5.txt"" -sparse 1 -verbose 1 -method 1" << endl;

    cerr << "           " << endl;
    cerr << " Example 3:" << endl;
    cerr << "bmatch -w ""data/ijw_in_3.txt"" -d ""data/degree_in_3.txt"" -s 1 -v 2 -m 1" << endl;

    cerr << "           " << endl;
    cerr << " Example 2:" << endl;
    cerr << "bmatch -w ""data/matrix_in_2.txt"" -u 2 -v 1 -m 3" << endl;

    cerr << "           " << endl;
    cerr << " Example 1:" << endl;
    cerr << "bmatch -w ""data/matrix_in_1.txt"" -u 2 -v 1" << endl;

    cerr << "           " << endl;
    cerr << " Example 0:" << endl;
    cerr << "bmatch " << endl;


    cerr << endl << "Notes: " << endl << endl;
    cerr << setw(W) << " ";
    cerr << "Self-loops are handled: a self-loop increases the degree of a node" << endl;
    cerr << setw(W) << " ";
    cerr << "by 2.  Weights must be positive (Wij >= 0).  Edges with zero-weight" << endl;
    cerr << setw(W) << " ";
    cerr << "(Wij==0) are not allowed to partake in the matching." << endl << endl;

    cerr << setw(W) << " ";
    cerr << "If the weights, degrees or output file is not specified, then the" << endl;
    cerr << setw(W) << " ";
    cerr << "corresponding input / output data is entered in / displayed to the" << endl;
    cerr << setw(W) << " ";
    cerr << "display.  The format should match the examples given in the data" << endl;
    cerr << setw(W) << " ";
    cerr << "directory.  Enter a blank line to terminate input of a given data." << endl << endl;

    cerr << setw(W) << " ";
    cerr << "The output weight is calculated as sum_{i<=j} Wij Yij, except when" << endl;
    cerr << setw(W) << " ";
    cerr << "belief propagation is used, whose output is not necessarily" << endl;
    cerr << setw(W) << " ";
    cerr << "symmetric.  For BP, the output weight is 0.5 * sum_{i,j} Wij Yij." << endl << endl;

    cerr << setw(W) << " ";
    cerr << "The lower bounds are ignored by methods 3-5. Bipartite relaxation" << endl;
    cerr << setw(W) << " ";
    cerr << "assumes that the degree upper bounds can be met with equality." << endl << endl;
    /*
    cerr << setw(W) << " ";
    cerr << "The reduction of method 2 may not always yield the optimal matching," << endl;
    cerr << setw(W) << " ";
    cerr << "and sometimes may not satisfy the upper bounds.  This method was used" << endl;
    cerr << setw(W) << " ";
    cerr << "before we discovered the more accurate reduction of method 1." << endl << endl;
    */

    exit(1);
}

inline void Parameter::
print() 
{
    int W(25);
    int Warg(4);
    using namespace std;

    cerr.setf(ios::right);

    /*
     * EDIT HERE
     */

    cerr << setw(W)  << "-w -weights ["  << left << setw(Warg) << weights << right << "] input file, NULL => std. input" << endl;
    cerr << setw(W)  << "-d -degrees ["  << left << setw(Warg) << degrees << right << "] input file, NULL => std. input" << endl;
    cerr << setw(W)  << "-o -output  ["  << left << setw(Warg) << output  << right << "] output file, NULL => std. output" << endl;
    cerr << setw(W)  << "-l -const_l ["  << left << setw(Warg) << const_l << right << "] positive integer, negative => std. input" << endl;
    cerr << setw(W)  << "-u -const_u ["  << left << setw(Warg) << const_u << right << "] positive integer, negative => std. input" << endl;
    cerr << setw(W)  << "-s -sparse  ["  << left << setw(Warg) << sparse  << right << "] 0 => matrix, 1 => IJW input format" << endl;
    cerr << setw(W)  << "-m -method  ["  << left << setw(Warg) << method  << right << "] selects algorithm" << endl;
    cerr << setw(W)  << "-v -verbose ["  << left << setw(Warg) << verbose << right << "] positive integer" << endl;

    cerr.unsetf(ios::right);
}



inline void Parameter::
check_parameters()
{
}


inline void Parameter::
getArgs(const string & cmdline, int & argc, char **& argv)
{
    vector<string> arg_vec;
    arg_vec.push_back("getArgs");
    char * arg = new char[512];
    const char * pcmd = cmdline.c_str();
    int len;
    // init arg_vec;
    while ((sscanf(pcmd, "%s", arg)!=EOF)) {
        len = strlen(arg);
        // skip past id/label
        while(pcmd && (pcmd[0]==' '))   pcmd++;
        while(pcmd && (pcmd[0]>' '))    pcmd++;
        if (len <= 0) {
            cerr << "Error - missing arg" << endl;
            exit(1);
        }
        arg_vec.push_back(string(arg));
    }
    argc = arg_vec.size();
    argv = (char **)my_malloc(sizeof(pchar)*(argc));
    int i(0);
    vector<string>::iterator    arg_ii;
    if (verbosity()>=3)         cerr << "argv[] = ";
    for(arg_ii=arg_vec.begin();arg_ii!=arg_vec.end();(arg_ii++,i++)) {
        argv[i] = strdup((*arg_ii).c_str());
        if (verbosity()>=3)     cerr << " " << argv[i];
    }
    if (verbosity()>=3)         cerr << endl;
    delete[] arg;
}



inline void Parameter::
freeArgs(int & argc, char **& argv)
{
    int i;
    for(i=0;i<argc;i++) {
        free(argv[i]);
    }
    free(argv);
}

inline void Parameter::
unrecognizedOptionError(const char * opt_str)
{
    if (verbosity()>=1)         
        cerr << ".. Unrecognized option! > " << opt_str << endl;  
    else
        cerr << ">";
}

inline void * Parameter::
my_malloc(long size)
{
    void *ptr;
    ptr=(void *)malloc(size);
    if(!ptr) { 
        cerr << "Out of memory!" << endl;
        exit (1); 
    }
    return(ptr);
}


#endif

