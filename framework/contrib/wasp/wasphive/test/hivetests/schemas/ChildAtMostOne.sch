test{

    ChildAtMostOne = [ one=Red two three ]

    one{
    }
    two{
    }
    three{
    }
    four{
    }
    five{
        ChildAtMostOne=[ "../four" "../two" ]
    }
    six{
    }
    seven{
        ChildAtMostOne=[ "../six"=no "../four"=YES ]
    }

}
