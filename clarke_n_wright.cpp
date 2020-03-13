#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

float distances[1000][1000];
float savingDistances[1000][3];
float demands[1000];
float coords[1000][2];
int dimention = 0;
int capacity = 0;
int numsavingdistance = 0;
bool visited[1000];

struct node {
  int point;
  struct node * next;
};

struct route {
  float cost;
  struct route * next;
  struct node * init;
};

struct route *ROUTE = NULL;


void bubblesort(int k) {
  for(int i = 0; i < k - 1; i++) {
    for(int j = i + 1; j < k; j++){
      if(savingDistances[j][2] > savingDistances[i][2]) {
        float tempI = savingDistances[i][0];
        float tempJ = savingDistances[i][1];
        float tempS = savingDistances[i][2];

        savingDistances[i][0] = savingDistances[j][0];
        savingDistances[i][1] = savingDistances[j][1];
        savingDistances[i][2] = savingDistances[j][2];

        savingDistances[j][0] = tempI;
        savingDistances[j][1] = tempJ;
        savingDistances[j][2] = tempS;
      }
    }
  }
}

void prepareData() {
  // create distances matrix
  for(int i = 0; i < dimention; i++){
    for(int j = i + 1; j < dimention; j++){
      float temp = sqrt( (coords[i][0] - coords[j][0]) * (coords[i][0] - coords[j][0]) + (coords[i][1] - coords[j][1]) * (coords[i][1] - coords[j][1]));
      distances[i][j] = temp;
      distances[j][i] = temp;
     }
  }

  int k = 0;

  for(int i = 1; i < dimention; i++){
    for(int j = i + 1; j < dimention; j++){
      float s = distances[0][i] + distances[0][j] - distances[i][j];
      savingDistances[k][0] = i;
      savingDistances[k][1] = j;
      savingDistances[k][2] = s;
      k++;
    }
  }

  numsavingdistance = k;
  bubblesort(k);
}

void readfile(char *file_src){
  FILE *infile;
  infile = fopen(file_src, "r");

  if(infile == NULL) {
    printf("READ FILE ERROR\n");
  } else printf("READ FILE SUCCESS\n");

  fscanf(infile, "%d\n", &dimention);
  fscanf(infile, "%d\n", &capacity);

  printf("data: %d %d\n", dimention, capacity);

  float index, x, y;
  float demand = 0;
  for(int i = 0 ; i < dimention; i++){
    fscanf(infile, "%f %f %f\n", &index, &x, &y);
    coords[i][0] = x;
    coords[i][1] = y;
  }

  for(int i = 0; i < dimention; i++){
    fscanf(infile, "%f %f\n", &index, &demand);
    demands[i] = demand;
  }

  for(int i = 0; i < dimention; i++){
    visited[i] = false;
  }

  fclose(infile);

}

float caculatesumdemand(struct node *n, int extpoint) {
  float count = 0.0f;
  while(n != NULL) {
    count = count + demands[n -> point];
    n = n -> next;
  }

  if(extpoint != -1) count = count + demands[extpoint];
  return count;
}

void checkinterior(int pointj, int pointi) {
  struct route *temproute = ROUTE;
  while(temproute != NULL) {
    struct node *tempnode = temproute -> init;
    while(tempnode != NULL) {
      if(tempnode -> point == pointj) {
        if(tempnode -> next == NULL && caculatesumdemand(temproute -> init, pointi) <= capacity) {
          struct node *NEW_NODE = (struct node *)malloc(sizeof(struct node));
          NEW_NODE -> point = pointi;
          NEW_NODE -> next = NULL;
          tempnode -> next = NEW_NODE;
          visited[pointi] = true;
        } else if((tempnode == temproute -> init) && caculatesumdemand(temproute -> init, pointi) <= capacity) {
          struct node *NEW_NODE = (struct node *)malloc(sizeof(struct node));
          NEW_NODE -> point = pointi;
          NEW_NODE -> next = tempnode;
          temproute -> init = NEW_NODE;
          visited[pointi] = true;
        }
        return;
      }
      tempnode = tempnode -> next;
    }
    temproute = temproute -> next;
  }
}

void mergeRoute(int pointI, int pointJ) {
  struct node * nodeI = NULL;
  struct node * nodeJ = NULL;

  struct route * temproute = ROUTE;
  struct route * temprouteI = NULL;
  struct route * temprouteJ = NULL;
  while(temproute != NULL) {
    if(nodeI == NULL) {
      nodeI = temproute -> init;
      while (nodeI != NULL)
      {
        if(nodeI -> point == pointI) {
          temprouteI = temproute;
          break;
        }
        else nodeI = nodeI -> next;
      }
    }

    if(nodeJ == NULL) {
      nodeJ = temproute -> init;
      while(nodeJ != NULL) {
        if(nodeJ -> point == pointJ) {
          temprouteJ = temproute;
          break;
        }
        else nodeJ = nodeJ -> next;
      }
    }
    if(nodeI !=NULL && nodeJ != NULL) break;
    temproute = temproute -> next;
  }

  if(nodeI != NULL && nodeJ != NULL && ((caculatesumdemand(temprouteI -> init, -1) + caculatesumdemand(temprouteJ -> init, -1)) <= capacity )) {
    if(nodeI -> next == NULL && nodeJ -> next != NULL) {
      nodeI -> next = nodeJ;
      if(temprouteJ != NULL) temprouteJ -> init = NULL;
    } else if(nodeI -> next != NULL && nodeJ -> next == NULL) {
      nodeJ -> next = nodeI;
      if(temprouteI != NULL) temprouteI -> init = NULL;
    } else if(nodeI -> next != NULL && nodeJ -> next != NULL) {
      nodeI -> next = temprouteJ -> init;
      temprouteJ -> init = NULL;
    } else {
      struct node *initnode = temprouteJ -> init;
      while(initnode -> next != NULL){
        initnode = initnode -> next;
      }
      initnode-> next = nodeI;
      temprouteI -> init = NULL;
    }
  }
}

bool checkallvisited() {
  for(int i = 0; i < dimention; i++){
    if(!visited[i]) return false;
  }
  return true;
}

bool checkinthesameroute(int i, int j){
  struct route * temproute = ROUTE;
  bool checki = false;
  bool checkj = false;
  while (temproute != NULL)
  {
    struct node * tempnode = temproute -> init;
    while(tempnode != NULL){
      if(tempnode -> point == i) checki = true;
      if(tempnode -> point == j) checkj = true;
      if(checki && checkj) return true;
      tempnode = tempnode -> next;
    }
    temproute = temproute -> next;
  }

  return false;
}


void showResult(int optimized, char*prob) {
  float cost = 0.0f;
  struct route *temproute = ROUTE;
  while(temproute != NULL){
    struct node *tempnode = temproute -> init;
    struct node *prevtempnode = NULL;
    if(tempnode != NULL) cost += distances[tempnode -> point][0];
    while(tempnode != NULL) {

      if(prevtempnode != NULL) {
        cost += distances[prevtempnode -> point][tempnode -> point];
        prevtempnode = tempnode;
      } else {
        prevtempnode = tempnode;
      }
      printf("%d -> ", tempnode -> point);
      tempnode = tempnode -> next;
    }
    if(prevtempnode != NULL) cost += distances[prevtempnode -> point][0];
    printf("\n");
    temproute = temproute -> next;
  }
  for(int i = 1; i < dimention; i++){
    if(!visited[i]) {
      printf("un_vistited:\n");
      printf("%d\n",i);
      cost += 2 * distances[i][0];
    }
  }
  printf("%s - cost: %.3f - optimized: %d",prob, cost, optimized);
}

void clarkenwright() {
  struct route *CURRENT_TAIL_ROUTE = NULL;
  for(int m = 0; m < numsavingdistance; m++){
    if(!checkallvisited()) {
      int i, j, s;
      i = savingDistances[m][0];
      j = savingDistances[m][1];
      s = savingDistances[m][2];
    if(ROUTE == NULL){
      if((demands[i] + demands[j]) <= capacity){
        ROUTE = (struct route *)malloc(sizeof(struct route));
        struct node *NODEI = (struct node *)malloc(sizeof(struct node));
        NODEI -> point = i;
        struct node *NODEJ = (struct node *)malloc(sizeof(struct node));
        NODEJ -> point = j;
        NODEI -> next = NODEJ;
        NODEJ -> next = NULL;
        ROUTE -> init = NODEI;
        ROUTE -> next = NULL;
        visited[i] = true;
        visited[j] = true;
        CURRENT_TAIL_ROUTE = ROUTE;
      }
    } else {
      if(visited[i] && visited[j] && !checkinthesameroute(i,j)) {
        mergeRoute(i, j);
      }
      else if(!visited[i] && !visited[j] && ((demands[i] + demands[j]) <= capacity)) {
        struct route *NEW_ROUTE = (struct route *)malloc(sizeof(struct route));
        CURRENT_TAIL_ROUTE -> next = NEW_ROUTE;
        CURRENT_TAIL_ROUTE = NEW_ROUTE;

        struct node *NODEI = (struct node *)malloc(sizeof(struct node));
        NODEI -> point = i;
        struct node *NODEJ = (struct node *)malloc(sizeof(struct node));
        NODEJ -> point = j;
        NODEI -> next = NODEJ;
        NODEJ -> next = NULL;
        NEW_ROUTE -> init = NODEI;
        NEW_ROUTE -> next = NULL;
        visited[i] = true;
        visited[j] = true;
      } else if(!visited[i] && visited[j]) {
        checkinterior(j, i);
      } else if(visited[i] && !visited[j]) {
        checkinterior(i, j);
      }
    }
    }
  }
}

int main() {
  readfile("./A-n32-k5.vrp");
  prepareData();
  clarkenwright();
  showResult(784, "A-n32-k5");
  printf("\nDONE\n");

  ROUTE = NULL;

  readfile("./A-n33-k5.vrp");
  prepareData();
  clarkenwright();
  showResult(661, "A-n33-k5");
  printf("\nDONE\n");
///////
  ROUTE = NULL;
  readfile("./A-n33-k6.vrp");
  prepareData();
  clarkenwright();
  showResult(742, "A-n33-k6");
  printf("\nDONE\n");

  ROUTE = NULL;
  readfile("./A-n34-k5.vrp");
  prepareData();
  clarkenwright();
  showResult(778, "A-n34-k5");
  printf("\nDONE\n");

  ROUTE = NULL;
  readfile("./A-n36-k5.vrp");
  prepareData();
  clarkenwright();
  showResult(799, "A-n36-k5");
  printf("\nDONE\n");

  ROUTE = NULL;
  readfile("./A-n37-k5.vrp");
  prepareData();
  clarkenwright();
  showResult(669, "A-n37-k5");
  printf("\nDONE\n");

  ROUTE = NULL;
  readfile("./A-n37-k6.vrp");
  prepareData();
  clarkenwright();
  showResult(949, "A-n37-k6");
  printf("\nDONE\n");

  ROUTE = NULL;
  readfile("./A-n38-k5.vrp");
  prepareData();
  clarkenwright();
  showResult(730, "A-n38-k5");
  printf("\nDONE\n");

  ROUTE = NULL;
  readfile("./A-n39-k5.vrp");
  prepareData();
  clarkenwright();
  showResult(822, "A-n39-k5");
  printf("\nDONE\n");

  ROUTE = NULL;
  readfile("./A-n39-k6.vrp");
  prepareData();
  clarkenwright();
  showResult(831, "A-n39-k6");
  printf("\nDONE\n");


  // ROUTE = NULL;
  // readfile("./A-n44-k6.vrp");
  // prepareData();
  // clarkenwright();
  // showResult(937, "A-n44-k6");
  // printf("\nDONE\n");

  ROUTE = NULL;
  readfile("./A-n45-k6.vrp");
  prepareData();
  clarkenwright();
  showResult(944, "A-n45-k6");
  printf("\nDONE\n");

  ROUTE = NULL;
  readfile("./A-n45-k7.vrp");
  prepareData();
  clarkenwright();
  showResult(1146, "A-n47-k6");
  printf("\nDONE\n");

  ROUTE = NULL;
  readfile("./A-n46-k7.vrp");
  prepareData();
  clarkenwright();
  showResult(914, "A-n46-k7");
  printf("\nDONE\n");

  ROUTE = NULL;
  readfile("./A-n48-k7.vrp");
  prepareData();
  clarkenwright();
  showResult(1073, "A-n48-k7");
  printf("\nDONE\n");

  ROUTE = NULL;
  readfile("./A-n53-k7.vrp");
  prepareData();
  clarkenwright();
  showResult(1010, "A-n53-k7");
  printf("\nDONE\n");

  ROUTE = NULL;
  readfile("./A-n54-k7.vrp");
  prepareData();
  clarkenwright();
  showResult(1167, "A-n54-k7");
  printf("\nDONE\n");

  ROUTE = NULL;
  readfile("./A-n55-k9.vrp");
  prepareData();
  clarkenwright();
  showResult(1073, "A-n55-k9");
  printf("\nDONE\n");

  // ROUTE = NULL;
  // readfile("./A-n55-k7.vrp");
  // prepareData();
  // clarkenwright();
  // showResult(661, "A-n55-k7");
  // printf("\nDONE\n");

  ROUTE = NULL;
  readfile("./A-n61-k9.vrp");
  prepareData();
  clarkenwright();
  showResult(1034, "A-n61-k9");
  printf("\nDONE\n");

  ROUTE = NULL;
  readfile("./A-n65-k9.vrp");
  prepareData();
  clarkenwright();
  showResult(1174, "A-n65-k9");
  printf("\nDONE\n");

  return 0;
}