### Unconditional move from Closed to Thrown

The servo will move regardless of whether it has been locked.

``` mermaid

flowchart TD;
%% define the nodes ...
A([Start])
B[Unconditional move to thrown event received]
C{Is the<br>servo already<br>at thrown?}
D[Send leaving closed event]
E[Start servo moving<br>towards thrown]
F{Has the<br>servo reached<br>thrown?}
G[Send reached thrown event]
H[Continue moving<br>towards thrown]
I([End])

%% ... then define the links.
A --> B
B --> C
C ----> |Yes| G
C --> |No| D
D --> E
E --> F
F --> |Yes| G
F --> |No| H
H --> F
G --> I
```
