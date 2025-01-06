test{

    container{
        inside{
            DecreaseOver("../..")=Strict
        }
        badrule{
            inside{
                DecreaseOver("../..")=Neither
            }
        }
    }
    array{
        value{
            DecreaseOver("..")=Mono
        }
    }
    another_array{
        value{
            DecreaseOver("..")=Strict
        }
    }

}