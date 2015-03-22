## Arrays ##
Global arrays.

```

//Self explanatory
xvel[TPoints];
yvel[TPoints];
x[TPoints];
y[TPoints];

//An array of all pixels
allCoords[workWidth*workHeight];

//Predefined collision array
collisions[TElements][TElements];
```

## Atmosphere ##
Defines the environment in which the reactions take place. Modified through Preferences.

```
struct Atmosphere
{
    heat; //The temperature of the surroundings
    gravity; //-1 - 1, the multiplier for fall speed
    borderTop; //Whether elements can fall through the top
    borderLeft;// ... left
    borderBottom;// ... bottom
    borderRight;// ... right
    name; //The name given to the atmosphere
    red, green, blue; //The RGB color of the background, this color is then applied to the eraser
};
```

## Custom Elements ##
Users can create custom elements and save them to files with .ele extensions, to be loaded dynamically at runtime (and also downloaded, emailed, etc).

```
struct CustomElement
{
    //Stuff goes in here
};
```

### Input: ###
Overall:
  * name
Solid state:
  * state = 0;
  * density
  * special
  * fallspeed
  * RGB sliders
Melting point:
  * heat
Liquid state:
  * state = 1;
  * density
  * special
  * fallspeed
  * RGB sliders
Vaporizing point:
  * heat
Gaseous state:
  * state = 2;
  * density
  * special
  * fallspeed
  * RGB sliders

## File Structure ##
  * sdcard/thelements - root
  * sdcard/thelements/elements - custom elements folder, all .ele files to be loaded go in here
  * sdcard/thelements/atmospheres - all created .atm files go here to be offered during atmosphere selection
  * sdcard/thelements/saves - .sav files are placed in here with date and time in the filename, i.e. 2011-01-04-12:26:00.sav
  * sdcard/thelements/app - all files used by the app are placed in here - nothing right now