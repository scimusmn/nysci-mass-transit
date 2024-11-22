# memory layout

Data is communicated in a shared memory file named "Global\mass-transit-memory". The shared memory contains an array with 64 `tile` structs:

```
struct tile {
  uint16_t x;
  uint16_t y;
  uint8_t type;
  uint8_t _padding;
};
```

The `x` and `y` struct members represent the normalized (x, y) coordinate of the tile using fixed point arithmetic. Divide each coordinate member by 65536 to obtain values in the range [0, 1).

The `type` member indicates the type of the tile, and can take on the following values:

  * 0 to 7: Subway
  * 8 to 10: Ferry 
  * 11: Passenger rail

The `_padding` member is only present to clarify memory alignment.

Memory access must be negotiated with a Windows mutex object, named "mass-transit-mutex".
