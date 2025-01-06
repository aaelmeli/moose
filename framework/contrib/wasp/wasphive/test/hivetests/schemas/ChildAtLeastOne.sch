test{

    ChildAtLeastOne = [ one=Red 'two/value' 'three/value' ]

    one{
    }
    two{
        value{
        }
    }
    three{
        value{
        }
    }
    four{
    }
    five{
        ChildAtLeastOne=[ "../four" "../two/value" ]
    }
    six{
    }
    seven{
        ChildAtLeastOne=[ "../six"=no "../four"=YES ]
    }

}
