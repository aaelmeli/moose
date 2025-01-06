test{

    container{
        inside{
            IncreaseOver("../..")=Strict
        }
        badrule{
            inside{
                IncreaseOver("../..")=Neither
            }
        }
    }
    array{
        value{
            IncreaseOver("..")=Mono
        }
    }
    another_array{
        value{
            IncreaseOver("..")=Strict
        }
    }

}