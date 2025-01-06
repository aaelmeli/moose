
[test]

    [./container]
        inside=93.3
    [../]
    [./container]
        inside=24.9
    [../]
    [./container]
        inside=24.9
    [../]
    [./container]
        inside=19.4
        [./badrule]
        [../]
    [../]
    array=' 63.2 100.2 100.2 48.2 87.3 87.3 87.3 4.5 '
    array=' 163.2 100.2 100.2 something 87.3 87.3 87.3 4.5 '
    another_array=' 163.2 100.2 something 100.2 87.3 87.3 87.3 4.5 '

[]
