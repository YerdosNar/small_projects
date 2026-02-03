#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define MAX_CHARACTERS 1005
#define MAX_PARAGRAPHS 5

struct word {
    char* data;
};

struct sentence {
    struct word* data;
    int word_count;//denotes number of words in a sentence
};

struct paragraph {
    struct sentence* data  ;
    int sentence_count;//denotes number of sentences in a paragraph
};

struct document {
    struct paragraph* data;
    int paragraph_count;//denotes number of paragraphs in a document
};
struct document get_document(char* text) {
    // save pointers for strtok_r
    char *save_p, *save_s, *save_w;

    struct paragraph *paragraphs = malloc(sizeof(struct paragraph));

    char *p = strtok_r(text, "\n", &save_p);
    int p_num = 0;
    while(p) {
        struct sentence *sentences = malloc(sizeof(struct sentence));

        char *s = strtok_r(p, ".", &save_s);
        int s_num = 0;
        while(s) {
            struct word *words = malloc(sizeof(struct word));

            char *w = strtok_r(s, " ", &save_w);
            int w_num = 0;
            while(w) {
                words[w_num++].data = w;
                w = strtok_r(NULL, " ", &save_w);
                struct word *new_w_ptr = realloc(words, sizeof(struct word) * (w_num+1));
                if(!new_w_ptr) {
                    free(words);
                    exit(1);
                }
                words = new_w_ptr;
            }
            sentences[s_num].data = words;
            sentences[s_num++].word_count = w_num;
            s = strtok_r(NULL, ".", &save_s);

            struct sentence *new_s_ptr = realloc(sentences, sizeof(struct sentence) * (s_num+1));
            if(!new_s_ptr) {
                free(words);
                free(sentences);
                exit(1);
            }
            sentences = new_s_ptr;
        }
        paragraphs[p_num].data = sentences;
        paragraphs[p_num++].sentence_count = s_num;
        p = strtok_r(NULL, "\n", &save_p);

        struct paragraph *new_p_ptr = realloc(paragraphs, sizeof(struct paragraph) * (p_num+1));
        if(!new_p_ptr) {
            free(sentences);
            free(paragraphs);
            exit(1);
        }
        paragraphs = new_p_ptr;
    }

    struct document doc = {
        .data = paragraphs,
        .paragraph_count = p_num
    };

    return doc;
}

struct word kth_word_in_mth_sentence_of_nth_paragraph(struct document Doc, int k, int m, int n) {
    return Doc.data[n-1].data[m-1].data[k-1];
}

struct sentence kth_sentence_in_mth_paragraph(struct document Doc, int k, int m) {
    return Doc.data[m-1].data[k-1];
}

struct paragraph kth_paragraph(struct document Doc, int k) {
    return Doc.data[k-1];
}


void print_word(struct word w) {
    printf("%s", w.data);
}

void print_sentence(struct sentence sen) {
    for(int i = 0; i < sen.word_count; i++) {
        print_word(sen.data[i]);
        if (i != sen.word_count - 1) {
            printf(" ");
        }
    }
}

void print_paragraph(struct paragraph para) {
    for(int i = 0; i < para.sentence_count; i++){
        print_sentence(para.data[i]);
        printf(".");
    }
}

void print_document(struct document doc) {
    for(int i = 0; i < doc.paragraph_count; i++) {
        print_paragraph(doc.data[i]);
        if (i != doc.paragraph_count - 1)
            printf("\n");
    }
}

char* get_input_text() {
    int paragraph_count;
    scanf("%d", &paragraph_count);

    char p[MAX_PARAGRAPHS][MAX_CHARACTERS], doc[MAX_CHARACTERS];
    memset(doc, 0, sizeof(doc));
    getchar();
    for (int i = 0; i < paragraph_count; i++) {
        scanf("%[^\n]%*c", p[i]);
        strcat(doc, p[i]);
        if (i != paragraph_count - 1)
            strcat(doc, "\n");
    }

    char* returnDoc = (char*)malloc((strlen (doc)+1) * (sizeof(char)));
    strcpy(returnDoc, doc);
    return returnDoc;
}

int main()
{
    char* text = get_input_text();
    struct document Doc = get_document(text);

    int q;
    scanf("%d", &q);

    while (q--) {
        int type;
        scanf("%d", &type);

        if (type == 3){
            int k, m, n;
            scanf("%d %d %d", &k, &m, &n);
            struct word w = kth_word_in_mth_sentence_of_nth_paragraph(Doc, k, m, n);
            print_word(w);
        }

        else if (type == 2) {
            int k, m;
            scanf("%d %d", &k, &m);
            struct sentence sen= kth_sentence_in_mth_paragraph(Doc, k, m);
            print_sentence(sen);
        }

        else{
            int k;
            scanf("%d", &k);
            struct paragraph para = kth_paragraph(Doc, k);
            print_paragraph(para);
        }
        printf("\n");
    }
}
