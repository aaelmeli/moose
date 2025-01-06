
[test]

    should_exist_one   = 1
    should_exist_two   = ' 2 3 4 5 '
    should_exist_three = 11
    should_exist_four  = ' 12 13 14 15 '

    should_not_exist_one   = 21
    should_not_exist_two   = ' 22 23 24 25 '
    should_not_exist_three = 31
    should_not_exist_four  = ' 32 33 34 35 '

    [./invalid_rule]
        inside=5
    [../]

[]
