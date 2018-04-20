//
//  EventFrame.cpp
//  Memo
//
//  Created by Rex on 14-10-5.
//  Copyright (c) 2014年 King-App. All rights reserved.
//

#include "bs.h"
#include "EventFrame.h"

void on_accept(int sock, short event, void* arg);
void on_read(int sock, short event, void* arg);

EventFrame* EventFrame::m_instance = NULL;

state_t EventFrame::open(int port){
    m_sock = socket_tcp(BS_TRUE);
    assert(bs_sock_bind(m_sock, port) == BS_SUCCESS);
    assert(listen(m_sock, SOMAXCONN) == 0);
    debug_log("sock[%d] listen port[%d]", m_sock, port);
    
    m_base = event_base_new();
    event_set(&m_listen_event, m_sock, EV_READ|EV_PERSIST, on_accept, this);
    event_base_set(m_base, &m_listen_event);
    event_add(&m_listen_event, NULL);

    return BS_SUCCESS;
}

void on_accept(int sock, short event, void* arg)
{
    struct sockaddr_in  addr;
    socklen_t           sock_len = sizeof(struct sockaddr_in);
    EventFrame*         frame = (EventFrame*)arg;
    
    int accept_fd = accept(sock, (struct sockaddr*)&addr, &sock_len);
    debug_log("accept sock[%d] addr[%s:%d]", accept_fd, bs_sock_getip(&addr), bs_sock_getport(&addr));
    EventSocket* socket = frame->createSocket(accept_fd);
    if (socket != NULL) {
        frame->append(socket);
    }
}

void on_read(int sock, short event, void* arg){
    EventFrame* frame = (EventFrame*)arg;
    EventSocket*   socket = frame->eventSocket(sock);
    // 读取内容失败表示socket已经被关闭
    if(socket->onRead()<0){
        event_del(socket->getReadEvent());
        close(sock);
        frame->deleteSocket(socket);
    }
}

void EventFrame::append(EventSocket* socket){
    event_set(socket->getReadEvent(), socket->getSocket(), EV_READ|EV_PERSIST, on_read, this);
    event_base_set(m_base, socket->getReadEvent());
    event_add(socket->getReadEvent(), NULL);
    
    m_event_map[socket->getSocket()] = socket;
}

EventSocket* EventFrame::eventSocket(int sock){
    map<int, EventSocket*>::iterator iter = m_event_map.find(sock);
    if (iter == m_event_map.end()) {
        return NULL;
    }
    
    return iter->second;
}
