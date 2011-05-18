#include <vector>
#include <map>
#include <iostream>
#include <string>
#include <iterator>
//#include <utility>
using namespace std;

#ifndef _VECTOR_
#define _VECTOR_

#define INF 1e30
//#define INF HUGE_VAL



/* */
class SparseVec : public map<long, double, less<long> > {
public:
    // virtual ~SparseVec() { clear(); };

    /// compute the inner product of two Instances
    double dot(const SparseVec * rhs) const;
    /// compute the inner product of two sparse vectors
    static double dot_svec(const SparseVec * lhs, const SparseVec * rhs);
    /// test equal
    static bool eq_svec(const SparseVec * lhs, const SparseVec * rhs);
    /// test not equal
    static bool neq_svec(const SparseVec * lhs, const SparseVec * rhs);

    /// accumulate features
    void accumulate(const SparseVec * rhs, const double scale = 1.0);
    /// compute the inner product of two sparse vectors
    static void accumulate_svec(SparseVec * lhs, const SparseVec * rhs, const double scale = 1.0);

};

typedef SparseVec::value_type           SparseVecElement;

typedef SparseVec::iterator             SparseVecIterator;
typedef SparseVec::const_iterator       SparseVecConstIterator;

typedef vector<SparseVec *>             SparseVecVec;
typedef SparseVecVec::const_iterator    SparseVecVecConstIterator;
typedef SparseVecVec::iterator          SparseVecVecIterator;


typedef vector<long>                    LongIntVec;
typedef vector<double>                  DoubleVec;
typedef vector<long>::iterator          LongIntVecIterator;
typedef vector<long>::const_iterator    LongIntVecConstIterator;
typedef vector<double>::iterator        DoubleVecIterator;
typedef vector<double>::const_iterator  DoubleVecConstIterator;

template <class T> inline double sumVec(const vector<T> & x) 
{ 
    double  V(0.0);
    register typename vector<T>::const_iterator     aa(x.begin());
    for(; aa!=x.end(); aa++) {
        V += (*aa);
    }
    return V;
}

template <class T> inline vector<T> diffVec(const vector<T> & x, const vector<T> & y) 
{
    vector<T>               VEC;
    register typename vector<T>::const_iterator     aa(x.begin());
    register typename vector<T>::const_iterator     bb(y.begin());
    if (x.size() != y.size())
        return VEC;
    for(; aa!=x.end(); (aa++, bb++))
        VEC.push_back(*aa-*bb);
    return VEC;
}

template <class T> inline vector<T> sqrVec(const vector<T> & x) 
{
    vector<T>   VEC;
    register typename vector<T>::const_iterator     aa(x.begin());
    for(; aa!=x.end(); aa++) {
        VEC.push_back((*aa)*(*aa));
    }
    return VEC;
}

template <class T> inline double minVec(const vector<T> & x) 
{ 
    register typename vector<T>::const_iterator     aa(x.begin());
    double  V(*aa);
    for(aa++; aa!=x.end(); aa++) {
        if (V > (*aa)) 
            V = (*aa);
    }
    return V;
}

template <class T> inline double maxVec(const vector<T> & x) 
{ 
    register typename vector<T>::const_iterator     aa(x.begin());
    double  V(*aa);
    for(aa++; aa!=x.end(); aa++) {
        if (V < (*aa)) 
            V = (*aa);
    }
    return V;
}

template <class T> inline double normSqrVec(const vector<T> & x) 
{ 
    double  V(0.0);
    register typename vector<T>::const_iterator     aa(x.begin());
    for(; aa!=x.end(); aa++) {
        V += (double)(*aa)*(*aa);
    }
    return V;
}

template <class T> inline double meanVec(const vector<T> & x) 
{
    return sumVec(x)/(double)x.size();
}

template <class T> inline double varVec(const vector<T> & x, const double & mean) 
{
    return meanVec(sqrVec(diffVec(x, vector<T>(x.size(), mean))));
}

template <class T> inline vector<T> findVec(const vector<T> & x) 
{
    vector<T>   VEC;
    for(int ll = 0; ll < x.size(); ll++) 
        if (x[ll]) VEC.push_back(ll);
    return VEC;
}

template <class T> inline void pReleaseVec(vector<T> & x) 
{
    register typename vector<T>::iterator     aa(x.begin());
    for(; aa!=x.end(); aa++) {
        *aa = 0;
    }
    x.clear();
}


template <class T> inline void pDeleteVec(vector<T> & x) 
{
    register typename vector<T>::iterator     aa(x.begin());
    for(; aa!=x.end(); aa++) {
        delete (*aa);
        *aa = 0;
    }
}

template <class T> inline void printVec(ostream & os, const vector<T> & x, const char * sep = " ")
{
    copy(x.begin(), x.end(), ostream_iterator<T>(os, sep));
}




/* read MxN matrix file into row-major ordered array */
void read_mn_matrix(istream & in, int & M, int & N, DoubleVec & elements_vec);

/* write matrix from row-major ordered array */
void write_mn_matrix(ostream & out, const int & M, const int & N, const DoubleVec & elements_vec, const int & width);




/* make 0/1 */
void logical_svecvec(SparseVecVec & spmat);

/* delete */
void delete_svecvec(SparseVecVec & spmat);

/* sparse matrix from ijw, if duplicate weights specified, they are added */
void ijw_2_svecvec(const int & num_ijw, const DoubleVec & ijw_vec, const int & num_node, SparseVecVec & spmat);

/* sparse symm matrix from ijw, if duplicate weights specified, they are added*/
void ijw_2_svecvec_symm(const int & num_ijw, const DoubleVec & ijw_vec, const int & num_node, SparseVecVec & spmat);

/* extract ijw from sparse matrix */
void svecvec_2_ijw(const int & num_node, const SparseVecVec & spmat, int & num_ijw, DoubleVec & ijw_vec);










/* convert ijw format to dense matrix in row-major format
 *
 * if duplicate weights specified, they are added
 * */
void ijw_2_matrix(const int & num_ijw, const DoubleVec & ijw_vec, const int & M, const int & N, DoubleVec & elements_vec);

/* convert dense matrix in row-major format to ijw format */
void matrix_2_ijw(const int & M, const int & N, const DoubleVec & elements_vec, int & num_ijw, DoubleVec & ijw_vec);


/* read MxN matrix file and convert to ijw using row-major order */
void ijw_read_matrix(istream & in, int & num_ijw, DoubleVec & ijw_vec);


/* read IJW graph assuming 0-based indices */
void ijw_read_ijw(istream & in, int & num_ijw, DoubleVec & ijw_vec);


/* Write graph adjacency matrix assuming complete row-major 
 * listing of elements */
void ijw_write_matrix(ostream & out, const int & num_node, const int & num_ijw, const DoubleVec & ijw_vec, const int & width);


/* Write graph ijw listing of elements */
void ijw_write_ijw(ostream & out, const int & num_ijw, const DoubleVec & ijw_vec, const int & width);


/* add transpose */ 
void ijw_add_upper_tri_transpose(int & num_ijw, DoubleVec & ijw_vec, const int & exclude_diagonal);

/* keep only non-zeros from upper triangle with option to exclude diagonal */ 
void ijw_upper_tri(int & num_ijw, DoubleVec & ijw_vec, int exclude_diagonal);

/* keep only non-zeros */ 
void ijw_nnz(int & num_ijw, DoubleVec & ijw_vec);

/* transpose ijw array */
void ijw_transpose(int & num_ijw, DoubleVec & ijw_vec);



#endif


