#define SDL_MAIN_HANDLED
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <stack>
#include <sstream>
#include <unistd.h>
#include "util.c"

using namespace std;

typedef struct {int x, y;} Point;

unsigned int * pixels;
int width, height;
SDL_Surface * window_surface;
SDL_Renderer * renderer;

std::string titulo = "SDL Graphics";

int pos_x = 0;
int pos_y = 0;

unsigned char b;
double valord;
bool b1;

unsigned char code(double    x, double    y,
          double xmin, double xmax, double ymin, double ymax)
{
   unsigned char code=0;

   if (y > ymax) code += 8;
   if (y < ymin) code += 4;
   if (x > xmax) code += 2;
   if (x < xmin) code += 1;

   return code;
}

Point getPoint(int x, int y)
{
    Point p;
    p.x = x;
    p.y = y;
    return p;
}

Uint32 getPixel(int x, int y)
{
    if((x>=0 && x<=width) && (y>=0 && y<=height))
        return pixels[x + width * y];
    else
        return -1;
}

void setPixel(int x, int y, int r, int g, int b, int a)
{
    pixels[x + y * width] = SDL_MapRGBA(window_surface->format, r, g, b, a);
}

void setPixel(int x, int y, int r, int g, int b)
{
    setPixel(x, y, r, g, b, 255);
}

void showMousePosition(SDL_Window * window, int x, int y)
{
    std::stringstream ss;
    ss << titulo << " X: " << x << " Y: " << y;
    SDL_SetWindowTitle(window, ss.str().c_str());
}

void printMousePosition(int x, int y)
{
    printf("Mouse on x = %d, y = %d\n",x,y);
}

void setPixel(int x, int y, Uint32 color)
{
    if((x<0 || x>=width || y<0 || y>=height)) {
        printf("Coordenada inválida : (%d,%d)\n",x,y);
        return;
    }
    pixels[x + y * width] = color;
}

Uint32 RGB(int r, int g, int b) {
    return SDL_MapRGBA(window_surface->format, r, g, b, 255);
}

Uint8 getColorComponent( Uint32 pixel, char component ) {

    Uint32 mask;

    switch(component) {
        case 'b' :
        case 'B' :
            mask = RGB(0,0,255);
            pixel = pixel & mask;
            break;
        case 'r' :
        case 'R' :
            mask = RGB(255,0,0);
            pixel = pixel & mask;
            pixel = pixel >> 16;
            break;
        case 'g' :
        case 'G' :
            mask = RGB(0,255,0);
            pixel = pixel & mask;
            pixel = pixel >> 8;
            break;
    }
    return (Uint8) pixel;
}

void target(int x, int y, Uint32 color)
{
    setPixel(x, y,color);
    setPixel(x-2, y, color);
    setPixel(x-1, y, color);
    setPixel(x+1, y, color);
    setPixel(x+2, y, color);
    setPixel(x, y-2, color);
    setPixel(x, y-1, color);
    setPixel(x, y+1, color);
    setPixel(x, y+2, color);
}

void floodFillRec(int x,int y,Uint32 fillColor, Uint32 defaultColor)
{
    if (y< 0 || y > height - 1 || x < 0 || x > width - 1)
        return;

    if(getPixel(x,y)==defaultColor)
    {
        setPixel(x,y,fillColor);
        floodFillRec(x+1,y,fillColor,defaultColor);
        floodFillRec(x-1,y,fillColor,defaultColor);
        floodFillRec(x,y+1,fillColor,defaultColor);
        floodFillRec(x,y-1,fillColor,defaultColor);

        floodFillRec(x-1,y-1,fillColor,defaultColor);
        floodFillRec(x-1,y+1,fillColor,defaultColor);
        floodFillRec(x+1,y-1,fillColor,defaultColor);
        floodFillRec(x+1,y+1,fillColor,defaultColor);
    }
}

void floodFill(int x,int y,Uint32 fillColor, Uint32 defaultColor)
{
    if (y< 0 || y > height - 1 || x < 0 || x > width - 1)
        return;

    stack<Point> st;
    st.push(getPoint(x,y));
    while (st.size() > 0)
    {
        Point p = st.top();
        st.pop();
        int x = p.x;
        int y = p.y;
        if (y < 0 || y > height - 1 || x < 0 || x > width - 1)
            continue;

        if (getPixel(x, y) == defaultColor)
        {
            setPixel(x, y, fillColor);
            st.push(getPoint(x+1,y));
            st.push(getPoint(x-1,y));
            st.push(getPoint(x,y+1));
            st.push(getPoint(x,y-1));
        }
    }
}

void floodFill(int x,int y,Uint32 fillColor )
{
    Uint32 defaultColor = getPixel(x,y);
    floodFill(x,y,fillColor,defaultColor);
}

void drawLine(int x1, int y1, int x2, int y2, Uint32 cor)
{
    int x,y,dx,dy,dx1,dy1,px,py,xe,ye,i;

    dx=x2-x1;
    dy=y2-y1;

    dx1=fabs(dx);
    dy1=fabs(dy);

    px=2*dy1-dx1;
    py=2*dx1-dy1;

    if(dy1<=dx1)
    {
        if(dx>=0)
        {
            x=x1;
            y=y1;
            xe=x2;
        }
        else
        {
            x=x2;
            y=y2;
            xe=x1;
        }
        setPixel(x,y,cor);
        for(i=0;x<xe;i++)
        {
            x=x+1;
            if(px<0)
            {
                px=px+2*dy1;
            }
            else
            {
                if((dx<0 && dy<0) || (dx>0 && dy>0))
                {
                    y=y+1;
                }
                else
                {
                    y=y-1;
                }
                px=px+2*(dy1-dx1);
            }
            setPixel(x,y,cor);
        }
    }
    else
    {
        if(dy>=0)
        {
            x=x1;
            y=y1;
            ye=y2;
        }
        else
        {
            x=x2;
            y=y2;
            ye=y1;
        }
        setPixel(x,y,cor);
        for(i=0;y<ye;i++)
        {
            y=y+1;
            if(py<=0)
            {
                py=py+2*dx1;
            }
            else
            {
                if((dx<0 && dy<0) || (dx>0 && dy>0))
                {
                    x=x+1;
                }
                else
                {
                    x=x-1;
                }
                py=py+2*(dx1-dy1);
            }
            setPixel(x,y,cor);
        }
    }
}

void drawRectangle(int x1, int y1, int x2, int y2, Uint32 lineColor)
{
    drawLine(x1,y1, x2,y1, lineColor);
    drawLine(x1,y2, x2,y2, lineColor);
    drawLine(x1,y1, x1,y2, lineColor);
    drawLine(x2,y1, x2,y2, lineColor);
}

void drawRectangle(int x1, int y1, int x2, int y2, Uint32 lineColor, Uint32 fillColor)
{

    int half_x, half_y;

    if(x2 > x1) {
        half_x = x1 + (int) ((x2 - x1) / 2);
    } else {
        half_x = x2 + (int) ((x1 - x2) / 2);
    }

    if(y2 > y1) {
        half_y = y1 + (int) ((y2 - y1) / 2);
    } else {
        half_y = y2 + (int) ((y1 - y2) / 2);
    }


    drawRectangle(x1,y1,x2,y2,lineColor);

    floodFill(half_x,half_y,fillColor);

}

void drawWuLine(int x0, int y0, int x1, int y1, Uint32 clrLine )
{
    /* Make sure the line runs top to bottom */
    if (y0 > y1)
    {
        int aux = y0; y0 = y1; y1 = aux;
        aux = x0; x0 = x1; x1 = aux;
    }

    /* Draw the initial pixel, which is always exactly intersected by
    the line and so needs no weighting */
    setPixel( x0, y0, clrLine );

    int xDir, deltaX = x1 - x0;
    if( deltaX >= 0 )
    {
        xDir = 1;
    }
    else
    {
        xDir   = -1;
        deltaX = 0 - deltaX; /* make deltaX positive */
    }

    /* Special-case horizontal, vertical, and diagonal lines, which
    require no weighting because they go right through the center of
    every pixel */
    int deltaY = y1 - y0;
    if (deltaY == 0)
    {
        /* Horizontal line */
        while (deltaX-- != 0)
        {
            x0 += xDir;
            setPixel( x0, y0, clrLine );
        }
        return;
    }
    if (deltaX == 0)
    {
        /* Vertical line */
        do
        {
            y0++;
            setPixel( x0, y0, clrLine );
        } while (--deltaY != 0);
        return;
    }

    if (deltaX == deltaY)
    {
        /* Diagonal line */
        do
        {
            x0 += xDir;
            y0++;
            setPixel( x0, y0, clrLine );
        } while (--deltaY != 0);
        return;
    }

    unsigned short errorAdj;
    unsigned short errorAccaux, weighting;

    /* Line is not horizontal, diagonal, or vertical */
    unsigned short errorAcc = 0;  /* initialize the line error accumulator to 0 */

    Uint32 rl = getColorComponent( clrLine, 'r' );
    Uint32 gl = getColorComponent( clrLine, 'g' );
    Uint32 bl = getColorComponent( clrLine, 'b' );
    double grayl = rl * 0.299 + gl * 0.587 + bl * 0.114;

    /* Is this an X-major or Y-major line? */
    if (deltaY > deltaX)
    {
    /* Y-major line; calculate 16-bit fixed-point fractional part of a
    pixel that X advances each time Y advances 1 pixel, truncating the
        result so that we won't overrun the endpoint along the X axis */
        errorAdj = ((unsigned long) deltaX << 16) / (unsigned long) deltaY;
        /* Draw all pixels other than the first and last */
        while (--deltaY) {
            errorAccaux = errorAcc;   /* remember currrent accumulated error */
            errorAcc += errorAdj;      /* calculate error for next pixel */
            if (errorAcc <= errorAccaux) {
                /* The error accumulator turned over, so advance the X coord */
                x0 += xDir;
            }
            y0++; /* Y-major, so always advance Y */
                  /* The IntensityBits most significant bits of errorAcc give us the
                  intensity weighting for this pixel, and the complement of the
            weighting for the paired pixel */
            weighting = errorAcc >> 8;
            /*
            ASSERT( weighting < 256 );
            ASSERT( ( weighting ^ 255 ) < 256 );
            */
            Uint32 clrBackGround = getPixel(x0, y0 );
            Uint8 rb = getColorComponent( clrBackGround, 'r' );
            Uint8 gb = getColorComponent( clrBackGround, 'g' );
            Uint8 bb = getColorComponent( clrBackGround, 'b' );
            double grayb = rb * 0.299 + gb * 0.587 + bb * 0.114;

            Uint8 rr = ( rb > rl ? ( ( Uint8 )( ( ( double )( grayl<grayb?weighting:(weighting ^ 255)) ) / 255.0 * ( rb - rl ) + rl ) ) : ( ( Uint8 )( ( ( double )( grayl<grayb?weighting:(weighting ^ 255)) ) / 255.0 * ( rl - rb ) + rb ) ) );
            Uint8 gr = ( gb > gl ? ( ( Uint8 )( ( ( double )( grayl<grayb?weighting:(weighting ^ 255)) ) / 255.0 * ( gb - gl ) + gl ) ) : ( ( Uint8 )( ( ( double )( grayl<grayb?weighting:(weighting ^ 255)) ) / 255.0 * ( gl - gb ) + gb ) ) );
            Uint8 br = ( bb > bl ? ( ( Uint8 )( ( ( double )( grayl<grayb?weighting:(weighting ^ 255)) ) / 255.0 * ( bb - bl ) + bl ) ) : ( ( Uint8 )( ( ( double )( grayl<grayb?weighting:(weighting ^ 255)) ) / 255.0 * ( bl - bb ) + bb ) ) );
            setPixel( x0, y0, RGB( rr, gr, br ) );

            clrBackGround = getPixel(x0 + xDir, y0 );
            rb = getColorComponent( clrBackGround, 'r' );
            gb = getColorComponent( clrBackGround, 'g' );
            bb = getColorComponent( clrBackGround, 'b' );
            grayb = rb * 0.299 + gb * 0.587 + bb * 0.114;

            rr = ( rb > rl ? ( ( Uint8 )( ( ( double )( grayl<grayb?(weighting ^ 255):weighting) ) / 255.0 * ( rb - rl ) + rl ) ) : ( ( Uint8 )( ( ( double )( grayl<grayb?(weighting ^ 255):weighting) ) / 255.0 * ( rl - rb ) + rb ) ) );
            gr = ( gb > gl ? ( ( Uint8 )( ( ( double )( grayl<grayb?(weighting ^ 255):weighting) ) / 255.0 * ( gb - gl ) + gl ) ) : ( ( Uint8 )( ( ( double )( grayl<grayb?(weighting ^ 255):weighting) ) / 255.0 * ( gl - gb ) + gb ) ) );
            br = ( bb > bl ? ( ( Uint8 )( ( ( double )( grayl<grayb?(weighting ^ 255):weighting) ) / 255.0 * ( bb - bl ) + bl ) ) : ( ( Uint8 )( ( ( double )( grayl<grayb?(weighting ^ 255):weighting) ) / 255.0 * ( bl - bb ) + bb ) ) );
            setPixel( x0 + xDir, y0, RGB( rr, gr, br ) );
        }
        /* Draw the final pixel, which is always exactly intersected by the line
        and so needs no weighting */
        setPixel( x1, y1, clrLine );
        return;
    }
    /* It's an X-major line; calculate 16-bit fixed-point fractional part of a
    pixel that Y advances each time X advances 1 pixel, truncating the
    result to avoid overrunning the endpoint along the X axis */
    errorAdj = ((unsigned long) deltaY << 16) / (unsigned long) deltaX;
    /* Draw all pixels other than the first and last */
    while (--deltaX) {
        errorAccaux = errorAcc;   /* remember currrent accumulated error */
        errorAcc += errorAdj;      /* calculate error for next pixel */
        if (errorAcc <= errorAccaux) {
            /* The error accumulator turned over, so advance the Y coord */
            y0++;
        }
        x0 += xDir; /* X-major, so always advance X */
                    /* The IntensityBits most significant bits of errorAcc give us the
                    intensity weighting for this pixel, and the complement of the
        weighting for the paired pixel */
        weighting = errorAcc >> 8;
        /*
        ASSERT( weighting < 256 );
        ASSERT( ( weighting ^ 255 ) < 256 );
        */
        Uint32 clrBackGround = getPixel(x0, y0 );
        Uint8 rb = getColorComponent( clrBackGround, 'r' );
        Uint8 gb = getColorComponent( clrBackGround, 'g' );
        Uint8 bb = getColorComponent( clrBackGround, 'b' );
        double grayb = rb * 0.299 + gb * 0.587 + bb * 0.114;

        Uint8 rr = ( rb > rl ? ( ( Uint8 )( ( ( double )( grayl<grayb?weighting:(weighting ^ 255)) ) / 255.0 * ( rb - rl ) + rl ) ) : ( ( Uint8 )( ( ( double )( grayl<grayb?weighting:(weighting ^ 255)) ) / 255.0 * ( rl - rb ) + rb ) ) );
        Uint8 gr = ( gb > gl ? ( ( Uint8 )( ( ( double )( grayl<grayb?weighting:(weighting ^ 255)) ) / 255.0 * ( gb - gl ) + gl ) ) : ( ( Uint8 )( ( ( double )( grayl<grayb?weighting:(weighting ^ 255)) ) / 255.0 * ( gl - gb ) + gb ) ) );
        Uint8 br = ( bb > bl ? ( ( Uint8 )( ( ( double )( grayl<grayb?weighting:(weighting ^ 255)) ) / 255.0 * ( bb - bl ) + bl ) ) : ( ( Uint8 )( ( ( double )( grayl<grayb?weighting:(weighting ^ 255)) ) / 255.0 * ( bl - bb ) + bb ) ) );

        setPixel( x0, y0, RGB( rr, gr, br ) );

        clrBackGround = getPixel(x0, y0 + 1 );
        rb = getColorComponent( clrBackGround, 'r' );
        gb = getColorComponent( clrBackGround, 'g' );
        bb = getColorComponent( clrBackGround, 'b' );
        grayb = rb * 0.299 + gb * 0.587 + bb * 0.114;

        rr = ( rb > rl ? ( ( Uint8 )( ( ( double )( grayl<grayb?(weighting ^ 255):weighting) ) / 255.0 * ( rb - rl ) + rl ) ) : ( ( Uint8 )( ( ( double )( grayl<grayb?(weighting ^ 255):weighting) ) / 255.0 * ( rl - rb ) + rb ) ) );
        gr = ( gb > gl ? ( ( Uint8 )( ( ( double )( grayl<grayb?(weighting ^ 255):weighting) ) / 255.0 * ( gb - gl ) + gl ) ) : ( ( Uint8 )( ( ( double )( grayl<grayb?(weighting ^ 255):weighting) ) / 255.0 * ( gl - gb ) + gb ) ) );
        br = ( bb > bl ? ( ( Uint8 )( ( ( double )( grayl<grayb?(weighting ^ 255):weighting) ) / 255.0 * ( bb - bl ) + bl ) ) : ( ( Uint8 )( ( ( double )( grayl<grayb?(weighting ^ 255):weighting) ) / 255.0 * ( bl - bb ) + bb ) ) );

        setPixel( x0, y0 + 1, RGB( rr, gr, br ) );
    }

    /* Draw the final pixel, which is always exactly intersected by the line
    and so needs no weighting */
    setPixel( x1, y1, clrLine );
}

void bezierCurve(int x[] , int y[], bool points, Uint32 color)
{
	double xu = 0.0 , yu = 0.0 , u = 0.0 ;
	//int i = 0 ;

    if(points) {
        target(x[0],y[0], color);
        target(x[1],y[1], color);
        target(x[2],y[2], color);
        target(x[3],y[3], color);
    }

	for(u = 0.0 ; u <= 1.0 ; u += 0.0001)
	{
		xu = pow(1-u,3)*x[0]+3*u*pow(1-u,2)*x[1]+3*pow(u,2)*(1-u)*x[2]
			+pow(u,3)*x[3];
		yu = pow(1-u,3)*y[0]+3*u*pow(1-u,2)*y[1]+3*pow(u,2)*(1-u)*y[2]
			+pow(u,3)*y[3];
		setPixel((int)xu , (int)yu, color) ;
		//printf("(%d,%d)\n",(int)xu , (int)yu);
	}
}

/*Function to draw all other 7 pixels present at symmetric position*/
void drawCircle(int xc, int yc, int x, int y, Uint32 color)
{
    setPixel(xc+x,yc+y, color) ;
	setPixel(xc-x,yc+y, color);
	setPixel(xc+x,yc-y, color);
	setPixel(xc-x,yc-y, color);
	setPixel(xc+y,yc+x, color);
	setPixel(xc-y,yc+x, color);
	setPixel(xc+y,yc-x, color);
	setPixel(xc-y,yc-x, color);
}

/*Function for circle-generation using Bresenham's algorithm */
void circleBres(int xc, int yc, int r, Uint32 color)
{
	int x = 0, y = r;
	int d = 3 - 2 * r;
	while (y >= x)
	{
		/*for each pixel we will draw all eight pixels */
		drawCircle(xc, yc, x, y, color);
		x++;

		/*check for decision parameter and correspondingly update d, x, y*/
		if (d > 0)
		{
			y--;
			d = d + 4 * (x - y) + 10;
		}
		else
			d = d + 4 * x + 6;
		drawCircle(xc, yc, x, y, color);
	}
}

void randomFill() {

    int x = randomNumber(0,639);
    int y = randomNumber(0,479);
    int r = randomNumber(0,255);
    int g = randomNumber(0,255);
    int b = randomNumber(0,255);

    Uint32 color = RGB(r,g,b);

    floodFill(x,y,color);
    usleep(300000);

}

void display() {
    drawRectangle(pos_x+300,pos_y+100,pos_x+400,pos_y+200,RGB(255,0,0),RGB(0,255,0));
}

void display1()
{
        drawLine(639,100,0,050,RGB(255,0,0));
        drawLine(639,100,0,100,RGB(255,0,0));
        drawLine(639,100,0,150,RGB(255,0,0));

        drawWuLine(639,300,0,250,RGB(255,0,0));
        drawWuLine(639,300,0,300,RGB(255,0,0));
        drawWuLine(639,300,0,350,RGB(255,0,0));

        drawLine(100,479,050,000,RGB(0,0,255));
        drawLine(100,479,100,000,RGB(0,0,255));
        drawLine(100,479,150,000,RGB(0,0,255));

        drawWuLine(300,479,250,000,RGB(0,0,255));
        drawWuLine(300,479,300,000,RGB(0,0,255));
        drawWuLine(300,479,350,000,RGB(0,0,255));

        int xs[] = {70,150,589,629};
        int ys[] = {70,289,50,429};

        bezierCurve(xs,ys,true, RGB(255,0,0));


        xs[1] = 50;
        ys[1] = 400;
        xs[2] = 430;
        ys[2] = 20;

        bezierCurve(xs,ys,true, RGB(0,0,255));



        floodFill(146,413,RGB(0,255,0));
        floodFill(600,50,RGB(255,255,0));

        floodFill(200,200,RGB(255,0,0));
        floodFill(150,50,RGB(255,0,255));


        circleBres(500,400,50, RGB(0,0,255));
        floodFill(500,400,RGB(0,0,255));

        for(int r = 40; r >  10; r-=5) {
            circleBres(550,200,r, RGB(0,0,255));
            floodFill(550,200,RGB(255-(r*4),0,0));
        }

        randomFill();
}

void display2()
{

    drawLine(0,0,639,479,RGB(255,0,0));

    drawLine(0,240,639,240,RGB(0,0,255));

    drawWuLine(320,240,639,120,RGB(255,0,0));

    drawRectangle(100, 200, 200, 250, RGB(0,0,255));

    drawRectangle(30, 300, 350, 450, RGB(255,255,255), RGB(255, 0, 0));

    floodFill(10,20,RGB(200, 0, 127),RGB(0, 255, 255));
    floodFill(10,470,RGB(255, 255, 0),RGB(0, 255, 255));
    floodFill(100,10,RGB(0, 255, 0),RGB(0, 255, 255));
    floodFill(500,300,RGB(0, 0, 255),RGB(0, 255, 255));

    target(320,240,RGB(0, 0, 255));

    drawLine(320,0,320,479,RGB(255,0,0));
    drawLine(320,0,315,479,RGB(255,0,0));
    drawLine(320,0,325,479,RGB(255,0,0));
    drawLine(320,0,305,479,RGB(255,0,0));
    drawLine(320,0,335,479,RGB(255,0,0));

    drawLine(0,240,639,240,RGB(255,0,0));
}

void displayBaseGraphic()
{
    int xs[6];
    int ys[6];

    //Base graphic x
    drawLine(125,575,675,575,RGB(0,0,0));

    //Base graphic y
    drawLine(125,25,125,575,RGB(0,0,0));

    //y = x
    drawLine(675,25,125,575,RGB(0,255,0));

    //y = -x
    drawLine(125,25,675,575,RGB(255,0,0));

    //y = 2x²-6x+1x+1
    //xs[1] = 50;
    //ys[1] = 400;

    //xs[2] = 430;
    //ys[2] = 20;

    //xs[3] = 0;
    //ys[3] = 0;

    //xs[4] = 0;
    //ys[4] = 0;

    //xs[5] = 0;
    //ys[5] = 0;

    //xs[6] = 0;
    //ys[6] = 0;

    //bezierCurve(xs,ys,true, RGB(0,0,255));

}

int main()
{

    int xvel = 0;
    int yvel = 0;
    // Inicializações iniciais obrigatórias

    setlocale(LC_ALL, NULL);

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window * window = SDL_CreateWindow(titulo.c_str(),
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        800, 600,
        SDL_WINDOW_RESIZABLE);

    window_surface = SDL_GetWindowSurface(window);

    pixels = (unsigned int *) window_surface->pixels;
    width = window_surface->w;
    height = window_surface->h;

    // Fim das inicializações

    printf("Pixel format: %s\n",
        SDL_GetPixelFormatName(window_surface->format->format));

     while (1)
    {

        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                exit(0);
            }

            switch( event.type ){
            /* Look for a keypress */
                case SDL_KEYDOWN:
                    /* Check the SDLKey values and move change the coords */
                    switch( event.key.keysym.sym ){
                        case SDLK_LEFT:
                            xvel = -1;
                            printf("Pressionada a tecla LEFT\n");
                            break;
                        case SDLK_RIGHT:
                            xvel =  1;
                            printf("Pressionada a tecla RIGHT\n");
                            break;
                        case SDLK_UP:
                            yvel = -1;
                            printf("Pressionada a tecla UP\n");
                            break;
                        case SDLK_DOWN:
                            yvel =  1;
                            printf("Pressionada a tecla DOWN\n");
                            break;
                        case SDLK_PLUS:
                            yvel =  0;
                            printf("Pressionada a tecla PLUS\n");
                            break;
                        case SDLK_KP_PLUS:
                            yvel =  0;
                            printf("Pressionada a tecla PLUS\n");
                            break;
                        case SDLK_MINUS:
                            yvel =  0;
                            printf("Pressionada a tecla MINUS\n");
                            break;
                        case SDLK_KP_MINUS:
                            yvel =  0;
                            printf("Pressionada a tecla MINUS\n");
                            break;
                        default:
                            break;
                    }
                break;
                /* We must also use the SDL_KEYUP events to zero the x */
                /* and y velocity variables. But we must also be       */
                /* careful not to zero the velocities when we shouldn't*/
                case SDL_KEYUP:
                    switch( event.key.keysym.sym ){
                        case SDLK_LEFT:
                            /* We check to make sure the alien is moving */
                            /* to the left. If it is then we zero the    */
                            /* velocity. If the alien is moving to the   */
                            /* right then the right key is still press   */
                            /* so we don't tocuh the velocity            */
                            if( xvel < 0 )
                                xvel = 0;
                            break;
                        case SDLK_RIGHT:
                            if( xvel > 0 )
                                xvel = 0;
                            break;
                        case SDLK_UP:
                            if( yvel < 0 )
                                yvel = 0;
                            break;
                        case SDLK_DOWN:
                            if( yvel > 0 )
                                yvel = 0;
                            break;
                        default:
                            break;
                    }
                    break;
                    default:
                        break;
                }


            /* Update the alien position */
            pos_x += xvel;
            pos_y += yvel;

            if (event.type == SDL_WINDOWEVENT)
            {
                if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
                {
                    window_surface = SDL_GetWindowSurface(window);
                    pixels = (unsigned int *) window_surface->pixels;
                    width = window_surface->w;
                    height = window_surface->h;
                    printf("Size changed: %d, %d\n", width, height);
                }
            }

            /*Mouse is in motion*/
            if(event.type == SDL_MOUSEMOTION)
            {
                /*get x and y positions from motion of mouse*/
                showMousePosition(window,event.motion.x,event.motion.y);
            }
            if(event.type == SDL_MOUSEBUTTONDOWN)
            {
				/*If left mouse button down */
                if(event.button.button == SDL_BUTTON_LEFT)
				{
					printf("Mouse pressed on (%d,%d)\n",event.motion.x,event.motion.y) ;
					circleBres(event.motion.x, event.motion.y, 10, RGB(0,0,255));
				}
            }
        }

        // Set every pixel to white.
        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                //pixels[x + y * width] = SDL_MapRGBA(window_surface->format, 200, 200, 200, 255);
                setPixel(x, y, RGB(220,220,220));
            }
        }

        //display();

        //display1();

        //display2();

        displayBaseGraphic();


        SDL_UpdateWindowSurface(window);
    }
}
