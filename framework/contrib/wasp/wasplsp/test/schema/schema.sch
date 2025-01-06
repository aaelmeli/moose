
key_string
{
    MinOccurs=0
    MaxOccurs=1
    InputTmpl="flagtypes"
    InputType="quotedvalue"
    ValType=String
    Description="Key_string description here for test"
}

key_list
{
    MinOccurs=0
    MaxOccurs=1
    InputTmpl="flagtypes"
    InputName="array_overwrite"
    Description="Key_list description here for test"

    value
    {
        MinOccurs=1
        MaxOccurs=NoLimit
        ValType=Real
        InputDefault="9.9"
    }
}

object
{
    InputVariants=[ "variant 1"
                    "variant 2"
                    "variant 3" ]

    Description="Object description here For test"

    MinOccurs=3
    MaxOccurs=NoLimit

    key
    {
        MinOccurs=0
        MaxOccurs=1

        value
        {
            MinOccurs=1
            MaxOccurs=1
            ValType=Real
            MinValInc=0
            MaxValExc=NoLimit
        }
    }

    list
    {
        MinOccurs=1
        MaxOccurs=1

        value
        {
            MinOccurs=1
            MaxOccurs=5
            ValType=Int
            MinValInc=0
            MaxValExc=NoLimit
        }
    }

    use
    {
        MinOccurs=1
        MaxOccurs=1
        ValType=Int
        ExistsIn(Abs)=[ "../list/value" ]
    }

    food
    {
        MinOccurs=0
        MaxOccurs=1
        ValType=String
        ValEnums=[ apple orange kiwi potato tomato ]
    }
}
