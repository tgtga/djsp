src:
  all files here are automatically compiled into the libdjsp library
  none of them have entry points, so they can't be compiled into standalone executables

  intmath:
    useful functions operating only on machine-length integers
  logging:
    functions and variables relating to printing messages with iso 8601 time to stdout+logs
  step:
    functions for processing one step of the sequence
  oneshot:
    functions for calculating the length of one whole chain given the starting value
  sequence:
    functions for calculating chains over a range, with memoization and HWM output

experiments:
  old shit left over from v0.2.0 that i felt was useful enough to not nuke

djsp.rb:
  the main executable
