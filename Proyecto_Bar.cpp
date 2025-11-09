/*En un bar muy concurrido, los clientes llegan constantemente para hacer sus pedidos de bebidas; cada clientedebe esperar a que uno de los baristas tome su orden, la prepare con cuidado y la deje lista en la barra, donde los meseros se encargan de recogerla y entregarla al cliente correspondiente; sin embargo, el bar tiene espacio limitado en su barra y en la mesa de pedidos, lo que obliga a organizar el flujo de trabajo: si hay demasiados pedidos acumulados, los clientes deben esperar antes de ordenar, y si no hay bebidas listas, los meseros se ven obligados a esperar hasta que los baristas terminen; de este modo, el problema consiste en diseñar un sistema concurrente/paralelo con hilos que modele este escenario, coordinando a clientes, baristas y meseros mediante mecanismos ya sea con
memoria compartida  o bien con paso de mensajes (elegir sólo una forma de solución), garantizando la correctitud en la ejecución del programa que se implemente para solucionar el problema.

Sugerencias:
a) Los Clientes son hilos productores que llegan y hacen
pedidos.
    Los Baristas son hilos procesadores preparan las
bebidas. 
    Los Meseros son hilos consumidores entregan las
bebidas listas a los clientes.

b) Utilizar un buffer circular para almacenar los pedidos
     Los baristas toman pedidos del buffer circular y los procesan
     Los meseros toman los pedidos listos de otro buffer y los entregan

c) El número de clientes, baristas y meseros es variable, por ejemplo: 5 clientes, 2 baristas y 2 meseros
     El tiempo de preparación de las bebidas se puede simular con sleeps
     El tamaño de los buffers/colas y de los pedidos debe ser limitado para saber cuando estan llenos o vacíos;  por ejemplo: 60 pedidos, 
     un buffer circular de tamaño 8 para los pedidos almacenados, un buffer de tamaño 8 para los pedidos procesados*/

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>
#include <cstdlib> // para rand() y srand()
#include <ctime>   // para time()

using namespace std;

class bebidas
{
public:
    string nombre; //nombre de la bebida
    int cliente, borrado; //borrado=0 indica que el elemento no ha sido borrado y 1 indica que si y por ende se puede sobreescribir (poner otro pedido)
public:
    bebidas() //La bebidas se inicializarán borradas
    {
        this->cliente = -1;
        this->nombre = "No Asignado";
        this->borrado = 1;
    }

    asigCont(int cliente) //Abreviacion de asignar contenido
    {
        this->cliente=cliente;
        this->borrado=0;

        srand(time(NULL));
        int aleatorio = rand() % 5;
        switch (aleatorio) //Se eligirá el nombre de la bebida de forma aleatoria
        {
        case 0:
            this->nombre="Cuba";
            break;
        
        case 1:
            this->nombre="Paloma";
            break;

        case 2:
            this->nombre="Piña Colada";
            break;

        case 3:
            this->nombre="Mojito";
            break;

        case 4:
            this->nombre="Margarita";
            break;

        case 5:
            this->nombre="Manhattan";
            break;

        default:
            printf("Considera poner un puesto de tacos");
            exit();
            break;
        }
    }
};

class colaCircular
{
public:
    array<bebidas,8> pedidos;
    int inicio, fin;
    bool vacia, llena;
    mutex candado;
    condition_variable vc;
public:
    colaCircular()
    {
        int i;
        inicio = 0; //Estara en la posicion en la cual la cola empieza a tener elementos
        fin = 0; //Estara en la ultima posicion en la que la cola termina de tener elementos 
        for(i=0; i<8; i++)
        {
            pedidos[i]->bebidas();
        }
        vacia = true;
        llena = false;
    }

    esVacia() //Si esta vacia la cola tendra el valor de verdadero
    {
        for(int i=0; i<8; i++)
        {
            if(pedidos[i]->borrado==0)
            {
                vacia=false;
                break;
            }
        }
        vacia=true;
    }

    esLlena() //Si esta llena la cola tendra el valor de verdadero
    {
        for(int i=0; i<8; i++)
        {
            if(pedidos[i]->borrado==1)
            {
                llena=false;
                break;
            }
        }
        llena=true;
    }

    agregarPedido(int cliente)
    {
        unique_lock<mutex> lk(candado);
        while (llena) //Si la cola esta llena espera a que deje de estarlo
        {
            vc.wait(lk);
            esLlena();
        }
        if(fin==7)
        {
            fin=0;
        }
        else
        {
            fin++;
        }
        bebidas[fin]->asigCont(cliente);
        vc.notify_all();
        lk.unlock();
    }

    eliminarPedido()
    {
        unique_lock<mutex> lk(candado)
        while (vacia) //Si la cola esta vacia espera a que deje de estarlo
        {
            vc.wait(lk);
            vc.esVacia();
        }
        if(inicio==7)
        {
            inicio=0;
        }
        else
        {
            inicio++;
        }
        bebidas[inicio]->borrado=1;
        vc.notify_all();
        lk.unlock();
    }
};

//Se va a requerir una funcion tomarPedido pero en la clase barista
