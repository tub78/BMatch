#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>
using namespace std;
#include "Vector.hpp"
#include <cmath>


// compute the inner product of two sparse vectors
double SparseVec::
dot(const SparseVec * rhs) const
{
    return dot_svec(this, rhs);
}

// compute the inner product of two sparse vectors
double SparseVec::
dot_svec(const SparseVec * lhs, const SparseVec * rhs)
{
    register SparseVecConstIterator ai(lhs->begin());
    register SparseVecConstIterator bj(rhs->begin());
    double                  sum(0.0);

    while ((ai!=lhs->end()) && (bj!=rhs->end())) {
        if (ai->first > bj->first) {
            bj++;
        }
        else if (ai->first < bj->first) {
            ai++;
        }
        else {
            sum += ai->second * bj->second;
            ai++;
            bj++;
        }
    }
    return sum;
}

/// test equal
bool SparseVec::
eq_svec(const SparseVec * lhs, const SparseVec * rhs)
{
    return !(neq_svec(lhs, rhs));
}

/// test not equal
bool SparseVec::
neq_svec(const SparseVec * lhs, const SparseVec * rhs)
{
    register SparseVecConstIterator ai(lhs->begin());
    register SparseVecConstIterator bj(rhs->begin());

    while ((ai!=lhs->end()) && (bj!=rhs->end())) {
        if (ai->first > bj->first) {
            if (bj->second != 0.0)
                return true;
            bj++;
        }
        else if (ai->first < bj->first) {
            if (ai->second != 0.0)
                return true;
            ai++;
        }
        else {
            if (ai->second != bj->second)
                return true;
            ai++;
            bj++;
        }
    }
    return false;
}

// compute the sum of this and rhs sparse vectors 
void SparseVec::
accumulate(const SparseVec * rhs, const double s)
{
    accumulate_svec(this, rhs, s);
}

void SparseVec::
accumulate_svec(SparseVec * lhs, const SparseVec * rhs, const double scale)
{
    register SparseVecConstIterator svec(rhs->begin());

    for (svec=rhs->begin(); svec!=rhs->end(); svec++) {
        (*lhs)[svec->first] += scale*svec->second;
    }
}



/* read MxN matrix file into row-major order array */
void read_mn_matrix(istream & in, int & M, int & N, DoubleVec & elements_vec)
{
    register int i;
    register int j;
    register int k;
    double      wgt;
    long            lines_limit = 100000000;
    string          line;
    stringstream    ss;
    /* */
    i = 0;
    k = 0;
    M = 0;
    N = 0;
    elements_vec.clear();
    /* */
    while ((!in.eof()) && (i<lines_limit)) 
    {
        getline(in, line); 
        i++;
        if (int(line.size())==0)    { break; } /* trailing eof ...*/
        ss.str(line);
        j = 0;
        while (ss >> wgt) 
        {
            elements_vec.push_back(wgt);
            j++;
            k++;
        }
        M++;
        N = int(max(double(N), double(j)));
        if (k!=(N*M)) {
            cerr << "read_mn_matrix!> detected ragged matrix" << endl;
            M = 0;
            N = 0;
            elements_vec.clear();
            return;
        }
        ss.clear();
        //M = N*N;
        //elements_vec.reserve(2*i*N);
    }
    if (i==lines_limit) {
        cerr << "read_mn_matrix!> lines >= limit(" << lines_limit << ")" <<endl;
        M = 0;
        N = 0;
        elements_vec.clear();
        return;
    }
}
/* end read_mn_matrix */

/* write matrix */
void write_mn_matrix(ostream & out, const int & M, const int & N, const DoubleVec & elements_vec, const int & width)
{
    register int i;
    register int j;
    out.setf(ios::right);
    for (i=0; i<M; i++) {
        for (j=0; j<N; j++) {
            out << " " << setw(width) << elements_vec[N*i + j];
        }
        out << endl;
    }
}
/* end write_mn_matrix */



/* make 0/1 */
void logical_svecvec(SparseVecVec & spmat)
{
    register unsigned int i;
    SparseVecIterator sp;

    for (i=0; i<spmat.size(); i++) 
    {
        for (sp=spmat[i]->begin(); sp!=spmat[i]->end(); sp++) 
        {
            if (sp->second!=0.0) 
            {
                sp->second = 1.0;
            }
        }
    }
}
/* end logical_svecvec */


/* delete_svecvec */
void delete_svecvec(SparseVecVec & spmat)
{
    register int i;
    for (i=0; i<int(spmat.size()); i++) {
        if (spmat[i]!=0) {
            delete (spmat[i]);
        }
    }
    spmat.clear();
}

/* create sparse matrix from ijw */ 
/* if duplicate weights specified, they are added */
void ijw_2_svecvec(const int & num_ijw, const DoubleVec & ijw_vec, const int & num_node, SparseVecVec & spmat)
{
    register int i;
    register int j;
    register int k;
    delete_svecvec(spmat);
    spmat.clear();
    for (i=0; i<num_node; i++) {
        spmat.push_back(new SparseVec());
    }
    /* matching: sparse adjacency */
    for (k=0; k<num_ijw; k++) 
    {
        i = int(ijw_vec[3*k + 0]);
        j = int(ijw_vec[3*k + 1]);
        (*spmat[i])[j] += ijw_vec[3*k + 2];
    }
}
/* end ijw_2_svecvec */

/* create symmetric sparse matrix from ijw
 * by first calling ijw_add_upper_tri_transpose 
 * if duplicate weights specified, they are added */
void ijw_2_svecvec_symm(const int & num_ijw, const DoubleVec & ijw_vec, const int & num_node, SparseVecVec & spmat)
{
    int L_num_ijw = num_ijw;
    DoubleVec L_ijw_vec = ijw_vec;
    int L_num_node = num_node;

    ijw_add_upper_tri_transpose(L_num_ijw, L_ijw_vec, 1);
    ijw_2_svecvec(L_num_ijw, L_ijw_vec, L_num_node, spmat);
}
/* end ijw_2_svecvec_sym */


/* extract ijw from sparse matrix */
void svecvec_2_ijw(const int & num_node, const SparseVecVec & spmat, int & num_ijw, DoubleVec & ijw_vec)
{
    register int i;
    register int j;
    register double wgt;
    SparseVecIterator sp;

    ijw_vec.clear();
    ijw_vec.reserve(num_ijw);

    for (i=0; i<num_node; i++) 
    {
        for (sp=spmat[i]->begin(); sp!=spmat[i]->end(); sp++) 
        {
            j = sp->first;
            wgt = sp->second;
            if (wgt!=0)
            {
                ijw_vec.push_back(i);
                ijw_vec.push_back(j);
                ijw_vec.push_back(wgt);
            }
        }
    }
    num_ijw = int(ijw_vec.size()/3);
}
/* end svecvec_2_ijw */











/* convert ijw format to dense matrix in row-major format
 *
 * if duplicate weights specified, they are added
 * */
void ijw_2_matrix(const int & num_ijw, const DoubleVec & ijw_vec, const int & M, const int & N, DoubleVec & elements_vec)
{
    register int i;
    register int j;
    register int k;
    register int kkk;
    register double wgt;
    elements_vec.clear();
    elements_vec.resize(M*N, 0.0);
    for (kkk=0; kkk<num_ijw; kkk++)
    {
        i   = int(ijw_vec[3*kkk + 0]);
        j   = int(ijw_vec[3*kkk + 1]);
        wgt = ijw_vec[3*kkk + 2];
        k   = i*N + j;
        elements_vec[k] += wgt;
    }
}
/* end ijw_2_matrix */

/* convert dense matrix in row-major format to ijw format */
void matrix_2_ijw(const int & M, const int & N, const DoubleVec & elements_vec, int & num_ijw, DoubleVec & ijw_vec)
{
    register int i;
    register int j;
    register int k;
    num_ijw = M*N;
    ijw_vec.clear();
    ijw_vec.reserve(3*num_ijw);
    k = 0;
    for (i=0; i<M; i++) 
    {
        for (j=0; j<M; j++)
        {
            ijw_vec.push_back(i);
            ijw_vec.push_back(j);
            ijw_vec.push_back(elements_vec[k]);
            k++;
        }
    }
}
/* end matrix_2_ijw */


/* read MxN matrix file and convert to ijw using row-major order */
void ijw_read_matrix(istream & in, int & num_ijw, DoubleVec & ijw_vec)
{
    DoubleVec elements_vec;
    int M;
    int N;
    ijw_vec.clear();
    num_ijw = 0;
    /* */
    read_mn_matrix(in, M, N, elements_vec);
    if (M!=N)
    {
        cerr << "ijw_read_matrix!> matrix not square" << endl;
        return;
    }
    matrix_2_ijw(M, N, elements_vec, num_ijw, ijw_vec);
}
/* end ijw_read_matrix */


/* read IJW graph assuming 0-based indices */
void ijw_read_ijw(istream & in, int & num_ijw, DoubleVec & ijw_vec)
{
    int N;
    read_mn_matrix(in, num_ijw, N, ijw_vec);
    if (N!=3)
    {
        cerr << "ijw_read_ijw!> matrix must be N x 3" << endl;
        num_ijw = 0;
        ijw_vec.clear();
        return;
    }
}
/* ijw_read_ijw */


/* Write graph adjacency matrix assuming complete row-major 
 * listing of elements */
void ijw_write_matrix(ostream & out, const int & num_node, const int & num_ijw, const DoubleVec & ijw_vec, const int & width)
{
    DoubleVec elements_vec;
    ijw_2_matrix(num_ijw, ijw_vec, num_node, num_node, elements_vec);
    write_mn_matrix(out, num_node, num_node, elements_vec, 4);
}


/* Write graph ijw listing of elements */
void ijw_write_ijw(ostream & out, const int & num_ijw, const DoubleVec & ijw_vec, const int & width)
{
    write_mn_matrix(out, num_ijw, 3, ijw_vec, 4);
}

/* add transpose */ 
void ijw_add_upper_tri_transpose(int & num_ijw, DoubleVec & ijw_vec, const int & exclude_diagonal)
{
    int         numTT = num_ijw;
    DoubleVec   TT = ijw_vec;
    ijw_upper_tri(numTT,TT,exclude_diagonal);
    ijw_transpose(numTT,TT);
    ijw_vec.insert(ijw_vec.end(), TT.begin(), TT.end());
    num_ijw = num_ijw + numTT;
}
/* ijw_add_upper_tri_transpose */

/* keep only upper triangle with option to exclude diagonal */ 
void ijw_upper_tri(int & num_ijw, DoubleVec & ijw_vec, int exclude_diagonal)
{
    register int i;
    register int j;
    register double wgt;
    DoubleVecIterator dv0;
    DoubleVecIterator dv(ijw_vec.begin());
    while (dv!=ijw_vec.end())
    {
        dv0 = dv;
        i   = int(*dv++);
        j   = int(*dv++);
        wgt = *dv++;
        if ( (i>j) || (exclude_diagonal&&(i==j)) )
        {
            dv = ijw_vec.erase(dv0,dv);
        }
    }
    num_ijw = int(ijw_vec.size()/3);
}
/* end ijw_upper_tri */

/* filter ijw array keep only non-zeros */ 
void ijw_nnz(int & num_ijw, DoubleVec & ijw_vec)
{
    register int i;
    register int j;
    register double wgt;
    DoubleVecIterator dv0;
    DoubleVecIterator dv(ijw_vec.begin());
    while (dv!=ijw_vec.end())
    {
        dv0 = dv;
        i   = int(*dv++);
        j   = int(*dv++);
        wgt = *dv++;
        if (wgt==0)
        {
            dv = ijw_vec.erase(dv0,dv);
        }
    }
    num_ijw = int(ijw_vec.size()/3);
}
/* end ijw_nnz */


/* transpose ijw array */
void ijw_transpose(int & num_ijw, DoubleVec & ijw_vec)
{
    register double tmp;
    DoubleVecIterator dv(ijw_vec.begin());
    while (dv!=ijw_vec.end())
    {
        tmp   = dv[0];
        dv[0] = dv[1];
        dv[1] = tmp;
        dv = dv + 3;
    }
}
/* end ijw_transpose */



