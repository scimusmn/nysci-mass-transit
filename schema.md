# memory layout

Data is communicated in a shared memory file named "Global\mass-transit-memory". The shared memory contains a struct like this:

```
struct {
  struct tile tiles[64];
  int count;
};
```

The `count` member determines how many of the tile array elements are real data.

The `tile` struct looks like this:

```
struct tile {
  uint16_t x;
  uint16_t y;
  uint16_t type;
};
```

The `x` and `y` struct members represent the normalized (x, y) coordinate of the tile using fixed point arithmetic. Divide each coordinate member by 65536 to obtain values in the range [0, 1).

The `type` member indicates the type of the tile, and can take on the following values:

  * 0 to 7: Subway
    * 0: Blue Line (A, C, E)
    * 1: Orange Line (B, D, F, M)
    * 2: Light Green Line (G)
    * 3: Brown Line (J, Z)
    * 4: Yellow Line (N, Q, R)
    * 5: Red Line (1, 2, 3)
    * 6: Green Line (4, 5, 6)
    * 7: Purple Line (7)
  * 8: Ferry 
  * 9: Passenger rail

Memory access must be negotiated with a Windows mutex object, named "mass-transit-mutex".
