    numberOfFiles=27;
    const char* imagePrefix="Capture/Jupiter/Canyon-CNE-HWC1-webcam/";
    const char* testImages[]=
    {
        "1525045219.jpg",
        "1525045222.jpg",
        "1525045225.jpg",
        "1525045232.jpg",
        "1525045236.jpg",
        "1525045241.jpg",
        "1525045244.jpg",
        "1525045247.jpg",
        "1525045251.jpg",
        "1525045252.jpg",
        "1525045253.jpg",
        "1525045259.jpg",
        "1525045309.jpg",
        "1525045321.jpg",
        "1525045329.jpg",
        "1525045335.jpg",
        "1525045341.jpg",
        "1525045349.jpg",
        "1525045355.jpg",
        "1525045363.jpg",
        "1525045369.jpg",
        "1525045376.jpg",
        "1525045461.jpg",
        "1525045465.jpg",
        "1525045469.jpg",
        "1525045473.jpg",
        "1525045477.jpg",
    };
    strcpy(filenameFull, imagePrefix);
    strcat(filenameFull, testImages[fileIndex]);
    srcImage = imread(filenameFull, CV_LOAD_IMAGE_UNCHANGED);
