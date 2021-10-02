# Build

Our build process explained.

## Dependency Graph

This graph shows the dependency between the targets.

The lowest row of targets only shows the final output, required to play.


              +------+
              | libc |
              +-+--+-+
                |  |
            v---+  +---v
       +--------+  +-------+
       | engine |  | crash |
       +--+-+-+-+  +-------+
          | | |
    +-----+ v +------------+
    |  +---------+         |
    |  | lolpack |         |
    |  +----+----+         |
    |       |              |
    |       v              |
    |    +-----+           |
    |    | res +-----------+-------------+
    |    +--+--+                         |
    |       |                            |
    |       v                            v
    |   +---+------+  +----------+ +------------+ +------------+
    +-->| game dll |  | game exe | | server dll | | server exe |
        +----------+  +----------+ +------------+ +------------+

