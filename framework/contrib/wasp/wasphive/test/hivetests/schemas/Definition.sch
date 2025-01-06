% Schema to test the Definition creation 
% for use with definition driven interpreters
a{
    % 2 triplets of x,y,z
    x{InputAliases [STRIDE[0,3]]}
    y{InputAliases [STRIDE[1,3]]}
    %z{InputAliases [STRIDE[2,3]]}
    `import ("z.sch")

    % infinite triplets of i,j,k
    % the override x,y,z
    i{InputAliases [STRIDE[6,3]]}
    j{InputAliases [STRIDE[7,3]]}
    k{InputAliases [STRIDE[8,3]]}
}