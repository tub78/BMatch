/*
 * B-matching toolbox
 * Stu Andrews
 * January 2008
 * */
#include <cmath>
#include "greedyapprox.hpp"

GreedyApproxBmatch::
~GreedyApproxBmatch()
{
    //cerr << "GreedyApproxBmatch destructor> " << endl;

    delete_svecvec(Amat);
    //cerr << " .. destructor> done Amat" << endl; cerr.flush();

    delete_svecvec(Wmat);
    //cerr << " .. destructor> done Wmat" << endl; cerr.flush();

    bcap_vec.clear();
    //cerr << " .. destructor> done bcap_vec" << endl;cerr.flush();

    deg_vec.clear();
    //cerr << " .. destructor> done deg_vec" << endl; cerr.flush();

    //cerr << " .. destructor> B_match "<< B_match.size() << endl; cerr.flush();
    B_match.clear();
    //cerr << " .. destructor> done B_match" << endl; cerr.flush();

    //cerr << " .. destructor> M_walk "<< M_walk.size() << endl; cerr.flush();
    M_walk.clear();
    //cerr << " .. destructor> done M_walk" << endl; cerr.flush();

    num_node = 0;
    num_ijw = 0;
}



/* print memory */
void GreedyApproxBmatch::
print(const int & print_what)
{
    register int i;
    register int j;
    SparseVecConstIterator  svec;
    EdgeList::iterator np;

    /* */
    if (verbose==0) 
    {
        return;
    }

    if (print_what==0) {
        cerr << "Amat>" << endl;
        for (i=0; i<num_node; i++)
        {
            cerr << "  num_edge=" << Amat[i]->size();
            cerr << "  deg=" << deg_vec[i];
            cerr << "  cap=" << bcap_vec[i];
            /* */
            cerr << "  i=" << i ;
            cerr << "  j:w=[";
            j = 0;
            for(svec=Amat[i]->begin(); svec!=Amat[i]->end(); svec++) {
                if (++j > 50) break;
                cerr << " " << svec->first << ":" << svec->second << " "; 
            }
            if (svec!=Amat[i]->end())   cerr << "... ";
            cerr << "]" << endl;
        }
    }

    if (print_what==1) {
        cerr << "Wmat>" << endl;
        for (i=0; i<num_node; i++)
        {
            cerr << "  num_edge=" << Wmat[i]->size();
            cerr << "  deg=" << deg_vec[i];
            cerr << "  cap=" << bcap_vec[i];
            /* */
            cerr << "  i=" << i ;
            cerr << "  j:w=[";
            j = 0;
            for(svec=Wmat[i]->begin(); svec!=Wmat[i]->end(); svec++) {
                if (++j > 50) break;
                cerr << " " << svec->first << ":" << svec->second << " "; 
            }
            if (svec!=Wmat[i]->end())    cerr << "... ";
            cerr << "]" << endl;
        }
    }


    if (print_what==3) {
        cerr << "M_walk> = [";
        for (np=M_walk.begin(); np!=M_walk.end(); np++)
        {
            cerr << " " << np->first << "," << np->second << " ";
        }
        cerr << "]" << endl;
    }

    if (print_what==4) {
        cerr << "B_match> = [";
        for (np=B_match.begin(); np!=B_match.end(); np++)
        {
            cerr << " " << np->first << "," << np->second << " ";
        }
        cerr << "]" << endl;
    }
}
/* end print */


/* initialize assuming 0-based indices for nodes */
void GreedyApproxBmatch::
init(const int & in_verbose, const int& in_num_node, const DoubleVec & deg_bdd_vec, const int & in_num_ijw, const DoubleVec & ijw_vec) 
{
    register int i;

    register int u;
    register int v;
    register double wgt;
    wgt = 0;

    /* num nodes */
    num_node = in_num_node;
    /* num ijw */
    num_ijw = in_num_ijw;

    /* vector of node degree bounds */
    bcap_vec = LongIntVec(num_node, 0);

    /* vector of node degrees */
    deg_vec = LongIntVec(num_node, 0);

    /* b-matchings */
    /* B_match; */

    /* */
    verbose = in_verbose;

    /*
     * num_node = size(deg_bdd,1);
     * num_node = max(max(ijw_in([1,2],:)));
     * */

    /* ... sparse adj and wgt matrix and make symmetric
     *
     * Amat = sparse(ijw_in(1,:), ijw_in(2,:), 1.0, num_node, num_node);
     * Wmat = sparse(ijw_in(1,:), ijw_in(2,:), ijw_in(3,:), num_node, num_node);
     * Amat = Amat | Amat';
     * Wmat = Wmat + Wmat';
     *
     * ... correction for diagonals 
     *
     * Wmat = Wmat - 0.5*diag(diag(Wmat));
     * 
     * ... degrees in graph
     *
     * deg_vec = full(sum(Amat) + diag(Amat)');
     * */

    ijw_2_svecvec_symm(num_ijw, ijw_vec, num_node, Amat);
    ijw_2_svecvec_symm(num_ijw, ijw_vec, num_node, Wmat);
    logical_svecvec(Amat);

    /* get degrees w/ double counting for self-loops */
    for (i=0; i<num_ijw; i++) 
    {
        u   = int(ijw_vec[3*i + 0]);
        v   = int(ijw_vec[3*i + 1]);
        deg_vec[u] += 1;
        deg_vec[v] += 1;
    }


    /* ... node degree UPPER BOUNDS 
     *
     *  bcap_vec = deg_bdd(:,2)';
     * */
    /* N.B. index the SECOND COLUMN of deg_bdd_vec */
    for (i=0; i<num_node; i++) {
        bcap_vec[i] = int(deg_bdd_vec[2*i+1]);
    }

    /* ... b-matchings (1- heavy / 2- light halves) with rows = (uu,vv,wgt)
     *
     * B_match = [];
     * M_walk = [];
     *  */
    B_match.clear();
    M_walk.clear();


    /* print */
    print(1); /* Wmat */
    print(0); /* Amat */
    print(3); /* walk */
    print(4); /* bmatch */
}
/* end init */


/* solve */
int GreedyApproxBmatch::
solve() 
{
    register unsigned int i;
    int     i_start_at;

    int     u;
    int     v;
    int     u_nbr;
    int     deg_v;
    double  max_wgt;

    SparseVecConstIterator  sv;
    SparseVec *             nbrs_svec;
    SparseVec *             wgt_svec;

    /* EdgeList    M_walk; */
    double      M1_weight;
    double      M2_weight;

    /* ... while there are nodes with degree capacity */
    while (1) {

        /* pick first node with pos. degree and pos. degree capacity bval 
         *  uu_node = find(bcap_vec&deg_vec,1);
         * */
        for (u=0; u<num_node; u++)
        {
            if ((bcap_vec[u]>0) && (deg_vec[u]>0)) 
            {
                break;
            }
        }

        /*  if isempty(uu_node)
         *      break;
         *  end
         * */
        if (u==num_node) 
        {
            break;
        }

        /* ... find walk
         * M_walk = [];
         * */
        M_walk.clear();
        
        /* ... walk */
        while (1) 
        {
            /* */
            nbrs_svec = Amat[u];
            wgt_svec = Wmat[u];

            /* 
             *  bcap_vec(uu_node) = bcap_vec(uu_node) - 1;
             * */
            bcap_vec[u] -= 1;
            
            /*
             * rm self-loop if insufficient capacity remaining 
             *  if bcap_vec(uu_node)==0
             *      Amat(uu_node,uu_node) = 0.0;
             *  end
             * */
            /* test u's existence in nbrs_svec w/o accidentally adding (u,0) */
            if ((bcap_vec[u]==0) && (nbrs_svec->find(u)!=nbrs_svec->end()))
            {
                nbrs_svec->erase(u);
                deg_vec[u] = int(max(0.0,deg_vec[u]-2.0));
            }
            
            /*
             * ... stop if no more nbrs
             * if deg_vec(uu_node)==0
             *     break;
             * end
             * */
            if (deg_vec[u]==0) 
            {
                break;
            }
            
            /* 
             * ... get heaviest edge (u,v) 
             * I_uu_nbrs = find(Amat(:,uu_node));
             * [max_wgt, vv_node] = max(Wmat(I_uu_nbrs,uu_node));
             * vv_node = I_uu_nbrs(vv_node);
             * */
            sv      = nbrs_svec->begin();
            v       = sv->first;
            max_wgt = (*wgt_svec)[v];
            /* */
            for (sv++; sv!=nbrs_svec->end(); sv++) 
            {
                u_nbr = sv->first;
                if ((*wgt_svec)[u_nbr]>max_wgt) {
                    v = u_nbr;
                    max_wgt = (*wgt_svec)[u_nbr];
                }
            }
            
            /*
             * ... remove (u,v) from G 
             * Amat(vv_node,uu_node) = 0.0;
             * Amat(uu_node,vv_node) = 0.0;
             * */
            Amat[u]->erase(v);
            Amat[v]->erase(u);
            
            /*
             * ... decrease degrees of uu, vv 
             * deg_vec(uu_node) = deg_vec(uu_node) - 1;
             * deg_vec(vv_node) = deg_vec(vv_node) - 1;
             * */
            deg_vec[u] -= 1;
            deg_vec[v] -= 1;
            
            
            /* 
             * if degree capacity is met, remove all nbrs 
             *  if bcap_vec(uu_node)==0
             * */
            if (bcap_vec[u]==0) 
            {
                /*
                 * ... remove (xx, uu) for all xx nbrs of uu 
                 * Amat(I_uu_nbrs,uu_node) = 0.0;
                 * 
                 * ... decrease degrees of all xx nbrs of uu 
                 * deg_vec(I_uu_nbrs) = deg_vec(I_uu_nbrs) - 1;
                 * */
                for (sv=nbrs_svec->begin(); sv!=nbrs_svec->end(); sv++) 
                {
                    u_nbr = sv->first;
                    Amat[u_nbr]->erase(u);
                    deg_vec[u_nbr] -= 1;
                }
                /* 
                 * ... increment vv degree because decremented twice (matlab)
                 * deg_vec(vv_node) = deg_vec(vv_node) + 1;
                 * */
                /* ... not needed b/c deg_vec(v) is only decremented once (cpp)
                 * deg_vec[v] += 1;
                 * */

                /*
                 * ... remove (uu,xx) for all xx nbrs of uu 
                 * Amat(uu_node,I_uu_nbrs) = 0.0;
                 * */
                nbrs_svec->clear();
                /*
                 * ... ensure that uu_node degree is zero 
                 * (even when uu_node==vv_node) 
                 *  deg_vec(uu_node) = 0.0;
                 * */
                deg_vec[u] = 0;
                /* 
                 * testing ... remove for efficiency later 
                 *  assert(all(deg_vec==[sum(Amat) + diag(Amat)']));
                 * */
                /* check existence w/o accidentally adding element */
                /* test v's existence in Amat[v] w/o accidentally adding (v,0)*/
                deg_v = Amat[v]->size();
                if (Amat[v]->find(v)!=Amat[v]->end())
                {
                    deg_v += 1;
                }
                if (deg_vec[v]!=deg_v) 
                {
                    cerr << " -XXX- " << endl;
                }

            } /* end */
            
            /* 
             * add edge (u,v) to path 
             *  if uu_node<=vv_node
             *      M_walk(end+1,:) = [uu_node,vv_node,max_wgt];
             *  else
             *      M_walk(end+1,:) = [vv_node,uu_node,max_wgt];
             *  end
             * */
            if (u<=v) {
                M_walk.push_back(NodePair(u,v));
            } else {
                M_walk.push_back(NodePair(v,u));
            }


            /*
            cerr << " ---- STEP";
            cerr << "(" << u << ", " << v << ", wgt=" << max_wgt << ")";
            cerr << endl;
            */
            print(0);
            print(3);
            print(4);
            /* */
            

            /*
             * continue walk from node vv 
             *  uu_node = vv_node;
             * */
            u = v;


        } /* end walk */ 
        


        /* 
         * ... append walk to (2b)-matching 
         *  if size(M_walk,1)>0
         * */
        if (M_walk.size()>0) {
            /*
             * ... first, split walk into two 
             *  M1_walk = M_walk(1:2:end,:);
             *  M2_walk = M_walk(2:2:end,:);
             * */
            M1_weight = 0.0;
            for (i=0; i<M_walk.size(); i=i+2) {
                M1_weight += (*Wmat[M_walk[i].first])[M_walk[i].second];
            }
            M2_weight = 0.0;
            for (i=1; i<M_walk.size(); i=i+2) {
                M2_weight += (*Wmat[M_walk[i].first])[M_walk[i].second];
            }
            /*
             * ... append heavy half-walk to heavy b-match, 
             * 
             *  if sum(M1_walk(:,3)) >= sum(M2_walk(:,3))
             *      B_match = [B_match; M1_walk];
             *  else
             *      B_match = [B_match; M2_walk];
             *  end
             * */
            if (M1_weight>=M2_weight) {
                i_start_at = 0;
            } else {
                i_start_at = 1;
            }
            for (i=i_start_at; i<M_walk.size(); i=i+2) {
                B_match.push_back(M_walk[i]);
            }
            
        } /* end append walk */ 

    } /* end while nodes with capacity */


    /* 
     * first half is heavy half 
     *  ijw_out = B_match;
     * */


    /* 
     * verbose output
     * disp(['     heavy wgt   = ', num2str(sum(B_match(:,3)))]);
     * */

    /*
     * return number of edges in bmatching
     * */
    return B_match.size();
}
/* end solve */


/* copy b-matching approx solution using 0-based indices for nodes */
void GreedyApproxBmatch::
solution(const int & num_bmatch_edge, DoubleVec & bmatch_edge_vec) 
{
    EdgeList::iterator np;

    bmatch_edge_vec.clear();
    bmatch_edge_vec.reserve(3*num_bmatch_edge);
    for (np=B_match.begin(); np!=B_match.end(); np++)
    {
        bmatch_edge_vec.push_back(np->first);
        bmatch_edge_vec.push_back(np->second);
        bmatch_edge_vec.push_back(1.0);
    }
}
/* end solution */



/* destructor */
RecursiveGreedyApproxBmatch::
~RecursiveGreedyApproxBmatch()
{
    //cerr << "RecursiveGreedyApproxBmatch destructor> " << endl;
}



/* solve_bmatching_problem */
int RecursiveGreedyApproxBmatch::
solve_bmatching_problem(const int & in_num_node, const DoubleVec & in_deg_bdd_vec, const int & in_num_ijw, const DoubleVec & in_ijw_vec, int & num_bmatch_edge, DoubleVec & bmatch_edge_vec, double & bmatch_wgt, const int & verbose)
{
    register int i;
    register int j;
    register int k;

    int success;
    int recur_success;

    int     L_num_node = in_num_node;
    int     L_num_ijw = in_num_ijw;
    DoubleVec L_ijw_vec = in_ijw_vec;
    DoubleVec L_deg_bdd_vec = in_deg_bdd_vec;

    /* degrees */
    DoubleVec deg_vec(L_num_node, 0);     /* of input graph */
    DoubleVec bmatch_deg_vec(L_num_node, 0);     /* of bmatch subgraph */

    /* adjacency and weights */
    SparseVecVec AAmat;
    SparseVecVec BBmat;
    SparseVecVec WWmat;
    SparseVecIterator sp;

    /* recursive input */
    int         num_recur_edge;
    DoubleVec   recur_edge_vec;
    double      recur_wgt;

    /* */
    num_bmatch_edge = 0;
    bmatch_edge_vec.clear();

    /* remove edges that are incident to nodes with upper bound at zero */
    ijw_2_svecvec_symm(L_num_ijw, L_ijw_vec, L_num_node, AAmat);

    for (i=0; i<L_num_node; i++)
    {
        if (L_deg_bdd_vec[2*i + 1]==0)
        {
            AAmat[i]->clear();
            for (j=0; j<L_num_node; j++) 
            {
                AAmat[j]->erase(i);
            }
        }
    }
    /* extract updated upper triangle ijw */
    svecvec_2_ijw(L_num_node, AAmat, L_num_ijw, L_ijw_vec);
    ijw_upper_tri(L_num_ijw,L_ijw_vec,0);
    
    /* make AAmat logical */
    logical_svecvec(AAmat);

    /* compute degree (self-loops=2)*/
    for (k=0; k<L_num_ijw; k++) {
        i = int(L_ijw_vec[3*k+0]);
        j = int(L_ijw_vec[3*k+1]);
        deg_vec[i] += 1;
        deg_vec[j] += 1;
    }
    /* ensure validity of bounds */
    for (i=0; i<L_num_node; i++) 
    {
        L_deg_bdd_vec[2*i+0] = max(0.0, min(L_deg_bdd_vec[2*i+0], deg_vec[i]));
        L_deg_bdd_vec[2*i+1] = max(0.0, min(L_deg_bdd_vec[2*i+1], deg_vec[i]));
    }

    /* debug */
    if (verbose>2)
    {
        cerr << "ijw_vec:" << endl;
        write_mn_matrix(cerr, L_num_ijw, 3, L_ijw_vec, 4);
        cerr << "deg_vec:" << endl;
        write_mn_matrix(cerr, L_num_node, 1, deg_vec, 4);
        cerr << "deg_bdd_vec:" << endl;
        write_mn_matrix(cerr, L_num_node, 2, L_deg_bdd_vec, 4);
    }


    /* */
    success = GreedyApproxBmatch::solve_bmatching_problem(L_num_node, L_deg_bdd_vec, L_num_ijw, L_ijw_vec, num_bmatch_edge, bmatch_edge_vec, bmatch_wgt, verbose);

    /* */
    if ((success) && (num_bmatch_edge>0))
    {
        /* setup recursion */

        /* matching: sparse symmetric adjacency */
        ijw_2_svecvec_symm(num_bmatch_edge, bmatch_edge_vec, in_num_node, BBmat);
        logical_svecvec(BBmat);

        /* compute degree (self-loops=2)*/
        for (k=0; k<num_bmatch_edge; k++) {
            i = int(bmatch_edge_vec[3*k+0]);
            j = int(bmatch_edge_vec[3*k+1]);
            bmatch_deg_vec[i] += 1;
            bmatch_deg_vec[j] += 1;
        }

        /* A_residual = A - B */
        for (i=0; i<L_num_node; i++) 
        {
            for (sp=BBmat[i]->begin(); sp!=BBmat[i]->end(); sp++) 
            {
                j = sp->first;
                AAmat[i]->erase(j);
            }
        }
        /* W_residual = A_residual.*WWmat */
        ijw_2_svecvec_symm(in_num_ijw, in_ijw_vec, in_num_node, WWmat);

        for (i=0; i<L_num_node; i++) 
        {
            for (sp=AAmat[i]->begin(); sp!=AAmat[i]->end(); sp++) 
            {
                j = sp->first;
                sp->second = (*WWmat[i])[j];
            }
        }
        /* ijw_vec = find(triu(W_residual)) */
        svecvec_2_ijw(L_num_node, AAmat, L_num_ijw, L_ijw_vec);
        ijw_upper_tri(L_num_ijw,L_ijw_vec,0);

        /* deg_bdd = L_deg_bdd_vec - bmatch_deg_vec; */
        for (i=0; i<L_num_node; i++)
        {
            L_deg_bdd_vec[2*i + 0] = L_deg_bdd_vec[2*i + 0] - bmatch_deg_vec[i];
            L_deg_bdd_vec[2*i + 1] = L_deg_bdd_vec[2*i + 1] - bmatch_deg_vec[i];
        }


        /* recurse and merge any additional edges */
        RecursiveGreedyApproxBmatch * ALG = new RecursiveGreedyApproxBmatch();
        recur_success = ALG->solve_bmatching_problem(L_num_node, L_deg_bdd_vec, L_num_ijw, L_ijw_vec, num_recur_edge, recur_edge_vec, recur_wgt, verbose);
        delete ALG;

        /* append to bmatch_edge_vec */
        if ((recur_success) && (num_recur_edge>0))
        {
            bmatch_edge_vec.insert(bmatch_edge_vec.end(), recur_edge_vec.begin(), recur_edge_vec.end());
            num_bmatch_edge += num_recur_edge;
            bmatch_wgt      += recur_wgt;
        }
    }

    /* local cleanup */
    delete_svecvec(AAmat);
    delete_svecvec(BBmat);
    delete_svecvec(WWmat);

    return success;
}
/* end solve_bmatching_problem */



