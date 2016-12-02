Concordia Engine Refactor
=========================

The **Engine Refactor** is moving flow of async function control out of `system.entity.base` and onto a lower level of abstraction.

# Serializable

Any and all contexts (tables) passed in creating callbacks MUST BE SERIALIZABLE! This ensures that we can properly save the game and load it. The idea here will be to go for using the `_cid` field on serializable tables as a replacement for the RAM address of the instances when saving and loading to a lot JSON file. The new design should only use this cid on loading and saving! To simplify, all operations are done using regular ol' RAM addresses; the cid only lets us translate those references to a file, and then recreate identical references when loading from a file.

Serializable objects extend `system.serializable.base` and should provide `load()`, `save()`, and a cid field. The tables only ever generate and keep a cid field; the engine only ever uses this to recreate live instances across files.

If a callback attempts to bind to an un-serializable table, your modpack will immediately throw a runtime exception and be removed from the lot (additionally, any modpacks adding UI elements will be terminated and removed from the display). This is because you cannot guarantee that the lot will save its state properly otherwise. Do not attempt to bind functions to un-serializable tables.

# Intended Behaviour

## API Design
* 'bluebear.bind( func, context, args... )'
 * Implemented as something similar to (pseudocode) `function bind( func, context ) return function () func( context ) end end`
 * We can get the args out for serialization by simply rebinding the function to a utility function that just scrapes `...`
* `bluebear.set_callback( func, time )`
 * Set a callback to be called in "time" ticks.
* `bluebear.defer( func )`
 * Alias to `bluebear.setCallback( func, 0 )`
* Adjust promises accordingly.

## Internals
`BlueBear::Engine::objectLoop` will be replaced with a new function called `BlueBear::Engine::eventLoop`. Where `objectLoop` iterates over all `system.entity.base` objects placed on the lot, `eventLoop` will instead process "messages" (function calls) placed in an event queue by either the engine itself or its scripts.

### Event Loop
The **Event Loop** essentially processes all function calls placed in the event queue, just like JavaScript on the browser. New functions are placed in the event queue either by the engine itself (loading a script), or within scripts themselves using the above calls.

`
while( queue.hasItem() ) { queue.takeOne().run(); }
`

Where "queue" is a FIFO queue type with a template class serving as a C++ abstraction of a Lua function call.

#### BlueBear::Engine::QueueEvent

This type abstracts the function call due to be executed.

#### Delaying function calls

Function calls delayed by `bluebear.set_callback` are delayed for ***AT LEAST*** the number of ticks specified in "time". Like `setTimeout` in JavaScript, the given time interval is not a guarantee, only a minimum.

This should be easy enough. Before `queue.takeOne().run()` above, simply check a secondary list of structs containing a queue item and deadline. If this tick is greater than or equal to the tick on the struct, throw it on the queue quickly before running the next queue item.

From MDN on how JavaScript works:

>Calling setTimeout will add a message to the queue after the time passed as a second argument. If there is no other message in the queue, the message is processed right away; however, if there are messages, the setTimeout message will have to wait for other messages to be processed. For that reason the second argument indicates a minimum time and not a guaranteed time.
