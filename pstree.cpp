#include "tlpi_hdr.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/dir.h>
#include <ctype.h>
#include <map>

#define MAXLINE 100
#define MAX_PATH 1024
#define NUM_CHILD 128
#define MAX 500

struct node {
    pid_t pid;
    pid_t ppid;
    char name[64];
    struct node *children[NUM_CHILD];
};

struct node *arr[MAX];


struct node *create_node(char *name)
{
    struct node *x =  (struct node *) malloc(sizeof(struct node));
    FILE *fp;
    if ((fp = fopen(name, "r")) == NULL) {
            printf("can't open file!");
            exit(1);
    }
    char line[100];
    char buf[32];
    char buf2[32];
    while (fgets(line, MAXLINE, fp)) {
        if (strncmp("Name", line, 4) == 0)
            sprintf(x->name, "%s", line + 6);
        else if (strncmp("Pid", line, 3) == 0) {
            sprintf(buf, "%s", line + 5);
            x->pid = atoi(buf);
        } else if (strncmp("PPid", line, 4) == 0) {
            sprintf(buf2, "%s", line + 6);
            x->ppid = atoi(buf2);
        }
    }
    for (int i = 0; i < NUM_CHILD; i++)
        x->children[i] = NULL;
    return x;
}

void create_arr(char *dir)
{
    for (int j = 0; j < MAX; j++)
        arr[j] = NULL;
    char name[MAX_PATH];
    struct dirent *dp;
    DIR *dfd;
    static int i = 0;
    if ((dfd = opendir(dir)) == NULL) {
        fprintf(stderr, "can't open %s\n", dir);
        exit(1);
    }
    while ((dp = readdir(dfd)) != NULL)
        if (isdigit(dp->d_name[0])) {
            sprintf(name, "%s/%s/status", dir, dp->d_name);
            arr[i] = create_node(name);
            i++;
        }
}


struct node *create_tree(char *dir)
{
    create_arr(dir);
    std::map <pid_t, int> map_tree;
    std::map <pid_t,int>::iterator it = map_tree.begin();
    for (int j = 0; arr[j]!=NULL;j++)
        map_tree.insert ( it, std::pair<pid_t,int>(arr[j]->pid,j) );
    //for (it = map_tree.begin(); it != map_tree.end(); ++it)
    //    printf("%d %d\n", it->first,it->second);
    int i = 0;
    for (; arr[i] != NULL; i++) {
        if (arr[i]->ppid != 0) {
            struct node *parent= arr[map_tree.find(arr[i]->ppid)->second];
            int z = 0;
            for (; parent->children[z] != NULL; z++);
            parent->children[z] = arr[i];
        }
    }
    return arr[map_tree.find(1)->second];
}

int compare(const void *a, const void *b)
{
    return strcmp((*(struct node **) a)->name, (*(struct node **) b)->name);
}

void sort_tree(struct node *root)
{
    if (root != NULL) { 
        int j = 0;
        for (; root->children[j] !=NULL; j++);
        qsort((void *) root->children, j, sizeof(struct node *), compare);
        for (int i = 0; root->children[i] != NULL; i++)
            sort_tree(root->children[i]);
    }
}

void print_tree(struct node *root, int j)
{
    if (root != NULL)
    {
        printf("%*c(%d)%s", j*10, ' ', root->pid, root->name);
        print_tree(root->children[0], j+1);
        for (int i = 1; root->children[i] != NULL; i++)
            print_tree(root->children[i], j+1);
    }
}


int main()
{
    char *name = "/proc";
    struct node *root = create_tree(name);
    sort_tree(root);
    print_tree(root, 0);
    return 0;
}



