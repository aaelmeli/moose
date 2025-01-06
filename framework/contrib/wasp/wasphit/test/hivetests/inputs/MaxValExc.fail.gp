
[test]
    controlone=15
    controltwo=-453e-1
    bad_two_numbers=6
    bad_two_numbers=7
    bad_string='some_string'
    valueone=58.7
    valuetwo=' 65 59 57 58 58.7 '
    valuethree=23
    valuefour=' 18.3 29.3 23 12.34 '
    valuefive=15
    valuesix=' 11 12 13 14 15 17.3 '
    valueseven=-45.3
    valueeight=' -52.4 -51.9 -10.3 -45.3 '
    valuenine=2000.90
    [./value_bad_one]
        inside=47
    [../]
    [./value_bad_two]
        inside=48
    [../]
    valueone=a_string
[]
