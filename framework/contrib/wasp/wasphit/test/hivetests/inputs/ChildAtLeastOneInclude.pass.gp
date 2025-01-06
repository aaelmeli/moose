
[test]
    !include calo.test1_members.i
[]
!include calo.test2_block.i

[test]
    two=' 2 3 4 '
    four=5
    five=6
[]
[test]
    two=2
    five=6
[]
[test]
    !include calo.test5.some_members.i    
    
    seven=' 11 12 '
[]
