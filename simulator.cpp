#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <list>
#include <set>
#include <queue>
#include <algorithm>
#include <random>

#define N 100
using namespace std;

// Infection = 0, Recovery = 1
typedef enum {Infection, Recovery} Event_type;

struct node
{
    int node_id;
    unsigned timestamp;
    Event_type event;
    // This function is used by set to order elements of node.
    bool operator<(const node& n) const
    {
        return (this->node_id < n.node_id);
    }
};

/**
 * @brief Adds edge between 2 nodes - u and v
 * @param adjacency_list[] is the adjacency list where we store all the adjacent nodes of a node
 * @param u node where we have to add edge connecting to v
 * @param v node where we have to add edge connecting to u
 **/
void add_edge(vector<node> adjacency_list[], node u, node v)
{
    adjacency_list[u.node_id].push_back(v);
    adjacency_list[v.node_id].push_back(u);
}

/**
 * @brief generates 0 or 1 with 50% probability (coin toss)
 * @return int - 0 or 1, 1 for heads and 0 for tails 
 */
int One_toss()
{
    random_device rd;      // used to obtain a seed for the random number engine
    mt19937 gen(rd());     // Standard mersenne_twister_engine seeded with rd()
    uniform_int_distribution<> toss(0, 1);

    return toss(gen);
}

//this comparision is for the nodes to be stored in priority que in min heap order with timestamp as keys
struct Compare_timestamp
{
    bool operator() (node const& n1, node const& n2)
    {
        return n1.timestamp > n2.timestamp;
    }
};

/**
 * @brief finds the shortest distance between dest and source and stores in distance array
 * @param adjacency_list contains all nodes and its adjacent nodes
 * @param source source node
 * @param dest node whose shortest distance is to be found from source node
 * @param predecessor array contains predecessor node of all nodes in BFS order traversal
 * @param distance array which stores distance of all nodes from source node
 * @return true when source and dest nodes are connected
 * @return false when source and dest nodes are not connected
 */
bool BFS(vector<node> adjacency_list[], node source, node dest, node predecessor[], int distance[])
{
	// this queue maintains the vertices whose adjacency list is to be visited
	list<node> queue;

	//if a node is not visited at least once, bool value is false else true.
	bool visited[N+1];

	// initially all vertices are unvisited so visited[i] is false and distance[i] for all i set to infinity
	for (int i = 0; i < N+1; i++) {
		visited[i] = false;
		distance[i] = INT_MAX;
		//predecessor[i] = -1;
	}

	// now source is first to be visited and distance from source to itself should be 0
	visited[source.node_id] = true;
	distance[source.node_id] = 0;
	queue.push_back(source);

	while (!queue.empty()) 
    {
		node u = queue.front();
		queue.pop_front();

		for (int i = 0; i < adjacency_list[u.node_id].size(); i++) 
        {
			if (visited[adjacency_list[u.node_id][i].node_id] == false) 
            {
				visited[adjacency_list[u.node_id][i].node_id] = true;
				distance[adjacency_list[u.node_id][i].node_id] = distance[u.node_id] + 1;
				predecessor[adjacency_list[u.node_id][i].node_id] = u;
				queue.push_back(adjacency_list[u.node_id][i]);

				// when dest node is found
				if (adjacency_list[u.node_id][i].node_id == dest.node_id)
					return true;
			}
		}
	}
	return false;
}

/**
 * @brief prints the shortest distance between source vertex and given vertex
 * @param adjacency_list contains all nodes and its adjacent nodes
 * @param s source node
 * @param vertex node whose shortest distance is to be found from source node
 * @return int distance between the node and source node.
 */
int shortest_Distance(vector<node> adjacency_list[], node s, node vertex)
{
	node predecessor[N+1];
    int distance[N+1];

	if (BFS(adjacency_list, s, vertex, predecessor, distance) == false) {
		cout << "Given node and source node are not connected" << endl;
		return 0;
	}    
	return distance[vertex.node_id];
}


int main()
{
    srand(time(0));

    node Node[N+1];
    vector<struct node> adjacency_list[N+1];
    int num_Recover[N], num_Infection[N], num_Susceptible[N];
    int infection_timestamp[N+1];
    set <struct node> S, I, R;

    for(int i = 1; i < N+1; i++)
    {
        Node[i].node_id = i;
        S.insert(Node[i]);
    }

    //for loop - to generate edges between each pair of nodes.
    for(int j = 1; j < N+1; j++)
    {
        for(int i = j+1; i < N+1; i++)
        {
            int Random = One_toss(); // head = 1, tails = 0
            if(Random)
                add_edge(adjacency_list,Node[j],Node[i]);
        }
    }

    for( int i = 0; i<15; i++) // initialising num of people susceptible, infected and recovered for 15 days to the stats of day 0. it can be anything not necessarily 15. 
    {
        num_Susceptible[i] = N;
        num_Infection[i] = 0;
        num_Recover[i] = 0;
    }

    priority_queue <struct node, vector<node>, Compare_timestamp> Min_Q;

    // generating a random starting node u as random_node, so i have to insert it into the queue as infection event, with timestamp 0
    random_device random_node;      
    std::uniform_int_distribution<> random(1, 100);
    node u = Node[random(random_node)]; 

    cout << "Source node is " << u.node_id << endl << endl;
     
    // u is the source node.
    u.event = Infection;
    u.timestamp = 0;
    
    Min_Q.push(u);
    node e;
    S.extract(u); // removing u(source node) from S(susceptible set) as its infected now.

    while(!Min_Q.empty())
    {
        e = Min_Q.top();
        Min_Q.pop();

        if(e.event == 1)   // recovery event
        {
            // inserting this node to R(recovery set) and remove it from I(infected set)
            // extracted from I, and inserted into R.
            R.insert(I.extract(e));
            // changing the count, for all timestamps in future.
            for(int j = e.timestamp; j < N; j++)
            {
                num_Recover[j]++;
                num_Infection[j]--;
            }
        }

        if(e.event == 0)   // infection event
        {
            I.insert(e);
            infection_timestamp[e.node_id] = e.timestamp;

            // changing the count, for all timestamps in future.
            for(int j = e.timestamp; j < N; j++)
            {
                num_Infection[j]++;
                num_Susceptible[j]--;
            }

            int j;
            // here for all neighbours of e
            for(auto x : adjacency_list[e.node_id])
            {
                // if that neighbour is in S, that is, if its susceptible, 
                if(S.contains(x))
                {
                    // We toss a fair coin 5 times, 
                    for(j = 1; j <= 5; j++)
                    {
                        int toss = One_toss();
                        if(toss == 1)   // since head = 1, we want the time when first head occurs among the 5 tosses.
                            break;
                    }
                    // now, j contains the instance of first head.

                    /* if a head occured in 5 times of the toss, then we insert the infection event of this node(x), with 
                    timestamp e.timestamp+j in the Min_Q and remove x from S(susceptible).*/
                    if(j >= 1 && j <= 5) 
                    {  
                        x.timestamp = e.timestamp+j;
                        x.event = Infection;
                        
                        Min_Q.push(x);
                        auto removed = S.extract(x); 
                    }
                    
                    // if there was no head on 5 times of toss, then we continue to the next neighbour.
                    if(j >= 6)
                        continue;

                    // if x gets infected (when we tossed a coin 5 times, just before this...), we generate a recovery event as follows:
                    if(x.event == 0)   // recovery event
                    { 
                        //generating random number k 
                        int min = e.timestamp+j;
                        int max = e.timestamp+j+5;
                        int k = rand()%(max-min + 1) + min;

                        // inserting recovery event of x, with the randomly generated k as timestamp.
                        x.timestamp = k;
                        x.event = Recovery;
                        Min_Q.push(x);
                    }
                }
            }
        }
    }

    for(int l = 0 ; l< 15 ; l++)
    {
        cout << "Time stamp: " << l << "  Susceptible: " << num_Susceptible[l] << ", infected: " << num_Infection[l] << ", recovery: " << num_Recover[l] << endl;
    }

    cout<<endl;
    cout<<" _______________________________________________" << endl;
    cout << "|infection time stamp" << "| node |" << " Shortest Distance |" << endl;
    cout<<"|-----------------------------------------------|" << endl;
    for(int i = 1; i< u.node_id ; i++)
    {
        cout << "|         " << i << "          |   " << infection_timestamp[i] << "  |         " << shortest_Distance(adjacency_list,u,Node[i]) << "         |" << endl;
    }
    for(int i = u.node_id+1; i< N+1 ; i++)
    {
        cout << "|         " << i << "          |   " << infection_timestamp[i] << "  |         " << shortest_Distance(adjacency_list,u,Node[i]) << "         |" << endl;
    }
    return 0;
}