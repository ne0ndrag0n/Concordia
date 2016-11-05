Wall Panel Models
=================

# Rotation-based Texturing

Concordia places wall panels and textures them according to your current rotation. When rotating, the walls need a new texture from the texture atlas builder, using these settings.

## Y-Axis

### Rotation 0

#### Texture Adjustments
|Atlas Entry | Wallpaper Side| Face   |
|------------|---------------|---------
|Side2       | Left          | Front

<p align="center">
  <img src="http://i.imgur.com/sx0Ezzx.png">
</p>

### Rotation 1

#### Texture Adjustments
|Atlas Entry | Wallpaper Side| Face   |
|------------|---------------|---------
|Side2       | Right         | Rear

<p align="center">
  <img src="http://i.imgur.com/ZDdeeG1.png">
</p>


### Rotation 2

#### Texture Adjustments
|Atlas Entry | Wallpaper Side| Face   |
|------------|---------------|---------
|Side1       | Left          | Rear

<p align="center">
  <img src="http://i.imgur.com/xqZLgIk.png">
</p>

### Rotation 3

#### Texture Adjustments
|Atlas Entry | Wallpaper Side| Face   |
|------------|---------------|---------
|Side1       | Right         | Front

<p align="center">
  <img src="http://i.imgur.com/Q1q9l5Q.png">
</p>

## X-Axis

### Rotation 0

#### Texture Adjustments
|Atlas Entry | Wallpaper Side| Face   |
|------------|---------------|---------
|Side2       | Right         | Rear

<p align="center">
  <img src="http://i.imgur.com/3imZS1E.png">
</p>

### Rotation 1

#### Texture Adjustments
|Atlas Entry | Wallpaper Side| Face   |
|------------|---------------|---------
|Side1       | Left          | Rear

<p align="center">
  <img src="http://i.imgur.com/SglWuyf.png">
</p>


### Rotation 2

#### Texture Adjustments
|Atlas Entry | Wallpaper Side| Face   |
|------------|---------------|---------
|Side1       | Right         | Front

<p align="center">
  <img src="http://i.imgur.com/4ZplrKS.png">
</p>

### Rotation 3

#### Texture Adjustments
|Atlas Entry | Wallpaper Side| Face   |
|------------|---------------|---------
|Side2       | Left          | Front

<p align="center">
  <img src="http://i.imgur.com/mivWbIL.png">
</p>

# Corner Segments

Walls are split up into three segments: Two end pieces on each end, and one main piece. The end pieces are textured differently so that the entire face of a longer wall segment always faces the player. They can be shared by wall segments in opposing dimensions.

* Eliminate the edge pieces, they are no longer required or useful. Eliminate the edge logic. Possibly eliminate XWallInstance and YWallInstance in favor of WallSegment (see below), (type)WallInstance was a total disaster design-wise.
* Split all wall segments into three parts: the left 0.1, the middle 0.8, and the right 0.1. These form a rig of three different model instances held in one struct (WallSegment). WallCellBundlers should have more logic in order to know how to render themselves (move this out of display), and each pointer should point to the three-instance WallSegment.

## Brief Notes

* Differs by rotation
 * Rotation 0: Nudging the X piece one up may conflict with a Y piece in the wall cell above.
 * Rotation 1: No conflicts, but you may need an extra piece so the corner shows up at the top.
  * If you're drawing a corner cell (both X and Y present) and there is no Y-piece above, you need the extra piece which
    always goes to the left of the X piece in the corner cell.
 * Rotation 2: Nudging the Y piece one to the left may conflict with an X piece in the wall cell to the left.
 * Rotation 3: No nudging into any adjacent cells. Corner cells will always conflict in the corner, so they share a corner piece.
