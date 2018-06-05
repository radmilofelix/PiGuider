#ifndef GLOBALSETTINGS_H
#define GLOBALSETTINGS_H


// Enable if building for PC, also enable CONFIG += PCRUN in the project file
#ifndef RUNONPC
#define RUNONPC
#endif

#ifndef DEBUG
#define DEBUG
    #ifndef CAPTUREFROMFILE
    #define CAPTUREFROMFILE
    #endif
    #ifndef NODSLR
//    #define NODSLR
    #endif
#endif

#ifndef IMAGELABELSVERTIACALALIGNMENTMIDDLE
#define IMAGELABELSVERTIACALALIGNMENTMIDDLE
#endif


#endif // GLOBALSETTINGS_H
