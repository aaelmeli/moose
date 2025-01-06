# Extra Definition Driven Interpreter 
The extra definition driven input interpreter (EDDI) provides an input processing capability which requires very little syntax without sacrificing input component identification.

Specifically, EDDI supports comments, data blocks, arrays, sectioned/indexed/named values, and file includes. 

## Input Syntax
The syntax pattern is as follows:

```
file := block*
block := '[' name ']' commands
commands := named_command | unnamed_command
unnamed_command := (name value+ (key_value)*)+ 
named_command := (name _name (value section?)+ (key_value)*)+ 
key_value := name =? value
name := string
_name := anything
section:= '/'
```
where the block `name` must adhere to the pattern `([A-Za-z0-9_\.])+`. A `value` can be an integer, real, string, or quoted string.
A file can have zero or more blocks. 

An example is as follows:
```
# Comments look like this
[block_name1]
   # keys have optional '=' 
   name = 1
   name = 3.14159
   name = "string value"
   
   array 1 2 3 4
   
[block2] 
   command 1.2 1 2 3 3 4
       command_key1 "value"
           command_key2 = 2.71
[block3]
  # command with sections
  name 1 2 3 / a b c / 4.0 5.0
```

The above example illustrates arbitrary hierarchy. 

Note that the block indentation is recommended for clarity but is not required.

If a collision occurs between a command value and name, the command value must be quoted to disambiguate it.

```
[block_name1]
  command1 string
  command2_named_string 
```
must be 


```
[block_name1]
  command1 'string'
  command2_named_string 
```

## Extra Definition Driven
In order to annotate the generated parse tree the interpreter takes instruction from the 
input schema, and the extra definition constructs are embedded in the `InputAliases` array
of the input field. Examples follow.

### Named fields
The EDDI logic recognizes the `_name` child component as a special identifier of the object.
All subsequent aliasing constructs (index, section, strided) will be adjusted. E.g., child indexing will be displaced such that the name/identifier will not be annotated by indexing/stride/section aliasing.

### Index-based field naming
Indexed fields are aliased using the '_INDEX' where INDEX is zero-based index into the non-decorative list of child nodes.

```
% Example of data object with x and y children at index 0,1 
data{ 
   x{InputAliases['_0']} 
   y{InputAliases['_1']}
}
```
Note: Subsequent indexing will receive the generic `value` child name instea of `x` or `y`.

### Strided field naming
Strided field naming allows for creating tuples of data given an arbitrary list of data.
The Strides must be equal so as to avoid ambiguous aliasing.
```
% Example of x, y, z tuple. User can provide as many x,y,z tupes
data {
  x{InputAliases[STRIDE[0,3]]}
  y{InputAliases[STRIDE[1,3]]}
  z{InputAliases[STRIDE[2,3]]}
}
```

Strides can be overridden by specifying additional starts.

```
% Example of x, y, z tuple. User can provide x,y,z tupes up to index 9, after which as many a,b,c tuples are allowed
data {
  x{InputAliases[STRIDE[0,3]]}
  y{InputAliases[STRIDE[1,3]]}
  z{InputAliases[STRIDE[2,3]]}
  a{InputAliases[STRIDE[10,3]]}
  b{InputAliases[STRIDE[11,3]]}
  c{InputAliases[STRIDE[12,3]]}
}
```

### Sections
Sections allow for separating data via a visual delimiter, `/`. Section aliasing allows for naming the values in a given section using zero-based index annotated as 's_INDEX' in the `InputAliases` array.

```
% Example of strided data definition
data {
  radius{InputAliases['s_0']}
  material{InputAliases['s_1']}
  density{InputAliases['s_2']}
}
```

