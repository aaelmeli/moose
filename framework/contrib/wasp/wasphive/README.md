# HIVE

The Hierarchical Input Validation Engine (HIVE) uses a set of rules to describe the schema of an application's input. These rules describe scalar and relational input restrictions. They can use a [Sequence Input Retrieval Engine (SIREN) Expression](/waspsiren/README.md#sequence-input-retrieval-engine-siren) path to define restrictions related to relative sets of input elements. Schema files for HIVE are written using the [Standard Object Notation (SON) Syntax](/waspson/README.md#standard-object-notation-son). Applications use HIVE and schema files to facilitate input validation, introspection, and input creation assistance. SIREN Expressions, SON Syntax, and Template Files are beyond the scope of this section.

The section layout is as follows:

* The [Input Validation Rules Summary](#input-validation-rules-summary) section provides brief descriptions of each input validation rule. These rules do not contain defaults. The rules are only used when they have been specified.

* The [Input Validation Details and Examples](#input-validation-details-and-examples) section provides a more detailed description, examples, and exact syntax of input validation rule. This section supplies an example schema, an example input that will pass validation against the schema, an example input that will fail validation against the schema, and the validation messages that HIVE produces when validating the failing input against the provided schema. If the user is incorporating a specific rule for an application, then the examples section for that particular rule should be fully understood syntactically and semantically.

* The [Input Assistance Details](#input-assistance-details) section provides descriptions and details of the rules that may be used by input generation applications for input assistance and autocompletion.

In this document, the term ***input*** is used when referring to a file that is to be validated, and ***schema*** is used when referring to the file that describes the definition and rules against which the input is validated. Currently, schema files must be written in the SON syntax, which is used herein for example input files.


## **Input Validation Rules Summary**

* [***MinOccurs***](#minoccurs-details-and-examples): describes the minimum number of times that an element is allowed to appear under its parent context.
* [***MaxOccurs***](#maxoccurs-details-and-examples): describes the maximum number of times that an element is allowed to appear under its parent context.
* [***ValType***](#valtype-details-and-examples): describes the allowed value type for the element (Int, Real, String).
* [***ValEnums***](#valenums-details-and-examples): describes a list of allowed value choices for the element.
* [***MinValInc***](#minvalinc-details-and-examples): describes the minimum inclusive value that this element is allowed to have if it is a number (the provided input value must be greater than or equal to this).
* [***MaxValInc***](#maxvalinc-details-and-examples): describes the maximum inclusive value that this element is allowed to have if it is a number (the provided input value must be less than or equal to this).
* [***MinValExc***](#minvalexc-details-and-examples): describes the minimum exclusive value of the element in the input if it is a number (the provided input value must be strictly greater than this).
* [***MaxValExc***](#maxvalexc-details-and-examples): describes the maximum exclusive value of the element in the input if it is a number (the provided input value must be strictly less than this).
* [***ExistsIn***](#existsin-details-and-examples): describes a set of lookup paths into relative sections of the input file and possible constant values where the value of the element being validated must exist.
* [***NotExistsIn***](#notexistsin-details-and-examples): describes a set of lookup paths into relative sections of the input file where the value of the element being validated must not exist.
* [***SumOver***](#sumover-details-and-examples): describes what sum the values must add to under a given context.
* [***SumOverGroup***](#sumovergroup-details-and-examples): describes what sum the values must add to under a given context when grouped by dividing another input element's value by a given value.
* [***IncreaseOver***](#increaseover-details-and-examples): describes that the values under the element must be increasing in the order that they are read.
* [***DecreaseOver***](#decreaseover-details-and-examples): describes that the values under the element must be decreasing in the order that they are read.
* [***ChildAtMostOne***](#childatmostone-details-and-examples): describes one or more lists of lookup paths into relative sections of the input file (and possible values) where at most one is allowed to exist.
* [***ChildExactlyOne***](#childexactlyone-details-and-examples): describes one or more lists of lookup paths into relative sections of the input file (and possible values) where at exactly one is allowed to exist.
* [***ChildAtLeastOne***](#childatleastone-details-and-examples): describes one or more lists of lookup paths into relative sections of the input file (and possible values) where at least one must exist.
* [***ChildCountEqual***](#childcountequal-details-and-examples): describes one or more lists of lookup paths into relative sections of the input file where the number of values must be equal.
* [***ChildUniqueness***](#childuniqueness-details-and-examples): describes one or more lists of lookup paths into relative sections of the input file where the values at all of these paths must be unique.



## **Input Validation Details and Examples**

### Miscellaneous Details and Examples

Before exploring the details of all of the validation rules, the first thing to note is that the hierarchy of a schema file must represent a union of all possible input hierarchies. This is just to say that every hierarchical node in an input file that is to be validated must have an exact mapping to a node at the same hierarchical path in the schema. If there is an element in an input file that does not have an exact mapping to an associated node in the schema, then that element is said to be invalid. Once the hierarchy of the schema is built, then rules can be added to every element for validation. Every element in the input document is represented by a SON object in the schema.  All rules for an element at a given context are represented by either SON flag-values or SON flag-arrays.

Hierarchical nodes in the schema that do not have an associated node in the input are not traversed further. For example, if a schema defines nodes **A**, **B**, and **C** at the root level, but a given input only contains nodes **A** and **C** at its root level, then the rules directly inside of node **B** are examined to check if **B** is a required portion in the input. However, the children of node **B** are not traversed further, because it has been verified that those children are not in the input.

At the basic level, there are two types of validation messages that may be reported by HIVE.

1. Problems with the ***input*** file are reported in the form given below:
```javascript
    line:X column:Y - Validation Error: MESSAGE
```

	*Input validation applications may capture the line and column reported so that the offending input elements can be navigated to simply by clicking on the message.*

2. Problems with the ***schema*** file are reported in the following form:
```javascript
    Validation Error: Invalid Schema Rule: MESSAGE line:X column:Y
```

	*This message example denotes an actual error in the schema file (not the input) at the provided line and column number.*

Schema example:
```javascript
    test{
    
        should_exist_one{
        }
        should_exist_two{
            value{
            }
        }
        invalid_rule{
            inside{
                BadRuleName=10
            }
        }
    
    }
```

Input example that **PASSES** validation on schema above:
```javascript
    test{
    
        should_exist_one   = 1
        should_exist_two   = [ 2 3 4 5 ]
    
    }
```

  - ***Notes:*** This input passes validation against the provided schema because both input elements (i.e., `test/should_exist_one` and `test/should_exist_two`) exist in the schema, and no schema validation rules are broken, as no other rules exist for the input elements provided in the input.

Input example that **FAILS** validation on schema above:
```javascript
    test{
    
        should_not_exist_one   = 21
        should_not_exist_two   = [ 22 23 24 25 ]
        invalid_rule{
            inside=5
        }
    
    }
```

HIVE validation messages that occur when validating the failing input shown above against the schema above:
```javascript
    Validation Error: Invalid Schema Rule: "BadRuleName" line:11 column:13
    
    line:3 column:5 - Validation Error: /test/should_not_exist_one is not a valid piece of input
    
    line:4 column:5 - Validation Error: /test/should_not_exist_two is not a valid piece of input
```

  - ***Notes:*** This input fails to validate against the provided schema because, as described above, neither `/test/should_not_exist_one` nor `/test/should_not_exist_two` exist in the schema.  Also, an element exists in the input that has an invalid rule, named `BadRuleName` in the schema.

#### Unknown Node Validation
There are instances where the input parser can generate a parse tree that has unknown nodes. I.e., the syntax is correct, but the data is unknown. 

In this situation the element can be added to the schema and explicitly tagged as `UNKNOWN`. An example schema follows:

```javascript
test{
    some_element(UNKNOWN) { ... }
}
```
When this schema is applied to an input where `test/some_element` exists a validation error will be emitted for each occurrence of `/test/some_element` within
the input. An example validation error message follows:

```javascript
line: 4 column: 5 - Validation Error: some_element is unknown
line: 5 column: 5 - Validation Error: some_element is unknown
```

When the element tagged as `UNKNOWN` is a terminal element, I.e. no children, the element's data is included in the message.

```javascript
test{
    some_element { value(UNKNOWN) {} }
}
```

```javascript
line: 4 column: 16 - Validation Error: value 'data1' is unknown
line: 5 column: 16 - Validation Error: value 'data2' is unknown
```

### MinOccurs Details and Examples

The ***Minimum Occurrence*** rule describes the minimum number of times that an element must occur under its parent context. It is used mostly to denote whether a piece of input is required or optional. Most often, this rule will have a literal constant for minimum allowances. The value must be an integer. For example, `MinOccurs = 0` denotes that this element is optional under its parent context, and `MinOccurs = 1` denotes that this element is required to occur at least once under its parent. If a negative number is specified for the value of this rule, then it is treated the same as `MinOccurs = 0`. This rule may also contain a relative input lookup path from the element being validated. The syntax for this usage is `MinOccurs = "../../some/relative/input/path"`. If the lookup path describes a set containing a single value, and if that value is an integer, then that value will be used to determine the minimum allowed occurrences of the element being validated.

Schema example:
```javascript
    test{
    
        control{
        }
        bad_two_numbers{
        }
        bad_real{
        }
        bad_string{
        }
        valueone{
            MinOccurs=10
        }
        valuetwo{
            MinOccurs="../control"
        }
        valuethree{
            inside{
                MinOccurs=-5
            }
        }
        value_bad_one{
            inside{
                MinOccurs="../../bad_two_numbers"
            }
        }
        value_bad_two{
            inside{
                MinOccurs="../../bad_real"
            }
        }
        value_bad_three{
            inside{
                MinOccurs="../../bad_string"
            }
        }
    
    }
```

Input example that **PASSES** validation on schema above:
```javascript
    test{
    
        control=15
    
        valueone=1
        valueone=2
        valueone=3
        valueone=4
        valueone=5
        valueone=6
        valueone=7
        valueone=8
        valueone=9
        valueone=10
    
        valuetwo=1
        valuetwo=2
        valuetwo=3
        valuetwo=4
        valuetwo=5
        valuetwo=6
        valuetwo=7
        valuetwo=8
        valuetwo=9
        valuetwo=10
        valuetwo=11
        valuetwo=12
        valuetwo=13
        valuetwo=14
        valuetwo=15
    
    }
```

  - ***Notes:*** This input passes validation against the provided schema because `valueone` must occur at least 10 times under its parent context, and it does.  Also, `valuetwo` must occur at least a number of times equal to whatever integer is location at a relative location of  `"../control"`.  A relative lookup from `valuetwo` to `"../control"` yields one integer with the value `15`.  `valuetwo` exists under its parent context at least `15` times, so all is well.

Input example that **FAILS** validation on schema above:
```javascript
    test{
    
        control=15
    
        valueone=1
        valueone=2
        valueone=3
        valueone=4
        valueone=5
        valueone=6
        valueone=7
        valueone=8
        valueone=9
    
        valuetwo=1
        valuetwo=2
        valuetwo=3
        valuetwo=4
        valuetwo=5
        valuetwo=6
        valuetwo=7
        valuetwo=8
        valuetwo=9
        valuetwo=10
        valuetwo=11
        valuetwo=12
        valuetwo=13
        valuetwo=14
    
    }
    
    test{
        bad_two_numbers=6
        bad_two_numbers=7
        bad_real=8.2
        bad_string='some_string'
        valuethree{
        }
        value_bad_one{
        }
        value_bad_two{
        }
        value_bad_three{
        }
    }
```

HIVE validation messages that occur when validating the failing input shown above against the schema above:
```javascript
    line:1 column:1 - Validation Error: test has 9 "valueone" occurrences - when there should be a minimum occurrence of 10

    line:1 column:1 - Validation Error: test has 14 "valuetwo" occurrences - when there should be a minimum occurrence of "15" from "../control"

    line:32 column:1 - Validation Error: test has 0 "valueone" occurrences - when there should be a minimum occurrence of 10

    line:37 column:5 - Validation Error: valuethree has 0 "inside" occurrences - when there should be a minimum occurrence of -5

    line:39 column:5 - Validation Error: inside minimum occurrence checks against "../../bad_two_numbers" which returns more than one value

    line:41 column:5 - Validation Error: inside minimum occurrence checks against "../../bad_real" which does not return a valid number

    line:43 column:5 - Validation Error: inside minimum occurrence checks against "../../bad_string" which does not return a valid number
```

  - ***Notes:*** This input fails to validate against the provided schema because `valueone` only occurs 9 times under its parent context, when its `MinOccurs` rule in the schema denotes that it should occur at least 10 times. `valuetwo` should occur at least 15 times under its parent context, because its `MinOccurs` rules in the schema contains a path to `"../control"`. A relative lookup from `valuetwo` to `"../control"` yields one integer with the value `15`.  However, `valuetwo` only occurs 14 times under its parent.  The second `test` element in the input has zero `valueone` elements when there should be at least 10 as previously described.


### MaxOccurs Details and Examples

The ***Maximum Occurrence*** rule describes the maximum number of times that an element is allowed to occur under its parent context. Most often, this element will have a literal constant value to describe a number of maximum allowances. The value must be integer or '**NoLimit**' (indicating that there is no upper limit on the number of times this element can occur). This rule may also have a relative input lookup path from the element being validated. If the lookup path describes a set containing a single value, and if that value is an integer, then that value will be used to determine the maximum allowed occurrences of the element being validated.

Schema example:
```javascript
    test{
    
        MaxOccurs=NoLimit
        control{
        }
        bad_two_numbers{
        }
        bad_real{
        }
        bad_string{
        }
        valueone{
            MaxOccurs=10
        }
        valuetwo{
            MaxOccurs="../control"
        }
        value_bad_one{
            inside{
                MinOccurs="../../bad_two_numbers"
            }
        }
        value_bad_two{
            inside{
                MinOccurs="../../bad_real"
            }
        }
        value_bad_three{
            inside{
                MinOccurs="../../bad_string"
            }
        }
    
    }
```

Input example that **PASSES** validation on schema above:
```javascript
    test{
    
        control=15
    
        valueone=1
        valueone=2
        valueone=3
        valueone=4
        valueone=5
        valueone=6
        valueone=7
        valueone=8
        valueone=9
        valueone=10
    
        valuetwo=1
        valuetwo=2
        valuetwo=3
        valuetwo=4
        valuetwo=5
        valuetwo=6
        valuetwo=7
        valuetwo=8
        valuetwo=9
        valuetwo=10
        valuetwo=11
        valuetwo=12
        valuetwo=13
        valuetwo=14
        valuetwo=15
    
    }
```


Input example that **FAILS** validation on schema above:
```javascript
    test{
    
        control=15
    
        valueone=1
        valueone=2
        valueone=3
        valueone=4
        valueone=5
        valueone=6
        valueone=7
        valueone=8
        valueone=9
        valueone=10
        valueone=11
    
        valuetwo=1
        valuetwo=2
        valuetwo=3
        valuetwo=4
        valuetwo=5
        valuetwo=6
        valuetwo=7
        valuetwo=8
        valuetwo=9
        valuetwo=10
        valuetwo=11
        valuetwo=12
        valuetwo=13
        valuetwo=14
        valuetwo=15
        valuetwo=16
    
    }
    
    test{
        valueone=1
        valueone=2
        valueone=3
        valueone=4
        valueone=5
        valueone=6
        valueone=7
        valueone=8
        valueone=9
        valueone=10
        valueone=11
        valueone=12
        valueone=13
        valueone=14
    }
    test{
        control=2
        bad_two_numbers=6
        bad_two_numbers=7
        bad_real=8.2
        bad_string='some_string'
        valueone=1
        valuetwo=1
        valuetwo=2
        valuetwo=3
        valuetwo=4
        value_bad_one{
        }
        value_bad_two{
        }
        value_bad_three{
        }
    }
```

HIVE validation messages that occur when validating the failing input shown above against the schema above:
```javascript
    line:1 column:1 - Validation Error: test has 11 "valueone" occurrences - when there should be a maximum occurrence of 10

    line:1 column:1 - Validation Error: test has 16 "valuetwo" occurrences - when there should be a maximum occurrence of "15" from "../control"

    line:36 column:1 - Validation Error: test has 14 "valueone" occurrences - when there should be a maximum occurrence of 10

    line:52 column:1 - Validation Error: test has 4 "valuetwo" occurrences - when there should be a maximum occurrence of "2" from "../control"

    line:63 column:5 - Validation Error: inside minimum occurrence checks against "../../bad_two_numbers" which returns more than one value

    line:65 column:5 - Validation Error: inside minimum occurrence checks against "../../bad_real" which does not return a valid number

    line:67 column:5 - Validation Error: inside minimum occurrence checks against "../../bad_string" which does not return a valid number
```


### ValType Details and Examples

The ***Value Type*** rule checks the type of the element value in the input. This can be one of the following: 
* `Int` - meaning a negative or positive integer
* `Real` - meaning a negative or positive floating point value (or integer)
* `String` - meaning a literal string of text

Schema example:
```javascript
    test{
    
        one{
            ValType=Int
        }
        two{
            ValType=Int
        }
        three{
            ValType=Int
        }
        four{
            ValType=Real
        }
        five{
            ValType=Real
        }
        six{
            ValType=Real
        }
        seven{
            ValType=String
        }
        eight{
            ValType=String
        }
        nine{
            inside{
                ValType=BadType
            }
        }
    }
```

Input example that **PASSES** validation on schema above:
```javascript
    test{
        one=-8
        two=0
        three=83
        four=-9.4
        five=3
        six='+9e-3'
        seven=ThisIsAString
        eight="This Is Also A String"
    }
```

Input example that **FAILS** validation on schema above:
```javascript
    test{
        one=-8.3
        two=0.3
        three="+8e-3"
        four='*'
        five=StringHere
        six='another string here'
        seven=4.5
        eight=5E-4
        nine='hello world'
    }
```

HIVE validation messages that occur when validating the failing input shown above against the schema above:
```javascript
    Validation Error: Invalid Schema Rule: Bad ValType Option "BadType" at line:29 column:21 - Expected [ Int Real String ]

    line:2 column:5 - Validation Error: one value "-8.3" is not of type Int

    line:3 column:5 - Validation Error: two value "0.3" is not of type Int

    line:4 column:5 - Validation Error: three value "+8e-3" is not of type Int

    line:5 column:5 - Validation Error: four value "*" is not of type Real

    line:6 column:5 - Validation Error: five value "StringHere" is not of type Real

    line:7 column:5 - Validation Error: six value "another string here" is not of type Real
```


### ValEnums Details and Examples

The ***Value Enumerations*** rule contains a static list of values choices. It compares the element's input value with the provided choices. If the element's value is not in the schema's list of allowed enumerations, then this check will fail. Also, a `REF:` construct may be used to reference a SON array of values that must exist in the schema after an  `EndOfSchema{}` declaration.  These referenced SON arrays can be conveniently defined in one place but be used by `ValEnums` rules on many different elements. If a validation message is produced, then a short list of closest matches is provided to the user alphabetically. Note that this check is case insensitive, and if the value that is being checked is an integer, then leading zeros are ignored.

Schema example:
```javascript
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
```

Input example that **PASSES** validation on schema above:
```javascript
    test{
        one="yes"
        two='Maybe'
        three=blue
        four=4
        five=oRanGe
        five=0002
    }
```

Input example that **FAILS** validation on schema above:
```javascript
    test{
        one=red
        two="Green"
        three=yes
        four=-4
        five=007
        six=something
    }
```

HIVE validation messages that occur when validating the failing input shown above against the schema above:
```javascript
    Validation Error: Invalid Schema Rule: Enum Reference "BadReference" at line:19 column:20 not found in schema

    line:2 column:5 - Validation Error: one value "red" is not one of the allowed values: [ "maybe" "no" "yes" ]

    line:3 column:5 - Validation Error: two value "green" is not one of the allowed values: [ "maybe" "no" "yes" ]

    line:4 column:5 - Validation Error: three value "yes" is not one of the allowed values: [ ... "green" "indigo" "orange" "red" "violet" "yellow" ]

    line:5 column:5 - Validation Error: four value "-4" is not one of the allowed values: [ "1" "2" "3" "4" "5" ]

    line:6 column:5 - Validation Error: five value "7" is not one of the allowed values: [ ... "3" "4" "5" "blue" "green" "indigo" ... ]
```


### MinValInc Details and Examples

The ***Minimum Value Inclusive*** rule provides a number (real or integer) to which the associated input value must be greater than or equal. Most often, this rule will contain a constant number defining the minimum allowable value for this element. For example, *MinValInc = 0.0* denotes that this element's value must be zero or greater. This rule may also have a relative input lookup path from the element being validated. If the set in the input represented by the relative path is a single value, and if that value is a number, then that value will be used to determine the lowest allowed value for the element being validated. If an element at this relative lookup path exists in the input and it is not a number, then it will fail this check. However, if this element does not exist at all in the input, then this validation check is delegated to the `MinOccurs` check and will not fail.

Schema example:
```javascript
    test{
    
        controlone{
        }
        controltwo{
        }
        bad_two_numbers{
        }
        bad_string{
        }
        valueone{
            MinValInc=58.7
        }
        valuetwo{
            value{
                MinValInc=58.7
            }
        }
        valuethree{
            MinValInc=23
        }
        valuefour{
            value{
                MinValInc=23
            }
        }
        valuefive{
            MinValInc="../controlone"
        }
        valuesix{
            value{
                MinValInc="../../controlone"
            }
        }
        valueseven{
            MinValInc="../controltwo"
        }
        valueeight{
            value{
                MinValInc="../../controltwo"
            }
        }
        value_bad_one{
            inside{
                MinValInc="../../bad_two_numbers"
            }
        }
        value_bad_two{
            inside{
                MinValInc="../../bad_string"
            }
        }
    
    }
```

Input example that **PASSES** validation on schema above:
```javascript
    test{
        controlone=15
        controltwo=-45.3
        valueone=58.7
        valuetwo=[ 65 66 67 68 58.7 ]
        valuethree=23
        valuefour=[ 38.3 30.3 23 32.34 ]
        valuefive=15
        valuesix=[ 21 22 23 24 15 ]
        valueseven=-45.3
        valueeight=[ -32.4 31.9 -30.3 -45.3 ]
    }
```

Input example that **FAILS** validation on schema above:
```javascript
    test{
        controlone=15
        controltwo=-45.3
        bad_two_numbers=6
        bad_two_numbers=7
        bad_string='some_string'
        valueone=58.6
        valuetwo=[ 65 56 58.6 58 88.7 ]
        valuethree=22.9
        valuefour=[ 28.3 20.3 22.9 12.34 2e2 ]
        valuefive=14
        valuesix=[ 11 12 15 14 15.1 ]
        valueseven=-45.4
        valueeight=[ -45.4 -41.9 -100.3 -45.3 -4E-8 -7e+3 ]
        value_bad_one{
            inside=47
        }
        value_bad_two{
            inside=48
        }
        valueone='a-string'
    }
```

HIVE validation messages that occur when validating the failing input shown above against the schema above:
```javascript
    line:7 column:5 - Validation Error: valueone value "58.6" is less than the allowed minimum inclusive value of 58.7

    line:8 column:19 - Validation Error: valuetwo value "56" is less than the allowed minimum inclusive value of 58.7

    line:8 column:22 - Validation Error: valuetwo value "58.6" is less than the allowed minimum inclusive value of 58.7

    line:8 column:27 - Validation Error: valuetwo value "58" is less than the allowed minimum inclusive value of 58.7

    line:9 column:5 - Validation Error: valuethree value "22.9" is less than the allowed minimum inclusive value of 23

    line:10 column:22 - Validation Error: valuefour value "20.3" is less than the allowed minimum inclusive value of 23

    line:10 column:27 - Validation Error: valuefour value "22.9" is less than the allowed minimum inclusive value of 23

    line:10 column:32 - Validation Error: valuefour value "12.34" is less than the allowed minimum inclusive value of 23

    line:11 column:5 - Validation Error: valuefive value "14" is less than the allowed minimum inclusive value of "15" from "../controlone"

    line:12 column:16 - Validation Error: valuesix value "11" is less than the allowed minimum inclusive value of "15" from "../../controlone"

    line:12 column:19 - Validation Error: valuesix value "12" is less than the allowed minimum inclusive value of "15" from "../../controlone"

    line:12 column:25 - Validation Error: valuesix value "14" is less than the allowed minimum inclusive value of "15" from "../../controlone"

    line:13 column:5 - Validation Error: valueseven value "-45.4" is less than the allowed minimum inclusive value of "-45.3" from "../controltwo"

    line:14 column:18 - Validation Error: valueeight value "-45.4" is less than the allowed minimum inclusive value of "-45.3" from "../../controltwo"

    line:14 column:30 - Validation Error: valueeight value "-100.3" is less than the allowed minimum inclusive value of "-45.3" from "../../controltwo"

    line:14 column:49 - Validation Error: valueeight value "-7e+3" is less than the allowed minimum inclusive value of "-45.3" from "../../controltwo"

    line:16 column:9 - Validation Error: inside minimum inclusive value checks against "../../bad_two_numbers" which returns more than one value

    line:19 column:9 - Validation Error: inside minimum inclusive value checks against "../../bad_string" which does not return a valid number

    line:21 column:5 - Validation Error: valueone value "a-string" is wrong value type for minimum inclusive value
```


### MaxValInc Details and Examples

The ***Maximum Value Inclusive*** rule provides a number (real or integer) to which the associated input value must be less than or equal. Most often, this rule will contain a constant number defining the maximum allowable value this element. For example, *MaxValInc = 0.0* denotes that this element's value must be zero or less. This rule may also have a relative input lookup path from the element being validated. If the set in the input represented by the relative path is a single value, and if that value is a number, then that value will be used to determine the highest allowed value for the element being validated. If an element at this relative lookup path exists in the input and it is not a number, then it will fail this check. However, if this element does not exist at all in the input, then this validation check is delegated to the `MinOccurs` check and will not fail.

Schema example:
```javascript
    test{
    
        controlone{
        }
        controltwo{
        }
        bad_two_numbers{
        }
        bad_string{
        }
        valueone{
            MaxValInc=58.7
        }
        valuetwo{
            value{
                MaxValInc=58.7
            }
        }
        valuethree{
            MaxValInc=23
        }
        valuefour{
            value{
                MaxValInc=23
            }
        }
        valuefive{
            MaxValInc="../controlone"
        }
        valuesix{
            value{
                MaxValInc="../../controlone"
            }
        }
        valueseven{
            MaxValInc="../controltwo"
        }
        valueeight{
            value{
                MaxValInc="../../controltwo"
            }
        }
        value_bad_one{
            inside{
                MaxValInc="../../bad_two_numbers"
            }
        }
        value_bad_two{
            inside{
                MaxValInc="../../bad_string"
            }
        }
    
    }
```

Input example that **PASSES** validation on schema above:
```javascript
    test{
        controlone=15
        controltwo=-45.3
        valueone=58.7
        valuetwo=[ 55 56 57 58 58.7 ]
        valuethree=23
        valuefour=[ 18.3 20.3 23 12.34 ]
        valuefive=15
        valuesix=[ 11 12 13 14 15 ]
        valueseven=-45.3
        valueeight=[ -52.4 -51.9 -100.3 -45.3 ]
    }
```

Input example that **FAILS** validation on schema above:
```javascript
    test{
        controlone=15
        controltwo=-45.3
        bad_two_numbers=6
        bad_two_numbers=7
        bad_string='some_string'
        valueone=58.8
        valuetwo=[ 65 56 58.8 58 88.7 ]
        valuethree=23.9
        valuefour=[ 18.3 20.3 23.1 12.34 2e2 ]
        valuefive=19
        valuesix=[ 11 12 18.2 14 15.1 ]
        valueseven=-45.1
        valueeight=[ -52.4 -41.9 -100.3 -45.3 -4E-8 -7e+3 ]
        value_bad_one{
            inside=47
        }
        value_bad_two{
            inside=48
        }
        valueone='a-string'
    }
```

HIVE validation messages that occur when validating the failing input shown above against the schema above:
```javascript
    line:7 column:5 - Validation Error: valueone value "58.8" is greater than the allowed maximum inclusive value of 58.7

    line:8 column:16 - Validation Error: valuetwo value "65" is greater than the allowed maximum inclusive value of 58.7

    line:8 column:22 - Validation Error: valuetwo value "58.8" is greater than the allowed maximum inclusive value of 58.7

    line:8 column:30 - Validation Error: valuetwo value "88.7" is greater than the allowed maximum inclusive value of 58.7

    line:9 column:5 - Validation Error: valuethree value "23.9" is greater than the allowed maximum inclusive value of 23

    line:10 column:27 - Validation Error: valuefour value "23.1" is greater than the allowed maximum inclusive value of 23

    line:10 column:38 - Validation Error: valuefour value "2e2" is greater than the allowed maximum inclusive value of 23

    line:11 column:5 - Validation Error: valuefive value "19" is greater than the allowed maximum inclusive value of "15" from "../controlone"

    line:12 column:22 - Validation Error: valuesix value "18.2" is greater than the allowed maximum inclusive value of "15" from "../../controlone"

    line:12 column:30 - Validation Error: valuesix value "15.1" is greater than the allowed maximum inclusive value of "15" from "../../controlone"

    line:13 column:5 - Validation Error: valueseven value "-45.1" is greater than the allowed maximum inclusive value of "-45.3" from "../controltwo"

    line:14 column:24 - Validation Error: valueeight value "-41.9" is greater than the allowed maximum inclusive value of "-45.3" from "../../controltwo"

    line:14 column:43 - Validation Error: valueeight value "-4E-8" is greater than the allowed maximum inclusive value of "-45.3" from "../../controltwo"

    line:16 column:9 - Validation Error: inside maximum inclusive value checks against "../../bad_two_numbers" which returns more than one value

    line:19 column:9 - Validation Error: inside maximum inclusive value checks against "../../bad_string" which does not return a valid number

    line:21 column:5 - Validation Error: valueone value "a-string" is wrong value type for maximum inclusive value
```


### MinValExc Details and Examples

The ***Minimum Value Exclusive*** rule provides a number (real or integer) to which the associated input value must be greater. Most often, this rule will contain a constant number, and the associated input value must be greater than this number. For example, *MinValExc = 0.0* denotes that this element value must be greater than zero (not equal). This rule may also have a relative input lookup path from the element being validated. If the set in the input represented by the relative path is a single value, and if that value is a number, then that value will be used to determine the minimum exclusive allowed input value. If an element at this relative lookup path exists in the input and it is not a number, then it will fail this check. However, if this element does not exist at all in the input, then this validation check is delegated to the `MinOccurs` check and will not fail.

Schema example:
```javascript
    test{
    
        controlone{
        }
        controltwo{
        }
        bad_two_numbers{
        }
        bad_string{
        }
        valueone{
            MinValExc=58.7
        }
        valuetwo{
            value{
                MinValExc=58.7
            }
        }
        valuethree{
            MinValExc=23
        }
        valuefour{
            value{
                MinValExc=23
            }
        }
        valuefive{
            MinValExc="../controlone"
        }
        valuesix{
            value{
                MinValExc="../../controlone"
            }
        }
        valueseven{
            MinValExc="../controltwo"
        }
        valueeight{
            value{
                MinValExc="../../controltwo"
            }
        }
        valuenine{
            MinValExc=NoLimit
        }
        value_bad_one{
            inside{
                MinValExc="../../bad_two_numbers"
            }
        }
        value_bad_two{
            inside{
                MinValExc="../../bad_string"
            }
        }
    
    }
```

Input example that **PASSES** validation on schema above:
```javascript
    test{
        controlone=15
        controltwo=-45.3
        valueone=58.8
        valuetwo=[ 65 66 67 68 58.8 ]
        valuethree=23.1
        valuefour=[ 38.3 30.3 23.1 32.34 ]
        valuefive=16
        valuesix=[ 21 22 23 24 16 ]
        valueseven=-45.2
        valueeight=[ -32.4 31.9 -30.3 -45.2 ]
        valuenine=-2000.90
    }
```


Input example that **FAILS** validation on schema above:
```javascript
    test{
        controlone=15
        controltwo=-453E-1
        bad_two_numbers=6
        bad_two_numbers=7
        bad_string='some_string'
        valueone=58.7
        valuetwo=[ 65E-1 66 7 68 58.7 ]
        valuethree=23
        valuefour=[ 383E-2 3.3 23 32.34 ]
        valuefive=15
        valuesix=[ -21 22 2.3E-4 24 15 ]
        valueseven=-45.3
        valueeight=[ -132.4 -3.19E5 -30.3 -45.3 ]
        valuenine=-2000.90
        value_bad_one{
            inside=47
        }
        value_bad_two{
            inside=48
        }
        valueone='a-string'
    }
```

HIVE validation messages that occur when validating the failing input shown above against the schema above:
```javascript
    line:7 column:5 - Validation Error: valueone value "58.7" is less than or equal to the allowed minimum exclusive value of 58.7

    line:8 column:16 - Validation Error: valuetwo value "65E-1" is less than or equal to the allowed minimum exclusive value of 58.7

    line:8 column:25 - Validation Error: valuetwo value "7" is less than or equal to the allowed minimum exclusive value of 58.7

    line:8 column:30 - Validation Error: valuetwo value "58.7" is less than or equal to the allowed minimum exclusive value of 58.7

    line:9 column:5 - Validation Error: valuethree value "23" is less than or equal to the allowed minimum exclusive value of 23

    line:10 column:17 - Validation Error: valuefour value "383E-2" is less than or equal to the allowed minimum exclusive value of 23

    line:10 column:24 - Validation Error: valuefour value "3.3" is less than or equal to the allowed minimum exclusive value of 23

    line:10 column:28 - Validation Error: valuefour value "23" is less than or equal to the allowed minimum exclusive value of 23

    line:11 column:5 - Validation Error: valuefive value "15" is less than or equal to the allowed minimum exclusive value of "15" from "../controlone"

    line:12 column:16 - Validation Error: valuesix value "-21" is less than or equal to the allowed minimum exclusive value of "15" from "../../controlone"

    line:12 column:23 - Validation Error: valuesix value "2.3E-4" is less than or equal to the allowed minimum exclusive value of "15" from "../../controlone"

    line:12 column:33 - Validation Error: valuesix value "15" is less than or equal to the allowed minimum exclusive value of "15" from "../../controlone"

    line:13 column:5 - Validation Error: valueseven value "-45.3" is less than or equal to the allowed minimum exclusive value of "-453E-1" from "../controltwo"

    line:14 column:18 - Validation Error: valueeight value "-132.4" is less than or equal to the allowed minimum exclusive value of "-453E-1" from "../../controltwo"

    line:14 column:25 - Validation Error: valueeight value "-3.19E5" is less than or equal to the allowed minimum exclusive value of "-453E-1" from "../../controltwo"

    line:14 column:39 - Validation Error: valueeight value "-45.3" is less than or equal to the allowed minimum exclusive value of "-453E-1" from "../../controltwo"

    line:17 column:9 - Validation Error: inside minimum exclusive value checks against "../../bad_two_numbers" which returns more than one value

    line:20 column:9 - Validation Error: inside minimum exclusive value checks against "../../bad_string" which does not return a valid number

    line:22 column:5 - Validation Error: valueone value "a-string" is wrong value type for minimum exclusive value
```


### MaxValExc Details and Examples

The ***Maximum Value Exclusive*** rule provides a number (real or integer) to which the associated input value must be less. Most often, this rule will contain a constant number, and the associated input value must be less than this number. For example, *MaxValExc = 0.0* denotes that this element value must be less than zero (not equal). This rule may also have a relative input lookup path from the element being validated. If the set in the input represented by the relative path is a single value, and if that value is a number, then that value will be used to determine the maximum exclusive allowed input value. If an element at this relative lookup path exists in the input and it is not a number, then it will fail this check. However, if this element does not exist at all in the input, then this validation check is delegated to the `MinOccurs` check and will not fail.

Schema example:
```javascript
    test{
    
        controlone{
        }
        controltwo{
        }
        bad_two_numbers{
        }
        bad_string{
        }
        valueone{
            MaxValExc=58.7
        }
        valuetwo{
            value{
                MaxValExc=58.7
            }
        }
        valuethree{
            MaxValExc=23
        }
        valuefour{
            value{
                MaxValExc=23
            }
        }
        valuefive{
            MaxValExc="../controlone"
        }
        valuesix{
            value{
                MaxValExc="../../controlone"
            }
        }
        valueseven{
            MaxValExc="../controltwo"
        }
        valueeight{
            value{
                MaxValExc="../../controltwo"
            }
        }
        valuenine{
            MaxValExc=NoLimit
        }
        value_bad_one{
            inside{
                MaxValExc="../../bad_two_numbers"
            }
        }
        value_bad_two{
            inside{
                MaxValExc="../../bad_string"
            }
        }
    
    }
```

Input example that **PASSES** validation on schema above:
```javascript
    test{
        controlone=15.1
        controltwo=-452E-1
        valueone=58.6
        valuetwo=[ 55 56 57 58 58.6 ]
        valuethree=22.9
        valuefour=[ 18.3 20.3 22.9 12.34 ]
        valuefive=15
        valuesix=[ 11 12 13 14 15 ]
        valueseven=-45.3
        valueeight=[ -52.4 -51.9 -100.3 -45.3 ]
        valuenine=2000.90
    }
```

Input example that **FAILS** validation on schema above:
```javascript
    test{
        controlone=15
        controltwo=-453e-1
        bad_two_numbers=6
        bad_two_numbers=7
        bad_string='some_string'
        valueone=58.7
        valuetwo=[ 65 59 57 58 58.7 ]
        valuethree=23
        valuefour=[ 18.3 29.3 23 12.34 ]
        valuefive=15
        valuesix=[ 11 12 13 14 15 17.3 ]
        valueseven=-45.3
        valueeight=[ -52.4 -51.9 -10.3 -45.3 ]
        valuenine=2000.90
        value_bad_one{
            inside=47
        }
        value_bad_two{
            inside=48
        }
        valueone='a-string'
    }
```

HIVE validation messages that occur when validating the failing input shown above against the schema above:
```javascript
    line:7 column:5 - Validation Error: valueone value "58.7" is greater than or equal to the allowed maximum exclusive value of 58.7

    line:8 column:16 - Validation Error: valuetwo value "65" is greater than or equal to the allowed maximum exclusive value of 58.7

    line:8 column:19 - Validation Error: valuetwo value "59" is greater than or equal to the allowed maximum exclusive value of 58.7

    line:8 column:28 - Validation Error: valuetwo value "58.7" is greater than or equal to the allowed maximum exclusive value of 58.7

    line:9 column:5 - Validation Error: valuethree value "23" is greater than or equal to the allowed maximum exclusive value of 23

    line:10 column:22 - Validation Error: valuefour value "29.3" is greater than or equal to the allowed maximum exclusive value of 23

    line:10 column:27 - Validation Error: valuefour value "23" is greater than or equal to the allowed maximum exclusive value of 23

    line:11 column:5 - Validation Error: valuefive value "15" is greater than or equal to the allowed maximum exclusive value of "15" from "../controlone"

    line:12 column:28 - Validation Error: valuesix value "15" is greater than or equal to the allowed maximum exclusive value of "15" from "../../controlone"

    line:12 column:31 - Validation Error: valuesix value "17.3" is greater than or equal to the allowed maximum exclusive value of "15" from "../../controlone"

    line:13 column:5 - Validation Error: valueseven value "-45.3" is greater than or equal to the allowed maximum exclusive value of "-453e-1" from "../controltwo"

    line:14 column:30 - Validation Error: valueeight value "-10.3" is greater than or equal to the allowed maximum exclusive value of "-453e-1" from "../../controltwo"

    line:14 column:36 - Validation Error: valueeight value "-45.3" is greater than or equal to the allowed maximum exclusive value of "-453e-1" from "../../controltwo"

    line:17 column:9 - Validation Error: inside maximum exclusive value checks against "../../bad_two_numbers" which returns more than one value

    line:20 column:9 - Validation Error: inside maximum exclusive value checks against "../../bad_string" which does not return a valid number

    line:22 column:5 - Validation Error: valueone value "a-string" is wrong value type for maximum exclusive value
```


### ExistsIn Details and Examples

The ***Exists In*** rule is used as a key to stipulate that an element in the input must be defined somewhere else in the input. This rule will always contain one or more relative input lookup paths from the element being validated. The pieces of input at these paths will be collected into a set. This rule may also contain one or more optional constant values. If these exist, then the constant values will also be added to the set. Then, all of the values in the input being validated by this rule must exist in the set built from the lookup paths and the constant values in order to pass the validation. If any element does not exist in this set, then the validation check fails. This rule may use an optional `Abs` modifier flag that can occur as a parenthetical identifier. The `Abs` modifier flag indicates that the absolute value of all numbers added to the set checked for existence are used. Then, even if the value of the element being validated is negative and a value at one of the rule's relative input lookup paths is positive, but they have the same absolute value, this validation check will pass. `EXTRA:` may be used within an `ExistsIn` to specify constant values that are allowed. An `EXTRAREF:` construct may be used to reference a SON array of values that must exist in the schema after an  `EndOfSchema{}` declaration.  The values are also allowed by the `ExistsIn` rule.  These referenced SON arrays can be conveniently defined in one place but be used by `ExistsIn` rules on many different elements. If the allowed `EXTRA` values are actually a contiguous range of integer values, then a `RANGE:[ start end ]` construct may be used for convenience instead of writing a separate `EXTRA:` for every element. These are all shown in the syntax example below. Note that this check is case insensitive, and if the value that is being checked is an integer, then leading zeros are ignored.

Schema example:
```javascript
    test{
    
        defineone{
        }
        definetwo{
        }
        definethree{
        }
        useone{
            value{
                ExistsIn=[ "../../defineone/value"
                           "../../definetwo/value"
                           "../../definethree/value" ]
            }
        }
        usetwo{
            value{
                ExistsIn=[ EXTRA:"ford"
                           EXTRA:"chevy"
                           EXTRA:"bmw"
                           EXTRAREF:ReferencedColors
                           EXTRAREF:ReferencedNumbers
                           "../../defineone/value"
                           "../../definetwo/value"
                           "../../definethree/value" ]
            }
        }
        usethree{
            value{
                ExistsIn(Abs)=[ EXTRAREF:ReferencedNumbers
                                "../../definetwo/value" ]
            }
        }
        usefour_reg{
            value{
                ExistsIn=[ EXTRA:"hamburger"
                           EXTRA:"hotdog"
                           EXTRA:800
                           EXTRA:900
                           "../../defineone/value"
                           "../../definethree/value" ]
            }
        }
        usefive_reg{
            value{
                ExistsIn=[ EXTRAREF:ReferencedColors
                           "../../definetwo/value"
                           "../../definethree/value"
                            EXTRA:0
                            RANGE:[ 1200 1300 ]
                            RANGE:[ 1400 1500 ]
                            RANGE:[ 1600 1700 ]
                            RANGE:[ 1800 1900 ] ]
            }
        }
        usefour_abs{
            value{
                ExistsIn(Abs)=[ EXTRA:"hamburger"
                           EXTRA:"hotdog"
                           EXTRA:800
                           EXTRA:900
                           "../../defineone/value"
                           "../../definethree/value" ]
            }
        }
        usefive_abs{
            value{
                ExistsIn(Abs)=[ EXTRAREF:ReferencedColors
                           "../../definetwo/value"
                           "../../definethree/value"
                            EXTRA:0
                            RANGE:[ 1200 1300 ]
                            RANGE:[ 1400 1500 ]
                            RANGE:[ 1600 1700 ]
                            RANGE:[ 1800 1900 ] ]
            }
        }
        usesix{
            value{
                ExistsIn(BadFlag)=[ "../../defineone/value" ]
            }
        }
        useseven{
            value{
                ExistsIn=[ "../../defineone/value"
                           RANGE:[ 25 fifty ] ]
            }
        }
        useeight{
            value{
                ExistsIn=[ "../../defineone/value"
                           RANGE:[ 50 25 ] ]
            }
        }
        usenine{
            value{
                ExistsIn=[ "../../defineone/value"
                           RANGE:[ 25 50 100 ] ]
            }
        }
        useten{
            value{
                ExistsIn=[ EXTRAREF:BadReference
                           "../../defineone/value" ]
            }
        }
        useeleven{
            value{
                ExistsIn=[ "../../../../defineone/value" ]
            }
        }
    
    }
    EndOfSchema{}
    ReferencedColors=[ red orange yellow green blue indigo violet ]
    ReferencedNumbers=[ 1 2 3 4 5 ]
```

Input example that **PASSES** validation on schema above:
```javascript
    test{
    
        defineone=one
        defineone=two
        defineone=three
        defineone=four
    
        definetwo=[ england spain germany italy canada ]
        definetwo=-200
        definetwo=300
        definetwo=[ 500 -600 ]
    
        definethree=science
        definethree=math
        definethree=[ geography economics recess lunch ]
    
        useone=two
        useone=germany
        useone=[ three recess lunch italy canada ]
    
        usetwo=[ ford bmw red 1 4 math ]
        usetwo=3
        usetwo=blue
    
        usethree=england
        usethree=italy
        usethree=[ 5 "3" -2 canada "1" ]
        usethree=-4
    
        usefour_abs=geography
        usefour_abs=[ hamburger 900 math hotdog "four" ]
        usefour_abs=three
        usefour_abs=[ 800 -800 ]
        usefour_abs=-900
    
        usefive_abs=orange
        usefive_abs=economics
        usefive_abs=[ "indigo" violet "geography" ]
        usefive_abs=science
        usefive_abs=[ 600 -600 300 -300 1200 1300 ]
        usefive_abs=200
        usefive_abs=[ -500 500 -200 -1850 ]
        usefive_abs=-1675
    
    }
```

Input example that **FAILS** validation on schema above:
```javascript
    test{
    
        defineone=one
        defineone=two
        defineone=three
        defineone=four
    
        definetwo=[ england spain germany italy canada ]
        definetwo=-200
        definetwo=300
        definetwo=[ 500 -600 ]
    
        definethree=science
        definethree=math
        definethree=[ geography economics recess lunch ]
    
        useone=seven
        useone=japan
        useone=[ three spelling yellow italy canada 2 ]
    
        usetwo=[ ford honda red -1 4 math ]
        usetwo=-3
        usetwo=purple
    
        usethree=red
        usethree=three
        usethree=[ 5 "3" -2.3 blue "1" ]
        usethree=lunch
    
        usefour_reg=geography
        usefour_reg=[ hamburger 900 spain hotdog fries ]
        usefour_reg=orange
        usefour_reg=[ 800 -800 ]
        usefour_reg=-900
    
        usefive_reg=orange
        usefive_reg=economics
        usefive_reg=[ "indigo" violet "geography" ]
        usefive_reg=science
        usefive_reg=[ 600 2 300 five ]
        usefive_reg=200
        usefive_reg=[ -500 -3 -200 ]
    
        usesix=one
        useseven=two
        useeight=three
        usenine=four
        useten=one
        useeleven=two
    
    }
```

HIVE validation messages that occur when validating the failing input shown above against the schema above:
```javascript
    Validation Error: Invalid Schema Rule: "50" start of range is greater than or equal to "25" end of range at line:92 column:32

    Validation Error: Invalid Schema Rule: Bad ExistsIn Option "BadFlag" at line:80 column:22 - Expected [ Abs ]

    Validation Error: Invalid Schema Rule: Bad ExistsIn Path "../../../../defineone/value" at line:109 column:24

    Validation Error: Invalid Schema Rule: Enum Reference "BadReference" at line:103 column:24 not found in schema

    Validation Error: Invalid Schema Rule: Range does not have exactly two values at line:98 column:24

    Validation Error: Invalid Schema Rule: fifty range value not a valid number at line:86 column:35

    line:17 column:12 - Validation Error: useone value "seven" does not exist in set: [ ../../defineone/value ../../definetwo/value ../../definethree/value ]

    line:18 column:12 - Validation Error: useone value "japan" does not exist in set: [ ../../defineone/value ../../definetwo/value ../../definethree/value ]

    line:19 column:20 - Validation Error: useone value "spelling" does not exist in set: [ ../../defineone/value ../../definetwo/value ../../definethree/value ]

    line:19 column:29 - Validation Error: useone value "yellow" does not exist in set: [ ../../defineone/value ../../definetwo/value ../../definethree/value ]

    line:19 column:49 - Validation Error: useone value "2" does not exist in set: [ ../../defineone/value ../../definetwo/value ../../definethree/value ]

    line:21 column:19 - Validation Error: usetwo value "honda" does not exist in set: [ ../../defineone/value ../../definetwo/value ../../definethree/value ]

    line:21 column:29 - Validation Error: usetwo value "-1" does not exist in set: [ ../../defineone/value ../../definetwo/value ../../definethree/value ]

    line:22 column:12 - Validation Error: usetwo value "-3" does not exist in set: [ ../../defineone/value ../../definetwo/value ../../definethree/value ]

    line:23 column:12 - Validation Error: usetwo value "purple" does not exist in set: [ ../../defineone/value ../../definetwo/value ../../definethree/value ]

    line:25 column:14 - Validation Error: usethree value "red" does not exist in set: [ ../../definetwo/value ]

    line:26 column:14 - Validation Error: usethree value "three" does not exist in set: [ ../../definetwo/value ]

    line:27 column:22 - Validation Error: usethree value "2.3" does not exist in set: [ ../../definetwo/value ]

    line:27 column:27 - Validation Error: usethree value "blue" does not exist in set: [ ../../definetwo/value ]

    line:28 column:14 - Validation Error: usethree value "lunch" does not exist in set: [ ../../definetwo/value ]

    line:31 column:33 - Validation Error: usefour_reg value "spain" does not exist in set: [ ../../defineone/value ../../definethree/value ]

    line:31 column:46 - Validation Error: usefour_reg value "fries" does not exist in set: [ ../../defineone/value ../../definethree/value ]

    line:32 column:17 - Validation Error: usefour_reg value "orange" does not exist in set: [ ../../defineone/value ../../definethree/value ]

    line:33 column:23 - Validation Error: usefour_reg value "-800" does not exist in set: [ ../../defineone/value ../../definethree/value ]

    line:34 column:17 - Validation Error: usefour_reg value "-900" does not exist in set: [ ../../defineone/value ../../definethree/value ]

    line:40 column:19 - Validation Error: usefive_reg value "600" does not exist in set: [ ../../definetwo/value ./../definethree/value ]

    line:40 column:23 - Validation Error: usefive_reg value "2" does not exist in set: [ ../../definetwo/value ./../definethree/value ]

    line:40 column:29 - Validation Error: usefive_reg value "five" does not exist in set: [ ../../definetwo/value ../../definethree/value ]

    line:41 column:17 - Validation Error: usefive_reg value "200" does not exist in set: [ ../../definetwo/value ../../definethree/value ]

    line:42 column:19 - Validation Error: usefive_reg value "-500" does not exist in set: [ ../../definetwo/value ../../definethree/value ]

    line:42 column:24 - Validation Error: usefive_reg value "-3" does not exist in set: [ ../../definetwo/value ../../definethree/value ]
```


### NotExistsIn Details and Examples

The ***Not Exists In*** rule will always contain one or more relative input lookup paths from the element being validated. The pieces of input at these paths will be collected into a set. If the value of the element being validated exists in this set, then this validation check fails. If it does not exist, then the validation check passes. This rule may use an optional `Abs` modifier flag that can occur as a parenthetical identifier. The `Abs` modifier flag indicates that the absolute value of all numbers added to the set checked for existence are used. Then, even if the value of the element being validated is negative and a value at one of the rule's relative input lookup paths is positive, but they have the same absolute value, this validation check will fail. Note that this check is case insensitive, and if the value that is being checked is an integer, then leading zeros are ignored.

Schema example:
```javascript
    test{
    
        defineone{
        }
        definetwo{
        }
        definethree{
        }
        useone{
            value{
                NotExistsIn=[ "../../defineone/value"
                              "../../definetwo/value"
                              "../../definethree/value" ]
            }
        }
        usetwo{
            value{
                NotExistsIn(Abs)=[ "../../defineone/value"
                                   "../../definetwo/value" ]
            }
        }
        usethree{
            value{
                NotExistsIn=[ "../../defineone/value"
                              "../../definethree/value" ]
            }
        }
        usefour{
            value{
                NotExistsIn=[ "../../definetwo/value"
                              "../../definethree/value" ]
            }
        }
        usefive{
            value{
                NotExistsIn=[ "../../definethree/value" ]
            }
        }
        usesix{
            value{
                NotExistsIn(BadFlag)=[ "../../defineone/value" ]
            }
        }
    
    }
```

Input example that **PASSES** validation on schema above:
```javascript
    test{
    
        defineone=one
        defineone=two
        defineone=three
        defineone=four
        defineone=0
    
        definetwo=[ england spain germany italy canada ]
        definetwo=-200
        definetwo=300
        definetwo=[ 500 0 -600 ]
    
        definethree=science
        definethree=math
        definethree=[ geography economics 0 recess lunch ]
    
        useone=200
        useone=japan
        useone=[ -500 600 seven -300 art ]
    
        usetwo=[ science "recess" ]
        usetwo="lunch"
        usetwo=economics
        usetwo=[ "math" geography ]
    
        usethree=canada
        usethree=england
        usethree=[ -200 "italy" 300 ]
        usethree="-600"
    
        usefour="one"
        usefour=[ "two" one ]
        usefour="four"
        usefour=[ "four" three ]
        usefour="three"
   
        usefive=[ 300 -600 ]
        usefive="one"
        usefive=[ three italy "england" ]
    
    }
```

Input example that **FAILS** validation on schema above:
```javascript
    test{
    
        defineone=one
        defineone=two
        defineone=three
        defineone=four
    
        definetwo=[ england spain germany italy canada ]
        definetwo=-200
        definetwo=300
        definetwo=[ 500 0 -600 ]
    
        definethree=science
        definethree=math
        definethree=[ geography economics 0 recess lunch ]
    
        useone=two
        useone=germany
        useone=[ three recess lunch italy canada ]
    
        usetwo=[ two germany -600 ]
        usetwo="four"
        usetwo="600"
        usetwo=[ -200 200 one ]
    
        usethree=four
        usethree=lunch
        usethree=[ two "three" ]
        usethree="science"
    
        usefour=300
        usefour=[ -600 economics ]
        usefour=recess
        usefour=[ lunch -200 ]
        usefour=math
    
        usefive=[ recess "math" ]
        usefive="science"
        usefive=[ math economics "geography" ]
    
        usesix=one
    
    }
```

HIVE validation messages that occur when validating the failing input shown above against the schema above:
```javascript
    Validation Error: Invalid Schema Rule: Bad NotExistsIn Option "BadFlag" at line:41 column:25 - Expected [ Abs ]
    
    line:17 column:12 - Validation Error: useone value "two" also exists at "../../defineone/value" on line:4 column:15
    
    line:18 column:12 - Validation Error: useone value "germany" also exists at "../../definetwo/value" on line:8 column:31
    
    line:19 column:14 - Validation Error: useone value "three" also exists at "../../defineone/value" on line:5 column:15
    
    line:19 column:20 - Validation Error: useone value "recess" also exists at "../../definethree/value" on line:15 column:41
    
    line:19 column:27 - Validation Error: useone value "lunch" also exists at "../../definethree/value" on line:15 column:48
    
    line:19 column:33 - Validation Error: useone value "italy" also exists at "../../definetwo/value" on line:8 column:39
    
    line:19 column:39 - Validation Error: useone value "canada" also exists at "../../definetwo/value" on line:8 column:45
    
    line:21 column:14 - Validation Error: usetwo value "two" also exists at "../../defineone/value" on line:4 column:15
    
    line:21 column:18 - Validation Error: usetwo value "germany" also exists at "../../definetwo/value" on line:8 column:31
    
    line:21 column:26 - Validation Error: usetwo value "600" also exists at "../../definetwo/value" on line:11 column:23
    
    line:22 column:12 - Validation Error: usetwo value "four" also exists at "../../defineone/value" on line:6 column:15
    
    line:23 column:12 - Validation Error: usetwo value "600" also exists at "../../definetwo/value" on line:11 column:23
    
    line:24 column:14 - Validation Error: usetwo value "200" also exists at "../../definetwo/value" on line:9 column:15
    
    line:24 column:19 - Validation Error: usetwo value "200" also exists at "../../definetwo/value" on line:9 column:15
    
    line:24 column:23 - Validation Error: usetwo value "one" also exists at "../../defineone/value" on line:3 column:15
    
    line:26 column:14 - Validation Error: usethree value "four" also exists at "../../defineone/value" on line:6 column:15
    
    line:27 column:14 - Validation Error: usethree value "lunch" also exists at "../../definethree/value" on line:15 column:48
    
    line:28 column:16 - Validation Error: usethree value "two" also exists at "../../defineone/value" on line:4 column:15
    
    line:28 column:20 - Validation Error: usethree value "three" also exists at "../../defineone/value" on line:5 column:15
    
    line:29 column:14 - Validation Error: usethree value "science" also exists at "../../definethree/value" on line:13 column:17
    
    line:31 column:13 - Validation Error: usefour value "300" also exists at "../../definetwo/value" on line:10 column:15
    
    line:32 column:15 - Validation Error: usefour value "-600" also exists at "../../definetwo/value" on line:11 column:23
    
    line:32 column:20 - Validation Error: usefour value "economics" also exists at "../../definethree/value" on line:15 column:29
    
    line:33 column:13 - Validation Error: usefour value "recess" also exists at "../../definethree/value" on line:15 column:41
    
    line:34 column:15 - Validation Error: usefour value "lunch" also exists at "../../definethree/value" on line:15 column:48
    
    line:34 column:21 - Validation Error: usefour value "-200" also exists at "../../definetwo/value" on line:9 column:15
    
    line:35 column:13 - Validation Error: usefour value "math" also exists at "../../definethree/value" on line:14 column:17
    
    line:37 column:15 - Validation Error: usefive value "recess" also exists at "../../definethree/value" on line:15 column:41
    
    line:37 column:22 - Validation Error: usefive value "math" also exists at "../../definethree/value" on line:14 column:17
    
    line:38 column:13 - Validation Error: usefive value "science" also exists at "../../definethree/value" on line:13 column:17
    
    line:39 column:15 - Validation Error: usefive value "math" also exists at "../../definethree/value" on line:14 column:17
    
    line:39 column:20 - Validation Error: usefive value "economics" also exists at "../../definethree/value" on line:15 column:29
    
    line:39 column:30 - Validation Error: usefive value "geography" also exists at "../../definethree/value" on line:15 column:19
```


### SumOver Details and Examples

The ***Sum Over*** rule must always contain a **context expression** and an **expected sum value**. The expected sum value is the desired sum when all of the elements in the given context are summed. The context contains a relative ancestry path in the input hierarchy that the values will be summed over. For a simple array, this will usually be "**..**" but may go back further in lineage if needed (e.g., "**../../..**").

Schema example:
```javascript
    test{
    
        container{
            inside{
                SumOver("../..")=107.6
            }
        }
        array{
            value{
                SumOver("..")=209.4
            }
        }
        invalid_array{
            value{
                SumOver("..")=123.4
            }
        }
    
    }
```

Input example that **PASSES** validation on schema above:
```javascript
    test{
    
        container{
            inside=59.4
        }
        container{
            inside=24.9
        }
        container{
            inside=23.3
        }
        array=[ 4.5 87.3 83.2 34.4 ]
    
    }
```

Input example that **FAILS** validation on schema above:
```javascript
    test{
    
        container{
            inside=59.4
        }
        container{
            inside=28.9
        }
        container{
            inside=23.3
        }
        array=[ 4.5 87.3 83.5 34.4 ]
        invalid_array= [ 1.2 4.5 something 8.8 ]
    
    }
```

HIVE validation messages that occur when validating the failing input shown above against the schema above:
```javascript
    line:1 column:1 - Validation Error: test children "inside" sum to 111.6 - instead of the required sum of 107.6

    line:12 column:5 - Validation Error: array children "value" sum to 209.7 - instead of the required sum of 209.4

    line:13 column:30 - Validation Error: invalid_array value "something" is wrong value type for sum over
```


### SumOverGroup Details and Examples

The ***Sum Over Group*** rule must always contain a **context path**, a **group sum value**, a **compare path**, and a **group divide value**. The compare path is used to acquire another element in the input hierarchy relative to the current element being validated. This value must exist in the input and be a number. Then, this value is divided by the group divide value. This performs integer division to split the input element that will be added into groups. Then each group must successfully add to the group sum value. If any group does not add to the group sum value, then this validation check fails. If every group (when split by performing an integer division on the value at the compare path relative location by the group divide value) adds to the same desired group sum, then this validation check passes.

Schema example:
```javascript
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
```

Input example that **PASSES** validation on schema above:
```javascript
    test{
    
        inside{
    
            id=121
    
            container{
                id=72123
                inside=59.4
            }
            container{
                id=72456
                inside=24.9
            }
            container{
                id=72789
                inside=23.3
            }
            container{
                id=82123
                inside=59.6
            }
            container{
                id=82456
                inside=44.7
            }
            container{
                id=82789
                inside=3.3
            }
            container{
                id=92123
                inside=0.4
            }
            container{
                id=92456
                inside=107.1
            }
            container{
                id=92789
                inside=0.1
            }
    
            array=[ 4.5 87.3 83.2 54.4 ]
    
        }
    
        inside{
            id=124
            array=[ 4.5 67.3 83.2 34.4 ]
        }
    
        inside{
            id=1324
            array=[ 4.5 87.3 83.2 14.4 ]
        }
    
        inside{
            id=1322
            array=[ 24.5 87.3 83.2 34.4 ]
        }
    
    }
```

Input example that **FAILS** validation on schema above:
```javascript
    test{
    
        inside{
    
            id=121
    
            container{
                id=72123
                inside=59.4
            }
            container{
                id=72456
                inside=14.9
            }
            container{
                id=72789
                inside=23.3
            }
            container{
                id=82123
                inside=59.6
            }
            container{
                id=82456
                inside=54.7
            }
            container{
                id=82789
                inside=83.3
            }
            container{
                id=92123
                inside=9.4
            }
            container{
                id=92456
                inside=107.1
            }
            container{
                id=92789
                inside=0.8
                badoptions{
                }
            }
    
            array=[ 4.9 87.3 3.2 54.4 ]
    
        }
    
        inside{
            id=124
            array=[ 4.5 67.3 83.2 134.4 ]
        }
    
        inside{
            id=1324
            array=[ 4.5 97.3 83.2 14.1 ]
        }
    
        inside{
            id=1322
            array=[ 24.5 87.3 83.2 14.4 ]
            invalid_array= [ 1.2 4.5 something 8.8 ]
        }
    
    }
```

HIVE validation messages that occur when validating the failing input shown above against the schema above:
```javascript
    Validation Error: Invalid Schema Rule: SumOverGroup missing ComparePath at line:15 column:21

    Validation Error: Invalid Schema Rule: SumOverGroup missing GroupDivide at line:20 column:21

    Validation Error: Invalid Schema Rule: SumOverGroup missing GroupSum at line:25 column:21

    line:1 column:1 - Validation Error: test children "value" sum to 408.5 for 1320 group - instead of the required sum of 418.8

    line:1 column:1 - Validation Error: test children "value" sum to 439.2 for 120 group - instead of the required sum of 418.8

    line:3 column:5 - Validation Error: inside children "inside" sum to 97.6 for 72000 group - instead of the required sum of 107.6

    line:3 column:5 - Validation Error: inside children "inside" sum to 117.3 for 92000 group - instead of the required sum of 107.6

    line:3 column:5 - Validation Error: inside children "inside" sum to 197.6 for 82000 group - instead of the required sum of 107.6

    line:63 column:34 - Validation Error: invalid_array value "something" is wrong value type for sum over group
```


### IncreaseOver Details and Examples

The ***Increase Over*** rule must contain a required modifier flag that occurs as a parenthetical identifier and indicates the monotonicity. The flag must either be `Strict`, meaning that the values must be strictly increasing in the order that they are read (no two values are the same), or `Mono`, meaning that multiple values are allowed to be the same as long as they never decrease. For example *3 4 5 5 6 7* would pass a `Mono` check but would fail a `Strict` check due to two of the values being the same. This rule also contains a context path that describes the relative ancestry in the input hierarchy under which the values must increase. For a simple array, this will usually be "**..**" but may go back further in lineage if needed (e.g., "**../../..**").

Schema example:
```javascript
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
```

Input example that **PASSES** validation on schema above:
```javascript
    test{
    
        container{
            inside=19.4
        }
        container{
            inside=24.9
        }
        container{
            inside=93.3
        }
        container{
            inside=193.3
        }
        array=[ 4.5 87.3 87.3 87.3 98.2 100.2 100.2 163.2 ]
    
    }
```

Input example that **FAILS** validation on schema above:
```javascript
    test{
    
        container{
            inside=19.4
        }
        container{
            inside=24.9
        }
        container{
            inside=24.9
        }
        container{
            inside=93.3
            badrule{
            }
        }
        array=[ 4.5 87.3 87.3 87.3 48.2 100.2 100.2 63.2 ]
        array=[ 4.5 87.3 87.3 something 48.2 100.2 100.2 63.2 ]
        another_array=[ 4.5 87.3 something 87.3 48.2 100.2 100.2 63.2 ]
    
    }
```

HIVE validation messages that occur when validating the failing input shown above against the schema above:
```javascript
    Validation Error: Invalid Schema Rule: Bad IncreaseOver Option "Neither" at line:9 column:39 - Expected [ Mono Strict ]

    line:1 column:1 - Validation Error: test children "inside" are not strictly increasing at line:10 column:9

    line:17 column:5 - Validation Error: array children "value" are not monotonically increasing at line:17 column:32

    line:17 column:5 - Validation Error: array children "value" are not monotonically increasing at line:17 column:49

    line:18 column:27 - Validation Error: array value "something" is wrong value type for increasing

    line:19 column:30 - Validation Error: another_array value "something" is wrong value type for increasing
```


### DecreaseOver Details and Examples

The ***Decrease Over*** rule must contain a required modifier flag that occurs as a parenthetical identifier and indicates the monotonicity. The flag must either be `Strict`, meaning that the values must be strictly decreasing in the order that they are read (no two values are the same), or `Mono`, meaning that multiple values are allowed to be the same as long as they never increase. For example *7 6 5 5 4 3* would pass a `Mono` check but would fail a `Strict` check due to two of the values being the same. This rule also contains a context path that describes the relative ancestry in the input hierarchy under which the values must decrease. For a simple array, this will usually be "**..**" but may go back further in lineage if needed (e.g., "**../../..**").

Schema example:
```javascript
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
```

Input example that **PASSES** validation on schema above:
```javascript
    test{
    
        container{
            inside=193.3
        }
        container{
            inside=93.3
        }
        container{
            inside=24.9
        }
        container{
            inside=19.4
        }
        array=[ 163.2 100.2 100.2 98.2 87.3 87.3 87.3 4.5 ]
    
    }
```

Input example that **FAILS** validation on schema above:
```javascript
    test{
    
        container{
            inside=93.3
        }
        container{
            inside=24.9
        }
        container{
            inside=24.9
        }
        container{
            inside=19.4
            badrule{
            }
        }
        array=[ 63.2 100.2 100.2 48.2 87.3 87.3 87.3 4.5 ]
        array=[ 163.2 100.2 100.2 something 87.3 87.3 87.3 4.5 ]
        another_array=[ 163.2 100.2 something 100.2 87.3 87.3 87.3 4.5 ]
    
    }
```

HIVE validation messages that occur when validating the failing input shown above against the schema above:
```javascript
    Validation Error: Invalid Schema Rule: Bad DecreaseOver Option "Neither" at line:9 column:39 - Expected [ Mono Strict ]

    line:1 column:1 - Validation Error: test children "inside" are not strictly decreasing at line:10 column:9

    line:17 column:5 - Validation Error: array children "value" are not monotonically decreasing at line:17 column:18

    line:17 column:5 - Validation Error: array children "value" are not monotonically decreasing at line:17 column:35

    line:18 column:31 - Validation Error: array value "something" is wrong value type for decreasing

    line:19 column:33 - Validation Error: another_array value "something" is wrong value type for decreasing
```


### ChildAtMostOne Details and Examples

The ***Child At Most One*** rule contains multiple relative input lookup paths. Each of these lookup paths can optionally have an assigned lookup value. There may be more than one of these rules for any given element in the schema. Of the given list of elements, *at most one* must exist in the input in order for this rule to pass. If there is a lookup value associated with the lookup path, then that path's value in the input must be equal to that provided in the schema in order for that element to count toward existence. This comparison is case insensitive.

Schema example:
```javascript
    test{
    
        ChildAtMostOne = [ one two three ]
    
        one{
        }
        two{
        }
        three{
        }
        four{
        }
        five{
            ChildAtMostOne=[ "../four" "../two" ]
        }
        six{
        }
        seven{
            ChildAtMostOne=[ "../six" ]
        }
    
    }
```

Input example that **PASSES** validation on schema above:
```javascript
    test{
        one=1
        four=4
        six=6
    }
    test{
        four=4
    }
    test{
        two=3
    }
    test{
        five=5
    }
    test{
        seven=7
    }
    test{
        three=[ 2 3 4 ]
        four=5
        five=6
    }
    test{
        two=[ 2 3 4 ]
        five=6
    }
    test{
        five=6
        four=5
    }
    test{
        one=[ 2 3 4 ]
        seven=5
    }
    test{
        one=[ 2 3 4 ]
        six=6
        seven=5
    }
    test{
        one=[ 2 3 4 ]
        five=5
    }
    test{
        two=2
        six=[ 8 9 10 ]
        seven=[ 11 12 ]
    }
```

Input example that **FAILS** validation on schema above:
```javascript
    test{
        one=1
        three=2
    }
    test{
        one=1
        two=[ 6 7 8 9 ]
        three=3
    }
    test{
        two=[ 1 2 3 4 ]
        three=5
    }
    test{
        four=[ 6 7 8 9 ]
        two=5
        five=4
    }
```

HIVE validation messages that occur when validating the failing input shown above against the schema above:
```javascript
    line:1 column:1 - Validation Error: test has more than one of: [ one two three ] - at most one must occur

    line:5 column:1 - Validation Error: test has more than one of: [ one two three ] - at most one must occur

    line:10 column:1 - Validation Error: test has more than one of: [ one two three ] - at most one must occur

    line:17 column:5 - Validation Error: five has more than one of: [ "../four" "../two" ] - at most one must occur
```


### ChildExactlyOne Details and Examples

The ***Child Exactly One*** rule contains multiple relative input lookup paths. Each of these lookup paths can optionally have an assigned lookup value. There may be more than one of these rules for any given element in the schema. Of the given list of elements, *exactly one* must exist in the input in order for this rule to pass. If there is a lookup value associated with the lookup path, then that path's value in the input must be equal to that provided in the schema in order for that element to count toward existence. This comparison is case insensitive.

Schema example:
```javascript
    test{
    
        ChildExactlyOne = [ one two three ]
    
        one{
        }
        two{
        }
        three{
        }
        four{
        }
        five{
            ChildExactlyOne=[ "../four" "../two" ]
        }
        six{
        }
        seven{
            ChildExactlyOne=[ "../six" ]
        }
    
    }
```

Input example that **PASSES** validation on schema above:
```javascript
    test{
        one=1
        four=4
        six=6
    }
    test{
        three=[ 2 3 4 ]
        four=5
        five=6
    }
    test{
        two=[ 2 3 4 ]
        five=6
    }
    test{
        one=[ 7 8 9 ]
        four=2
        five=6
    }
    test{
        two=2
        six=[ 8 9 10 ]
        seven=[ 11 12 ]
    }
```


Input example that **FAILS** validation on schema above:
```javascript
    test{
        four=5
        five=6
    }
    test{
        one=1
        three=2
    }
    test{
        one=1
        two=[ 6 7 8 9 ]
        three=3
    }
    test{
        one=[ 6 7 8 9 ]
        five=9
    }
    test{
        two=[ 6 7 8 9 ]
        five=9
        four=7
    }
    test{
        three=[ 6 7 8 9 ]
        seven=9
    }
```

HIVE validation messages that occur when validating the failing input shown above against the schema above:
```javascript
    line:1 column:1 - Validation Error: test has zero of: [ one two three ] - exactly one must occur

    line:5 column:1 - Validation Error: test has more than one of: [ one two three ] - exactly one must occur

    line:9 column:1 - Validation Error: test has more than one of: [ one two three ] - exactly one must occur

    line:16 column:5 - Validation Error: five has zero of: [ "../four" "../two" ] - exactly one must occur

    line:20 column:5 - Validation Error: five has more than one of: [ "../four" "../two" ] - exactly one must occur

    line:25 column:5 - Validation Error: seven has zero of: [ "../six" ] - exactly one must occur
```


### ChildAtLeastOne Details and Examples

The ***Child At Least One*** rule contains multiple relative input lookup paths. Each of these lookup paths can optionally have an assigned lookup value. There may be more than one of these rules for any given element in the schema. Of the given list of elements, *at least one* must exist in the input in order for this rule to pass. If there is a lookup value associated with the lookup path, then that path's value in the input must be equal to that provided in the schema in order for that element to count toward existence. This comparison is case insensitive.

Schema example:
```javascript
    test{
    
        ChildAtLeastOne = [ one 'two/value' 'three/value' ]
    
        one{
        }
        two{
            value{
            }
        }
        three{
            value{
            }
        }
        four{
        }
        five{
            ChildAtLeastOne=[ "../four" "../two/value" ]
        }
        six{
            value{
            }
        }
        seven{
            ChildAtLeastOne=[ "../six/value" ]
        }
    
    }
```

Input example that **PASSES** validation on schema above:
```javascript
    test{
        one=1
        four=4
        six=6
    }
    test{
        three=[ 2 3 4 ]
        four=5
        five=6
    }
    test{
        two=[ 2 3 4 ]
        four=5
        five=6
    }
    test{
        two=2
        five=6
    }
    test{
        two=2
        three=[ 5 6 7 ]
        six=[ 8 9 10 ]
        seven=[ 11 12 ]
    }
```

Input example that **FAILS** validation on schema above:
```javascript
    test{
        four=5
        five=6
    }
    test{
        three=2
        five=5
    }
    test{
        one=1
        three=[ 5 6 7 ]
        seven=[ 11 12 ]
    }
```

HIVE validation messages that occur when validating the failing input shown above against the schema above:
```javascript
    line:1 column:1 - Validation Error: test has zero of: [ one 'two/value' 'three/value' ] - at least one must occur

    line:7 column:5 - Validation Error: five has zero of: [ "../four" "../two/value" ] - at least one must occur

    line:12 column:5 - Validation Error: seven has zero of: [ "../six/value" ] - at least one must occur
```


### ChildCountEqual Details and Examples

The ***Child Count Equal*** rule is usually used to ensure that arrays in the input have an equal number of value members. Each of these lookup paths can optionally have an assigned lookup value. There may be more than one of these rules for any given element in the schema. This rule contains multiple relative input look paths and a required modifier flag that occurs as a parenthetical identifier. This modifier flag can be either `IfExists` or `EvenNone`. If the modifier flag is `IfExists`, then the pieces of input in the relative lookup paths must be equal only if they actually exist. However, If the modifier flag is `EvenNone`, then this stricter rule denotes that the relative input lookup path nodes in the input must be equal regardless of whether they exist or not. If there is a lookup value associated with the lookup path, then that path's value in the input must be equal to that provided in the schema in order for that element to count toward existence. This comparison is case insensitive.

Schema example:
```javascript
    test{
    
        ChildCountEqual(IfExists) = [ "one/value"   'two/value'  "three/value" ]
        ChildCountEqual(EvenNone) = [ "four/value"  'five/value' "six/value"   ]

        badflags{
            inside{
                ChildCountEqual           = [ "three/value" "six/value"  ]
                ChildCountEqual(BadFlag)  = [ "one/value"   "four/value" ]
            }
        }
    
        one{
            value{
            }
        }
        two{
            value{
            }
        }
        three{
            value{
            }
        }
        four{
            value{
            }
        }
        five{
            value{
            }
        }
        six{
            value{
            }
        }
    
        settings{
            override{
                ChildCountEqual(EvenNone) = [ color=orange               '../orange_rgb' ]
                ChildCountEqual(IfExists) = [ '../override/color'=yellow "../yellow_rgb" ]
                color{
                }
            }
            orange_rgb{
                value{
                }
            }
            yellow_rgb{
                value{
                }
            }
        }

    }
```

Input example that **PASSES** validation on schema above:
```javascript
    test{
    
        one=[ a b c ]
        one=d
        one=[ e f ]
    
        three=[ "!" "@" "#" ]
        three="$"
        three=[ "%" "^" ]
    
        four=[ red orange yellow ]
        four=green
        four=[ blue indigo ]
    
        five=[ canada poland england ]
        five=mexico
        five=[ italy france ]
    
        six=[ algebra chemistry history ]
        six=calculus
        six=[ physics geometry ]
    
        settings{
            override{
                color=Orange
            }
            orange_rgb=[ 255 165 0 ]
        }
        settings{
            override{
                color=orange
                color=ORANGE
            }
            orange_rgb=[ 255 165 0 ]
            orange_rgb=[ 250 175 0 ]
        }
        settings{
            override{
                color=yellow
            }
            yellow_rgb=[ 255 165 0 ]
        }
        settings{
            override{
            }
            yellow_rgb=[ 255 165 0 ]
            yellow_rgb=[ 250 170 0 ]
        }
        settings{
            override{
                color=yellow
            }
        }

    }
```


Input example that **FAILS** validation on schema above:
```javascript
    test{
    
        one=[ a b c ]
        one=d
        one=[ e f ]
    
        two=[ 1 2 3 ]
        two=4
        two=[ 5 6 7 ]
    
        three=[ "!" "@" "#" ]
        three="$"
        three=[ "%" "^" ]
    
        four=[ red orange yellow ]
        four=green
        four=[ blue indigo ]
    
        six=[ algebra chemistry history ]
        six=calculus
        six=[ physics geometry ]
    
        badflags{
        }
    
        settings{
            override{
                color=orange
            }
        }
        settings{
            override{
            }
            orange_rgb=[ 255 165 0 ]
        }
        settings{
            override{
                color=yellow
            }
            yellow_rgb=[ 255 165 0 ]
            yellow_rgb=[ 250 170 0 ]
        }
        settings{
            override{
                color=yellow
                color=yellow
                color=yellow
            }
            yellow_rgb=[ 255 165 0 ]
        }

    }
```

HIVE validation messages that occur when validating the failing input shown above against the schema above:
```javascript
    Validation Error: Invalid Schema Rule: Bad ChildCountEqual Option "" at line:8 column:43 - Expected [ IfExists EvenNone ]

    Validation Error: Invalid Schema Rule: Bad ChildCountEqual Option "BadFlag" at line:9 column:43 - Expected [ IfExists EvenNone ]

    line:1 column:1 - Validation Error: test does not have an equal number of existing: [ "one/value" 'two/value' "three/value" ]

    line:1 column:1 - Validation Error: test does not have an equal number of: [ "four/value" 'five/value' "six/value" ]

    line:27 column:9 - Validation Error: override does not have an equal number of: [ color=orange '../orange_rgb' ]

    line:32 column:9 - Validation Error: override does not have an equal number of: [ color=orange '../orange_rgb' ]

    line:37 column:9 - Validation Error: override does not have an equal number of existing: [ '../override/color'=yellow "../yellow_rgb" ]

    line:44 column:9 - Validation Error: override does not have an equal number of existing: [ '../override/color'=yellow "../yellow_rgb" ]
```


### ChildUniqueness Details and Examples

The ***Child Uniqueness*** rule is used quite often. Every value in this set must occur once and only once among all other values at all other paths. There may be more than one of these rules on any given element. This rule may use an optional `Abs` modifier flag that can occur as a parenthetical identifier. The `Abs` modifier flag indicates that the absolute value of all numbers added to the set checked for uniqueness are used. Then, even if one value is negative and the other is positive, but they have the same absolute value, this validation check will fail. For example, if one ChildUniqueness relative input lookup path contains *-5* and another relative lookup input path contains *5* then this validation check will fail if the `Abs` modifier flag is used.

Schema example:
```javascript
    test{
    
        ChildUniqueness          = [ one/value                       ]
        ChildUniqueness          = [ one/value two/value             ]
        ChildUniqueness(Abs)     = [           two/value three/value ]
        badflags{
            inside{
                ChildUniqueness(BadFlag) = [ four/value              ]
            }
        }
    
        one{
            value{
            }
        }
        two{
            value{
            }
        }
        three{
            value{
            }
        }
        four{
            value{
            }
        }
    
    }
```

Input example that **PASSES** validation on schema above:
```javascript
    test{
    
        one=[ 12 a b 11 c 0 -4 ]
        one=d
        one=e
        one=[ f -12 g h ]
    
        two=[ 1 2 3 -11 ]
        two=4
        two=5
        two=[ 6 7 8 ]
    
        three=[ "!" "@" "#" ]
        three="$"
        three="%"
        three=[ "^" "&" 0 "*" ]
    
    }
```


Input example that **FAILS** validation on schema above:
```javascript
    test{
    
        one=[ a b c ]
        one=d
        one="%"
        one=[ 8 b h ]
    
        two=[ 1 b 3 0 ]
        two="%"
        two="*"
        two=[ 6 7 8 -3 ]
    
        three=[ 8 "@" c ]
        three="$"
        three="%"
        three=[ "^" b 0 -7 "*" ]
    
        badflags{
        }
    
    }
```

HIVE validation messages that occur when validating the failing input shown above against the schema above:
```javascript
    Validation Error: Invalid Schema Rule: Bad ChildUniqueness Option "BadFlag" at line:8 column:29 - Expected [ Abs ]
    
    line:3 column:13 - Validation Error: one/value value "b" also exists at "one/value" on line:6 column:13
    
    line:3 column:13 - Validation Error: one/value value "b" also exists at "one/value" on line:6 column:13
    
    line:5 column:9 - Validation Error: one/value value "%" also exists at "two/value" on line:9 column:9
    
    line:6 column:11 - Validation Error: one/value value "8" also exists at "two/value" on line:11 column:15
    
    line:6 column:13 - Validation Error: one/value value "b" also exists at "one/value" on line:3 column:13
    
    line:6 column:13 - Validation Error: one/value value "b" also exists at "one/value" on line:3 column:13
    
    line:8 column:13 - Validation Error: two/value value "b" also exists at "one/value" on line:3 column:13
    
    line:8 column:13 - Validation Error: two/value value "b" also exists at "three/value" on line:16 column:17
    
    line:8 column:15 - Validation Error: two/value value "3" also exists at "two/value" on line:11 column:17
    
    line:8 column:17 - Validation Error: two/value value "0" also exists at "three/value" on line:16 column:19
    
    line:9 column:9 - Validation Error: two/value value "%" also exists at "one/value" on line:5 column:9
    
    line:9 column:9 - Validation Error: two/value value "%" also exists at "three/value" on line:15 column:11
    
    line:10 column:9 - Validation Error: two/value value "*" also exists at "three/value" on line:16 column:24
    
    line:11 column:13 - Validation Error: two/value value "7" also exists at "three/value" on line:16 column:21
    
    line:11 column:15 - Validation Error: two/value value "8" also exists at "one/value" on line:6 column:11
    
    line:11 column:15 - Validation Error: two/value value "8" also exists at "three/value" on line:13 column:13
    
    line:11 column:17 - Validation Error: two/value value "3" also exists at "two/value" on line:8 column:15
    
    line:13 column:13 - Validation Error: three/value value "8" also exists at "two/value" on line:11 column:15
    
    line:15 column:11 - Validation Error: three/value value "%" also exists at "two/value" on line:9 column:9
    
    line:16 column:17 - Validation Error: three/value value "b" also exists at "two/value" on line:8 column:13
    
    line:16 column:19 - Validation Error: three/value value "0" also exists at "two/value" on line:8 column:17
    
    line:16 column:21 - Validation Error: three/value value "7" also exists at "two/value" on line:11 column:13
    
    line:16 column:24 - Validation Error: three/value value "*" also exists at "two/value" on line:10 column:9
```


## **Input Assistance Details**

Six of the previously described validation rules (`MaxOccurs`, `ChildAtMostOne`, `ChildExactlyOne`, `ValEnums`, `ExistsIn`, and `ValType`) and seven new rules (`InputTmpl`, `InputName`, `InputType`, `InputVariants`, `InputDefault`, `InputChoices`, and `Description`) may also be used by graphical user interfaces to aid with input creation. They may be use for autocompletion assistance or input introspection.

### MaxOccurs Assistance Details

The ***Maximum Occurrence*** rule may be used by input assistance application logic for filtering options as needed from the autocompletion list. An element should only be added up to MaxOccurs times via autocomplete. For example, if an element has `MaxOccurs = 1`, it can only be added once to the document. After is it added once, it is filtered from the autocompletion list.

### ChildAtMostOne Assistance Details

The ***Child At Most One*** rule may be used by input assistance application logic for filtering options as needed from the autocompletion list. If at most one of multiple choices is allowed at any context, then as soon as one of those choices is added to the document, the others could be filtered from the autocompletion list. For example, if an element has `ChildAtMostOne = [ choice1 choice2 choice3 ]` and `choice2` is added, then `choice1` and `choice3` will not be available on the next autocomplete.

### ChildExactlyOne Assistance Details

The ***Child Exactly One*** rule may be used by input assistance application logic for filtering options as needed from the autocompletion list. If exactly one of multiple choices is allowed at any context, then as soon as one of those choices is added to the document, the others could be filtered from the autocompletion list. For example, if an element has `ChildExactlyOne = [ choice1 choice2 choice3 ]` and `choice2` is added, then `choice1` and `choice3` will not be available on the next autocomplete.

### ValEnums Assistance Details

The ***Value Enumerations*** rule may be used by input assistance application logic to provide a set of choices that are legal at a given context based on a static set of values supplied in the schema. For example, if an element has `ValEnums = [ "a" "b" "c" "d" ]`, then those values could be provided as autocompletion options.

### ExistsIn Assistance Details

The ***Exists In*** rule may be used by input assistance application logic to provide a set of choices that are legal at a given context based on values supplied elsewhere in the input. For example, if an element has `ExistsIn = [ "../../some/context1" "../../some/context2" ]`, and the values `1`, `2`, `3`, and `4` exist in the input at those relative contexts, then those values could be provided as autocompletion options.

### ValType Assistance Details

The ***Value Type*** rule may be used by input assistance application logic to drop in a legitimate default value of the correct type for flag-values and flag-arrays if no `InputDefault` is provided in the schema. For example, the following defaults could be used:

    * For an element with a `ValType = Int` rule, `1` may be inserted.
    * For an element with a `ValType = Real` rule, `0.0` may be inserted.
    * For an element with a `ValType = String` rule, `'insert-string-here'` may be inserted.
 
To override this behavior, please see [InputDefault Assistance Details](#inputdefault-assistance-details).

### InputTmpl Assistance Details

The ***Input Template*** rule may be used by input assistance application logic to pick which ***Template File*** to use for autocompletion. For example, if a context has `InputTmpl = MyCustomTemplate` then a template named `MyCustomTemplate.tmpl` may be used by the application for autocompletion.

### InputName Assistance Details

The ***Input Name*** rule may be used by input assistance application logic to override the name of the actual node that the template provided by `InputTmpl` uses for autocompletion, if desired. For example, if the name of an element in the input hierarchy is `something_one`, then the name in the schema must be the same, but a template named `MySomething.tmpl` should use the name `something_two` instead for autocompletion, then `something_one` can be overridden via:
```javascript
    something_one{
	    InputName = "something_two"
        InputTmpl = "MySomething"
    }
```

### InputType Assistance Details

The ***Input Type*** rule may be used by input assistance application logic to let the template provided by `InputTmpl` for autocompletion know what type to switch on, if desired. If a template can handle multiple situations in different ways, depending on the type it is dealing with, then the application can let the template know what the type of the current autocompletion context is with this rule. For example, if there is a template named `FlagTypes.tmpl` that can handle the types `FlagValue` and `FlagArray` differently, then the application can let the template know it is dealing with a `FlagValue` via:
```javascript
    flag_value_node{
        InputType = "FlagValue"
        InputTmpl = "FlagTypes"
    }
```

Alternately, the application can let the same template know it is dealing, instead, with a `FlagArray` via:
```javascript
    flag_array_node{
        InputType = "FlagArray"
        InputTmpl = "FlagType"
    }
```

### InputVariants Assistance Details

The ***Input Variants*** rule may be used by input assistance application logic to provide multiple choices of autocompletion templates for a single context. For example, if an element has `InputVariants = [ 'simple_version' 'middle_version' 'complex_version' ]` and `simple_version.tmpl`, `middle_version.tmpl,` and `complex_version.tmpl` exist in the template directory provided by application's grammar, then all three of those choices will be available at that context via autocomplete and use their associate templates.

### InputDefault Assistance Details

The ***Input Default*** rule may be used by input assistance application logic to explicitly tell a template what value should be dropped in for flag-values and flag-arrays via `InputDefault = 'explicit_default_value'`. This should override the `ValType` logic described in [ValType Assistance Details](#valtype-assistance-details).

### InputChoices Assistance Details

The ***Input Choices*** rule is useful if there is a set of recommended choices (static or relative path lookups) for a parameter's value, but it is still technically legal to use a different arbitrary value. It may be used by input assistance application logic to provide a set of choices that are legal at a given context based on a static set of values supplied in the schema. For example, if an element has `InputChoices = [ "a" "b" "c" "d" ]`, then those values could be provided as autocompletion options. For autocompletion purposes, this usage is similar to the `ValEnums` logic described in [ValEnums Assistance Details](#valenums-assistance-details), however it should only be used for input autocompletion assistance and not for input validation.

The `PATH:` tag may be used within `InputChoices` by input assistance application logic to provide a set of choices that are legal at a given context based on values supplied elsewhere in the input. For example, if an element has `InputChoices = [ PATH:"../../some/context1" PATH:"../../some/context2" ]`, and the values `1`, `2`, `3`, and `4` exist in the input at those relative contexts, then those values could be provided as autocompletion options. For autocompletion purposes, this `PATH:` usage is similar to the `ExistsIn` logic described in [ExistsIn Assistance Details](#existsin-assistance-details), however, it should only be used for input autocompletion assistance and not for input validation.

### Description Assistance Details

The ***Input Description*** rule may be used by input assistance application logic to give a short one line description in the autocompletion dropdown list via `Description = 'autocomplete dropdown description'`. These descriptions can be very useful to novice users unfamiliar with all of the parameters at a given context.

### Input Aliases

The ***Input Aliases*** rule is harnessed by definition driven interpreters (DDI, EDDI, etc.) to accommodate name-aliases or position-depedent named components. 

#### Name-Aliased 
e.g., `d 1 2 3` may be a shorthand for `data 1 2 3`. 

The `InputAliases` can accommodate this via the following snippet:

```javascript
data{ InputAliases["d"] }
```

#### Index-Aliased
Alternatively, for index-aliased, e.g., `data 3.14 carbon 3.52` 

is a data array with 3 values `3.14`, `carbon`, `3.52`. These are by default generically named `value`. However, this
generic name prevents specific rules from being applied to them. `InputAliases` allow for naming these components.

e.g., the following snippet defines radius, material, and density as index 0, 1, and 2 respectively. 

```javascript
data{
   radius  { InputAliases["_0"] ... }
   material{ InputAliases["_1"] ... }
   density { InputAliases["_2"] ... }
}
```
> Note: Index-Aliases are only supported by EDDI

### Strided-Aliased
Alternatively, if the data is strided by the `InputAliases` supports the `STRIDE[<start>, <stride>]` construct

e.g., the following `InputAliases` allows the data array to be arbitrarily long but still apply `radius`, `material`, and `density` naming and associated rules.

```javascript
data{
   radius  { InputAliases[STRIDE[0,3]] ... }
   material{ InputAliases[STRIDE[1,3]] ... }
   density { InputAliases[STRIDE[2,3]] ... }
}
```


> Note: The `STRIDE`'s stride must match across strided children of `data`.
> Note: Strided-aliases are only supported by EDDI

### Section-Aliased
The EDDInterpreter additionally supports 'section' input, delimited by `/`. This facilitate visual aggregation of an array's components. 

E.g., `data 3.14  3.5 / carbon graphite / 3.52  2.26` describes arbitrary-length tuples of `radius`, `material`, and `density`. Such an example can be properly defined via the section `InputAliases` construct, `s_<section>`.

The following aliased input definition names the indices between the section `/` delimeters: 

```javascript
data{
    radius  { InputAliases["s_0"] }
    material{ InputAliases["s_1"] }
    density { InputAliases["s_3"] }
}
```
