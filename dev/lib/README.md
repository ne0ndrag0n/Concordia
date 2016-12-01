Third-Party Libraries
=====================

Concordia makes use of various **third-party libraries**. This document describes how to install them, what versions are used, and if any modifications were made.

# SFGUI
## Version: (tree 1f54526) (ne0ndrag0n/SFGUI)

## Modifications
* **TODO** Added the `CPaddingNudgeX` and `CPaddingNudgeY` properties to the BREW engine. This allows us to make "nudging" possible in labels where it wouldn't otherwise be possible.
 * I would like to get these PR'd into the main SFGUI project, but the current implementation is a hack. This is because the only padding property currently in SFGUI is mixed into a math problem affecting all dimensions (see `sfg::eng::BREW::CreateWindowDrawable`). When time is available (and it never will be), overhaul the way ne0ndrag0n/SFGUI handles the padding property so that we can submit a proper PR to the TankOS/SFGUI mainline.
 * *Warning:* `Cxyz` properties are internal properties used by Concordia. They are intended for the global theme used by ConcordiaME and thus were barely tested at all. If you're a mod developer, you use them at your own peril. If you're thinking of using them, you probably don't need 'em. Mods using internal properties or private APIs are not eligible for inclusion on ne0ndrag0n ModWorld.
* **TODO** Add the ability to dependency inject logging in SFGUI so that log messages emitted by SFGUI print properly (in our case, LogWyvern format)
 * This is definitely something we'd be able to PR into the TankOs project since it involves well-designed, properly-implemented changes that work for every user.
