ConcordiaME
===========

**ConcordiaME** (Concordia Markup Engine) is the GUI toolkit Concordia exposes to mods and system scripts. It is based on SFGUI and uses XML for structure, SFGUI/BREW for stylesheets, and standard Lua for scripting.

# Preliminary Notes
Most of this functionality is contained within `BlueBear::Graphics::WidgetBuilder`. We'll probably be changing the name of WidgetBuilder as it will eventually encompass minimal callback functionality (handoff commands to the Engine thread).

Everything is case-sensitive.

# SFGUI Widgets Exposed by ConcordiaME

| XML Tag        | SFGUI Widget        |
|----------------|---------------------|
| `<Window>`     | `sfg::Window`       |
| `<Label>`      | `sfg::Label`        |
| `<Box>`        | `sfg::Box`          |
| `<Alignment>`  | `sfg::Alignment`    |

**TODO:** More to come

# Details by Widget Type
## Window
`<Window><child></child>...</Window>`

### Features
| Feature                           | Applies |
|-----------------------------------|---------|
| Accepts child elements            | YES     |
| Accepts ID and class              | YES     |

### Attributes
| Attribute         | Type      | Description                                                              |
|-------------------|-----------|--------------------------------------------------------------------------|
| id                | string    | Window Style ID                                                          |
| class             | string    | Window Style class                                                       |
| left              | float     | Upper left corner X position (negative values start at opposite end)     |
| top               | float     | Upper left corner Y position (negative values start at opposite end)     |
| width             | float     | Window width                                                             |
| min-width         | float     | Minimum window width                                                     |
| height            | float     | Window height                                                            |
| min-height        | float     | Minimum window height                                                    |
| title             | string    | Title to be displayed in the Window titlebar                             |
| titlebar          | bool      | Display titlebar                                                         |
| background        | bool      | Display window background (false for fully transparent)                  |
| resize            | bool      | Display resize handle in lower right corner (and permit user-resize)     |
| shadow            | bool      | Display drop shadow                                                      |
| close             | bool      | Display close button in titlebar. It's up to you to write an onClose.    |

## Label
`<Label>Example Label Text</Label>`

### Features
| Feature                           | Applies |
|-----------------------------------|---------|
| Accepts child elements            | NO      |
| Accepts ID and class              | YES     |

### Attributes
| Attribute         | Type      | Description                                                              |
|-------------------|-----------|--------------------------------------------------------------------------|
| id                | string    | Label Style ID                                                           |
| class             | string    | Label Style class                                                        |
| left              | float     | Upper left corner X position (negative values start at opposite end)     |
| top               | float     | Upper left corner Y position (negative values start at opposite end)     |
| width             | float     | Label width                                                              |
| min-width         | float     | Minimum label width                                                      |
| height            | float     | Label height                                                             |
| min-height        | float     | Minimum label height                                                     |
| alignment_x       | float     | Set alignment of widget in the X direction (leftmost is 0.0)             |
| alignment_y       | float     | Set alignment of widget in the Y direction (topmost is 0.0)              |

## Box
`<Box><child></child>...</Box>`

### Features
| Feature                           | Applies |
|-----------------------------------|---------|
| Accepts child elements            | YES     |
| Accepts ID and class              | YES     |

### Attributes
| Attribute         | Type      | Description                                                                        |
|-------------------|-----------|------------------------------------------------------------------------------------|
| id                | string    | Label Style ID                                                                     |
| class             | string    | Label Style class                                                                  |
| orientation       | string    | Orientation of contained elements. Can be "horizontal" (default) or "vertical".    |
| spacing           | float     | Spacing between contained elements. Default 0.                                     |

## Alignment
`<Alignment><child></child>...</Alignment>`

### Features
| Feature                           | Applies |
|-----------------------------------|---------|
| Accepts child elements            | YES     |
| Accepts ID and class              | YES     |

### Attributes
| Attribute         | Type      | Description                                                              |
|-------------------|-----------|--------------------------------------------------------------------------|
| id                | string    | Style ID                                                                 |
| class             | string    | Style class                                                              |
| left              | float     | Upper left corner X position (negative values start at opposite end)     |
| top               | float     | Upper left corner Y position (negative values start at opposite end)     |
| width             | float     | Width                                                                    |
| min-width         | float     | Minimum width                                                            |
| height            | float     | Height                                                                   |
| min-height        | float     | Minimum height                                                           |
| alignment_x       | float     | Set alignment of widget in the X direction (leftmost is 0.0)             |
| alignment_y       | float     | Set alignment of widget in the Y direction (topmost is 0.0)              |
