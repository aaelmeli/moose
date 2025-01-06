application{
    MinOccurs=1
    MaxOccurs=1
    InputTmpl="application"
    Description="specify path to executable, arguments, and input filename"
    value{
        MinOccurs=1
        MaxOccurs=1
        ValType=String
    }
    ChildCountEqual(EvenNone) = [input_file/value input_tmpl/value]
    input_file{
        MinOccurs=1
        MaxOccurs=1
        InputTmpl="input_file"
        Description="specify application's input file name"
        value{
            MinOccurs=1
            MaxOccurs=NoLimit
            ValType=String
        }
    }
    input_tmpl{
        MinOccurs=1
        MaxOccurs=1
        InputTmpl="input_tmpl"
        Description="specify input template name"
        value{
            MinOccurs=1
            MaxOccurs=NoLimit
            ValType=String
        }
    }
    left_delimiter{
        MinOccurs=0
        MaxOccurs=1
        InputTmpl="left_delimiter"
        Description="specify left delimiter to pass to HALITE executable"
        InputDefault="'<'"
        value{
            MinOccurs=1
            MaxOccurs=NoLimit
            ValType=String
        }
    }
    right_delimiter{
        MinOccurs=0
        MaxOccurs=1
        InputTmpl="right_delimiter"
        InputDefault="'>'"
        Description="specify right delimiter to pass to HALITE executable"
        value{
            MinOccurs=1
            MaxOccurs=NoLimit
            ValType=String
        }
    }
}

extract_from{

    MinOccurs=1
    MaxOccurs=NoLimit
    InputTmpl="extract_from"
    Description="specify a file name and what data to extract from that file"
    value{
        MinOccurs=1
        MaxOccurs=1
        ValType=String
    }
    ChildExactlyOne[ find using ]
    using{
        MinOccurs=0
        MaxOccurs=NoLimit
        InputTmpl="using"
        Description="specify a custom command for data extraction"
        value{
            MinOccurs=1
            MaxOccurs=1
            ValType=String
        }

    }
    find{

        MinOccurs=0
        MaxOccurs=NoLimit
        InputTmpl="find"

        InputVariants=[ "grab all lines containing a search pattern"
                        "grab either last line or last N lines"
                        ]

        ChildAtMostOne  = [ pattern between last_line ]
        ChildAtLeastOne = [ pattern between last_line first_line column]

        pattern{
           MinOccurs=0
           MaxOccurs=1
           ValType=String
           InputTmpl="pattern"
           Description="grab all lines containing a search pattern"
        }
        last_line{
           MinOccurs=0
           MaxOccurs=1
           InputTmpl="last_line"
           Description="grab either last line or last N lines"
           value{
               MinOccurs=0
               MaxOccurs=1
               ValType=Int
           }
        }
        first_line{
           MinOccurs=0
           MaxOccurs=1
           InputTmpl="first_line"
           Description="grab either first line or first N line"
           value{
               MinOccurs=0
               MaxOccurs=1
               ValType=Int
           }
        }
        
        between{
           MinOccurs=0
           MaxOccurs=1
           InputTmpl="between"
           Description="grab between pattern_start and pattern_end"
           value{
               MinOccurs=2
               MaxOccurs=2
               ValType=String
           }
        }
        
        column{
            MinOccurs=0
            MaxOccurs=NoLimit
            InputTmpl="column"
            Description="grab specific columns"
            value{
                MinOccurs=1
                MaxOccurs=NoLimit
                ValType=Int
            }
            delimiter{
                MinOccurs=0
                MaxOccurs=1
                InputTmpl="delimiter"
                value{
                    MinOccurs=1
                    MaxOccurs=1
                    ValType=String
                }
            }
            as{
     			MinOccurs=0
      			MaxOccurs=1
      			InputTmpl="as"
      			value{
          			MinOccurs=1
          			MaxOccurs=1
          			ValType=String
      			}
 			}
        }

    }

}

scheduler{
    MinOccurs=0
    MaxOccurs=1
    InputTmpl="scheduler"
    Description="Scheduler specifications"

    header {
        Description="List of commands to execute before the application is executed"
        InputTmpl=flagvalue
        InputType=header
        MinOccurs=1
        MaxOccurs=1
        value
        {
            ValType=String
            MinOccurs=1
            MaxOccurs=NoLimit
        }
    }
    submit_path {
        Description="Path to the scheduler job submission program"
        InputTmpl=flagvalue
        InputType=quoted
        MinOccurs=1
        MaxOccurs=1
        ValType=String
        InputDefault="/usr/local/bin/qsub"
    }
    
    polling_frequency {
        Description="Seconds between calls to check job status"
        ValType=Int
        MinValInc=1
        MinOccurs=0
        MaxOccurs=1
    }
}
