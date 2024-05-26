# Schematic

## LED Matrix


|     |    | Col | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 |
|-----|----|-----|---|---|---|---|---|---|---|---|
|     |    | Pin | 13| 3 | 4 | 10| 6 | 11| 15| 16|
| Row | Pin|     |   |   |   |   |   |   |   |   |
|  1  |  9 |     |   |   |   |   |   |   |   |   |
|  2  | 14 |     |   |   |   |   |   |   |   |   |
|  3  |  8 |     |   |   |   |   |   |   |   |   |
|  4  | 12 |     |   |   |   |   |   |   |   |   |
|  5  |  1 |     |   |   |   |   |   |   |   |   |
|  6  |  7 |     |   |   |   |   |   |   |   |   |
|  7  |  2 |     |   |   |   |   |   |   |   |   |
|  8  |  5 |     |   |   |   |   |   |   |   |   |

## Connections

```
       C1   C2   C3   C4   C5   C6   C7   C8 
        |    |    |    |    |    |    |    | 
        \    \    \    \    \    \    \    \ 
        /    /    /    /    /    /    /    /  50 Ohms 
        \    \    \    \    \    \    \    \ 
        /    /    /    /    /    /    /    / 
        |    |    |    |    |    |    |    | 
    R1 _V_  _V_  _V_  _V_  _V_  _V_  _V_  _V_
        |    |    |    |    |    |    |    | 
        |\   |\   |\   |\   |\   |\   |\   |\     D     S
        | -----o----o----o----o----o----o----o-----   v-------GND
        |    |    |    |    |    |    |    |       |---|
    R2 _V_  _V_  _V_  _V_  _V_  _V_  _V_  _V_      ^^^^T
        |    |    |    |    |    |    |    |         g |------r1
        |\   |\   |\   |\   |\   |\   |\   |\     D     S                          
        | -----o----o----o----o----o----o----o-----   v-------GND                             
                                                   |---|                 
                                                   ^^^^T                  
                                                     g |------r2              
    ...                                                        
    R16                                                         
```

# Port Usage

|  Port |  Usage  |
| ------ | ------- |
| 1    | Row 1 - 8 |
| 2    | Columns   |
| 3    |           |
| 4    |           |
| 5    | Row 9 - 16 |
| 6    | Joystick  |



# Display Refresh

The DMA feature of the uC will be used to generate the display raster

## DMA Usage

3 DMA channels are used

 * PORT 1   -  R1 ... R8
 * PORT 2   -  C1 ... C8
 * PORT 5   -  R9 ...R16


# E-Paper

| Epaper | cpu        |
| ------ | ---        |
| busy   | P3.7 GPIO 
| rst    | P3.6 GPIO
| dc     | P3.2 GPIO
| cs     | P3.0 GPIO
| clk    | P3.3 Clk
| din    | P3.1 SIMO
| gnd    |
| vcc    |

