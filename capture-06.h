    numberOfFiles=52;
    const char* imagePrefix="Capture/Polaris/Canyon-CNE-CWC3-webcam/";
    const char* testImages[]=
    {
        "1525126258.jpg",
        "1525126262.jpg",
        "1525126352.jpg",
        "1525126393.jpg",
        "1525126408.jpg",
        "1525126467.jpg",
        "1525126489.jpg",
        "1525126516.jpg",
        "1525126557.jpg",
        "1525126577.jpg",
        "1525126600.jpg",
        "1525126650.jpg",
        "1525126655.jpg",
        "1525126677.jpg",
        "1525126713.jpg",
        "1525126743.jpg",
        "1525126744.jpg",
        "1525126747.jpg",
        "1525126753.jpg",
        "1525126765.jpg",
        "1525126773.jpg",
        "1525126783.jpg",
        "1525126791.jpg",
        "1525126800.jpg",
        "1525126838.jpg",
        "1525126874.jpg",
        "1525126875.jpg",
        "1525126879.jpg",
        "1525126883.jpg",
        "1525126885.jpg",
        "1525126902.jpg",
        "1525126911.jpg",
        "1525126934.jpg",
        "1525126938.jpg",
        "1525126953.jpg",
        "1525126964.jpg",
        "1525126977.jpg",
        "1525127010.jpg",
        "1525127051.jpg",
        "1525127107.jpg",
        "1525127146.jpg",
        "1525127152.jpg",
        "1525127157.jpg",
        "1525127178.jpg",
        "1525127199.jpg",
        "1525127229.jpg",
        "1525127251.jpg",
        "1525127272.jpg",
        "1525127300.jpg",
        "1525127329.jpg",
        "1525127353.jpg",
        "1525127368.jpg",
    };
    strcpy(filenameFull, imagePrefix);
    strcat(filenameFull, testImages[fileIndex]);
    srcImage = imread(filenameFull, CV_LOAD_IMAGE_UNCHANGED);
