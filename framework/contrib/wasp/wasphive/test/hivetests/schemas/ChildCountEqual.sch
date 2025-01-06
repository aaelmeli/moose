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
        `import('empty_value.son')
    }
    two{
        `import('empty_value.son')
    }
    three{
        `import('empty_value.son')
    }
    four{
        `import('empty_value.son')
    }
    five{
        `import('empty_value.son')
    }
    six{
        `import('empty_value.son')
    }

    settings{
        override{
            ChildCountEqual(EvenNone) = [ color=orange               '../orange_rgb' ]
            ChildCountEqual(IfExists) = [ '../override/color'=yellow "../yellow_rgb" ]
            color{
            }
        }
        orange_rgb{
            `import('empty_value.son')
        }
        yellow_rgb{
            `import('empty_value.son')
        }
    }

}
