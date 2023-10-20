#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct app {char name[60];char id[80];char category[20];float rating;} tApp;

typedef struct registro {char dado[80]; long endereco;} tReg;

typedef struct nodo_endereco {long endereco;struct nodo_endereco *next;} tNodo_endereco;

typedef struct nodo_categoria {char categoria[20];tNodo_endereco *endereco;struct nodo_categoria *next;} tNodo_categoria;

typedef struct AVLNode {float rating;long *enderecos; int qnt_enderecos; int altura; struct AVLNode *r;struct AVLNode* l;} tAVLNode;

//cria arquivo binario a partir do arquivo csv tratado
void cria_arquivo_binario(){
	printf("\n# CRIANDO ARQUIVO DE DADOS #\n");
    FILE* f_csv = fopen("Google-Playstore-Ordenado.csv", "rt");

    if (f_csv == NULL) {
        printf("erro no arquivo: apps.txt\n");
        return;
    }

    FILE* f_bin = fopen("apps.bin", "wb");

    char st[200];

    while(fgets(st, 200, f_csv)) {
        char *name, *id, *category, *rating;
        tApp app;
        name = strtok(st, ",");
        id = strtok(NULL, ",");
        category = strtok(NULL, ",");
        rating = strtok(NULL, ",");

        strcpy(app.name, name);
        strcpy(app.id, id);
        strcpy(app.category, category);
        app.rating=atof(rating);
        fwrite(&app, sizeof(tApp), 1, f_bin);
    }
    fclose(f_bin);

    fclose(f_csv);
    printf("# ARQUIVO DE DADOS CRIADO COM SUCESSO #\n\n");
}

//indice exaustivo a partir do arquivo binario de registros. dados ficam armazenados da seguinte forma: id,endereco
void cria_arquivo_indice_campoId() {
	printf("\n# CRIANDO ARQUIVO DE INDICE NA CHAVE ID #\n");

    FILE* f_bin = fopen("apps.bin", "rb");
    FILE* f_index = fopen("indice_campoId.bin", "w+b");

    if (f_bin == NULL) {
        printf("erro no arquivo: apps.bin\n");
        return;
    }

    int endereco = 0;
    tApp app;
    tReg reg;

    while (fread(&app, sizeof(tApp), 1, f_bin)) {
        strcpy(reg.dado, app.id);
        reg.endereco=endereco;
        fwrite(&reg, sizeof(tReg), 1, f_index);
        endereco++;
    }

    fclose(f_bin);
    fclose(f_index);
    printf("# ARQUIVO DE INDICE CRIADO COM SUCESSO #\n\n");
}

//indice exaustivo a partir do arquivo binario de registros. dados ficam armazenados da seguinte forma: nome,endereco
void cria_arquivo_indice_campoNome() {
	printf("\n# CRIANDO ARQUIVO DE INDICE DO CAMPO NOME #\n");

    FILE* f_bin = fopen("apps.bin", "rb");
    FILE* f_index = fopen("indice_campoNome.bin", "w+b");

    if (f_bin == NULL) {
        printf("erro no arquivo: apps.bin\n");
        return;
    }

    long i, tamanho, quantidade;
    fseek(f_bin, 0, SEEK_END);
    tamanho = ftell(f_bin);
    quantidade=tamanho/sizeof(tApp);
    fseek(f_bin, 0, SEEK_SET);

    long endereco, menorend, ume;
    char menor[60], ultmenor[60];
    tApp app;
    tReg reg;

    for(i=0; i<quantidade; i++){
        endereco=0;
        while (fread(&app, sizeof(tApp), 1, f_bin)) {
            if(i==0){
                if(endereco==0 || strcmp(app.name, menor)<0){
                    strcpy(menor, app.name);
                    menorend=endereco;
                }
            }
            else if(strcmp(app.name, ultmenor)>0 && strcmp(menor, ultmenor)==0){
                strcpy(menor, app.name);
                menorend=endereco;
            }
            else if(strcmp(app.name, menor)<0 && strcmp(app.name, ultmenor)>0){
                strcpy(menor, app.name);
                menorend=endereco;
            }
            endereco++;
        }
        strcpy(reg.dado, menor);
        reg.endereco=menorend;
        fwrite(&reg, sizeof(tReg), 1, f_index);
        strcpy(ultmenor, menor);
        fseek(f_bin, 0, SEEK_SET);


    }

    fclose(f_index);

    printf("# ARQUIVO DE INDICE CRIADO COM SUCESSO #\n\n");
}


int pesquisaBinariaIndiceId(char nomechave[]){
    FILE *f_bin=fopen("indice_campoId.bin", "rb");
    long tamanho, fim, meio, inicio=0;
    tReg reg;
    fseek(f_bin, 0, SEEK_END);
    tamanho = ftell(f_bin);
    fseek(f_bin, 0, SEEK_SET);
    fim=tamanho/sizeof(tReg) - 1;
    while(inicio<=fim){
        meio=(inicio+fim)/2;
        fseek(f_bin, meio*sizeof(tReg), SEEK_SET);
        fread(&reg, sizeof(tReg), 1, f_bin);

        if(strcmp(nomechave, reg.dado)>0){
            inicio=meio + 1;
        }
        else if(strcmp(nomechave, reg.dado)<0){
            fim=meio - 1;
        }
        else{
            printf("\n%s encontrado nos registros. \n", nomechave);
            return reg.endereco;
        }
    }
    printf("\n%s nao encontrado nos registros. \n", nomechave);
    return -1;
}
void consultaDados_IdIndice(char nomechave[]){
    printf("\n# CONSULTA DE ID EM ÍNDICE INICIADA #\n");

    int endereco = pesquisaBinariaIndiceId(nomechave);
    if(endereco>=0){
        tApp app;
        FILE *f_bin=fopen("apps.bin", "rb");
        fseek(f_bin, endereco*sizeof(tApp), SEEK_SET);
        fread(&app, sizeof(tApp), 1, f_bin);
        printf("\nDados Obtidos: \nNome: %s\nId: %s\nCategoria: %s\nAvaliacao: %.2f\n", app.name, app.id, app.category, app.rating);
    }
    printf("\n# CONSULTA DE ID EM INDICE REALIZADA COM SUCESSO #\n\n");
}

int pesquisaBinariaIndiceNome(char nomechave[]){
    FILE *f_bin=fopen("indice_campoNome.bin", "rb");
    long tamanho, fim, meio, inicio=0;
    tReg reg;
    fseek(f_bin, 0, SEEK_END);
    tamanho = ftell(f_bin);
    fseek(f_bin, 0, SEEK_SET);
    fim=tamanho/sizeof(tReg) - 1;
    while(inicio<=fim){
        meio=(inicio+fim)/2;
        fseek(f_bin, meio*sizeof(tReg), SEEK_SET);
        fread(&reg, sizeof(tReg), 1, f_bin);

        if(strcmp(nomechave, reg.dado)>0){
            inicio=meio + 1;
        }
        else if(strcmp(nomechave, reg.dado)<0){
            fim=meio - 1;
        }
        else{
            printf("\n%s encontrado nos registros. \n", nomechave);
            return reg.endereco;
        }
    }
    printf("\n%s nao encontrado nos registros. \n", nomechave);
    return -1;
}
void consultaDados_NomeIndice(char nomechave[]){
    printf("\n# CONSULTA DE NOME EM ARQUIVO INICIADA #\n");

    int endereco = pesquisaBinariaIndiceNome(nomechave);
    if(endereco>=0){
        tApp app;
        FILE *f_bin=fopen("apps.bin", "rb");
        fseek(f_bin, endereco*sizeof(tApp), SEEK_SET);
        fread(&app, sizeof(tApp), 1, f_bin);
        printf("\nDados Obtidos: \nNome: %s\nId: %s\nCategoria: %s\nAvaliacao: %.2f\n", app.name, app.id, app.category, app.rating);
    }
    printf("\n# CONSULTA DE NOME EM ARQUIVO REALIZADA COM SUCESSO #\n\n");
}

tApp *pesquisaBinariaArquivo(char nomechave[]){
    FILE *f_bin=fopen("apps.bin", "rb");
    long tamanho, fim, meio, inicio=0;
    tApp app;
    fseek(f_bin, 0, SEEK_END);
    tamanho = ftell(f_bin);
    fseek(f_bin, 0, SEEK_SET);
    fim=tamanho/sizeof(tApp) - 1;
    while(inicio<=fim){
        meio=(inicio+fim)/2;
        fseek(f_bin, meio*sizeof(tApp), SEEK_SET);
        fread(&app, sizeof(tApp), 1, f_bin);

        if(strcmp(nomechave, app.id)>0){
            inicio=meio + 1;
        }
        else if(strcmp(nomechave, app.id)<0){
            fim=meio - 1;
        }
        else{
            printf("\n%s encontrado nos registros. \n", nomechave);
            tApp *r = (tApp*)malloc(sizeof(tApp));
            strcpy(r->name, app.name);
            strcpy(r->id, app.id);
            strcpy(r->category, app.category);
            r->rating=app.rating;
            return r;
        }
    }
    printf("\n%s nao encontrado nos registros. \n", nomechave);
    return NULL;
}
void consultaDados_IdArquivo(char nomechave[]){
    printf("\n# CONSULTA DE ID EM ARQUIVO INICIADA #\n");

    tApp *app = pesquisaBinariaArquivo(nomechave);
    if(app!=NULL)
        printf("\nDados Obtidos: \nNome: %s\nId: %s\nCategoria: %s\nAvaliacao: %.2f\n", app->name, app->id, app->category, app->rating);

    printf("\n# CONSULTA DE ID EM ARQUIVO REALIZADA COM SUCESSO #\n\n");

}




void adiciona_endereco(tNodo_categoria** node, long end) {
    if ((*node)->endereco->endereco == -1) {
        (*node)->endereco->endereco = end;

        (*node)->endereco->next = (tNodo_endereco*)malloc(sizeof(tNodo_endereco));
        (*node)->endereco->next->endereco = -1;
    } else {
        tNodo_endereco* aux = (*node)->endereco;
        while (aux->endereco != -1) {
            aux = aux->next;
        }
        aux->endereco = end;
        aux->next = (tNodo_endereco*)malloc(sizeof(tNodo_endereco));
        aux->next->endereco = -1;
    }
}
void cria_nodo(tNodo_categoria** node, int inicia_next) {
    if ((*node) != NULL) return;
    (*node) = (tNodo_categoria*)malloc(sizeof(tNodo_categoria));
    strcpy((*node)->categoria, "\0");
    (*node)->next = NULL;
    (*node)->endereco = (tNodo_endereco*)malloc(sizeof(tNodo_endereco));
    (*node)->endereco->endereco = -1;
    (*node)->endereco->next = NULL;
    if (inicia_next) {
        cria_nodo(&(*node)->next, 0);
    }
}

void insere_nodo(tNodo_categoria ** root, tApp app, long endereco) {
    if ((*root) == NULL) {
        cria_nodo(&(*root), 1);
        adiciona_endereco(&(*root), endereco);
        strcpy((*root)->categoria, app.category);
        return;
    }

    tNodo_categoria* aux = (*root);
    while(strcmp(aux->categoria, "\0")!=0 && strcmp(aux->categoria, app.category)!=0) {
        aux = aux->next;
    }

    if (strcmp(aux->categoria,"\0")==0) {
        cria_nodo(&(aux->next), 1);
        strcpy(aux->categoria, app.category);
    }
    adiciona_endereco(&aux, endereco);
}

//indice em memoria por listas encadeadas a partir do arquivo de registros binario. a estrutura do nodo contem categoria e pointer para uma lista de enderecos
void cria_multilista_indice_campoCategoria(tNodo_categoria ** root){

    printf("\n# CRIANDO MULTILISTA DE CATEGORIAS #\n");

    FILE* f_bin = fopen("apps.bin", "rb");

    if (f_bin == NULL) {
        printf("erro no arquivo: apps.bin\n");
        return;
    }

    long endereco = 0;
    tApp app;

    while (fread(&app, sizeof(tApp), 1, f_bin)) {
        insere_nodo(&(*root), app, endereco);
        endereco ++;
    }

    printf("# MULTILISTA CRIADA COM SUCESSO #\n\n");
}

//indice em memoria por AVL a partir do arquivo de registros binario. a estrutura do nodo contem avaliacao, nodo da direita, nodo da esquerda e pointer para enderecos
void consultaDados_CategoriaMemoria(char category[], tNodo_categoria* root) {
    printf("\n# CONSULTA EM MEMÓRIA POR CATEGORIAS INICIADA #\n");

    FILE* f_bin = fopen("apps.bin", "rb");
    tNodo_categoria* aux = root;

    while(strcmp(aux->categoria, "\0")!=0 && strcmp(aux->categoria, category)!=0) {
        aux = aux->next;
    }

    if (strcmp(aux->categoria,"\0")==0) {
        printf("Nao ha registros com a categoria %s \n", category);
        return;
    }

    tApp app;
    tNodo_endereco* endereco = aux->endereco;

    printf("\nDados Obtidos com a categoria %s: \n", category);
    while(endereco->endereco != -1) {
        fseek(f_bin, endereco->endereco*sizeof(tApp), SEEK_SET);
        fread(&app, sizeof(tApp), 1, f_bin);
        printf("\nNome: %s | Id: %s | Categoria: %s | Avaliacao: %.2f\n", app.name, app.id, app.category, app.rating);
        endereco = endereco->next;
    }

    printf("\n# CONSULTA POR CATEGORIA EM MEMORIA REALIZADA COM SUCESSO #\n\n");

}

void cria_nodo_avl(tAVLNode** node, int inicia_subnodos) {
    if ((*node) != NULL) return;
    (*node) = (tAVLNode*)malloc(sizeof(tAVLNode));
    (*node)->enderecos = (long*)malloc(sizeof(long) * 200);
    (*node)->qnt_enderecos = 0;
    (*node)->l = NULL;
    (*node)->r = NULL;
    (*node)->altura = 1;

    if (inicia_subnodos) {
        cria_nodo_avl(&(*node)->l, 0);
        cria_nodo_avl(&(*node)->r, 0);
    }
}

int get_altura(tAVLNode* node) {
    if (node == NULL) return 0;
    return node->altura;
}

void atualiza_altura(tAVLNode* node) {
    if (node != NULL) {
        int altura_esq = get_altura(node->l);
        int altura_dir = get_altura(node->r);
        node->altura = (altura_esq > altura_dir ? altura_esq : altura_dir) + 1;
    }
}

int fator_balanceamento(tAVLNode* node) {
    if (node == NULL) return 0;
    return get_altura(node->l) - get_altura(node->r);
}

tAVLNode* rotacao_direita(tAVLNode* y) {
    tAVLNode* x = y->l;
    tAVLNode* T2 = x->r;

    x->r = y;
    y->l = T2;

    atualiza_altura(y);
    atualiza_altura(x);

    return x;
}

tAVLNode* rotacao_esquerda(tAVLNode* x) {
    tAVLNode* y = x->r;
    tAVLNode* T2 = y->l;

    y->l = x;
    x->r = T2;

    atualiza_altura(x);
    atualiza_altura(y);

    return y;
}
void adiciona_endereco_avl(tAVLNode* node, long end) {
    node->enderecos[node->qnt_enderecos] = end;
    node->qnt_enderecos++;
}
tAVLNode* insere_nodo_avl(tAVLNode* node, tApp app, long endereco) {
    if (node == NULL) {
        cria_nodo_avl(&node, 0);
        adiciona_endereco_avl(node, endereco);
        node->rating = app.rating;
        return node;
    }

    if (app.rating < node->rating) {
        node->l = insere_nodo_avl(node->l, app, endereco);
    } else if (app.rating > node->rating) {
        node->r = insere_nodo_avl(node->r, app, endereco);
    } else {
        adiciona_endereco_avl(node, endereco);
        return node;
    }

    atualiza_altura(node);

    int balance = fator_balanceamento(node);

    if (balance > 1 && app.rating < node->l->rating) {
        return rotacao_direita(node);
    }
    if (balance < -1 && app.rating > node->r->rating) {
        return rotacao_esquerda(node);
    }
    if (balance > 1 && app.rating > node->l->rating) {
        node->l = rotacao_esquerda(node->l);
        return rotacao_direita(node);
    }
    if (balance < -1 && app.rating < node->r->rating) {
        node->r = rotacao_direita(node->r);
        return rotacao_esquerda(node);
    }

    return node;
}


void cria_tree_indice_campoAvaliacao(tAVLNode** root) {
    printf("\n# CRIANDO AVL DE AVALIACOES #\n");

    FILE* f_bin = fopen("apps.bin", "rb");

    if (f_bin == NULL) {
        printf("erro no arquivo: apps.bin\n");
        return;
    }

    long endereco = 0;
    tApp app;

    while (fread(&app, sizeof(tApp), 1, f_bin)) {
        *root = insere_nodo_avl(*root, app, endereco);
        endereco++;
    }

    printf("# AVL CRIADA COM SUCESSO #\n\n");
}

void consultaDados_AvaliacaoMemoria(float avaliacao, tAVLNode* root) {
    printf("\n# CONSULTA EM MEMORIA POR AVALIACAO INICIADA #\n");

    tAVLNode* aux = root;
    FILE* f_bin = fopen("apps.bin", "rb");

    while (aux != NULL) {
        if (avaliacao < aux->rating) {
            aux = aux->l;
        } else if (avaliacao > aux->rating) {
            aux = aux->r;
        } else {
            break;
        }
    }

    if (aux == NULL || aux->rating != avaliacao) {
        printf("Nao ha registros com a avaliacao %.2f \n", avaliacao);
        return;
    }

    int i;
    long endereco;
    tApp app;
    printf("\nDados Obtidos com a avaliacao %.2f: \n", avaliacao);
    for (i = 0; i < aux->qnt_enderecos; i++) {
        endereco = aux->enderecos[i];
        fseek(f_bin, endereco * sizeof(tApp), SEEK_SET);
        fread(&app, sizeof(tApp), 1, f_bin);
        printf("\nNome: %s | Id: %s | Categoria: %s | Avaliacao: %.2f\n", app.name, app.id, app.category, app.rating);
    }
    fclose(f_bin);

    printf("\n# CONSULTA POR AVALIACAO EM MEMORIA REALIZADA COM SUCESSO #\n\n");

}

int main()
{

    tNodo_categoria* list_root = NULL;
    tAVLNode *root = NULL;

    cria_arquivo_binario();
    cria_arquivo_indice_campoId();
    cria_arquivo_indice_campoNome();
    cria_multilista_indice_campoCategoria(&list_root);
    cria_tree_indice_campoAvaliacao(&root);
    char staux[60];
    float av;
    int op;
    printf("0-Sair \n");
    printf("1-Deseja consultar APP pelo seu Id? (Consulta binaria no arquivo) \n");
    printf("2-Deseja consultar APP pelo seu Id? \n");
    printf("3-Deseja consultar APP pelo seu Nome? \n");
    printf("4-Deseja consultar APPS com determinada Categoria? \n");
    printf("5-Deseja consultar APPS com determinada Avaliacao? \n");
    scanf("%d", &op);
    while(op){
        switch(op){
            case 0:
                exit(0);
                break;
            case 1:
                consultaDados_IdArquivo("app.motos.jahir.motocontrols");
                break;
            case 2:
                consultaDados_IdIndice("app.motos.jahir.motocontrol");
                break;
            case 3:
                consultaDados_NomeIndice("bible word search");
                break;
            case 4:
                consultaDados_CategoriaMemoria("education", list_root);
                break;
            case 5:
                printf("\nInsira avaliacao: \n");
                scanf("%f", &av);
                consultaDados_AvaliacaoMemoria(av, root);
                break;
            default:
                exit(0);
                break;
        }
        printf("\n");
        scanf("%d", &op);
    }


}

