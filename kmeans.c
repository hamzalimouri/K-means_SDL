/* ********************************************************/
/*                                                        */
/*                                                        */
/*   kmeans.c                                             */
/*                                                        */
/*   By: Hamza LIMOURI                                    */
/*                                                        */
/*   Created: 2020/09/09 12:04:06 by HAMZALIMOURI         */
/*                                                        */
/* ****************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include<time.h>
#include <SDL2/SDL.h>

#define WINDOW_WIDTH 500
#define WINDOW_HEIGHT 500
#define N 700

typedef struct
{
    SDL_Surface *surface;   //A structure that contains the definition of a surface
    SDL_Rect rect;          //A structure that contains the definition of a rectangle
}Point;

typedef struct element
{
    Point *point;
    struct element *next;
}Element;

typedef struct 
{
    Point *centre;
    Element *first;
    Element *last;
}Cluster;

// Creation fonctions

Point *Create_point(float x, float y, int w, int h)
{
    Point *p = (Point*)malloc(sizeof(Point));
    p->surface = SDL_CreateRGBSurface(              // Use this function to allocate a new RGB surface.
                                        0,          //the flags are unused and should be set to 0    
                                        w,          //width
                                        h,          //height
                                        32,         //If depth is greater than 8 bits, the pixel format is set using the [RGBA]mask parameters
                                        0,          //Rmask
                                        0,          //Gmask
                                        0,          //Bmask
                                        0);         //Amask
    p->rect.x = x;                                  // the X location of the rectangle
    p->rect.y = y;                                  // the Y location of the rectangle
    return p;
}

Element *Create_Elementent(Point *point)
{
    Element *e = (Element*)malloc(sizeof(Element));
    e->point = point;
    e->next = NULL;
    return e;
}

Cluster *Create_cluster()
{
    Cluster *c = (Cluster*)malloc(sizeof(Cluster));
    c->centre = NULL;
    c->first = NULL;
    c->last = NULL;
    return c;
}

// Initialisation fonctions

void Initialise_data(Point **point, int w, int h)
{
    // Use current time as seed for random generator 
    srand(time(0));
    for (int i = 0; i < N; i++)
    {
        int x = rand() % 490, y = rand() % 490;
        point[i] = Create_point(x, y, w, h);
    }
}

void Initialise_color(uint32_t *color, int k, SDL_Surface *screen)
{
    // Use current time as seed for random generator 
    srand(time(0));
    int x = 255 / k, r = x;
    for (int i = 0; i < k; i++)
    {
        int g = rand() % 255, b = rand() % 255;
        color[i] = SDL_MapRGB(screen->format, r, g, b); // Use this function to map an RGB triple to an 
                                                        //opaque pixel value for a given pixel format.
        r += x;
    }
}

void Initialise_cluster(Cluster **cluster, int k)
{
    // Use current time as seed for random generator 
    srand(time(0));
    for (int i = 0; i < k; i++)
    {
        cluster[i] = Create_cluster();
        int x = rand() % 490, y = rand() % 490;
        cluster[i]->centre = Create_point(x, y, 15, 15);
    }
}

// Draw fonctions

void Draw_point( SDL_Surface *des, Point *point, uint32_t color)
{
    SDL_FillRect(point->surface, NULL, color);                  //Use this function to perform a fast fill of a rectangle with a specific color.
    SDL_BlitSurface(point->surface, NULL, des, &point->rect);   //Use this function to perform a fast surface copy to a destination surface.
}

void Draw_line(Point *a, Point *b, SDL_Renderer* renderer)
{
    SDL_SetRenderDrawColor(renderer, 0,0,0,0);                  //Use this function to set the color used for drawing operations
    SDL_RenderDrawLine(renderer, a->rect.x, a->rect.y, b->rect.x, b->rect.y);   //Use this function to draw a line on the current rendering target.    //Use this function to update the screen with any rendering performed since the previous call.
}

void drawCluster(Cluster *cluster, SDL_Surface *des, uint32_t color)
{
    Element *e = cluster->first;
    while (e)
    {
        Draw_point(des, e->point, color);
        e = e->next;
    }
    Draw_point(des, cluster->centre, SDL_MapRGB(des->format, 0, 0, 0));     // Draw the centre of cluster
}

// ADD fonction

void AddToCluster(Point *p, Cluster *cluster)
{
    Element *e = Create_Elementent(p);
    if (cluster->first == NULL)
    {
        cluster->first = e;
        cluster->last = e;
    }
    else
    {
        cluster->last->next = e;
        cluster->last = e;
    }
}

//Display fonction

void Display(Cluster *cluster)
{
    Element *e = cluster->first;
    while (e)
    {
        printf("(%d, %d)\t", e->point->rect.x, e->point->rect.y);
        e = e->next;
    }
    printf("\nle centre du cluster est (%d, %d)\n", cluster->centre->rect.x, cluster->centre->rect.y);
    cluster->first = cluster->last = NULL;
}

// Calcule distance between point and the centre of cluster

int dist(Point *point, Cluster **cluster, int k)
{
    float temp = sqrt(pow(cluster[0]->centre->rect.x - point->rect.x, 2) + pow(cluster[0]->centre->rect.y - point->rect.y, 2));
    int index = 0;
    for (int i = 1; i < k; i++)
    {
        float temp2 = sqrt(pow(cluster[i]->centre->rect.x - point->rect.x, 2) + pow(cluster[i]->centre->rect.y - point->rect.y, 2));
        if(temp > temp2)
        {
            temp = temp2;
            index = i;
        }
    }
    return index;
}

// calcule the new center

Point *CalculeCenter(Cluster *cluster)
{
    Element *e = cluster->first;
    float sumx = 0, sumy = 0;
    int c = 0;
    while (e)
    {
        sumx += e->point->rect.x;
        sumy += e->point->rect.y;
        c++;
        e = e->next;
    }
    return Create_point(sumx / c, sumy / c, 15, 15);
}

// K-means fonction

void kmeans(int k, Point **point, Cluster **cluster, SDL_Surface *des, SDL_Window *window, uint32_t *colors, SDL_Renderer* renderer)
{
    Point *center_screen = Create_point(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, 0, 0);
    int c, etape = 1;
    while (1)
    {
        c = 0;
        printf("--------------------- Etape %d -------------------\n", etape++);
        for (int i = 0; i < N; i++)
        {
            int index = dist(point[i], cluster, k);     // Search close centre index
            AddToCluster(point[i], cluster[index]);     // Add point to Cluster
        }
        for (int i = 0; i < k; i++)
        {
            Point *temp = CalculeCenter(cluster[i]);    // calcule new centre
            if (temp->rect.x == cluster[i]->centre->rect.x && temp->rect.y == cluster[i]->centre->rect.y)   // compare between the new centre and the old
                c++;                                    // if equal we increment c
            else        
                cluster[i]->centre = temp;              // else affect the new centre
            drawCluster(cluster[i], des, colors[i]);    // call DrawCluster fonction
            Draw_line(center_screen, cluster[i]->centre, renderer);     // call Drawline fonction
            printf("Cluster %d\n",i + 1);               
            Display(cluster[i]);                        // call Display fonction
        }
        if (c == k)                                     // if c == k so all centres do not changes
            break;
        SDL_UpdateWindowSurface(window);                //Use this function to copy the window surface to the screen.
        SDL_Delay(1000);                                //Use this function to wait a specified number of milliseconds before returning.
        SDL_FillRect(des, NULL, SDL_MapRGB(des->format, 255, 255, 255));
    }
}

int main()
{
    int k;
    printf("Enter the number of the classter ( >= 1 ): ");
    scanf("%d", &k);
    Point *point[N];        //points Array
    uint32_t colors[k];     //colors Array
    Cluster *cluster[k];    //clusters Array
    
/*#####################################################*/


    SDL_Init(SDL_INIT_EVERYTHING);      //Use this function to initialize the SDL library.

    SDL_Window *window;                 ////A structure that contains the definition of a window

    // Create an application window with the following settings:
    window = SDL_CreateWindow(
        "K-means",                  // window title
        SDL_WINDOWPOS_UNDEFINED,           // initial x position
        SDL_WINDOWPOS_UNDEFINED,           // initial y position
        WINDOW_WIDTH,                               // width, in pixels
        WINDOW_HEIGHT,                               // height, in pixels
        SDL_WINDOW_RESIZABLE                  // window can be resized
    );
    // Check that the window was successfully created
    if (window == NULL) {
        // In the case that the window could not be made...
        printf("Could not create window: %s\n", SDL_GetError());
        return 1;
    }
    SDL_Renderer *r;
    SDL_Surface *screen = SDL_GetWindowSurface(window);         //Use this function to get the SDL surface associated with the window.
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 255, 255, 255));  //Use this function to perform a fast fill of a rectangle with a specific color.
    r = SDL_CreateSoftwareRenderer(screen);     //Use this function to create a 2D software rendering context for a surface.

/*#####################################################*/

    Initialise_data(point, 10, 10);
    Initialise_cluster(cluster, k);
    Initialise_color(colors, k, screen);
    
    kmeans(k, point, cluster, screen, window, colors, r);

/*#####################################################*/
    
    SDL_Event event;
    int running = 1;
    while (running) {
        while (SDL_PollEvent(&event)) {     //Use this function to poll for currently pending events. 
            if(event.type == SDL_QUIT)
            {
                running = 0;
                break;
            }
        }
    }
    // Close and destroy the window
    SDL_DestroyWindow(window);

    SDL_Quit();
    
    return 0;
}