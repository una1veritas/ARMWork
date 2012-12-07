// (c) Wolfgang Wieser 2012. GPL v2. 

struct MandelContext
{
	// Image size in pixels. 
	int width,height;
	
	// Z plane coordinates. 
	float x0,y0,xs,ys;
	
	// Iteration limit. 
	int maxit;
};

int ComputeMandel_XY(const struct MandelContext *c,int px,int py);
int ComputeMandelbrot(int w,int h);

int SqrtBenchmark(void);
