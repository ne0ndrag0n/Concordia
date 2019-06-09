New GridLayout Control
======================

GridLayout control will be a more coherent version of the current Layout control which is difficult to understand and not really full-featured.

# Style Properties

## New Properties

### Applied to GridLayout
`grid-columns` - Specify a number and proportion of columns.

`grid-rows` - Specify a number and proportion of rows.

### Applied to children
`grid-placement` - Place this child element in a grid cell created by `grid-column` and `grid-rows` attributes.

## New Stylesheet Data Types
`LayoutProportions (std::vector<int>)`

## New Functions
`LayoutProportions createLayout( ... )` - Specify a total number of rows/columns and a proportion in a layout.

`glm::uvec4 getPlacement( x, y )` - Specify the cell coordinates of a manual child element grid placement.

## Existing Properties

### Applied to GridLayout
`vertical-orientation` and `horizontal-orientation` - Specifies the vertical and horizontal orientation of child elements within grid cells.

### Applied to children
`vertical-orientation` and `horizontal-orientation` - Specifies the vertical and horizontal orientation of this child element within its grid cell.

# Style Examples

## 3x4
```scss
GridLayout.-bb-gridlayout-test {
   grid-columns: createLayout( 1, 2, 1 );
   grid-rows: createLayout( 1, 1, 1, 1 );
}
```
```
+---------------------+
| 1/4 |   1/2   | 1/4 |  1/4
+---------------------+
| 1/4 |   1/2   | 1/4 |  1/4
+---------------------+
| 1/4 |   1/2   | 1/4 |  1/4
+---------------------+
| 1/4 |   1/2   | 1/4 |  1/4
+---------------------+
| 1/4 |   1/2   | 1/4 |  1/4
+---------------------+
| 1/4 |               |  1/4
+---------------------+
```

## 4x3
```scss
GridLayout.-bb-gridlayout-test {
   grid-columns: createLayout( 1, 1, 1, 1 );
   grid-rows: createLayout( 1, 2, 1 );
}
```
```
+-----------------------+
| 1/4 | 1/4 | 1/4 | 1/4 |  1/4
+-----------------------+
| 1/4 | 1/4 | 1/4 | 1/4 |  2/4 (1/2)
|     |     |     |     |
+-----------------------+
| 1/4 | 1/4 | 1/4 | 1/4 |  1/4
+-----------------------+
| 1/4 | 1/4 | 1/4 | 1/4 |  1/4
+-----------------------+
```

## 4x1
```scss
GridLayout.-bb-gridlayout-test {
   grid-columns: createLayout( 1, 1, 1, 1 );
}
```
```
+-----------------------+
| 1/4 | 1/4 | 1/4 | 1/4 |  1/1
+-----------------------+
| 1/4 | 1/4 | 1/4 | 1/4 |  1/1
+-----------------------+
| 1/4 | 1/4 | 1/4 | 1/4 |  1/1
+-----------------------+
| 1/4 | 1/4 | 1/4 | 1/4 |  1/1
+-----------------------+
...
```

## 1x1 (unspecified)
```scss
GridLayout.-bb-gridlayout-test {

}
```
```
+-----------------------+
|          1/1          |  1/1
+-----------------------+
|          1/1          |  1/1
+-----------------------+
|          1/1          |  1/1
+-----------------------+
|          1/1          |  1/1
+-----------------------+
|          1/1          |  1/1
+-----------------------+
|          1/1          |  1/1
+-----------------------+
|          1/1          |  1/1
+-----------------------+
|          1/1          |  1/1
+-----------------------+
|          1/1          |  1/1
+-----------------------+
|          1/1          |  1/1
+-----------------------+
|          1/1          |  1/1
+-----------------------+
|          1/1          |  1/1
+-----------------------+
|          1/1          |  1/1
+-----------------------+
|          1/1          |  1/1
+-----------------------+
|          1/1          |  1/1
+-----------------------+
|          1/1          |  1/1
+-----------------------+
```