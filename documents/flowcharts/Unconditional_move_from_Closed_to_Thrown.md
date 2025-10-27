### Unconditional move from Closed to Thrown

The servo will move regardless of whether it has been locked.

``` mermaid
graph TD;
  A[Unconditional move to thrown event received]
  A --> B{Is the<br>servo already<br>at thrown?}
  B --> |Yes| F
  B --> |No| C[Send closed leaving event]
  C --> D
  D[Start servo moving towards thrown]
  D --> E{Has the<br>servo reached<br>thrown?}
  E ----> |Yes| F[Send reached thrown event]
  E --> |No| G[Continue moving<br>towards thrown]
  G --> E
```
