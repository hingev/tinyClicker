
# tinyClicker

A bodged presentation clicker based on two parts.
The transmitter is an Attiny85 and the Receiver is an STM32F103.

```
+-------------------------------------------------------+
|                                                       |
|                                        IR LED         |
|              BUT1            t85                      |
|                            +-------+     XXXX         |
|               +--+         |       +-----+XXXX        |
|      +--------+  +---------+       |     XXXX         |
|      |                     |       |                  |
|      +--------+  +---------+       |                  |
|      |        +--+         +-------+                  |
|    +---+                               +              |
|     +-+      BUT2                      |              |
|      +                                 |              |
|                            <-----------+              |
+-------------------------------------------------------+
                             |
                             |
                             |
                 +-----------+
                 |
 +---------------+---------------------------------------------+
 |                                                             |
 |  TL 1838                                                    |
 |                                                             |
 |    XXXXX                                                    |
 |    XXXXX+      +------------------------------+             |
 |    XXXXX|      |                              |   USB HID   |
 |         |      |       STM32F103              |   AS A      |
 |         +------+                              +--------->   |
 |                |                              |   KEYBOARD  |
 |                +------------------------------+             |
 |                                                             |
 |                                                             |
 |                                                             |
 +-------------------------------------------------------------+

```
