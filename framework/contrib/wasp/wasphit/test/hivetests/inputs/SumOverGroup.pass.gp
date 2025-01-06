
[test]

    [./inside]

        id=121

        [./container]
            id=72123
            inside=59.4
        [../]
        [./container]
            id=72456
            inside=24.9
        [../]
        [./container]
            id=72789
            inside=23.3
        [../]
        [./container]
            id=82123
            inside=59.6
        [../]
        [./container]
            id=82456
            inside=44.7
        [../]
        [./container]
            id=82789
            inside=3.3
        [../]
        [./container]
            id=92123
            inside=0.4
        [../]
        [./container]
            id=92456
            inside=107.1
        [../]
        [./container]
            id=92789
            inside=0.1
        [../]

        array=' 4.5 87.3 83.2 54.4 '

    [../]

    [./inside]
        id=124
        array=' 4.5 67.3 83.2 34.4 '
    [../]

    [./inside]
        id=1324
        array=' 4.5 87.3 83.2 14.4 '
    [../]

    [./inside]
        id=1322
        array=' 24.5 87.3 83.2 34.4 '
    [../]

[]
