Rules for Concurrent Operation
==============================

These **golden rules** shall be followed to avoid concurrency bugs that will be essentially impossible to diagnose:

* Do not switch a shader program while something else may be writing uniforms (usually right before a draw operation). This basically means don't do concurrent draws. Draw operations may still block to allow things like geometry uploads to take place.
