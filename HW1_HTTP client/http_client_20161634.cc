#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#define MAXDATASIZE 1000
#define MAXLEN 100
#define MEMSET(string) memset(string, '\0', sizeof(string))

int getcontentLen(const char *line){
    int contentLen=-1;
    char *temp;
    temp=(char*)malloc(sizeof(char)*(100+strlen(line)));

    strcpy(temp, line);
    for(int i=0;i<(int)strlen(temp);i++){
        if(temp[i]>='A' && temp[i]<='Z'){
            temp[i]+=32;
        }
    }


    if(strstr(temp, "content-length: ")!=NULL){
        contentLen=atoi(temp+16);
    }
    free(temp);

    return contentLen;
}

int main(int argc, char *argv[]){

    if(argc!=2){
        printf("usage: http_client http://hostname[:port][/path/to/file]\n");
        exit(1);
    }
    if(strncmp("http://", argv[1],7)){
        printf("usage: http_client http://hostname[:port][/path/to/file]\n");
        exit(1);
    }
    int sockfd, numbytes;
    struct addrinfo hints, *servinfo;
    int rv;
    char s[INET_ADDRSTRLEN];
    char buffer[MAXDATASIZE];
    char hostname[MAXLEN];
    char portnum[MAXLEN];
    char path[MAXLEN];
    bool portexist=false;
    bool pathexist=false;
    char temp[MAXLEN];
    MEMSET(temp);

    FILE *output;
    output=fopen("20161634.out", "w");

    strcpy(buffer, argv[1]);
    int arglen=strlen(argv[1]);
    int i,j;
    for(i=7, j=0;i<arglen;i++,j++){
        if(buffer[i]!=':' && buffer[i]!='/'){
            temp[j]=buffer[i];
        }
        if(!portexist && !pathexist && (buffer[i]==':' || buffer[i]=='/')){
            strcpy(hostname, temp);
            MEMSET(temp);
            j=-1;
            if(buffer[i]==':'){
                portexist=true;
            }
            else if(buffer[i]=='/'){
                pathexist=true;
                sprintf(portnum,"%d", 80);
                j=i;
            }
        }
        if(portexist && buffer[i]=='/'){
            strcpy(portnum, temp);
            pathexist=true;
            j=i;
        }

        if(buffer[i]=='/') break;
    }

    MEMSET(path);

    path[0]='/';

    for(int n=1,i=j+1;i<arglen;i++,n++){
        path[n]=buffer[i];
    }
    
    if(!portexist && !pathexist){
        strcpy(hostname, temp);
        sprintf(portnum,"%d", 80);
        MEMSET(path);
        path[0]='/';
    }
    if(!portexist && pathexist){
        sprintf(portnum,"%d", 80);
    }
    if(portexist && !pathexist){
        MEMSET(path);
        path[0]='/';
    }


    memset(&hints, 0, sizeof hints);
    hints.ai_family=AF_UNSPEC;
    hints.ai_socktype=SOCK_STREAM;

    if((rv=getaddrinfo(hostname, portnum, &hints, &servinfo))!=0){
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    if((sockfd=socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol))==-1){
        perror("client: socket");
        return 2;
    }

    if(connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen)==-1){
        close(sockfd);
        perror("client: connect");
        return 2;
    }

    inet_ntop(servinfo->ai_family, &((struct sockaddr_in*)servinfo->ai_addr)->sin_addr, s, sizeof s);
    
    freeaddrinfo(servinfo);

    //initialize string for connection request (two lines)
    MEMSET(buffer);
    sprintf(buffer, "GET %s HTTP/1.1\r\nHost: %s:%s\r\n\r\n", path, hostname,portnum);


    if(send(sockfd, buffer, strlen(buffer), 0)==-1){
        perror("send");
        close(sockfd);
        exit(1);
    }
    int cur_len=0;
    int contentlength=-1;
    bool headerend=false;
    int linecnt=0;
    char *tmp;
    char *ptr=NULL;
    while(1){

        if((numbytes=recv(sockfd, buffer, sizeof(buffer), 0))==-1){
            perror("recv");
            close(sockfd);
            exit(1);
        }

        buffer[numbytes]='\0';
        tmp=buffer;
        if(!headerend){
            do{
                ptr=strstr(tmp,"\r\n");
                if(ptr)*ptr='\0';

                if(linecnt==0){
                    printf("%s\n", tmp);
                }
                if(getcontentLen(tmp)!=-1){
                    contentlength=getcontentLen(tmp);
                }
                int tmplen=(int)strlen(tmp);
                tmp=ptr+2;

                if(tmplen==0){
                    headerend=true;
                    break;
                }
                linecnt++;
            }while(ptr!=NULL);

        }
        if(headerend){
            if(contentlength==-1){
                printf("Content-Length not specified\n");
                exit(1);
            }
            cur_len+=strlen(tmp);
            fprintf(output, "%s", tmp);
        }

        if(cur_len==contentlength){
            break;
        }
    }

    printf("%d bytes written to 20161634.out\n", contentlength);
    fclose(output);

}