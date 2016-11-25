Concordia
=========

<img src="http://i.imgur.com/3xK1zDE.png">

## Version 0.0.3 - Milestone "Picasso"

# Introduction

**Concordia** is a social simulation for PC and Linux. Construct your dream house, place your dolls within, and guide your household to success as its members navigate the exciting world of Concordia!

## Features

* Dolls featuring artificial intelligence: can you guide their lives better than they can, or do you prefer to let them live hands-off?
* Scenarios for your household: start off healthy, wealthy and privileged...or disabled, poor, and minority. Your life experience is dictated by the family you create and the traits you select in each individual doll!
* Select any one of your dolls and accompany them to work: guide your household to success within any one of seven career paths!
* Tend to their physical well-being by fulfilling five motive groups, and their emotional well-being in one of four personal aspirations. They can't do it alone!
* Extend the game and its features with the **Concordia Construction Kit**: create new scenarios, needs & wants, dolls, traits, objects, and so much more!

## Technical Highlights

* Lua 5.3 scripting system: everything the game uses outside of its base structure as a dollhouse game is designed as an extensible script (e.g. all objects and types of objects are defined as Lua classes)
* Isometric 2.5D presentation with a retro-inspired pixel art look-and-feel
* SFML-powered multimedia framework featuring the SFGUI widget toolkit
* Multi-threaded architecture, using modern C++14 to make the most of your multi-core CPU

## Dependencies
* SFML 2.3.x
* SFGUI (manual install required)
* Lua 5.3.x
* JsonCpp 1.7.x
* GLM
* GLEW
* Assimp
* Amanieu/asyncplusplus (manual install required, make sure libasync++.so is in /usr/lib)
