#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct _TREENODE{
	char ch;
	struct _TREENODE* lchild;
	struct _TREENODE* rchild;
}TreeNode;


//���ŷ�����������
TreeNode* CreateBinaryTree(){
	
	char ch;
	scanf("%c",&ch);
	getchar();

	if (ch == '#'){
		return NULL;
	}

	TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
	node->ch = ch;
	node->lchild = CreateBinaryTree();
	node->rchild = CreateBinaryTree();
	
	return node;
}

void FreeTreeSpace(TreeNode* root){
	if (root == NULL){
		return;
	}

	//�ͷ�������
	FreeTreeSpace(root->lchild);
	//�ͷ�������
	FreeTreeSpace(root->rchild);
	//�ͷŵ�ǰ�ڵ�
	free(root);
}

//��ӡ������
void Recusion(TreeNode* root){
	if (root == NULL){
		return;
	}
	printf("%c",root->ch);
	Recusion(root->lchild);
	Recusion(root->rchild);
}


int main(){

	TreeNode* root = CreateBinaryTree();
	Recusion(root);
	FreeTreeSpace(root);

	system("pause");
	return EXIT_SUCCESS;
}