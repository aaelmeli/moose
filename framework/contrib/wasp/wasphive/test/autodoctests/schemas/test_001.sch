flagvalue_numbers{
    Description = "Description for flagvalue_numbers"
    flagvalue_ints{
        Description = "Description for flagvalue_ints"
        flagvalue_int1{
            Description = "Description for flagvalue_int1"
            ValType=Int
        }
        flagvalue_int2{
            Description = "Description for flagvalue_int2"
            MinOccurs=1
            MaxOccurs=5
            ValType=Int
            ValEnums=[ REF:ReferencedEnumsDigit1 44 55 66 77 ]
        }
        flagvalue_int3{
            Description = "Description for flagvalue_int3"
            MaxOccurs=10
            ValType=Int
            MinValInc=0
            MaxValInc=10

        }
        flagvalue_int4{
            MinOccurs=1
            ValType=Int
            MinValExc=0
            MaxValExc=10
        }
        flagvalue_int5{
            Description = "Description for flagvalue_int5"
            MinOccurs='../flagvalue_int4'
            MaxOccurs=NoLimit
            ValType=Int
            MinValInc='../flagvalue_int1'
            MaxValExc='../flagvalue_int2'
        }
        flagvalue_int6{
            Description = "Description for flagvalue_int6"
            MinOccurs='../flagvalue_int1'
            MaxOccurs='../flagvalue_int2'
            ValType=Int
            MinValExc='../flagvalue_int3'
            MaxValInc='../flagvalue_int4'
        }
    }
    flagvalue_reals{
        Description = "Description for flagvalue_reals"
        MinOccurs=1
        MaxOccurs='../flagvalue_ints/flagvalue_int6'
        flagvalue_real1{
            Description = "Description for flagvalue_real1"
            ValType=Real
        }
        flagvalue_real2{
            Description = "Description for flagvalue_real2"
            MinOccurs=1
            MaxOccurs=1
            ValType=Real
            MinValExc=NoLimit
            MaxValExc=NoLimit
            ValEnums=[ REF:ReferencedEnumsFloat1 4.4 5.5 6.6 7.7 ]
            ExistsIn=[ EXTRA:100.1 EXTRA:200.2 EXTRAREF:ReferencedEnumsFloat2
                       "../flagvalue_real3" "../../../flagarray_numbers/flagarray_reals/flagarray_real4/value" ]
        }
        flagvalue_real3{
            Description = "Description for flagvalue_real3"
            MinOccurs=10
            MaxOccurs=10
            ValType=Real
            MinValInc=0
            MaxValInc=10
        }
        flagvalue_real4{
            MinOccurs=1
            ValType=Real
            MinValExc=0
            MaxValExc=10
        }
        flagvalue_real5{
            Description = "Description for flagvalue_real5"
            MinOccurs='../flagvalue_real4'
            MaxOccurs=NoLimit
            ValType=Real
            MinValInc='../flagvalue_real1'
            MaxValExc='../flagvalue_real2'
        }
        flagvalue_real6{
            Description = "Description for flagvalue_real6"
            MinOccurs='../flagvalue_real1'
            MaxOccurs='../flagvalue_real1'
            ValType=Real
            MinValExc='../flagvalue_real3'
            MaxValInc='../flagvalue_real4'
        }
    }
    
}
flagarray_numbers{
    Description = "Description for flagarray_numbers"
    MinOccurs='../flagvalue_numbers/flagvalue_ints/flagvalue_int1'
    MinOccurs='../flagvalue_numbers/flagvalue_ints/flagvalue_int2'
    flagarray_ints{
        Description = "Description for flagarray_ints"
        MaxOccurs=10
        flagarray_int1{
            Description = "Description for flagarray_int1"
            value{
                ValType=Int
            }
        }
        flagarray_int2{
            MinOccurs=1
            MaxOccurs=1
            value{
                ValType=Int
                MinOccurs=5
                MaxOccurs=8
                MinValInc= -10
                MaxValExc=  10
                ValEnums=[ REF:ReferencedEnumsDigit1 44 55 66 77 ]
                ExistsIn=[ EXTRA:100 EXTRA:200 EXTRAREF:ReferencedEnumsDigit2
                           "../../flagarray_int3" "../../../flagarray_ints/flagarray_int4/value" ]
            }
        }
        flagarray_int3{
            Description = "Description for flagarray_int3"
            MinOccurs="../../../flagvalue_numbers/flagvalue_ints/flagvalue_int1"
            MaxOccurs="../../../flagvalue_numbers/flagvalue_ints/flagvalue_int1"
            value{
                ValType=Int
                MinOccurs="../../../../flagvalue_numbers/flagvalue_ints/flagvalue_int1"
                MaxOccurs="../../../../flagvalue_numbers/flagvalue_ints/flagvalue_int1"
                IncreaseOver("..")=Strict
                MinValInc=100
                MaxValExc="../../../../flagvalue_numbers/flagvalue_ints/flagvalue_int4"
            }
        }
        flagarray_int4{
            Description = "Description for flagarray_int4"
            MinOccurs="../../../flagvalue_numbers/flagvalue_ints/flagvalue_int1"
            MaxOccurs="../../../flagvalue_numbers/flagvalue_ints/flagvalue_int2"
            value{
                ValType=Int
                MinOccurs="../../../../flagvalue_numbers/flagvalue_ints/flagvalue_int1"
                MaxOccurs="../../../../flagvalue_numbers/flagvalue_ints/flagvalue_int2"
                DecreaseOver("..")=Mono
                MinValExc="../../../../flagvalue_numbers/flagvalue_ints/flagvalue_int4"
                MaxValInc="../../../../flagvalue_numbers/flagvalue_ints/flagvalue_int4"
            }
        }
    }
    flagarray_reals{
        Description = "Description for flagarray_reals"
        MinOccurs="../../flagvalue_numbers/flagvalue_ints/flagvalue_int5"
        MaxOccurs="../../flagvalue_numbers/flagvalue_ints/flagvalue_int6"
        flagarray_real1{
            Description = "Description for flagarray_real1"
            value{
                ValType=Int
            }
        }
        flagarray_real2{
            Description = "Description for flagarray_real2"
            MinOccurs=1
            MaxOccurs=1
            value{
                ValType=Int
                MinOccurs=5
                MaxOccurs=8
                MinValExc=NoLimit
                MaxValInc=500.5
                ExistsIn=[ "../../flagarray_real3" "../../../flagarray_reals/flagarray_real4/value" ]
            }
        }
        flagarray_real3{
            MinOccurs="../../../flagvalue_numbers/flagvalue_ints/flagvalue_int1"
            MaxOccurs="../../../flagvalue_numbers/flagvalue_ints/flagvalue_int1"
            value{
                ValType=Int
                MinOccurs="../../../../flagvalue_numbers/flagvalue_ints/flagvalue_int1"
                MaxOccurs="../../../../flagvalue_numbers/flagvalue_ints/flagvalue_int1"
                IncreaseOver("..")=Strict
                MinValInc=100
                MaxValExc="../../../../flagvalue_numbers/flagvalue_reals/flagvalue_real4"
            }
        }
        flagarray_real4{
            Description = "Description for flagarray_real4"
            MinOccurs="../../../flagvalue_numbers/flagvalue_ints/flagvalue_int1"
            MaxOccurs="../../../flagvalue_numbers/flagvalue_ints/flagvalue_int2"
            value{
                ValType=Int
                MinOccurs="../../../../flagvalue_numbers/flagvalue_ints/flagvalue_int1"
                MaxOccurs="../../../../flagvalue_numbers/flagvalue_ints/flagvalue_int2"
                DecreaseOver("..")=Mono
                MinValExc="../../../../flagvalue_numbers/flagvalue_reals/flagvalue_real4"
                MaxValInc="../../../../flagvalue_numbers/flagvalue_reals/flagvalue_real4"
            }
        }
    }   
}
flagvalue_strings{
    Description = "Description for flagvalue_strings"
    MinOccurs=1
    flagvalue_string1{
        Description = "Description for flagvalue_string1"
        value{
            MinOccurs=1
            MaxOccurs=1
            ValType=String
        }
    }
    flagvalue_string2{
        MinOccurs=10
        MaxOccurs="../../flagarray_strings/flagarray_string1/value"
        Description = "Description for flagvalue_string2"
        ValType=String
        ValEnums=[ REF:ReferencedEnumsNumtxt Thirty Fourty Fifty Sixty ]
        ExistsIn=[ EXTRA:RegExtra1 EXTRA:RegExtra2
                   EXTRAREF:ReferencedEnumsColors EXTRAREF:ReferencedEnumsShapes
                   "../../flagarray_strings/flagarray_string1/value" ]
        
    }
    flagvalue_string3{
        MaxOccurs="../../flagvalue_numbers/flagvalue_ints/flagvalue_int4"
        Description = "Description for flagvalue_string3"
        ValType=String
        ValEnums=[ choice01 choice02 choice03 choice04 ]
    }
    flagvalue_string4{
        MinOccurs=1
        MaxOccurs=NoLimit
        ValType=String
        ValEnums=[ choice11 choice12 choice13 choice14 ]
        ExistsIn=[ EXTRA:None EXTRA:All Extra:Some
                   "../../flagvalue_numbers/flagvalue_ints/flagvalue_int4" ]
    }
    flagvalue_string5{
        MinOccurs=5
        MaxOccurs=6
        Description = "Description for flagvalue_string5"
        ValType=String
        ValEnums=[ choice21 choice22 choice23 choice24 ]
        ExistsIn=[ "../../flagvalue_strings/flagvalue_string1"
                   "../flagvalue_string2"
                   "../../flagvalue_numbers/flagvalue_ints/flagvalue_int3"
                   "../../flagarray_strings/flagarray_string4"
                   "../../root_string"]
    }
}
flagarray_strings{
    Description = "Description for flagarray_strings"
    MaxOccurs="../flagvalue_numbers/flagvalue_ints/flagvalue_int6"
    flagarray_string1{
        Description = "Description for flagarray_string1"
        value{
            MinOccurs=2
            MaxOccurs=2
            ValType=String
        }
    }
    flagarray_string2{
        MinOccurs=10
        MaxOccurs="../flagarray_string1/value"
        Description = "Description for flagarray_string2"
        value{
            ValType=String
            ValEnums=[ REF:ReferencedEnumsNumtxt Thirty Fourty Fifty Sixty ]
            ExistsIn=[ EXTRA:RegExtra1 EXTRA:RegExtra2
                       EXTRAREF:ReferencedEnumsColors EXTRAREF:ReferencedEnumsShapes
                       "../../flagarray_string1/value" ]
        }
    }
    flagarray_string3{
        MaxOccurs="../../flagvalue_numbers/flagvalue_ints/flagvalue_int4"
        Description = "Description for flagarray_string3"
        value{
            MinOccurs=10
            MaxOccurs=11
            ValType=String
            ValEnums=[ choice01 choice02 choice03 choice04 ]
        }
    }
    flagarray_string4{
        MinOccurs=1
        MaxOccurs=NoLimit
        value{
            MaxOccurs="../../../flagvalue_numbers/flagvalue_ints/flagvalue_int2"
            ValType=String
            ValEnums=[ choice11 choice12 choice13 choice14 ]
            ExistsIn=[ EXTRA:None EXTRA:All Extra:Some
                       "../../../flagvalue_numbers/flagvalue_ints/flagvalue_int4" ]
        }
    }
    flagarray_string5{
        MinOccurs=5
        MaxOccurs="../../flagvalue_numbers/flagvalue_ints/flagvalue_int1"
        Description = "Description for flagarray_string5"
        value{
            MinOccurs=9
            MaxOccurs=19
            ValType=String
            ValEnums=[ choice21 choice22 choice23 choice24 ]
            ExistsIn=[ "../../../flagvalue_strings/flagvalue_string1"
                       "../../flagarray_string2"
                       "../../../flagvalue_numbers/flagvalue_ints/flagvalue_int3"
                       "../../../flagarray_strings/flagarray_string4"
                       "../../../root_tagged_value/value"]
        }
    }
}
root_string{
    Description = "Description for root_string"
    MaxOccurs=NoLimit
    ValType=String
    ValEnums=[ choice21 choice22 choice23 choice24 ]
    ExistsIn=[ "../root_tagged_value/id" ]
}
root_tagged_value{
    Description = "Description for root_tagged_value"
    MinOccurs=5
    MaxOccurs=6
    id{
        MinOccurs=1
        MaxOccurs=1
        ValType=Int
        ExistsIn=[ "../../flagvalue_numbers/flagvalue_ints/flagvalue_int1"
                   "../../flagarray_numbers/flagarray_ints/flagarray_int1/value"
                   "../../flagarray_numbers/flagarray_ints/flagarray_int2" ]
    }
    value{
        MinOccurs=1
        MaxOccurs=1
        ValType=String
        ExistsIn=[ EXTRA:yes EXTRA:no EXTRAREF:ReferencedEnumsNumtxt "../../root_string" ]
    }
}
root_tagged_array{
    Description = "Description for root_tagged_array"
    MinOccurs=5
    MaxOccurs=7
    id{
        MinOccurs=1
        MaxOccurs=1
        ValType=String
        ValEnums=[ a b c d e f REF:ReferencedEnumsShapes ]
    }
    value{
        MinOccurs="../../root_tagged_value/id"
        MaxOccurs=15
        ValType=Real
        MinValExc= -18.8
        MaxValExc=  18.8
    }
}

ReferencedEnumsNumtxt=[ zero one two three four five six seven eight nine ]
ReferencedEnumsColors=[ red orange yellow green blue indigo violet ]
ReferencedEnumsShapes=[ square circle triangle pentagon hexagon ]
ReferencedEnumsDigit1=[ 0 1 2 3 4 ]
ReferencedEnumsDigit2=[ 7 8 9 10 11 ]
ReferencedEnumsFloat1=[ 1.1 2.2 3.3 ]
ReferencedEnumsFloat2=[ 6.6 7.7 8.8 ]
