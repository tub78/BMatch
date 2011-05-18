% [Ymat, wgt, deg_out] = bmatch_matrix(Wmat, deg_bdd, [method], [verbose])
%
%B-matching on an undirected non-bipartite graph.
%
%(also known as the maximum weight degree-constrained subgraph problem)
%
%Solves:
%       optimize_Yij sum_ij W_ij Y_ij s.t. l_i <= sum_j Y_ij <= u_i, 1 <= i <= n
%       and Wij and Yij are symmetric
%
%Method: 
%    1. exact maxwgt solution using goblin via subgraph complement
%    2. exact mincost solution using goblin
%    3. greedy 1/2 approximation to maxwgt solution
%    4. greedy 1/2 approximation to maxwgt solution with recursion
%    5. bipartite relaxation to maxwgt solution using belief propagation
%           
%Notes: 
%       Self-loops are handled: a self-loop increases the degree of a node
%       by 2.  Weights must be positive (Wij >= 0).  Edges with zero-weight
%       (Wij==0) are not allowed to partake in the matching.  To specify an 
%       allowed edge with zero weight, call bmatch_ijw directly.
%
%       The output weight is calculated as sum_{i<=j} Wij Yij, except when
%       belief propagation is used, whose output is not necessarily
%       symmetric.  For BP, the output weight is 0.5 * sum_{i,j} Wij Yij.
%
%       The lower bounds are ignored by methods 3-5. Bipartite relaxation
%       assumes that the degree upper bounds can be met with equality.

%
%Parameters:
%   Wmat : N x N [wgts]
%
%   DEG_BDD : scalar, Nx1, 1xN, Nx2 or 2xN
%
%   METHOD : scalar 1-5
%
%   VERBOSE : [1] - verbose, 0 - quiet, 2 - turn-on mex verbosity
%
%   Ymat : N x N [matching]
%
%   WEIGHT : scalar
%       - a negative weight indicates a failure
%
% See also: BMATCH_IJW
%
%
%Example 1:
%   
%W = [0  5 10 4;
%5  0 3  7;
%10 3 0  6;
%4  7 6  0];
%[Y,wgt,deg_out]=bmatch_matrix(W, 2, 1); num2str(Y,-5),num2str(deg_out,-5)
%[Y,wgt,deg_out]=bmatch_matrix(W, 5, 1); num2str(Y,-5),num2str(deg_out,-5)
%
%Example 2:
%   
%W = [1  5 10 4;
%5  3 3  7;
%10 3 5  6;
%4  7 6  7];
%[Y,wgt,deg_out]=bmatch_matrix(W, 2, 1); num2str(Y,-5),num2str(deg_out,-5)
%[Y,wgt,deg_out]=bmatch_matrix(W, 5, 1); num2str(Y,-5),num2str(deg_out,-5)
%
%Example 3:
%
%W = load('in_big34.txt');
%W = W + eps;
%[Y,wgt,deg_out]=bmatch_matrix(W, 17, 1); num2str(Y,-5),num2str(deg_out,-3)
%
%Example 4:
%
%W = load('in_big34.txt');
%W = W + eps;
%deg = ceil(17*rand(1,34));
%[Y,wgt,deg_out]=bmatch_matrix(W,deg,1);num2str(Y,-5),num2str(deg_out,-3)
%
%
%Example 5:
%
%N = 50;
%W = sprandsym(N,0.25);
%W(logical(W)) = W(logical(W)) - min(min(W(logical(W)))) + 1.0;
%deg = sum(W~=0)+diag(W~=0)';num2str(deg,-3)
%deg = min(deg, ceil(0.25*N*rand(1,N)));num2str(deg,-3)
%[Y,wgt,deg_out]=bmatch_matrix(W,deg,1);num2str(deg_out,-3), remaining=num2str(deg-deg_out,-5)
%
% 
%Example 6:  
% 
%   If we shift the edge weights, being careful to preserve the sparsity 
%   pattern (i.e. the allowable edges in the graph), the new optimal solution 
%   has the same weight when recalculated with the original weights
% 
%W1 = W;
%W1(logical(W1)) = W1(logical(W1)) + 1000.0;
%[Y,wgt,deg_out]=bmatch_matrix(W,deg,1);
%[Y1,wgt1] = bmatch_matrix(W1,deg,1);
%wgt1_recalc = sum(sum(triu(Y1.*W)))
%
%   *!* N.B. this is not always true ... a counter example follows
%
%W = [0.0 2.5 1;
% 2.5 0.0 1.0;
% 1.0 1.0 0.0];
%deg = [1,1,2];
%[Y,wgt,deg_out]=bmatch_matrix(W,deg,1);num2str(Y,-5),num2str(deg_out,-3)
%W1 = W;
%W1(logical(W1)) = W1(logical(W1)) + 1.0;
%[Y1,wgt1,deg_out1]=bmatch_matrix(W1,deg,1);num2str(Y1,-5),num2str(deg_out1,-3)
%
function [Ymat, wgt, deg_out] = bmatch_matrix(W, deg_bdd, in_method, in_verbose)

%
method = 1;
if (nargin>=3) & (~isempty(in_method))
    method = in_method;
end
%
verbose = 1;
if (nargin>=4) & (~isempty(in_verbose))
    verbose = in_verbose;
end
%
N = size(W,1);
%
wrong_size_deg_bdd_msg = [' bmatch_matrix> deg_bdd must be scalar, Nx1, 1xN, Nx2 or 2xN'];
num_elements_deg_bdd = numel(deg_bdd);
if num_elements_deg_bdd==1
    deg_bdd = [zeros(1,N); deg_bdd*ones(1,N)];
    %
elseif num_elements_deg_bdd==N
    deg_bdd = [zeros(1,N); deg_bdd(:)'];
    %
elseif num_elements_deg_bdd==2*N && size(deg_bdd,1)==2
    deg_bdd = deg_bdd;
    %
elseif num_elements_deg_bdd==2*N && size(deg_bdd,1)==N
    deg_bdd = deg_bdd';
    %
else
    error(wrong_size_deg_bdd_msg);
end

%
is_w_sparse = issparse(W);

% 
[I,J,V] = find(triu(W));
ijw_in = [I, J, V];

% solve
[ijw_out, wgt, deg_out] = bmatch_ijw(ijw_in, deg_bdd, method, verbose);

%
Ymat = sparse(ijw_out(:,1), ijw_out(:,2), 1.0, N, N); 
Ymat = Ymat|Ymat';

%
if ~is_w_sparse
    Ymat = full(Ymat);
    deg_out = full(deg_out);
end

%
return;
%%%%%%%%%
%

