#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "heap_priority_queue.h"
#include "ktmem.h"

void help(char *argv){
    printf("\n");
    printf("usage: %s <COMMAND> FILE_PATH\n\n", argv);
    printf("Commands:\n");
    printf("    compress        compress data\n");
    printf("    decompress      decompress data\n");
    printf("    help\n\n");

    printf("Options:\n\n");
    printf("    -h, --help      Print help\n");
    printf("    -v, --version   Print version\n");

}


long get_file_size(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Can't open file");
        exit(EXIT_FAILURE);
    }
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    printf("size file: %ld\n", size);
    fseek(file, 0, SEEK_SET);
    fclose(file);
    return size;
}

void print_binary(unsigned int value, int bits) {
    for (int i = bits - 1; i >= 0; i--) {
        unsigned int mask = 1 << i;
        putchar((value & mask) ? '1' : '0');
    }
    putchar('\n');
}

char* get_utf8_string(const unsigned char *str, int *index) {
    unsigned char c = str[*index];
    int length = 1;
    //printf("DEC = %d | CHAR = %c | HEX = %X\n", str[*index], str[*index], (unsigned char) str[*index]);
    //print_binary(c, 8);
    if ((c & 0x80) == 0x00) {
        length = 1; // 1 byte UTF-8 (ASCII)
    } else if ((c & 0xE0) == 0xC0) {
        length = 2; // 2 bytes
    } else if ((c & 0xF0) == 0xE0) {
        length = 3; // 3 bytes
    } else if ((c & 0xF8) == 0xF0) {
        length = 4; // 4 bytes
    }

    char* utf8_char = (char*)ktmalloc(length + 1);
    
    for (int i = 0; i < length; i++) {
        //printf("%c", str[*index + i]);
        utf8_char[i] = str[*index + i];
    }
    utf8_char[length] = '\0';

    *index += length;
    return utf8_char;
}

void read_file_with_buffer(const char *filename) {
    long file_size = get_file_size(filename);

    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Can't open file");
        exit(EXIT_FAILURE);
    }

    unsigned char *buffer = ktmalloc(file_size);
    printf("buffer sizeof: %lu \n", sizeof(buffer));

    size_t bytes_read;
    binary_heap_pq_t *heap = create_binary_heap_pq();


    while ((bytes_read = fread(buffer, 1, file_size, file)) > 0) {
        printf("Bytes readed: %ld\n", bytes_read);
        int i = 0;
        while (i < bytes_read) {
            //printf("char: %d %d\n", buffer[i], iscntrl(buffer[i]));
            if(!(buffer[i] >= 0 && buffer[i] <=31) && buffer[i]!=0x7F) {
                char *char_as_string = get_utf8_string(buffer, &i);
                //printf("Char as string %s\n", char_as_string);
                if(is_empty(heap)){
                    insert_node(heap, char_as_string, 1);
                } else {

                    heap_pq_node_t *current = heap->head;
                    heap_pq_node_t *previuos = NULL;

                    short is_present = 0;
                    while (current && !is_present) {
                        const char *element = current->tree_node->data->element;
                        int weight = current->tree_node->data->weight;

                        // comparar si un string es igual a otro, es decir, sí el
                        // char_as_string que es el leido, es igual al de algun nodo
                        //TODO: ignore case.
                        if(strcmp(element, char_as_string)==0) {
                            is_present = 1;
                            weight = weight + 1;

                            if(previuos == NULL) { //is the first node
                                heap_pq_node_t *topNode;
                                remove_top_node(heap, &topNode);
                                insert_node(heap, element, weight);
                                ktfree(topNode);

                            } else {
                                delete_node(heap, &previuos, &current);
                                insert_node(heap, element, weight);
                            }
                            ktfree(char_as_string);

                            break;
                        }
                        previuos = current;
                        current = current->next;
                    }

                    if(!is_present){
                        insert_node(heap, char_as_string, 1);
                    }

                }
            } else {
                printf("control %02x\n", buffer[i]);
                i++;
            }

        }
    }

    print_heap(heap);

    heap_pq_node_t *topNode;
    printf("---------------inciia destrucción---------------------\n");
    while (!is_empty(heap)) {
        remove_top_node(heap, &topNode);
        //printf("aqui para: \n");
        //printf("%c: %d\n", topNode->data->element, topNode->data->weight);
        //printf("aqui para: \n");

        ktfree(topNode);
    }
    printf("Destruccion\n");

    destroy_heap_priority_queue(heap);

    ktfree(buffer);
    fclose(file);
}

void compress_file(char *file_path){

    read_file_with_buffer(file_path);

}



int main(int argc, char *argv[]) {

    if (argc < 3) {
        if(argc ==2 && (strcmp(argv[1], "-v")==0 ||strcmp(argv[1], "--version")==0)){
            printf("%s version 0.0.1, build xxx\n", argv[0]);
            return 1;
        }
        help(argv[0]);
        return 1;
    }

    char *command = argv[1];
    char *file_path = argv[2];

    if(strcmp(command, "compress")==0){
        compress_file(file_path);
    } else if (strcmp(command, "decompress")==0){
        printf("Not implemented yet\n");
    } else {
        help(argv[0]);
    }

}

