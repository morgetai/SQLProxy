#include<QCoreApplication>
#include"ProxyServer.h"
#include<iostream>
#include<vector>
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    ProxyServer server;
    server.setServerPort(999);
    server.setServerAdress("localhost");
    server.setRemoteServerPort(3306);
    server.setRemoteServerAdress("127.0.0.1");
    server.Run();

    return a.exec();
}
