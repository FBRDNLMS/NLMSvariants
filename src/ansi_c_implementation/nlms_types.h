
/* *svg graph building* */
typedef struct {
	double xVal[7];
	double yVal[7];
}point_t;

/* *ppm read, copy, write* */
typedef struct {
	unsigned char red, green, blue;
}colorChannel_t;

typedef struct {
	int x, y;
	colorChannel_t *data;
}imagePixel_t;

enum fileSuffix_t{ // used in conjunction with mkFileName()
    PURE_WEIGHTS,
    USED_WEIGHTS,
    DIRECT_PREDECESSOR,
    RESULTS,
    LOCAL_MEAN,
    TEST_VALUES,
    DIFFERENTIAL_PREDECESSOR
};


