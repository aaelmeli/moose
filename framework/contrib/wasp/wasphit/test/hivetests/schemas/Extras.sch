test{

    should_exist_one{
        MinOccurs=1
    }
    should_exist_two{
        MinOccurs=1
    }
    should_exist_three{
        MinOccurs=1
        value{
            MinOccurs=1
        }
    }
    should_exist_four{
        MinOccurs=1
        value{
            MinOccurs=1
        }
    }
    invalid_rule{
        inside{
            BadRuleName=10
        }
    }


}