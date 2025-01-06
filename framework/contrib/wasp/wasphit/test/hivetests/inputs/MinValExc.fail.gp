
[test]
    controlone=15
    controltwo=-453E-1
    bad_two_numbers=6
    bad_two_numbers=7
    bad_string='some_string'
    valueone=58.7
    valuetwo=' 65E-1 66 7 68 58.7 '
    valuethree=23
    valuefour=' 383E-2 3.3 23 32.34 '
    valuefive=15
    valuesix=' -21 22 2.3E-4 24 15 '
    valueseven=-45.3
    valueeight=' -132.4 -3.19E5 -30.3 -45.3 '
    valuenine=-2000.90
    [./value_bad_one]
        inside=47
    [../]
    [./value_bad_two]
        inside=48
    [../]
    valueone=a_string
[]
