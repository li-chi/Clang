Clang
=====

Progress

MFC -> delete a random function;
success, using //

MVIV -> delete the init value of a varDecl;
success, using //

MVAV -> delete the value assginemnt with RHS value;
success //new using floatLiteral & IntegerLiteral

MVAE -> delete the value assginemnt with RHS value;
success //new using callExpr & binaryOperator

MIA -> if (a) b, ->  delete if a;
success

MIFS -> if (a) b -> delete all;
success

MIEB -> if(a) b else (c) d，only leave d;
success

MLC -> if(a&&b)，delete &&b;
success //new

MLPA -> delete one assginment statement;
success

WVAV -> Assignment a random num in assignment;
success //new a random 0~999 assigned to float or int

WPFV -> use another var for the para in the fun
1. count the num of funtion with para
2. random choose one -> random choose one para
3. find all varDecl before the choosing function
4. random choose one var
5. replace the choose var with the choosing para

WAEP -> f(a+b) —>> f(a) or f(a-b)…
success //new