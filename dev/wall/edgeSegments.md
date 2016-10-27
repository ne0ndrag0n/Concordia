Edge Segments
=============

To make sure that tiles available for use always face the player fully, wall segments are nudged and in some cases replaced with *edge segments*.

## Y-Axis Wall Segments

### Rotations 0 and 1
* If there is no wall segment at [ y-1 ][ x ] in the current wall map, or the wall segment is anywhere along [ 0 ][ x ], use an edge piece instead of a standard piece.

### Rotations 2 and 3
* If there is no wall segment at [ y+1 ][ x ] in the current wall map, or the wall segment is anywhere along [ y_map_max ][ x ], use an edge piece instead of a standard piece.

## X-Axis Wall Segments
// TODO
