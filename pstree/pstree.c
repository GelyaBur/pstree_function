#include "tlpi_hdr.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/dir.h>
#include <ctype.h>

#define MAXLINE 100
#define MAX_PATH 1024
#define NUM_CHILD 150

struct node {
    pid_t pid;
    pid_t ppid;
    char name[64];
    struct node *children[NUM_CHILD];
    struct node *parent;
};

struct node_list {
    struct node *x;
    struct node_list *next;
};

void push(struct node_list **head, struct node *t)
{
    struct node_list *new_node = (struct node_list *) malloc(sizeof(struct node_list));
    new_node->x = t;
    new_node->next = (*head);
    (*head) = new_node;
}


struct node *create_node_list(char *name)
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
    x->parent = NULL;
    return x;
}

struct node_list *create_list(char *dir)
{
    char name[MAX_PATH];
    struct dirent *dp;
    DIR *dfd;
    struct node_list *head = NULL;

    if ((dfd = opendir(dir)) == NULL) {
        fprintf(stderr, "can't open %s\n", dir);
        exit(1);
    }
    while ((dp = readdir(dfd)) != NULL)
        if (isdigit(dp->d_name[0])) {
            sprintf(name, "%s/%s/status", dir, dp->d_name);
            push(&head, create_node_list(name));
        }
    return head;
}

struct node_list *find_node(struct node_list *head, pid_t pid)
{
    if (!pid) {
        return NULL;
    }
    while (head) {
        if (head->x->pid == pid)
            return head;
        head = head->next;
    }
}

struct node *create_tree(struct node_list *head)
{
    struct node_list *cur = head;
    while (cur) {
        int i = 0;
        struct node_list *parent = find_node(head, cur->x->ppid);
        if (parent != NULL) {
            cur->x->parent = parent->x;
            for (; parent->x->children[i] != NULL; i++);
            parent->x->children[i] = cur->x;
        }
        cur = cur->next;
    }
    return find_node(head, 1)->x;
}

void print_list()
{
    static int i = 0;
    struct node_list *cur = create_list("/proc");
    while (cur != NULL) {
        i++;
        printf("%s %d %d\n", cur->x->name, cur->x->pid, cur->x->ppid);
        cur = cur->next;
    }
    printf("i = %d\n", i);
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
        //struct node *cur = root;
        //while (cur->parent != NULL) {
        //    i++;
        //    cur = cur->parent;
        //
        //}
        printf("%*c(%d)%s", j*10, ' ', root->pid, root->name);
        print_tree(root->children[0], j+1);
        for (int i = 1; root->children[i] != NULL; i++)
            print_tree(root->children[i], j+1);
    }
}


int main()
{
    struct node_list *head = create_list("/proc");
    struct node *root = create_tree(head);
    sort_tree(root);
    print_tree(root, 0);
    return 0;
}



