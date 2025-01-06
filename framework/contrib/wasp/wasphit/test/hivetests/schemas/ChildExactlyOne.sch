test{

    ChildExactlyOne = [ one two three ]

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
        ChildExactlyOne=[ "../six" ]
    }

}
