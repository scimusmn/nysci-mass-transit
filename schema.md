# schema

Data is communicated as JSON. The most basic element is a tile object, represented as follows:

```
{
  x: NORMALIZED_X,
  y: NORMALIZED_Y,
  type: TYPE
}
```

where the `x` and `y` elements are the (x, y) coordinates between [0, 1], and the `type` member is an integer that represents what kind of transit the tile is used for.

The full state of the map is communicated as an array of tile objects. The order of this array is not necessarily stable from one frame to the next.


Example: if the map were laid out as follows:

```
+-------------------------------------------------+
|                                                 |
|                                                 |
|     [0]                            [2]          |
|                                                 |
|                                                 |
|                                                 |
|                                                 |
|                     [2]                         |
|                                                 |
|                                                 |
|                                                 |
|                                                 |
|     [2]                            [1]          |
|                                                 |
|                                                 |
|                                                 |
+-------------------------------------------------+
```

then an appropriate representation might be

```
[
  { x: 0.2, y: 0.2, type: 0 },
  { x: 0.8, y: 0.2, type: 2 },
  { x: 0.5, y: 0.5, type: 2 },
  { x: 0.2, y: 0.8, type: 2 },
  { x: 0.8, y: 0.8, type: 1 }
]
```
