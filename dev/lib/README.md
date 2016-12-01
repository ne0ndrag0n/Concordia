Third-Party Libraries
=====================

Concordia makes use of various **third-party libraries**. This document describes how to install them, what versions are used, and if any modifications were made.

# SFGUI
## Version: (tree 1f54526) (ne0ndrag0n/SFGUI)

## Modifications
* **TODO** Add the `CFontCompensation` property, which will allow us to nudge incorrectly calculated Y-axis font spacing in all widgets. This is so **Perfect DOS VGA** or **Terminus** looks good as presented.
 * *Warning:* `Cxyz` properties are internal properties used by Concordia. They are intended for the global theme used by ConcordiaME and thus were barely tested at all. If you're a mod developer, you use them at your own peril. If you're thinking of using them, you probably don't need 'em. Mods using internal properties or private APIs are not eligible for inclusion on ne0ndrag0n ModWorld.
 * Consider forking `sfg::eng::BREW` to something like `sfg::eng::CBrewEngine`. There's just intermittent poking around here to get what we need, which shouldn't require we modify the library.
* **TODO** Add the ability to dependency inject logging in SFGUI so that log messages emitted by SFGUI print properly (in our case, LogWyvern format)
 * This is definitely something we'd be able to PR into the TankOs project since it involves well-designed, properly-implemented changes that work for every user.
