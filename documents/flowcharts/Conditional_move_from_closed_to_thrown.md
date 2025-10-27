### Conditional move from Closed to Thrown

The servo will only move if it is not locked.

``` mermaid
flowchart TD;
%% define the nodes ...
A([Start])
B[Conditional move to thrown event received]
C{Is the servo<br>locked?}
D[Send servo locked event]
E[Lock servo]
F{Is the<br>servo already<br>at thrown?}
G[Send thrown reached event]
H[Send closed leaving event]
I[Start servo moving<br>towards thrown]
J{Has the<br>servo reached<br>thrown?}
K[Continue moving<br>towards thrown]
L([End])


%% ... then define the links.
A --> B
B --> C
C --> |Yes| D
C --> |No| E
E --> F
F ----> |Yes| G
F --> |No| H
H --> I
I --> J
J --> |Yes| G
J --> |No| K
K --> J

D --> L
G --> L




```
