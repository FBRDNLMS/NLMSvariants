
/* * machine learning * */					
typedef struct {					
	double *weights;
	unsigned windowSize;
	unsigned samplesCount;
	double learnrate;
} mldata_t;

/* *svg graph building* */
typedef struct {						// Axis x,y
	double xVal[7];
	double yVal[7];
}point_t;

/* *ppm read, copy, write* */
typedef struct {
	unsigned char red, green, blue;
}colorChannel_t;

typedef struct {						// Storage for image data 
	int x, y;
	colorChannel_t *data;
}imagePixel_t;						

/** file handling* */
enum fileSuffix_t{ 						// Used in conjunction with mkFileName()
    PURE_WEIGHTS,
    USED_WEIGHTS,
    DIRECT_PREDECESSOR,
    RESULTS,
    LOCAL_MEAN,
    TEST_VALUES,
    DIFFERENTIAL_PREDECESSOR
};

enum fileHeader{						
	LOCAL_MEAN_HEADER,
	DIRECT_PREDECESSOR_HEADER,
	DIFFERENTIAL_PREDECESSOR_HEADER
};

