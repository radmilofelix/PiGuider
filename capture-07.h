    numberOfFiles=24;
    const char* imagePrefix="Capture/Polaris/Trust-16429-webcam/";
    const char* testImages[]=
    {
        "1525124292.jpg",
        "1525124295.jpg",
        "1525124317.jpg",
        "1525124321.jpg",
        "1525124326.jpg",
        "1525124330.jpg",
        "1525124332.jpg",
        "1525124398.jpg",
        "1525124510.jpg",
        "1525124527.jpg",
        "1525124541.jpg",
        "1525124697.jpg",
        "1525124710.jpg",
        "1525124732.jpg",
        "1525124791.jpg",
        "1525124813.jpg",
        "1525124814.jpg",
        "1525124856.jpg",
        "1525124899.jpg",
        "1525124939.jpg",
        "1525125324.jpg",
        "1525125352.jpg",
        "1525125370.jpg",
        "1525125373.jpg",
    };
    strcpy(filenameFull, imagePrefix);
    strcat(filenameFull, testImages[fileIndex]);
    srcImage = imread(filenameFull, CV_LOAD_IMAGE_UNCHANGED);
