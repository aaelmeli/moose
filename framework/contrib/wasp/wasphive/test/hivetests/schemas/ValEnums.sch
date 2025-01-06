test{

    one{
        ValEnums=[ yes no maybe ]
    }
    two{
        ValEnums=[ yes no maybe ]
    }
    three{
        ValEnums=[ REF:ReferencedColors ]
    }
    four{
        ValEnums=[ REF:ReferencedNumbers ]
    }
    five{
        ValEnums=[ REF:ReferencedNumbers REF:ReferencedColors ]
    }
    six{
        ValEnums=[ REF:BadReference REF:ReferencedNumbers ]
    }

}

EndOfSchema{}

ReferencedColors=[ red orange yellow green blue indigo violet ]
ReferencedNumbers=[ 1 2 3 4 5 ]