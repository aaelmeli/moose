test{

    ChildAtMostOne = [ one two three ]

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
        ChildAtMostOne=[ "../six" ]
    }

}
