/**
 * file :	bs_socks5.c
 * author :	Rex
 * create :	2018-03-31 19:12
 * func : 
 * history:
 */

#include <stdio.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "bs_socks5.h"

#define MAX_USER 10
#define BUFF_SIZE 1024

#define AUTH_CODE 0x02

#define TIME_OUT 6000000

#define USER_NAME "yunshu"
#define PASS_WORD "ph4nt0m"

// Select auth method, return 0 if success, -1 if failed
status_t bs_socks5_method( int sock )
{
    char recv_buffer[BUFF_SIZE] = { 0 };
    char reply_buffer[2] = { 0 };
    
    socks5_method_req_t *method_request;
    socks5_method_res_t *method_response;
    
    // recv socks5_method_req_t
    int ret = recv( sock, recv_buffer, BUFF_SIZE, 0 );
    if( ret <= 0 )
    {
        perror( "recv error" );
        close( sock );
        
        return -1;
    }
    
    //printf( "select_method: recv %d bytes\n", ret );
    
    // if client request a wrong version or a wrong number_method
    method_request = (socks5_method_req_t *)recv_buffer;
    method_response = (socks5_method_res_t *)reply_buffer;
    
    method_response->version = VERSION;
    
    // if not socks5
    if( (int)method_request->version != VERSION )
    {
        method_response->select_method = 0xff;
        
        send( sock, method_response, sizeof(socks5_method_res_t), 0 );
        close( sock );
        
        return -1;
    }
    
    method_response->select_method = AUTH_CODE;
    if( -1 == send( sock, method_response, sizeof(socks5_method_res_t), 0 ) )
    {
        close( sock );
        return -1;
    }
    
    return 0;
}

// test password, return 0 for success.
status_t bs_socks5_auth( int sock, status_t (*auth_check)(const char* username, const char* password))
{
    char recv_buffer[BUFF_SIZE] = { 0 };
    char reply_buffer[BUFF_SIZE] = { 0 };
    
    socks5_auth_req_t *auth_request;
    socks5_auth_res_t *auth_response;
    
    // auth username and password
    int ret = recv(sock, recv_buffer, BUFF_SIZE, 0);
    if( ret <= 0 )
    {
        perror( "recv username and password error" );
        close( sock );
        return -1;
    }
    //printf( "AuthPass: recv %d bytes\n", ret );
    
    auth_request = (socks5_auth_req_t *)recv_buffer;
    
    memset( reply_buffer, 0, BUFF_SIZE );
    auth_response = (socks5_auth_res_t *)reply_buffer;
    auth_response->version = 0x01;
    
    char recv_name[256] = { 0 };
    char recv_pass[256] = { 0 };
    
    // auth_request->name_len is a char, max number is 0xff
    char pwd_str[2] = { 0 };
    strncpy( pwd_str, auth_request->name + auth_request->name_len, 1 );
    int pwd_len = (int)pwd_str[0];
    
    strncpy( recv_name, auth_request->name, auth_request->name_len );
    strncpy( recv_pass, auth_request->name + auth_request->name_len + sizeof(auth_request->pwd_len), pwd_len );
    
    //printf( "username: %s\npassword: %s\n", recv_name, recv_pass );
    // check username and password
    /*
    if( (strncmp( recv_name, USER_NAME, strlen(USER_NAME) ) == 0) &&
       (strncmp( recv_pass, PASS_WORD, strlen(PASS_WORD) ) == 0)
       )
    */
    if (auth_check==NULL || auth_check(recv_name, recv_pass) == BS_SUCCESS) {
        auth_response->result = 0x00;
        if(-1 == send(sock, auth_response, sizeof(socks5_auth_res_t), 0)){
            close(sock);
            return -1;
        }
        else
        {
            return 0;
        }
    }
    else{
        auth_response->result = 0x01;
        send(sock, auth_response, sizeof(socks5_auth_res_t), 0);
        
        close(sock);
        return -1;
    }
}

// parse command, and try to connect real server.
// return socket for success, -1 for failed.
int bs_socks5_command( int sock )
{
    char recv_buffer[BUFF_SIZE] = {0};
    char reply_buffer[BUFF_SIZE] = {0};
    
    socks5_req_t *socks5_request;
    socks5_res_t *socks5_response;
    
    // recv command
    int ret = recv(sock, recv_buffer, BUFF_SIZE, 0);
    if(ret <= 0)
    {
        perror( "recv connect command error" );
        
        close( sock );
        return -1;
    }
    
    socks5_request = (socks5_req_t *)recv_buffer;
    if((socks5_request->version != VERSION) || (socks5_request->cmd != CONNECT) ||
       (socks5_request->address_type == IPV6))
    {
        //printf( "connect command error.\n" );
        close( sock );
        return -1;
    }
    
    // begain process connect request
    struct sockaddr_in sin;
    
    memset( (void *)&sin, 0, sizeof(struct sockaddr_in) );
    sin.sin_family = AF_INET;
    
    // get real server&#39;s ip address
    if( socks5_request->address_type == IPV4 )
    {
        memcpy( &sin.sin_addr.s_addr, &socks5_request->address_type + sizeof(socks5_request->address_type) , 4 );
        memcpy( &sin.sin_port, &socks5_request->address_type + sizeof(socks5_request->address_type) + 4, 2 );
        
        //printf( "Real Server: %s %d\n", inet_ntoa( sin.sin_addr ), ntohs( sin.sin_port ) );
    }
    else if( socks5_request->address_type == DOMAIN )
    {
        char domain_length = *(&socks5_request->address_type + sizeof(socks5_request->address_type));
        char target_domain[ 256] = { 0 };
        
        strncpy( target_domain, &socks5_request->address_type + 2, (unsigned int)domain_length );
        
        //printf( "target: %s\n", target_domain );
        
        struct hostent *phost = gethostbyname( target_domain );
        if( phost == NULL )
        {
            //printf( "Resolve %s error!\n" , target_domain );
            
            close( sock );
            return -1;
        }
        memcpy( &sin.sin_addr , phost->h_addr_list[0] , phost->h_length );
        
        memcpy( &sin.sin_port, &socks5_request->address_type + sizeof(socks5_request->address_type) +
               sizeof(domain_length) + domain_length, 2 );
    }
    
    // try to connect to real server
    int real_server_sock = socket( AF_INET, SOCK_STREAM, 0 );
    if( real_server_sock < 0 )
    {
        perror( "Socket creation failed\n");
        
        close( sock );
        return -1;
    }
    
    memset( reply_buffer, 0, sizeof(BUFF_SIZE) );
    
    socks5_response = (socks5_res_t *)reply_buffer;
    
    socks5_response->version = VERSION;
    socks5_response->reserved = 0x00;
    socks5_response->address_type = 0x01;
    memset( socks5_response + 4, 0 , 6 );
    
    ret = connect( real_server_sock, (struct sockaddr *)&sin, sizeof(struct sockaddr_in) );
    if( ret == 0 )
    {
        socks5_response->reply = 0x00;
        if( -1 == send( sock, socks5_response, 10, 0 ) )
        {
            close( sock );
            
            return -1;
        }
    }
    else
    {
        perror( "Connect to real server error" );
        socks5_response->reply = 0x01;
        send( sock, socks5_response, 10, 0 );
        
        close( sock );
        return -1;
    }
    
    return real_server_sock;
}

status_t bs_socks5_process( int sock, int real_server_sock)
{
    char recv_buffer[BUFF_SIZE] = { 0 };
    
    fd_set fd_read;
    struct timeval time_out;
    
    time_out.tv_sec = 0;
    time_out.tv_usec = TIME_OUT;
    
    int ret = 0;
    
    while( 1 )
    {
        FD_ZERO( &fd_read );
        FD_SET( sock, &fd_read );
        FD_SET( real_server_sock, &fd_read );
        
        ret = select( (sock > real_server_sock ? sock : real_server_sock) + 1, &fd_read, NULL, NULL, &time_out );
        if( -1 == ret )
        {
            perror( "select socket error" );
            break;
        }
        else if( 0 == ret )
        {
            //perror( "select time out" );
            continue;
        }
        
        //printf( "[DEBUG] testing readable!\n" );
        if( FD_ISSET(sock, &fd_read) )
        {
            //printf( "client can read!\n" );
            memset( recv_buffer, 0, BUFF_SIZE );
            ret = recv( sock, recv_buffer, BUFF_SIZE, 0 );
            if( ret > 0 )
            {
                //printf( "%s", recv_buffer );
                //printf( "recv %d bytes from client.\n", ret );
                ret = send( real_server_sock, recv_buffer, ret, 0 );
                if( ret == -1 )
                {
                    perror( "send data to real server error" );
                    break;
                }
                //printf( "send %d bytes to client!\n", ret );
            }
            else if( ret == 0 )
            {
                //printf( "client close socket.\n" );
                break;
            }
            else
            {
                //perror( "recv from client error" );
                break;
            }
        }
        
        else if( FD_ISSET(real_server_sock, &fd_read) )
        {
            //printf( "real server can read!\n" );
            memset( recv_buffer, 0, BUFF_SIZE );
            ret = recv( real_server_sock, recv_buffer, BUFF_SIZE, 0 );
            if( ret > 0 )
            {
                //printf( "%s", recv_buffer );
                //printf( "recv %d bytes from real server.\n", ret );
                ret = send( sock, recv_buffer, ret, 0 );
                if( ret == -1 )
                {
                    perror( "send data to client error" );
                    break;
                }
            }
            else if( ret == 0 )
            {
                //printf( "real server close socket.\n" );
                break;
            }
            else
            {
                perror( "recv from real server error" );
                break;
            }
        }
    }
    
    return 0;
}

int bs_socks5( void *client_sock )
{
    int sock = *(int *)client_sock;
    
    if( bs_socks5_method( sock ) == -1 )
    {
        //printf( "socks version error\n" );
        return -1;
    }
    
    if( bs_socks5_auth( sock ) == -1 )
    {
        //printf( "auth password error\n" );
        return -1;
    }
    
    int real_server_sock = bs_socks5_command( sock );
    if( real_server_sock == -1 )
    {
        //printf( "parse command error.\n" );
        return -1;
    }
    
    bs_socks5_process( sock, real_server_sock );
    
    close( sock );
    close( real_server_sock );
    
    return 0;
}

/*
int main( int argc, char *argv[] )
{
    if( argc != 2 )
    {
        printf( "Socks5 proxy for test,code by YunShu\n" );
        printf( "Usage: %s <proxy_port>\n", argv[0] );
        printf( "Options:\n" );
        printf( " <proxy_port> ---which port of this proxy server will listen.\n" );
        
        return 1;
    }
    
    struct sockaddr_in sin;
    
    memset( (void *)&sin, 0, sizeof( struct sockaddr_in) );
    sin.sin_family = AF_INET;
    sin.sin_port = htons( atoi(argv[1]) );
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    
    int listen_sock = socket( AF_INET, SOCK_STREAM, 0 );
    if( listen_sock < 0 )
    {
        perror( "Socket creation failed\n");
        return -1;
    }
    
    int opt = SO_REUSEADDR;
    setsockopt( listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt) );
    
    if( bind( listen_sock, (struct sockaddr*)&sin, sizeof(struct sockaddr_in) ) < 0 )
    {
        perror( "Bind error" );
        return -1;
    }
    
    if( listen( listen_sock, MAX_USER ) < 0 )
    {
        perror( "Listen error" );
        return -1;
    }
    
    struct sockaddr_in cin;
    int client_sock;
    int client_len = sizeof( struct sockaddr_in );
    
    while( client_sock = accept( listen_sock, (struct sockaddr *)&cin, (socklen_t *)&client_len ) )
    {
        printf( "Connected from %s, processing......\n", inet_ntoa( cin.sin_addr ) );
        
        pthread_t work_thread;
        if( pthread_create( &work_thread, NULL, (void *)bs_socks5, (void *)&client_sock ) )
        {
            perror( "Create thread error..." );
            close( client_sock );
        }
        else
        {
            pthread_detach( work_thread );
        }
    }
}
*/
