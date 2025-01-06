test{
    inside{
        id{
        }
        container{
            id{
            }
            inside{
                SumOverGroup("../..")=[ ComparePath="../id"
                                        GroupDivide=1000
                                        GroupSum=107.6 ]
            }
            badoptions{
                badruleone{
                    SumOverGroup("../..")=[ 
                                            GroupDivide=1000
                                            GroupSum=107.6 ]
                }
                badruletwo{
                    SumOverGroup("../..")=[ ComparePath="../id"

                                            GroupSum=107.6 ]
                }
                badrulethree{
                    SumOverGroup("../..")=[ ComparePath="../id"
                                            GroupDivide=1000
                                           ]
                }
            }
        }
        array{
            value{
                SumOverGroup("../../..")=[ ComparePath="../../id"
                                           GroupDivide=10
                                           GroupSum=418.8 ]
            }
        }
        invalid_array{
            value{
                SumOverGroup("../../..")=[ ComparePath="../../id"
                                           GroupDivide=100
                                           GroupSum=123.4 ]
            }
        }
    }
}