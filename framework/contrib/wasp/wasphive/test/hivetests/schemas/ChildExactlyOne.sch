test{

    ChildExactlyOne = [ one=Red two three ]

    one{
    }
    two{
    }
    three{
    }
    four{
    }
    five{
        ChildExactlyOne=[ "../four" "../two" ]
    }
    six{
    }
    seven{
        ChildExactlyOne=[ "../six"=no "../four"=YES ]
    }

}
