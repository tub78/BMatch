% [ijw_out, wgt, deg_out] = bmatch_ijw(ijw_in, deg_bdd, [method], [verbose])
%
%Sparse interface for Bmatching.  For details, see: BMATCH_MATRIX
%
%Parameters:
%   IJW_IN : N_input x 3 [u, v, wgt]
%       - List of edges with non-zero weight
%       - Edge weights specified more than once are added together
%
%   IJW_OUT : N_bmatch x 3 [u, v, wgt]
%       - List of edges in matching
%
%Example 1: shows how to format the input for the sparse routine
%   
%W = [0  5 10 4;
%5  0 3  7;
%10 3 0  6;
%4  7 6  0];
%N = 4;
%DEG = 2;
%
%[I,J,V] = find(triu(W));
%deg_bdd = repmat([0;DEG],1,N);
%[ijw_out, wgt, deg_out] = bmatch_ijw([I, J, V], deg_bdd);
%Ymat = sparse(ijw_out(:,1), ijw_out(:,2), 1.0, N, N);
%num2str(Ymat|Ymat',-5), num2str(deg_out,-5)
%
%
%Example 2: create a large block diagonal matrix ... all of the methods
%       take advantage of the sparsity pattern, except for the current 
%       implementation of belief propagation (this example requires variables
%       defined in the first example)
%
%N_block = 10;
%Iblk = [];
%Jblk = [];
%Vblk = [];
%for k = 1:N_block
%    num_sofar = length(Iblk);
%    Iblk = [Iblk; repmat(I,N_block,1) + num_sofar];
%    Jblk = [Jblk; repmat(J,N_block,1) + num_sofar];
%    Vblk = [Vblk; repmat(V,N_block,1)];
%end
%N_big = length(Iblk);
%DEG_big = 2*N_block;
%deg_bdd_big = repmat([0;DEG_big],1,N_big);
%[ijw_out, wgt, deg_out] = bmatch_ijw([Iblk, Jblk, Vblk], deg_bdd_big, 1, 1);
%[ijw_out, wgt, deg_out] = bmatch_ijw([Iblk, Jblk, Vblk], deg_bdd_big, 3, 1);
%[ijw_out, wgt, deg_out] = bmatch_ijw([Iblk, Jblk, Vblk], deg_bdd_big, 4, 1);
%[ijw_out, wgt, deg_out] = bmatch_ijw([Iblk, Jblk, Vblk], deg_bdd_big, 5, 1);
%
%
function [ijw_out,wgt,deg_out] = bmatch_ijw(ijw_in,deg_bdd,in_method,in_verbose)
%
method = 1;
if nargin>=3
    method = in_method;
end
% verbosity for this function
vvrb = 0;
if nargin>=4
    vvrb = in_verbose;
end
%
% max node number
N = max(max(ijw_in(:,[1,2])));
%
% convert to 0-based indices
ijw_in(:,[1,2]) = ijw_in(:,[1,2]) - 1;
[ijw_out,wgt,success] = bmatch_ijw_mex(full(ijw_in),full(deg_bdd'),method,vvrb);
%
%
if ~isempty(ijw_out)
    ijw_out(:,[1,2]) = ijw_out(:,[1,2]) + 1;
    Bmat = sparse(ijw_out(:,1), ijw_out(:,2), 1.0, N, N);
    Bmat = Bmat|Bmat';
else
    Bmat = spalloc(N,N,0);
end
%
% degrees
deg_out = sum(Bmat) + diag(Bmat)';
%
return;
%%%%%%%%%%
%

