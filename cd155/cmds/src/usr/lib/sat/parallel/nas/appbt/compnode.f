C******************************************************************************C
C******************************************************************************C
C*** This routine compiles the list of exchaning node for node with my_col  ***C
C******************************************************************************C
C******************************************************************************C

        subroutine compnode

        include 'appbt.incl'

C******************************************************************************C
C******************************************************************************C

        nodemax = my_row*node_col-1
        nodemin = my_rowm1*node_col

C*** Mapping sending and receiving nodes for nodes within same rows.

        ndsen_set = nodemax
        ndrec_set = nodemin

        do 100 icount = 1, ndcolm1

           node_sen = my_node + icount
           if (node_sen.gt.ndsen_set) node_sen = node_sen - node_col
           ndsen_col(icount,1) = node_sen
           ndsen_col(icount,2) = mod (node_sen,node_col) + 1

           node_rec = my_node - icount
           if (node_rec.lt.ndrec_set) node_rec = node_rec + node_col
           ndrec_col(icount) = mod (node_rec,node_col) + 1

 100    continue

C*** Mapping sending and receiving nodes for nodes with different rows.

        do 200 icount = 1, ndrowm1

           ndcol_jump = icount*node_col

           ndsen_set  = nodemax + ndcol_jump
           if (ndsen_set.ge.nnodes) ndsen_set = ndsen_set - nnodes

           ndrec_set  = nodemin - ndcol_jump
           if (ndrec_set.lt.0) ndrec_set = ndrec_set + nnodes

           node_sen = my_node + ndcol_jump
           if (node_sen.gt.ndsen_set) node_sen = node_sen - nnodes
           ndsen_row(icount,1) = node_sen
           ndsen_row(icount,2) = node_sen/node_col
              
           node_rec = my_node - ndcol_jump
           if (node_rec.lt.ndrec_set) node_rec = node_rec + nnodes
           ndrec_row(icount) = node_rec/node_col

 200    continue

        return 
        
        end

C******************************************************************************C
C******************************************************************************C
