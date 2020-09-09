/* ********************************************************/
/*                                                        */
/*                                                        */
/*   kmeans_basic.c                                       */
/*                                                        */
/*   By: Hamza LIMOURI                                    */
/*                                                        */
/*   Created: 2020/09/09 12:04:06 by HAMZALIMOURI         */
/*                                                        */
/* ****************************************************** */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct element
{
    int data;
    struct element *next;
}Element;

typedef struct 
{
    float centre;
    Element *first;
    Element *last;
}Cluster;

// Creation fonctions

Element *Create_Elementent(int data)
{
    Element *e = (Element*)malloc(sizeof(Element));
    e->data = data;
    e->next = NULL;
    return e;
}

Cluster *Create_cluster(float centre)
{
    Cluster *c = (Cluster*)malloc(sizeof(Cluster));
    c->centre = centre;
    c->first = NULL;
    c->last = NULL;
    return c;
}

// Initialization

void Initialise_cluster(int *A, int k, Cluster **cluster)
{
    for (int i = 0; i < k; i++)
    {
        cluster[i] = Create_cluster(A[i]);
    }
}

// ADD fonction

void AddToCluster(int data, Cluster *cluster)
{
    Element *e = Create_Elementent(data);
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

// Display cluster

void Display(Cluster *cluster)
{
    Element *e = cluster->first;
    while (e)
    {
        printf("%d\t", e->data);
        e = e->next;
    }
    printf("\nle centre du cluster est %.2f\n", cluster->centre);
    cluster->first = cluster->last = NULL;
}

// Calcule distance between point and centre of cluster

int dist(int data, Cluster **cluster, int k)
{
    float temp = data - cluster[0]->centre;
    temp = (temp > 0)?temp:temp * (-1);
    int index = 0;
    for (int i = 1; i < k; i++)
    {
        float temp2 = data - cluster[i]->centre;
        temp2 = (temp2 > 0)?temp2:temp2 * (-1);
        if(temp > temp2)
        {
            temp = temp2;
            index = i;
        }
    }
    return index;
}

// calcule the new centre 

float CalculeCenter(Cluster *cluster)
{
    Element *e = cluster->first;
    float sum = 0;
    int c = 0;
    while (e)
    {
        sum += e->data;
        c++;
        e = e->next;
    }
    return sum / c;
}

// k-means fonction

void kmeans(int k, int *A, Cluster **cluster, int N)
{
    int c, etape = 1;
    while (1)
    {
        c = 0;
        printf("--------------------- Etape %d -------------------\n", etape++);
        for (int i = 0; i < N; i++)
        {
            int index = dist(A[i], cluster, k); // Search close centre index
            AddToCluster(A[i], cluster[index]); // Add point to Cluster
        }
        for (int i = 0; i < k; i++)
        {
            float temp = CalculeCenter(cluster[i]);  // calcule new centre
            if (temp == cluster[i]->centre)          // compare between the new centre and the old
                c++;                                 // if equal we increment c
            else
                cluster[i]->centre = temp;           // else affect the new centre
            printf("Cluster %d\n",i + 1);       
            Display(cluster[i]);                     // call Display fonction
        }
        if (c == k)                                  // if c == k so all centres do not changes
            break;
    }
}

int main()
{
    int A[9] = {1, 2, 3, 6, 7, 8, 13, 15, 17};
    int k = 3;
    Cluster *cluster[k];
    
    Initialise_cluster(A, k, cluster);
    kmeans(k, A, cluster, 9);
    return 0;
}