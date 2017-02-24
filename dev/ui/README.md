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
| `<Button>`     | `sfg::Button`       |
| `<Entry>`      | `sfg::Entry`        |

**TODO:** More to come

# Details by Widget Type

## Common Attributes

These attributes are shared by all widget types:

| Attribute         | Type      | Description                                                              |
|-------------------|-----------|--------------------------------------------------------------------------|
| id                | string    | Widget ID                                                                |
| class             | string    | Widget class                                                             |
| visible           | bool      | Widget visibility                                                        |

## Window

Defines a window on-screen to contain other widget types.

### Features
| Feature                           | Applies |
|-----------------------------------|---------|
| Accepts child elements            | YES     |
| Accepts text                      | NO      |

### Attributes
| Attribute         | Type      | Description                                                              |
|-------------------|-----------|--------------------------------------------------------------------------|
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

Simple textual label widget.

### Features
| Feature                           | Applies |
|-----------------------------------|---------|
| Accepts child elements            | NO      |
| Accepts text                      | YES     |

### Attributes
| Attribute         | Type      | Description                                                              |
|-------------------|-----------|--------------------------------------------------------------------------|
| left              | float     | Upper left corner X position (negative values start at opposite end)     |
| top               | float     | Upper left corner Y position (negative values start at opposite end)     |
| width             | float     | Label width                                                              |
| min-width         | float     | Minimum label width                                                      |
| height            | float     | Label height                                                             |
| min-height        | float     | Minimum label height                                                     |
| alignment_x       | float     | Set alignment of widget in the X direction (leftmost is 0.0)             |
| alignment_y       | float     | Set alignment of widget in the Y direction (topmost is 0.0)              |

## Box

Groups one or more widgets together, and can be aligned vertically or horizontally.

### Features
| Feature                           | Applies |
|-----------------------------------|---------|
| Accepts child elements            | YES     |
| Accepts text                      | NO      |

### Attributes
| Attribute         | Type      | Description                                                                        |
|-------------------|-----------|------------------------------------------------------------------------------------|
| orientation       | string    | Orientation of contained elements. Can be "horizontal" (default) or "vertical".    |
| spacing           | float     | Spacing between contained elements. Default 0.                                     |

## Alignment

Allows the specification of additional attributes that control the position of a single child element. This widget only accepts one single child element.

### Features
| Feature                           | Applies |
|-----------------------------------|---------|
| Accepts child elements            | YES     |
| Accepts text                      | NO      |

### Attributes
| Attribute         | Type      | Description                                                              |
|-------------------|-----------|--------------------------------------------------------------------------|
| left              | float     | Upper left corner X position (negative values start at opposite end)     |
| top               | float     | Upper left corner Y position (negative values start at opposite end)     |
| width             | float     | Width                                                                    |
| min-width         | float     | Minimum width                                                            |
| height            | float     | Height                                                                   |
| min-height        | float     | Minimum height                                                           |
| alignment_x       | float     | Set alignment of widget in the X direction (leftmost is 0.0)             |
| alignment_y       | float     | Set alignment of widget in the Y direction (topmost is 0.0)              |
| scale_x           | float     | Proportion of X-space child is allowed to expand to (0.0 to 1.0)         |
| scale_y           | float     | Proportion of Y-space child is allowed to expand to (0.0 to 1.0)         |

## Button

Plain old, simple push-button widget.

### Features
| Feature                           | Applies |
|-----------------------------------|---------|
| Accepts child elements            | NO      |
| Accepts text                      | YES     |

### Attributes
| Attribute         | Type      | Description                                                              |
|-------------------|-----------|--------------------------------------------------------------------------|
| left              | float     | Upper left corner X position (negative values start at opposite end)     |
| top               | float     | Upper left corner Y position (negative values start at opposite end)     |
| width             | float     | Width                                                                    |
| min-width         | float     | Minimum width                                                            |
| height            | float     | Height                                                                   |
| min-height        | float     | Minimum height                                                           |

## Entry

Widget that accepts textual input.

### Features
| Feature                           | Applies |
|-----------------------------------|---------|
| Accepts child elements            | NO      |
| Accepts text                      | YES     |

### Attributes
| Attribute         | Type      | Description                                                              |
|-------------------|-----------|--------------------------------------------------------------------------|
| left              | float     | Upper left corner X position (negative values start at opposite end)     |
| top               | float     | Upper left corner Y position (negative values start at opposite end)     |
| width             | float     | Width                                                                    |
| min-width         | float     | Minimum width                                                            |
| height            | float     | Height                                                                   |
| min-height        | float     | Minimum height                                                           |


ConcordiaME GUI API
===================

# bluebear.gui

The GUI API is accessible on the global object `bluebear.gui` as well as more methods available on `LuaElement` objects. `bluebear.gui` exposes two methods allowing you to enter a widget tree: `get_element_by_id` and `get_element_by_class`. The "id" and "class" properties work similar to an HTML page within a web browser.

* `get_element_by_id` will return the **first** item encountered with the matching "id" attribute. The "id" attribute is meant to be unique across the entire application; therefore, it is suggested you use "id" as sparingly as possible to maintain compatibility with other mods. **Multiple IDs with the same name produces undefined behaviour.**
* `get_elements_by_class` will return all items matching the given class.

It is recommended you prefix your IDs and classes with a vendor prefix. For example, if your organisation is named "Toys Incorporated", you should prefix your widget ID or class with the "toys_" prefix. Because all widgets share a global namespace, this will help you avoid collisions with other mods. The `system` namespace is reserved for Concordia's built-in mods.

# LuaElement instances

The above functions will return `LuaElement` instances. `LuaElement` instances are just functions wrapping the shared_ptr for the sfg::Widget inside the game engine. That is, you can let Lua GC these objects, and the only thing that should happen is that the reference count is decremented.

## Methods

TODO
