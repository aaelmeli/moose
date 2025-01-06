# HALITE engine 
The HierarchicAL Input Template Expansion engine (HALITE) engine is a data-driven input template expansion engine intended to facilitate application input or data generation. 

When it comes to text creation there are typically two approaches: (1) write a program to generate the needed text, or (2) create templates and some program glue logic to read the templates and substitute attributes. 


The HALITE engine attempts to bridge these approaches by providing standard glue logic. In this way the templates preserve clarity of intended text, the templates facilitate reuse,
development is streamlined by eliminating the developer and only requiring a template designer, and most importantly templates are interchangeable, allowing the same data to be used to create a different look, perhaps for a different application to consume.

The HALITE engine provides a single point, data-driven expansion capability that eliminates the need for application-specific glue logic.

A template and an optional hierarchical data set is all that is needed to expand templates into text usable by the end consumer.

The supported data constructs are provide by JSON and are Objects, Arrays, and Keyed values. For more about JSON syntax see www.json.org.

The expression evaluation support scalar and vector variable reference and mathematical expression evaluations. 

## Template Evaluation Summary
This section describes the general approach used by the HALITE engine when evaluating a template and constructing the resulting text. 

There are 3 primary components:

1. the template, consisting of constructs discussed below,
2. the optional data, described in hierarchal object notation, and
3. the evaluation stream, which is the destination of evaluating template constructs.



Available template constructs are as follows:

1. Static text: plain text to be emitted to the expanded result which contains no attributes or expression evaluation
2. Attributes: parameters or expressions to be evaluated, optionally formatted, and substituted into the expanded result
3. Optional Attributes: parameters' expressions that when only present are emitted to the expanded result
4. Silent Attributes: parameters' or expressions that, when evaluated, are NOT emitted to the expanded result; useful for intermediate or cached attribute evaluations to be used later
5. Iterative Attributes: expressions that are evaluated iteratively for a specified range(s) with optional separator and format
6. Template imports: import a template into the existing template with optional use of data object
7. Iterative template imports: construct that imports a template for each element of an array via 'using' an array or repeatedly via range variables
8. Conditional blocks: support pre-processor style `#if/ifdef/ifndef` - `#elseif/else` - `#endif` conditional blocks which will only be emitted when the appropriate condition is true

Each construct is evaluated and emitted into the evaluation stream which can be redirected to a file when using the HALITE utility, or c++ `std::ostream` when using the wasphalite api.

## Attributes and Expressions
Attributes and expressions are delimited by an opening and closing delimiter. By default these delimiters are '<' and '>' respectively. These are configurable via corresponding HaliteInterpreter class methods.

Example template attribute statements are:

1. `<attr>` - default delimiters, '<','>'
2. `{attr}` - custom '{' and '}'
3. `${attr}` - custom '${' and '}'
4. `#{attr}` - custom '#{' and '}'
5. `[:attr:]` - custom '[:' and ':]'
6. etc.

Formal attribute expression syntax appears as follows:

` open_delim (name|expression) (':'['?''|'] format? separator? range* use? emit?)? close_delim`

where 

1. `open_delim` is configurable, with a default value of '<'
2. `?` indicates an optional attribute evaluation, which allows undefined variables to silently fail; ? MUST OCCUR immediately after the attribute options delimiter `:` 
3. `|` indicates [silent attribute](#silent-attributes) evaluation; conducts computation/variable creation without emitting the result to the evaluation stream; MUST OCCUR immediately after the attribute options delimiter `:`
4. 2 and 3 are optional and mutually exclusive
5. optional `format` is `'fmt=' format ';'`, and `format` is described in the [section](formatting) below
6. optional `separator` is `'sep=' separator ';'`, and `separator` is emitted for all but the last evaluation iteration
7. zero or more `range` specifications where a range looks like `range_variable '=' start[,end[,stride]];`. Start, optional end and stride must be integers or attributes convertable to integers. See [Iterative Attributes](#iterative-attributes).
8. the optional `use` statement facilitates scoped attribute access as depicted in [scoped attribute](#scoped-attribute) sections below
9. the optional `emit` statement facilitates emission of a delimiter at a given iteration stride as depicted in [Iterative Attributes](#iterative-attributes) sections below.
10. `close_delim` is configurable, default of '>'

### Intrinsic Attributes
There are several intrinsic attributes that can be useful.

| Name           | Value               | comment             |
|----------------|---------------------|---------------------|
| `pi`           | `3.14159265359`     |approximately `pi`| 
| `e`            | `2.7182818284590452353602874713527` | approximately `e`| 
| `nl`           | '\\n' | a newline character; useful for generating newlines |
| `_S_`           | '<' | the delimiter for the start of an attribute; useful for emitting it as a literal |
| `_E_`           | '>' | the delimiter for the end of an attribute; useful for emitting it as a literal |

The delimiter attribute default values of `<`, and `>', but can be changed. 

### Silent Attributes
Attributes and expressions that must be evaluated but not placed into the evaluation stream can be specified using the silent expression indicator:
```
<attr:|>
```

### Optional Attributes
Attributes may not be specified, and as such they must be considered optional by the template. Optional attributes appear as follows:
```
<attr:?>
```
Here the `:?` attribute option indicates nothing being evaluated will be placed into the evaluation stream unless the `attr` is defined.

This is most useful when combined with formatting to tackle text where data may be optionally available but when available requires context:
```
data record <x> <y> <z> <comment:? fmt=com="%s">
```
Here the `comment` is optional data, but when present requires a 'com="`comment`"' to indicate context. The format statement proides the context 'com=' only when `comment` is present.

### Attribute Patterns
Attribute names are defined as the regular expression `[A-Za-z_]([A-Za-z0-9\._%])*`. Examples of these are: 

1. `var`
2. `var_name`
3. `var.name`
4. `var1`
5. `var1.real`
6. etc.

If an attribute name contains character(s) that violate the regular expression, the variable name can be quoted. Examples of these are:

1. `'var(name)'`
2. `'my var(name)'`
3. etc.

If an attribute is an array of data, a 0-based index can be used to access the data element. 
Given data of
```
'array':["ted","fred",7, 3.14159 ]
```
the following attribute patterns are legal:

1. `array[0]` - "ted"
2. `array[1]` - "fred"
3. `array[2]` - 7
4. `size(array)` - 4

### Example Attribute Pattern
An example attribute substitution appears as follows:
```
the <FoxSpeed> <FoxColor> fox jumped over the <DogColor> dog.
```
or
```
the <'fox speed'> <'fox color'> fox jumped over the <'dog color'> dog.
```
Here the `FoxSpeed` or `'fox speed'` attributes might be 'quick' or 'fast', the `FoxColor` be 'red', and the `DogColor` or `'dog color'` be 'brown' or 'black.'

### Iterative Attributes
When an attribute is an array it can be iterated. This is facilitated by the `range`, `emit`, and `sep` attribute options.

By default the `sep` is a single whitespace.

The `range` construct appears as follows:

```
<array[i]: i=0,5> 
```
The `i` attribute iterates from 0 through 5 emitting the ith element of the `array` attribute.

The range can also use a stride to skip elements:

```
<array[i]: i=0,5,2> 
```

Separating the elements by a comma is accomplished by adding the `sep` attribute option with the desired comma separator:


```
<array[i]: i=0,5;sep=,> 
```

In the special case that a newline is the separator, the intrinsic HALITE attribute `nl` can be used:

```
<array[i]: i=0,5;sep=<nl>> 
```

On occassion, the target application input is fixed width or contains a restricting column limit. In this situation the `emit` attribute option allows the emission of
a configurable delimiter based on the iteration stride:

```
<array[i]: i=0,5;emit=<nl>,3> 
```

#### Fixed Width Iterative Attribute Example
An example of a 72-column fixed-width text block is as follows:

```
<i: i=0,50; fmt=%8d; emit=# Comment<nl>,9; sep=; >
```

Which produces:

```
       0       1       2       3       4       5       6       7       8# Comment
       9      10      11      12      13      14      15      16      17# Comment
      18      19      20      21      22      23      24      25      26# Comment
      27      28      29      30      31      32      33      34      35# Comment
      36      37      38      39      40      41      42      43      44# Comment
      45      46      47      48      49      50
```

Items to note are:
1. The format statement indicates 8-column fixed attribute substitution.
2. A delimiter of `# comment \n` is emitted every 9ths item.
3. The separator has been specified as empty space to preserve the 72 column limit (9*8).




### Expressions
The HALITE Engine uses the WASP [expression engine](/waspexpr) for expression evaluation which supports all regular math operators of 
multiplication '*', division '/', addition '+', subtraction '-', less than '.lt.', less than or equal '.lte.', greater than '.gt.',
greater than or equal '.gte', equal '==', not equal '!=', and precedence '(',')'

* regular trig functions - sin(x),cos(x),tan(x),asin(x),acos(x),atan(x),atan2(y, x),sec(x),cosec(x),cot(x),sinh(x),cosh(x),tanh(x),asinh(x),acosh(x),atanh(x)
* logarithmic functions - ln(x),log(x),lg(x)
* exp(x)
* pow(x)
* round(x),round(x, p)
* floor(x),ceil(x)
* if(cond, trueval, falseval)
* abs(x)
* modulo - mod(x,y),
* min(x,y), max(x,y)
* sqrt(x)
* defined('x') - indicates if the variable named x is defined


### Example Expression Patterns
An example attribute substitution appears as follows:
```
the quick red fox jumped over the brown dog going <miles/hour>mph fast.
```
or
```
the quick red fox jumped over the brown dog going <velocity_mph*1.60934>kph fast.
```

In addition to integer and double precision math operations, string concatenation is also available, such as
```
<"My result is "+numeric_result>
```
Here the `numeric_result` is concatenated to the string `My result is ` thus producing a final result that is string typed.

## Formatting 
Attribute and expressions can be formatted prior to insertion into the evaluation stream. This is influenced by the [C printf](http://www.cplusplus.com/reference/cstdio/printf) and  [Java.Format](https://docs.oracle.com/javase/7/docs/api/java/util/Formatter.html) capability.

Specifically, the following constructs are provided:

`%[flags][width][.precision]specifier` 
 
#### Format Specifiers 
The available specifiers are:

| Specifier | Description                                        | Example   |
|-----------|----------------------------------------------------|-----------|
| f         | Decimal floating point                             | 3.14159   |
| e         | Scientific notation (mantissa/exponent), lowercase | 3.9265e+2 |
| s         | String of characters                               | sample    |
| d         | Signed decimal value                               | 300       |

The format declarator percent `%` can be escaped with a double  percent specified, `%%`

#### Format Flags
The available flags are presented below

| flag | Description                                                 | Example             |
|------|-------------------------------------------------------------|---------------------|
| -    | The result will be left justified                           | 'left       '       |
| +    | The result will always include a sign                       | '+result'           |
| ' '  | The result will include a leading space for positive values | ' result'           |
| 0    | The result will be zero padded                              | '0result'           |
| (    | The result will enclose negative numbers in parentheses     | '(negative result)' |

#### Format Width
The width is the minimum number of characters to be written to the output. Most frequently used for padding.

#### Format Precision
For general argument types the precision is the maximum number of characters to be written to the output. 
For floating-point types (specifier = 'e','f'), the precision is the number of digits after the decimal point. 


#### Format Examples

String examples:

| Format                    | Result         | Description                                                                                            |
|---------------------------|----------------|--------------------------------------------------------------------------------------------------------|
| '<"str":fmt=%s>'          | `'str'`          | Print the raw string "str" as a string                                                                 |
| '<"str":fmt=%4s>'         | `' str'`         | Print the raw string "str" with a width of 4 as a string                                               |
| '<"str":fmt=%05s>'        | `'00str'`        | Print the raw string "str" with a width of 5 as a string padded with zeros                             |
| '<"-30":fmt=%05s>'        | `'00-30'`        | Print the raw string "-30" with a width of 5 as a string padded with zeros                             |
| '<"-30":fmt=%%05s="%05s"> | `'%05s="00-30"'` | Print the raw string "-30" with a format prefix of '%05s', a width of 5 as a string padded with zeros  |
| '<"str":fmt=%-10s>'       | `'str`         ' | Print the raw string "str" left justified with a minimum width of 10                                  |

Integer examples:

| Format                    | Result         | Description                                                                                            |
|---------------------------|----------------|--------------------------------------------------------------------------------------------------------|
| '\<3:fmt=%d\>'            | `'3'`            | Print the integer 3 as an integer                                                                 |
| '\<30:fmt=%4d\>'          | `'  30'`         | Print the integer 30 with a width of 4 as an integer                                               |
| '\<-30:fmt=%-5d\>'        | `'-30  '`        | Print the integer -30 left justified with a width of 5 as an integer                           |
| '\<-30:fmt=%05d\>'        | `'-0030'`        | Print the integer -30 with a width of 5 as an integer padded with zeros                             |
| '\<30:fmt=% d\>'          | `' 30'`          | Print the integer 30 with a leading space due to positive value  |
| '\<x=-30:fmt=% d\>'       | `'-30'`          | Print the variable x (-30) and if x > 0 include a leading space |
| '\<30:fmt=%+d\>'          | `'+30'`          | Print the integer 30 with its sign |
| '\<x=-30:fmt=%(d\>'       | `'(30)'`         | Print the variable x (-30) with parenthesis if x < 0 |
| '\<3.14159:fmt=%d\>'      | `'3'`            | Print the floating point value as an integer |

Float-Point examples:

| Format                    | Result         | Description                                                                                            |
|---------------------------|----------------|--------------------------------------------------------------------------------------------------------|
| '\<3.14159265:fmt=%f\>'   | `'3.141593'`     | Print the floating-point value 3.14159265 as a floating point value with default precision of 6 |
|'\<3.14159265:fmt=%7f\>'|`'3.141593'`|
|'\<3.14159265:fmt=%8f\>'|`'3.141593'`|
|'\<3.14159265:fmt=%9f\>'|`' 3.141593'`|
|'\<3.14159265:fmt=%10f\>'|`'  3.141593'`|
|'\<3.14159265:fmt=%.0f\>'|`'3'`|
|'\<3.14159265:fmt=%.1f\>'|`'3.1'`|
|'\<3.14159265:fmt=%.2f\>'|`'3.14'`|
|'\<3.14159265:fmt=%.3f\>'|`'3.142'`|
|'\<3.14159265:fmt=%1.0f\>'|`'3'`|
|'\<3.14159265:fmt=%4.1f\>'|`' 3.1'`|
|'\<3.14159265:fmt=%4.8f\>'|`'3.14159265'`|
|'\<3.14159265:fmt=%8.2f\>'|`'    3.14'`|
|'\<3.14159265:fmt=%10.3f\>'|`'     3.142'`|
|'\<3.14159265:fmt=%01.0f\>'|`'3'`|
|'\<3.14159265:fmt=%04.1f\>'|`'03.1'`|
|'\<3.14159265:fmt=%04.8f\>'|`'3.14159265'`|
|'\<3.14159265:fmt=%08.2f\>'|`'00003.14'`|
|'\<3.14159265:fmt=%010.3f\>'|`'000003.142'`|
|'\<3.14159265:fmt=%g\>'|`'3.14159'`|
|'\<3.14159265:fmt=%.0g\>'|`'3'`|
|'\<3.14159265:fmt=%.1g\>'|`'3'`|
|'\<3.14159265:fmt=%.2g\>'|`'3.1'`|
|'\<3.14159265:fmt=%.3g\>'|`'3.14'`|
|'\<3.14159265:fmt=%1.0g\>'|`'3'`|
|'\<3.14159265:fmt=%4.1g\>'|`'   3'`|
|'\<3.14159265:fmt=%4.8g\>'|`'3.1415927'`|
|'\<3.14159265:fmt=%8.2g\>'|`'     3.1'`|
|'\<3.14159265:fmt=%10.3g\>'|`'      3.14'`|
|'\<3.14159265:fmt=%01.0g\>'|`'3'`|
|'\<3.14159265:fmt=%04.1g\>'|`'0003'`|
|'\<3.14159265:fmt=%04.8g\>'|`'3.1415927'`|
|'\<3.14159265:fmt=%08.2g\>'|`'000003.1'`|
|'\<3.14159265:fmt=%010.3g\>'|`'0000003.14'`|
|'\<1e-4:fmt=%.4g\>'|`'0.0001000'`|
|'\<1e-4:fmt=%10.4g\>'|`' 0.0001000'`|
|'\<1e-4:fmt=%.5g\>'|`'0.00010000'`|
|'\<1e-5:fmt=%.5g\>'|`'1.0000e-05'`|
|'\<1e-6:fmt=%.6g\>'|`'1.00000e-06'`|
|'\<1e-2:fmt=%.7g\>'|`'0.01000000'`|
|'\<1e2:fmt=%10.7g\>'|`'  100.0000'`|
|'\<3.14159265:fmt=%e\>'|`'3.141593e+00'`|
|'\<3.14159265:fmt=%.0e\>'|`'3e+00'`|
|'\<3.14159265:fmt=%.1e\>'|`'3.1e+00'`|
|'\<3.14159265:fmt=%.2e\>'|`'3.14e+00'`|
|'\<3.14159265:fmt=%.3e\>'|`'3.142e+00'`|
|'\<3.14159265:fmt=%1.0e\>'|`'3e+00'`|
|'\<3.14159265:fmt=%4.1e\>'|`'3.1e+00'`|
|'\<3.14159265:fmt=%4.8e\>'|`'3.14159265e+00'`|
|'\<3.14159265:fmt=%8.2e\>'|`'3.14e+00'`|
|'\<3.14159265:fmt=%10.3e\>'|`' 3.142e+00'`|
|'\<3.14159265:fmt=%01.0e\>'|`'3e+00'`|
|'\<3.14159265:fmt=%04.1e\>'|`'3.1e+00'`|
|'\<3.14159265:fmt=%04.8e\>'|`'3.14159265e+00'`|
|'\<3.14159265:fmt=%08.2e\>'|`'3.14e+00'`|
|'\<3.14159265:fmt=%010.3e\>'|`'03.142e+00'`|
|'\<1e-4:fmt=%.4e\>'|`'1.0000e-04'`|
|'\<1e-4:fmt=%10.4e\>'|`'1.0000e-04'`|
|'\<1e-4:fmt=%.5e\>'|`'1.00000e-04'`|
|'\<1e-5:fmt=%.5e\>'|`'1.00000e-05'`|
|'\<1e-6:fmt=%.6e\>'|`'1.000000e-06'`|
|'\<1e-2:fmt=%.7e\>'|`'1.0000000e-02'`|
|'\<1e-2:fmt=%10.7e\>'|`'1.0000000e-02'`|
|'\<1e2:fmt=%10.7e\>'|`'1.0000000e+02'`|

#### Format Error Examples
Some flag specifiers are not legal with given specifiers. The following are examples of common scenarios:

| Format                    | Error         |
|---------------------------|----------------|
| '\<"some string":fmt=%(5s\>'   | `malformed format string: flag '(' does not match the conversion 's'`     |
| '\<"some string":fmt=%+5s\>'   | `malformed format string: flag '+' does not match the conversion 's'`     |
| '\<"some string":fmt=% s\>'   | `malformed format string: flag ' ' does not match the conversion 's'`     |

## Scoped Attribute
Because the data is hierarchical, access to lower levels of the data hierarchy can be required. 
Common single-level access is facilitated by scoping an attribute access via a 'use' statement.

### Object Scoped Attribute
Given hierarchical data of

```
{
   'fox' : { 'color' : 'red', 'speed' : 'quick' }
   ,'dog color' : 'brown'
}
``` 
the following template uses scoped attributes of `fox` `color` and `speed` to emit the desired result:

```
the <speed:use=fox> <color:use=fox> fox jumped over the <'dog color'> dog. 
```
or
```
<"the "+speed+" "+color:use=fox> fox jumped over the <'dog color'> dog. 
```
or
```
the <fox.speed> <fox.color> fox jumped over the <'dog color'> dog. 
```

### Array Scoped Attribute
In addition to object-scoped use statements, an array can be used. 
When an array is used, an iteration is implied over each element of the array.

Given the array data of :
```
{
    'parts' : 
    [
        { "what" : "quick red fox", "action" : "jumped over", "dog state" : "brown" }
        ,{ "what" : "honey badger", "action" : "didn't care about", "dog state" : "big angry" }
        ,{ "what" : "weasel", "action" : "slunk by", "dog state" : "sleeping" }
    ]
}
```
the following template uses the scoped attributes as follows:

```
<"The "+what+" "+action+" the "+'dog state'+" dog.":use=parts> 
```

The result is a whitespace-separated evaluation of the template using each element in the array:

```
The quick red fox jumped over the brown dog. The honey badger didn't care about the big angry dog. The weasel slunk by the sleeping dog.
```

A file import using an object or array facilitates more complex hierarchical data access.


## File Imports
The HALITE engine support file import where files consist of all template constructs described in this section.
File imports can be parameterized and both implicitly and explicitly iterative. 

The simplest file format appears as shown below:

```
#import relative/or/absolute/file/path.tmpl
```
Here the '#import' must occur at the start of the line. The path to the file can be relative to the current template, working directory, or an absolute path.

The capability for the file being relative to the working directory allows subtemplates to be overridden.

The path can also be templated on any available attribute. The subtemplate has immediate access to all attributes at 
the current data level. 

### Example Data
The given data are presented below:
```
{
    "x" : "blurg"
    ,"y" : "blarg"
    ,obj :
    { 
        "a" : "blurgit", "e" : "blarg" ,
        "sarg" : 
        {
            "bravo" : 2
            , "delta" : 4
            , "charlie" : 3
        }
        ...
    }
    ,array :
    [
        {...}
        ,...
    ]
}
```
The root template (entry for evaluation) has access to `x`, and `y`, but in order to access 
data members of `obj` either a scoped attribute evaluation or a parameterized template import
must be used.

### Parameterized File Import
Parameterized file imports facilitate access to data hierarchy and repetition of templates.

#### File Import Using an Object
The [example data](#example-data) contains the `obj` data layer which contains nested object `sarg` and other imaginary data `...`. To access all of the data, a subtemplate can be imported 'using' `obj` as follows:

```
#import some/file.tmpl using obj
```

The template `some/file.tmpl` can now access all attributes within `obj`. Additionally, all attributes in higher levels (`x`,`y`,`array`,`...`) are still accessible:

`some/file.tmpl`:
```
This is a nested template with access to obj's context
variable a=<a>, e=<e>

Variables still accessible from parent data are x:<x>, and y:<y>, etc.
```

#### Iterative File Import Using an Array or Ranges
The import of files can be repeated using 2 constructs: (1) implicit iteration via use of an array, or (2) explicit iteration using repeated ranges.

The implicit iteration via use of an array is syntactically identical to [import using an object](#file-import-using-an-object):

```
#import path/to/file.tmpl using my_array
```

The explict iteration via ranges is syntactically different to disambiguate and clearly indicates intent:

```
#repeat path/to/file.tmpl using (var=start[,end[,stride]];)+
```
Note that the statement starts with `#repeat` and not the regular `#import`. The range (`var`) can be specified as semicolon ';' delimited to produce embedded loops.

```
#repeat path/to/file.tmpl using i=1,5; j=2,6,2;
```
The above will loop `j`=2,4,6 for `i`=1 through 5. The variables `i` and `j` are available in the imported template.


## Conditional Blocks
Conditional blocks facilitate alternative paths through templates. 
Conditional blocks can be activated with a defined or undefined variable or an expression that evaluates to true or false.

`#if`, `#ifdef` and `#ifndef` indicate the start of a conditional block. Subsequently, an additional condition can be indicated
by `#elseif` or `#else`, and then finally terminated by a `#endif`.

```
#if condition
block
#endif
```
The above block will be emitted if the `condition` evaluates to true. The `condition` could be a variable name referencing a value that is true, or it could be an attribute expression.
```
#if ted_present
hello ted
#endif
```
or
```
#if < i .gt. 0 >
some logic pertaining to i > 0
#endif
```

The use of `#elseif` and `#else` allow for alternative logic if the initial or prior condition evaluate to false.
```
#if < ted_present && bill_present >
Bill and Ted's excellent adventure!
#elseif ted_present 
Where is Bill?
#elseif bill_present
Where is Ted?
#else
Where are Bill and Ted?
#endif
```
