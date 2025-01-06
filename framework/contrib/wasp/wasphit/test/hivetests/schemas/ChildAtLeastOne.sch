test{

    ChildAtLeastOne = [ one 'two/value' 'three/value' ]

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
        value{
        }
    }
    seven{
        ChildAtLeastOne=[ "../six/value" ]
    }

}
