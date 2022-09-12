SL(1): Cure your bad habit of mistyping
=======================================

SL (Steam Locomotive) runs across your terminal when you type "sl" as
you meant to type "ls". It's just a joke command, and not useful at
all.

## Usage
```bash
$ sl [-acFl] [-n N]
```

## DESCRIPTION
Sl is a program that causes a random SL (Steam Locomotive) or other vehicle/animal to run across your terminal when you type "sl" as you meant to type "ls". It\'s just a joke command, and not useful at all.

## OPTIONS
If neither `-c`, `-l` nor `-n N` are specified, a random train/vehicle/animal will appear.

`-a` - Causes an accident to happen

`-c` - Causes train/vehicle/animal to be a C51 locomotive. This overrides option `-n`, and can be overridden by option `-l`.

`-F` - Causes train/vehicle/animal to fly 

`-l` - Causes train/vehicle/animal to be a little locomotive. This overrides both options `-c` and `-n`.

`-n N` - Selects train/vehicle/animal N to be chosen for animation. This option is overridden by both `-c` and `-n`. The following options are available. All other options are invalid.
|Option|Description|
| :--- | :--- |
|0|Little locomotive|
|1|C51 locomotive|
|2|D51 locomotive|
|3|Horses|
|4|Biplane|

Copyright 1993,1998,2014 Toyoda Masashi (mtoyoda@acm.org)

![](demo.gif)