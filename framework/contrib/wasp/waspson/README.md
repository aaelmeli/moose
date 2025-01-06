# Standard Object Notation (SON)
The standard object notation is a lightweight data entry syntax intended to facilitate application input acquisition.

The supported constructs are Blocks, Objects, Arrays, and Keyed values. Additionally, Objects, Arrays, and Keyed values can be further disambiguated using identifiers.

SON can facilitate simple constructs such as property or configuration files using keyed-values. Blocks allow for top-level grouping.

Lastly, file imports are supported via 

## Keyed-Value 

The Keyed-Value is the simplest construct for representing information. 

`name = value` 

Where _name_ is a string, _=_ indicates assignment, and _value_ can be a string, or number.

Alternatively, the colon ':' can be used to indicate assignment.

`name : value`

An example property store file that illustrates an application window attribute follows:

```
x=544 y=100
width = 1920
height = 1080
```

Lastly, if the keyed-value needs to be further disambiguated an identifier can be added.

`name ( id ) = value`

An example property store file that illustrates an application with attributes of two windows (main, settings) follows:

```
x(main)=544 y(main)=100
width(main) = 1920
height(main) = 1080

x(settings) = 520 y(settings) = 800
width (settings) = 120 height(settings) = 120
```

## Blocks 
The traditional configuration file blocks allows for top-level grouping of key-values. 

```
[ui]
x(main)=544 y(main)=100
width(main) = 1920
height(main) = 1080
...

[database]
url="https://somewhere.com"
```
Blocks allow for any SON construct (except other blocks) to be nested/grouped.


## Hierarchy via Objects
Hierarchy or grouping can be added using the Object construct. Objects are useful in exhuming common context with a succinct handle. 

Objects can have nested objects, keyed-values, and arrays in any order. 

Objects have the following syntax.

```
object_name { ... }
```

```
object_name { 
... 
}
```

```
object_name 
{ 
... 
}
```
 
The example property store file above illustrates potential object use as follows.
 
```
main{
    x=544 y=100
    width = 1920
    height = 1080
}
settings{
    x = 520 y = 800
    width = 120 height = 120
}
```

Objects support the same identifier scheme as keyed-values. 
 
## Arrays of Data
 SON supports 1d arrays of data that data can be scalar values, keyed-values, and nested-objects or arrays. 
 Multi-dimensional data can be flattened to 1d and application specific context provided in associated keyed-value elements.
 
 Arrays have the following syntax.
 
```
array [ ... ]
```
 
```
array [
 ... 
]
```
 
```
array 
[ 
... 
]
```
 
The example property store file above illustrates potential array use as follows:
 
```
 name  [ main settings ]
 x     [  544      520 ]
 y     [  100      800 ]
 width [ 1920     1080 ]
 height[  120      120 ]
 
```
 
Arrays support the same identifier scheme as keyed-values with the one exception that nested arrays cannot have identifiers.


## File Import
Nested or reoccuring components can be included or imported into a parent document via the `import("filepath") syntax.

An example of a file import is as follows:

```
main{
    x=544 y=100
    width = 1920
    height = 1080
}
`import ("settings.son")
```
where settings.son contains:

```
settings{
    x = 520 y = 800
    width = 120 height = 120
}
```

File imports can occur anywhere in the document with the exception that a file component's syntax cannot be split across file boundaries. E.g., an object cannot have it's starting curly brace `{` in a different file from its terminating curly brace `}`. 