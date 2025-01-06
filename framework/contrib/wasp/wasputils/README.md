# Command Line Utilities 
The Workbench Analysis Sequence Processor package provides a set of command line utilities to aid in sequence processing and processor development.

The sequence processor construct parse trees from which two primary functions are typically desired:

1. the listing of the parse tree
2. the selection of input given a select statement.

## File Listing Utilities
Available Interpreters have corresponding *list utilities. 

SON, HIT, DDI, and JSON have corresponding sonlist, hitlist, ddilist, and jsonlist. 

These utilities produce an ordered directory-style listing of each parsed input component.

For the given example SON data file `example.son`:

```
object(identifier){
   key = value
   child ( name ) {
      x = 1
   }
}
array [ 1 2 3 ]
```

an invocation of the `sonlist` utility:

```
sonlist example.son
```
produces a directory-style listing of each component in the file:

```
/
/object
/object/decl (object)
/object/( (()
/object/id (identifier)
/object/) ())
/object/{ ({)
/object/key
/object/key/decl (key)
/object/key/= (=)
/object/key/value (value)
/object/child
/object/child/decl (child)
/object/child/( (()
/object/child/id (name)
/object/child/) ())
/object/child/{ ({)
/object/child/x
/object/child/x/decl (x)
/object/child/x/= (=)
/object/child/x/value (1)
/object/child/} (})
/object/} (})
/array
/array/decl (array)
/array/[ ([)
/array/value (1)
/array/value (2)
/array/value (3)
/array/] (])

```

These lists describe the interpreted hierachy and value.


Note that the Definition-Driven Interpreter (DDI) is different from the others as it requires a schema (definition) in order to parse.

```
ddilist /path/to/schema.sch /path/to/input.inp
```

## File Component Selection Utilities
The ability to select specific parts of the input can be useful in schema creation. All supported interpreters have corresponding select utilities.

Using the example.son file earlier a select statement of `sonselect example.son /array/value[1:3]` produces the following output:

```
Selecting /array/value[1:3]
---- 3 nodes selected with statement '/array/value[1:3]' ----
1) /array/value
1
2) /array/value
2
3) /array/value
3
```

Alternatively, `sonselect example.son /object/child/x` produces the following output:

```
Selecting /object/child/x
---- 1 nodes selected with statement '/object/child/x' ----
1) /object/child/x
x = 1
```

Subsequent selections select from prior selection sets, so `sonselect example.son /object/child/x ../key/value` produces:

```
Selecting /object/child/x
---- 1 nodes selected with statement '/object/child/x' ----
1) /object/child/x
x = 1
Selecting ../../../array
---- 1 nodes selected with statement '../../../array' ----
1) /array
array [ 1 2 3 ]
```
The first node selected is the `/object/child/x` node, which has the text `x = 1`. 
Subsequently, the relative path `../../../array` is used from `/object/child/x` to select three levels up `../../../`, and subsequently the `array` node.
Notice that the exact user input is reproduced. 

## XML Utilities
The XML standard is readily accessible in most programming languages where SON, HIT, DDI, etc. are not. 
As such, the *xml utilities provide a bridge for prototyping or coupling with higher-level scripts, etc.

```
sonxml example.son
```
produces

```xml
<document>
  <object>
    <decl loc="1.1" dec="true">object</decl>
    <LP loc="1.7" dec="true">(</LP>
    <id loc="1.8" dec="true">identifier</id>
    <RP loc="1.18" dec="true">)</RP>
    <LBC loc="1.19" dec="true">{</LBC>
    <key>
      <decl loc="2.4" dec="true">key</decl>
      <ASSIGN loc="2.8" dec="true">=</ASSIGN>
      <value loc="2.10">value</value>
    </key>
    <child>
      <decl loc="3.4" dec="true">child</decl>
      <LP loc="3.10" dec="true">(</LP>
      <id loc="3.12" dec="true">name</id>
      <RP loc="3.17" dec="true">)</RP>
      <LBC loc="3.19" dec="true">{</LBC>
      <x>
        <decl loc="4.7" dec="true">x</decl>
        <ASSIGN loc="4.9" dec="true">=</ASSIGN>
        <value loc="4.11">1</value>
      </x>
      <RBC loc="5.4" dec="true">}</RBC>
    </child>
    <RBC loc="6.1" dec="true">}</RBC>
  </object>
  <array>
    <decl loc="7.1" dec="true">array</decl>
    <LBK loc="7.7" dec="true">[</LBK>
    <value loc="7.9">1</value>
    <value loc="7.11">2</value>
    <value loc="7.13">3</value>
    <RBK loc="7.15" dec="true">]</RBK>
  </array>
</document>
```
Any xml element with the attribute `dec="true"`  indicates a 'decorative' input component, required syntax and could be ignored by most higher level interpreters. 

The attribute `loc="line.column"` indicates the input components location in the file. 

The element's name indicates the name of the input component. The leaf element's data are the data of interest stored in the parse tree.

## File Validation Utilities
The SON, DDI, and HIT interpreters have Hierarchical Input Validation Engine ([HIVE](/wasphive/README.md#hierarchical-input-validation-engine-hive)) adapters allowing them to be validated. 
As such, there are sonvalid, ddivalid, and hitvalid utilities. 

Invocation of the validation utilities requires a schema, and an input:

```
sonvalid /path/to/schema.sch /path/to/input.inp
```

The schema's contents are beyond the scope of this document. The product of *valid will be a return code of 0 only if no validation errors occur in the input. 
If an error occurs, a non-zero return code is produced and validation errors emitted. See the [HIVE](/wasphive/README.md#hierarchical-input-validation-engine-hive) documentation of types of validation errors.

## SON/DDI to JSON Utilities
 The JSON standard is readily accessible in most programming languages while SON and DDI are not.  JSON can easily be consumed into a native Python dictionary.

Invocation of the JSON conversion utilities requires a schema and an input:

```
sonjson /path/to/schema.sch /path/to/input.son
```

```
sonvalidjson /path/to/schema.sch /path/to/input.son
```

```
ddivalidjson /path/to/schema.sch /path/to/input.ddi
```

```
hitvalidjson /path/to/schema.sch /path/to/input.i
```

`sonjson` takes the SON input and converts it to a JSON with the results on stdout.

`sonvalidjson` takes the SON input, validates it against the provided schema, and converts it to a JSON with the results on stdout. If there are any validation errors, the JSON conversion still takes place with the results on stdout and any validation messages on stdderr.  If there are any validiation errors, this utility returns a non-zero code.

`ddivalidjson` takes the DDI input and validates it against the provided schema. If the input is not valid, then the utility will output the validation messages on stdderr and exit with a non-zero code. If there are no validation errors, then this utility converts the DDI input to JSON with the results on stdout.

The schema is used during the conversion so that if multiple input components of a given name are available at any level, then this is represented by a single JSON array of that given name with each element of that array being the input components.

Leaf values in the input document are represented by "value" in the converted JSON. Values that have a `ValType=Int` or `ValType=Real` in the schema are not quoted in the JSON.  All other values are quoted.

For example, if according to the provided schema, "foo" can only occur one time and its value is an integer that can only occur one time (i.e. not an array), then the SON:

```
foo=7
```

or the DDI:

```
foo 7
```

is represented by this JSON:

```
  "foo":{
    "value":7
  }
```

However, if the values of "foo" are strings instead of numbers (thus quoted in the JSON) and can occur multiple times (i.e. an array), then the SON:

```
foo = [ seven eight nine ]
```

or the DDI:

```
foo 'seven' 'eight' 'nine'
```

is represented by this in JSON:

```
  "foo":{
    "value":[ "seven", "eight", "nine" ]
  }
```

Further, if "foo" itself can appear multiple times, then in the converted JSON the contents of each foo are separate entries in an array.  So the SON:

```
foo = 6
foo = [ 7 8 9 ]
```

or the DDI:

```
foo 6
foo 7 8 9
```

is represented by this in JSON:

```
  "foo":[
    {
      "value":[ 6 ]
    },
    {
      "value":[ 7, 8, 9 ]
    }
  ]
```

Finally, the ID tag field in SON (DDI does not have this ID construct) is represented by the name "_id" in JSON.  Again, if this ID tag field is specified to be a number by the schema, then its value is not quoted in the JSON. Otherwise, it is quoted. So the SON:

```
foo(name_one) = 6
foo(name_two) = [ 7 8 9 ]
bar(10)       = three
```

is represented by this in JSON:

```
  "foo":[
    {
      "_id":"name_one",
      "value":[ 6 ]
    },
    {
      "_id":"name_two",
      "value":[ 7, 8, 9 ]
    }
  ],
  "bar":{
    "_id":10
    "value":"three"
    }
```

## The HierarchAL Input Template Expansion (HALITE) Engine 
The [HALITE](/wasphalite/README.md#hierarchial-input-template-expansion-engine-halite) engine has the corresponding `halite` command line utility. 

HALITE provides a data-driven template expansion capability and has sizable feature set for templating text data for input or other needs. 

The `halite` command line utility can be invoked with a template and optional [JSON](http://www.json.org/)-formatted data parameter set:

```
halite /path/to/template.tmpl
```
or with JSON parameter set:

```
halite /path/to/template.tmpl /path/to/data.json
```

The expanded template emitted on stdout, and errors/log information is emitted on stderr.

The template attribute delimiters can be specified as:

```
halite /path/to/template.tmpl /path/to/data.json --ldelim '${' --rdelim '}'
```


## Schema Skeleton Creation Utility

The ability to take multiple input files known to be valid and to create a schema skeleton from these can be a very useful starting point for schema creation.  Currently, a utility exists that allows this to be done with a series of SON input files.

Therefore, if a user has multiple SON formatted input files that are known to be valid, and the objectibve is to begin creating a schema for these files, the first step is to run

    sonschemaskel path/to/valid/input1.son path/to/valid/input2.son ...

and the resulting output will be a schema skeleton that can be used with the Hierarchical Input Validation Engine to validate the inputs.  These schema skeletons have actual rule stubs for each input node commented out.  These rules can reviewed in more detail in the [HIVE section](/wasphive/README.md#hierarchical-input-validation-engine-hive) and should be modified for each piece of input.

## Autogenerate Input Documentation

The ability to auto-generate Markdown input documentation from an input schema exists as a WASP utility. Input modifications can be made in just the schema, and then the input documentation can be automatically updated at any time by simply running:

```
docprint path/to/valid/schema.son > documentation.md
```

The result will be Markdown documentation for the entire input hierarchy represented in the provided schema. This documentation will contain tables of all fields available in the input with restrictions such as how many are expected, the value type (integer, real, string), allowed ranges for numbers, available value choices, input lookup keys, descriptions, etc.

The produced Markdown is GitLab compatible and can be converted to HTML via the [Pandoc](https://pandoc.org/).


