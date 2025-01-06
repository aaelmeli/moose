
[test]

    [./inside]

        id=121

        [./container]
            id=72123
            inside=8.4
        [../]
        [./container]
            id=72456
            inside=4.9
        [../]
        [./container]
            id=72789
            inside=3.3
        [../]
        [./container]
            id=82123
            inside=9.4
        [../]
        [./container]
            id=82456
            inside=4.9
        [../]
        [./container]
            id=82789
            inside=3.2
        [../]
        [./container]
            id=92123
            inside=4.4
        [../]
        [./container]
            id=92456
            inside=4.9
        [../]
        [./container]
            id=92789
            inside=3.9
            [./badoptions]
            [../]
        [../]

        array=' 127.3 3.2 5e-3 '

    [../]

    [./inside]
        id=124
        array=' 4.5 127.3 0.02 23.2 0.02 5e-3 4.5 '
    [../]

    [./inside]
        id=1324
        array=' 4.5 27.3 0.02 '
    [../]

    [./inside]
        id=1322
        array=' 5e-3 4.5 127.3 83.2 0.02 5e-3 13.2 '
        invalid_array= ' 1.2 4.5 something 8.8 '
    [../]

[]
