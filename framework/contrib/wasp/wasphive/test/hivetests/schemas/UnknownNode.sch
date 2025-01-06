test{

    % Schema tests that a non-terminal and terminal nodes that
    % are  marked as UNKNOWN produced the expected 
    % validation error
    some_value(UNKNOWN){
        value(UNKNOWN){}
    }
    some_known_value{}
}
