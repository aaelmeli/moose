test{

    defineone{
    }
    definetwo{
    }
    definethree{
    }
    useone{
        value{
            NotExistsIn=[ "../../defineone/value"
                          "../../definetwo/value"
                          "../../definethree/value" ]
        }
    }
    usetwo{
        value{
            NotExistsIn(Abs)=[ "../../defineone/value"
                               "../../definetwo/value" ]
        }
    }
    usethree{
        value{
            NotExistsIn=[ "../../defineone/value"
                          "../../definethree/value" ]
        }
    }
    usefour{
        value{
            NotExistsIn=[ "../../definetwo/value"
                          "../../definethree/value" ]
        }
    }
    usefive{
        value{
            NotExistsIn(Zero)=[ "../../defineone/value" ]
        }
    }
    usesix{
        value{
            NotExistsIn(AbsZero)=[ "../../definetwo/value" ]
        }
    }
    useseven{
        value{
            NotExistsIn=[ "../../definethree/value" ]
        }
    }
    useeight{
        value{
            NotExistsIn(BadFlag)=[ "../../defineone/value" ]
        }
    }

}
