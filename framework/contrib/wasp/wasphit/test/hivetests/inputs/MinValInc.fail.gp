
[test]
    controlone=15
    controltwo=-45.3
    bad_two_numbers=6
    bad_two_numbers=7
    bad_string='some_string'
    valueone=58.6
    valuetwo=' 65 56 58.6 58 88.7 '
    valuethree=22.9
    valuefour=' 28.3 20.3 22.9 12.34 2e2 '
    valuefive=14
    valuesix=' 11 12 15 14 15.1 '
    valueseven=-45.4
    valueeight=' -45.4 -41.9 -100.3 -45.3 -4E-8 -7e+3 '
    [./value_bad_one]
        inside=47
    [../]
    [./value_bad_two]
        inside=48
    [../]
    valueone=a_string
[]
