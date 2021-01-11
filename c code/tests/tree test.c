#include <stdio.h>
#include <stdlib.h>


typedef struct k{
    int ok1;
    int ok2;
}KEK;
typedef struct tree{
    KEK tuple;
    struct tree* subtrees;
}Tree;
// test1: updating tree using adresses directly
// remarks: an incomplete structure can't have an array of itself inside
// aka struct tree subtrees[15] doesn't work so we have to use pointers and malloc
// and realloc stuff and beware because using multiple varriables adresses
// to insert stuff near the pointer will cause chaos so malloc (aka make neigbours for
// the pointer then use the varriables data to fill the created memory points)

int test1(){
    KEK tup1 = {0,2};
    KEK tup2 = {1,2};
    KEK tup3 = {2,2};
    KEK tup4 = {3,2};
    Tree* temp = malloc(sizeof(Tree));
    Tree root,tree1,tree2,tree11;
    root.tuple = tup1;
    root.subtrees = malloc(sizeof(Tree)*15);
    tree1.tuple = tup2;
    tree2.tuple = tup3;
    tree11.tuple = tup4;
    root.subtrees = &tree1;
    temp = root.subtrees+1;
    temp = &tree2;
    root.subtrees[0].subtrees = malloc(sizeof(Tree)*15);
    root.subtrees[0].subtrees = &tree11;
    
    printf("var adress tree11: %p",&tree11);
    printf("var adress tree11 in root: %p",&root.subtrees[0].subtrees[0]);
    printf("tuple in tree1: %d %d",tree11.tuple.ok1,tree11.tuple.ok2);
    printf("tuple in tree1 in root: %d %d",root.subtrees[0].subtrees[0].tuple.ok1,root.subtrees[0].subtrees[0].tuple.ok2);
    
    return 0;
    //test results:
    //  this method actually works but it has some major flaws
    //  1- if the variables get freed (like outside of a function) the core guets dumped since the pointers
    //  won't be pointing to anything anymore
    //  2- while it is easy to navigate directly to the subtrees this way it presents some security issues
    //  with the varriables being changed outside of the function.
    //  --> i'll try getting the variable data then add it directly to the subtrees list so the automatic function flush
    //  doesn't cause any issue and i don't have a bunch of varriables i can't change in my code
    //   -> to insert smthing into a tree i'll need to pass in : a pointer to the tree_root, pointer to the curent_tree_node
    //  and the values i need to add to the current tree.
    //  --> if this method creates sm issues i can drop convinience for concistence by using a path leading to the current_tree_node
    //  represented as an array of integers [0,1,1] is equivalent to root.sub[0].sub[1].sub[1] 
}
// a problem that i just noticed is with allocating memory!
// supose this is the tree struct: (2 subtrees is the max)
// [data,X,X] the X is the place we allocated for the subtrees
// let's initialise the first one: [data,tree1,X]
// [data_tree1,X1,X1] so as u notice we allocated 2 other subtrees in tree 1
// problem is since &(tree+1) = &X =&X1 the palce we allocated for X gets filled
// thus creating a huuuge issue that breaks our current approach
// -> i've been avoiding linked lists but now i have to use them  

typedef struct tree{
    KEK tuple;
    struct linkd{
        struct tree* curr_subtree;
        struct linkd* NEXT; 
    }subtrees;
}Tree2;
int test2(){
    
}
int main()
{
    test1();
    test2();
}