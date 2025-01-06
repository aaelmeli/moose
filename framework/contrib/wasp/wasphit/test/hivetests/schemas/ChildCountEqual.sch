test{

    ChildCountEqual(IfExists) = [ "one/value"   'two/value'  "three/value" ]
    ChildCountEqual(EvenNone) = [ "four/value"  'five/value' "six/value"   ]

    badflags{
        inside{
            ChildCountEqual           = [ "three/value" "six/value"  ]
            ChildCountEqual(BadFlag)  = [ "one/value"   "four/value" ]
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
    five{
        value{
        }
    }
    six{
        value{
        }
    }

    settings{
        override{
            ChildCountEqual(EvenNone) = [ color=orange               '../orange_rgb' ]
            ChildCountEqual(IfExists) = [ '../override/color'=yellow "../yellow_rgb" ]
            color{
            }
        }
        orange_rgb{
            value{
            }
        }
        yellow_rgb{
            value{
            }
        }
    }

}
