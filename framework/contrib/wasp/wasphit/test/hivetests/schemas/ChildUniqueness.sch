test{

    ChildUniqueness          = [ one/value                       ]
    ChildUniqueness          = [ one/value two/value             ]
    ChildUniqueness(Abs)     = [           two/value three/value ]
    ChildUniqueness(Zero)    = [ one/value           three/value ]
    ChildUniqueness(AbsZero) = [ one/value two/value three/value ]
    badflags{
        inside{
            ChildUniqueness(BadFlag) = [ four/value              ]
        }
    }

    one{
        value{
        }
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
        value{
        }
    }

}
