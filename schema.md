# schema

The graph is represented as an adjacency list. It consists of an array of node objects, each of which contains the following:
```
{
  "position": [x, y],
  "lines": {
    "red": [n1, n2, ...],
    "blue": [n3, n4, ...],
  }
}
```

The "position" key contains an array of the normalized X and Y coordinates. The "lines" key contains an object whose keys are the names of various transit lines that the node belongs to and whose values are arrays of integer indices into the adjacency list.


Example: this graph:

```
[0, 1] ---- [ 1, 1 ]
                |
                |
                |
            [ 1, 0 ]
```

where all nodes are on the "green" line, would be represented as

```
[
  {
    "position": [0, 1],
    "lines": { "green": [ 1 ] }
  },
  {
    "position": [1, 1],
    "lines": { "green": [ 0, 2 ] }
  },
  {
    "position": [1, 0],
    "lines": { "green": [ 1 ] }
  }
]
```
