pytest{

    MinOccurs=1
    MaxOccurs=1

    object_one{
        
        MinOccurs=1
        MaxOccurs=1
        
        color{
            MinOccurs=1
            MaxOccurs=1
            ValType=String
            ValEnums=[ "red" "blue" "green" "orange" ]
        }

        foos{

            MinOccurs=1
            MaxOccurs=1

           foo{

                MinOccurs=1
                MaxOccurs=NoLimit

                id{
                    MinOccurs=1
                    MaxOccurs=1
                    ValType=String
                }

                numval{
                    MinOccurs=1
                    MaxOccurs=1
                    ValType=Real
                    MinValInc=0
                    MaxValExc=NoLimit
                }

                ChildExactlyOne=[ "aaas" "bbbs" "cccs" "ddds" "eees" "fffs" ]

                aaas{

                    MaxOccurs=1

                    a_num{

                        MinOccurs=1
                        MaxOccurs=NoLimit

                        id{
                            MinOccurs=1
                            MaxOccurs=NoLimit
                            ValType=String
                            ValEnums=[ REF:EnumList ]
                        }
                        value{
                            ValType=Real
                            MinValInc=0
                            MaxValInc=1
                            SumOver("../..") = 1   
                        }
                    }
                }
                bbbs{

                    MaxOccurs=1

                    b_num{

                        MinOccurs=1
                        MaxOccurs=NoLimit

                        id{
                            MinOccurs=1
                            MaxOccurs=NoLimit
                            ValType=String
                            ValEnums=[ REF:EnumList ]
                        }
                        value{
                            ValType=Real
                            MinValInc = 0
                            MaxValExc = NoLimit
                        }
                    }

                }
                cccs{

                    MaxOccurs=1

                    c_num{

                        MinOccurs=1
                        MaxOccurs=NoLimit

                        id{
                            MinOccurs=1
                            MaxOccurs=NoLimit
                            ValType=String
                            ValEnums=[ REF:EnumList ]
                        }
                        value{
                            ValType=Real
                            MinValInc=0
                            MaxValInc=1
                            SumOver("../..") = 1   
                        }
                    }
                }
                ddds{

                    MaxOccurs=1

                    d_num{

                        MinOccurs=1
                        MaxOccurs=NoLimit

                        id{
                            MinOccurs=1
                            MaxOccurs=NoLimit
                            ValType=String
                            ValEnums=[ REF:EnumList ]
                        }
                        value{
                            ValType=Real
                            MinValInc = 0
                            MaxValExc = NoLimit
                        }
                    }

                }

                eees{

                    MaxOccurs=NoLimit

                    id{
                        MinOccurs=1
                        MaxOccurs=1
                        ValType=Int
                        MinValInc=1
                        MaxValExc=NoLimit  
                    }

                    e_num{

                        MinOccurs=1
                        MaxOccurs=NoLimit

                        id{
                            MinOccurs=1
                            MaxOccurs=NoLimit
                            ValType=String
                            ValEnums=[ REF:EnumList ]
                        }
                        value{
                            ValType=Real
                            MinValInc=0
                            MaxValInc=1
                            SumOver("../..") = 1   
                        }
                    }
                }    

                fffs{

                    MaxOccurs=NoLimit

                    id{
                        MinOccurs=1
                        MaxOccurs=1
                        ValType=Int
                        MinValInc=1
                        MaxValExc=NoLimit  
                    }

                    f_num{

                        MinOccurs=1
                        MaxOccurs=NoLimit

                        id{
                            MinOccurs=1
                            MaxOccurs=1
                            ValType=String
                            ValEnums=[ REF:EnumList ]
                        }
                        value{
                            MinOccurs=1
                            MaxOccurs=1
                            ValType=Real
                            MinValInc=0
                            MaxValInc=1
                            SumOver("../..") = 1   
                        }
                    }
                }

            }

        }

        bars{

            MinOccurs=0
            MaxOccurs=1

            ChildAtLeastOne = [ typeone typetwo typethree typefour ]

            ChildUniqueness = [ typeone/id typetwo/id typethree/id typefour/id ]
            
            typeone{

                id{
                    MinOccurs=1
                    MaxOccurs=1
                    ValType=String
                }
                aaa{
                    MinOccurs=1
                    MaxOccurs=1
                    ValType=String
                    ValEnums = [ x y z ]
                }
                bbb{
                    MinOccurs=1
                    MaxOccurs=1
                    ValType=Real
                    MinValExc=0
                    MaxValExc=NoLimit
                }

            }

            typetwo{

                ChildAtLeastOne = [ x y z ]

                id{
                    MinOccurs=1
                    MaxOccurs=1
                    ValType=String
                }
                x{
                    MaxOccurs=1
                    ValType=Real
                    MinValExc=NoLimit
                    MaxValExc=NoLimit
                }
                y{
                    MaxOccurs=1
                    ValType=Real
                    MinValExc=NoLimit
                    MaxValExc=NoLimit
                }
                z{
                    MaxOccurs=1
                    ValType=Real
                    MinValExc=NoLimit
                    MaxValExc=NoLimit
                }

            }

            typethree{

                id{
                    MinOccurs=1
                    MaxOccurs=1
                    ValType=String
                }

                aaa{
                    MinOccurs=1
                    MaxOccurs=1
                    ValType=String
                    ValEnums = [ x y z ]
                }

                ChildExactlyOne = [ bbb ccc ]
                bbb{
                    MinOccurs=0
                    MaxOccurs=1
                    ValType=Real
                    MinValExc=0
                    MaxValExc=NoLimit
                }
                ccc{
                    MinOccurs=0 
                    MaxOccurs=1
                    ValType=Real
                    MinValExc=0
                    MaxValExc=NoLimit
                }

            }

            typefour{

                id{
                    MinOccurs=1
                    MaxOccurs=1
                    ValType=String
                }
                bbb{
                    MinOccurs=1
                    MaxOccurs=1
                    ValType=Real
                    MinValExc=0
                    MaxValExc=NoLimit

                }

            }

        }

    }

    object_two{
        
        MinOccurs=0
        MaxOccurs=1
        
        
        circle{

            MinOccurs=0
            MaxOccurs=1
            
            name{
                MinOccurs=1
                MaxOccurs=1
                ValType=String
                ValEnums=[ "henry" "susie" "john" "martha"  ]
            }           
            phone{
                MinOccurs=1
                MaxOccurs=1
                ValType=Int
                MinValInc=0
                MaxValExc=NoLimit
            }

       }
       
       square{

            MinOccurs=0
            MaxOccurs=1

            job{
                MinOccurs=1
                MaxOccurs=1
                ValType=String
                ValEnums=[ "lawyer" "teacher" "doctor"  ]
            }
            sport{
                MinOccurs=1
                MaxOccurs=1
                ValType=String
                ValEnums=[ "tennis" "football" "basketball" ]
            }       
            
            triangle{

                MinOccurs=0
                MaxOccurs=1

                sides{
                     MinOccurs=1
                     MaxOccurs=1
                     ValType=Int
                     MinValInc=1
                     MaxValExc=NoLimit
                 }
                 type{
                     MinOccurs=1
                     MaxOccurs=1
                     ValType=String
                     ValEnums=[ "equilateral" "isosceles" "scalene" ]
                 }

            }

            pentagon{
                
                MinOccurs=0
                MaxOccurs=1
                 
                rise{
                     MinOccurs=1
                     MaxOccurs=1
                     ValType=Real
                     MinValExc=0
                     MaxValExc=NoLimit
                 }
                run{
                     MinOccurs=1
                     MaxOccurs=1
                     ValType=Real
                     MinValExc=0
                     MaxValExc=NoLimit
                 }

                foo_name_choices{
                    MaxOccurs=1
                    value{
                       MinOccurs=0
                       MaxOccurs=NoLimit
                       ValType=String
                       ExistsIn=[ "../../../../../object_one/foos/foo/id" ]
                    }
                }    
            }   
        }
    }
}

EndOfSchema{}

EnumList = [  "ac206"        "ac207"        "ac208"        "ac235"       "ac236"
              "ac209"        "ac210"        "ac211"        "ac212"       "ac213"
              "ac214"        "ac215"        "ac216"        "ac217"       "ac218"
              "ac219"        "ac220"        "ac221"        "ac222"       "ac223"
              "ac224"        "ac225"        "ac226"        "ac227"       "ac228"
              "ac229"        "ac230"        "ac231"        "ac232"       "ac233" ] 
