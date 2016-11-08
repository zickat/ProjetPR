# include <stdlib.h>
# include <stdio.h>
# include <sys/time.h>

#define INF (1<<30) // a very large positive integer

struct direct_edge_struct;
struct direct_edge_struct {
  int destination_node;
  int weight;
  struct direct_edge_struct *next;
};

int **distance;
int num_nodes, num_edges;
struct direct_edge_struct *edges;
struct direct_edge_struct **nodes;
int edge_counter = 0;
int *min_distance;
char *tree;

int main ( int argc, char **argv );
void read_graph(char *filename);
double dijkstra();

double get_time(){
  struct timeval tv;
  gettimeofday(&tv,(void *)0);
  return (double) tv.tv_sec + tv.tv_usec*1e-6;
}

/******************************************************************************/
int main ( int argc, char **argv ){

  double comp_time;

  if (argc < 2){
    fprintf(stderr,"Usage: dijkstra <graph file name>\n");
    exit(-1);
  }
  else
    read_graph(argv[1]);

  comp_time = dijkstra();
/*
  printf("\nMinimal distance from node 0 to every other node:\n");
  for (int i = 1; i < num_nodes; i++)
      printf("Node %d: \t%d\n", i, min_distance[i]); */

  fprintf(stderr,"Computation time: %f\n", comp_time);

  free(nodes);
  free(edges);
  //free(distance);
  free(min_distance);
  free(tree);
  return 0;
}

/******************************************************************************/
int get_distance(int node1, int node2){
  if (node1 == node2)
    return 0;
  struct direct_edge_struct *edge = nodes[node1];
  while (edge != NULL){
    if (edge->destination_node == node2)
      return edge->weight;
    edge = edge->next;
  }
  // node2 has not been found as a direct neighbour of node 1
  return INF;
}

/******************************************************************************/
double dijkstra(){

  int shortest_dist;
  int nearest_node;
  double start,stop;

  start = get_time();
  tree[0] = 1;
  for (int i = 1; i < num_nodes; i++)
    tree[i] = 0;

  for (int i = 0; i < num_nodes; i++)
    min_distance[i] = get_distance(0,i);

  for (int step = 1; step < num_nodes; step++ ){
    // find nearest node
    shortest_dist = INF;
    nearest_node = -1;
    for (int i = 0; i < num_nodes; i++){
        if ( !tree[i] && min_distance[i] < shortest_dist ){
        shortest_dist = min_distance[i];
        nearest_node = i;
      }
    }

    if ( nearest_node == - 1 ){
      fprintf(stderr,"Warning: Search ended early, the graph might not be connected.\n" );
      break;
    }

    tree[nearest_node] = 1;
    for (int i = 0; i < num_nodes; i++)
      if ( !tree[i] ){
        int d = get_distance(nearest_node,i);
        if ( d < INF )
          if ( min_distance[nearest_node] + d < min_distance[i] )
            min_distance[i] = min_distance[nearest_node] + d;
      }
  }
  stop = get_time();
  return(stop-start);
}

/******************************************************************************/
void read_graph(char *filename){
  char line[256];
  int node1, node2, weight;

  FILE *graph = fopen(filename,"r");
  if (graph == NULL){
    fprintf(stderr,"File %s not found.\n",filename);
    exit(-1);
  }

  while (fgets(line, 256, graph) != NULL){
    switch(line[0]){
      case 'c': // comment
        break;

      case 'p': // graph size
        if (sscanf(&(line[5]),"%d %d\n", &num_nodes, &num_edges) != 2){
          fprintf(stderr,"Error in file format in line:\n");
          fprintf(stderr, "%s", line);
          exit(-1);
        }
        else
          fprintf(stderr,"Graph contains %d nodes and %d edges\n", num_nodes, num_edges);
          edges = malloc(num_edges*2 * sizeof(struct direct_edge_struct));
          if (edges == NULL){
            fprintf(stderr,"Error: cannot allocate memory.\n");
            exit(-1);
          }
          nodes = malloc(num_nodes * sizeof(struct direct_edge_struct *));
          if (nodes == NULL){
            fprintf(stderr,"Error: cannot allocate memory.\n");
            exit(-1);
          }
          for (int i=0; i<num_nodes; i++)
            nodes[i] = NULL;

          min_distance = malloc(num_nodes * sizeof(int));
          if (min_distance == NULL){
            fprintf(stderr,"Error: cannot allocate memory.\n");
            exit(-1);
          }
          tree = malloc(num_nodes * sizeof(char));
          if (tree == NULL){
            fprintf(stderr,"Error: cannot allocate memory.\n");
            exit(-1);
          }
        break;

      case 'a': // edge definition
        if (sscanf(&(line[2]),"%d %d %d\n", &node1, &node2, &weight) != 3){
          fprintf(stderr,"Error in file format in line:\n");
          fprintf(stderr, "%s", line);
          exit(-1);
        }
        node1--; node2--; // number nodes from 0
        //distance[node1-1][node2-1] = weight;
        struct direct_edge_struct *new_edge;
        struct direct_edge_struct *e;
        new_edge = &edges[edge_counter++];
        new_edge->destination_node = node2;
        new_edge->weight = weight;
        new_edge->next = NULL;
        if (nodes[node1] == NULL)
          nodes[node1] = new_edge;
        else {
          e = nodes[node1];
          while (e->next != NULL)
            e = e->next;
          e->next = new_edge;
        }
        new_edge = &edges[edge_counter++];
        new_edge->destination_node = node1;
        new_edge->weight = weight;
        new_edge->next = NULL;
        if (nodes[node2] == NULL)
          nodes[node2] = new_edge;
        else {
          e = nodes[node2];
          while (e->next != NULL)
            e = e->next;
          e->next = new_edge;
        }
        break;
    }
  }
  fclose(graph);
}